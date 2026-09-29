#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <math.h>
#include <ext/spl/spl_exceptions.h>
#include "kernel/operators.h"
#include "php_ext.h"
#include "kernel/buffer.h"
#include "include/buffer.h"
#include "include/dispatch.h"
#include "tensor/exceptions/invalidargumentexception.zep.h"

/**
 * Number of outputs accumulated per pass of the tiled kernels.
 *
 * 32 was measured against the register file rather than guessed: at four doubles
 * per vector the tile is eight accumulators, which all stay in registers on the
 * 256-bit routes, and the compiler unrolls the tap loop on top of that, so the
 * innermost loop covers a useful block of loads per iteration. Wider tiles spill
 * the tail of the accumulators to the stack, and narrower ones re-enter the loop
 * more often per unit of work. */
#define TENSOR_CONV_TILE 32

/**
 * Doubles of reversed-kernel scratch taken from the stack, which covers every
 * kernel up to 16x16 -- every kernel anyone convolves an image or a signal with
 * in practice -- without touching the allocator. Larger kernels take a heap block
 * instead. */
#define TENSOR_CONV_SCRATCH_DOUBLES 256

/**
 * `fma()` is spelled out on the two FMA routes and nowhere else, because it is a
 * single instruction where the hardware has one and a call into libm where it
 * does not: on the baseline route the compiler emits a call to fma() for every
 * element, which is much worse than the separate multiply and add it replaces.
 * The baseline and plain AVX routes therefore keep the two operations spelled out
 * and let the target attribute contract them where it can.
 *
 * Redefined between the two groups of instantiations below so that all sixteen
 * kernels share one body text and there is no second copy that could drift. */
#define TENSOR_CONV_ACC(acc, x, y) ((acc) + (x) * (y))

/**
 * Add two lengths, saturating at the ends of the range instead of wrapping.
 *
 * The padded length is `L + 2 * padding`, and padding comes straight from the
 * caller, so the sum can wrap for a padding near ZEND_LONG_MAX. A wrap is worse
 * here than a wrong answer would be elsewhere: it produces a *small* positive
 * output length, and the convolution loops still emit every sample that length
 * was derived for, writing past the end of the buffer.
 *
 * The length is then divided by the stride with a plain `/`, which cannot
 * overflow the way the usual `(n + s - 1) / s` for the ceiling of n/s does, and
 * the +1 is applied after the division rather than to the numerator.
 *
 * Saturating hands tensor_tensorbuffer_create() a length it already rejects with
 * "too large to allocate", which is the error a genuinely enormous convolution
 * gets from the allocator anyway. An error that says so beats a buffer overrun.
 */
static zend_long tensor_conv_add(zend_long a, zend_long b)
{
	if (UNEXPECTED(b > 0 && a > ZEND_LONG_MAX - b)) {
		return ZEND_LONG_MAX;
	}

	if (UNEXPECTED(b < 0 && a < ZEND_LONG_MIN - b)) {
		return ZEND_LONG_MIN;
	}

	return a + b;
}

/**
 * Round x up to a multiple of s, never below zero.
 *
 * Output indices cannot be negative, so a window reaching past the start of the
 * input clamps to the first tile rather than to a position that would have to be
 * preserved.
 */
static zend_long tensor_conv_round_up(zend_long x, zend_long s)
{
	return x <= 0 ? 0 : ((x + s - 1) / s) * s;
}

/**
 * Round x down to a multiple of s, or -1 if x is negative.
 *
 * The -1 is what makes the callers' `for (m = first; m <= last; m += s)` loops
 * simply not run when the last whole tile would not fit, which is the condition
 * that has to hold for the tiles' unchecked loads to be in bounds.
 */
static zend_long tensor_conv_round_down(zend_long x, zend_long s)
{
	return x < 0 ? -1 : (x / s) * s;
}

/**
 * Copy the kernel reversed along its fastest-varying axis into `out`.
 *
 * The reversal is per block rather than over the whole buffer, because in 2D only
 * the *last* axis is reversed: the kernel still slides down the image row by row,
 * it is only flipped within each row. Reversing the flat array would turn a
 * convolution into a 180-degree-rotated correlation and get every tap wrong,
 * while looking correct on a symmetric kernel.
 *
 * See the note on reversed access in the file comment; this is the whole of it.
 */
static void tensor_conv_reverse(double * restrict out, const double * restrict in, zend_long nblocks, zend_long block)
{
	zend_long b, i;

	for (b = 0; b < nblocks; ++b) {
		const double * restrict src = in + b * block;
		double * restrict dst = out + b * block;

		for (i = 0; i < block; ++i) {
			dst[i] = src[block - 1 - i];
		}
	}
}

/**
 * The kernel bodies.
 *
 * TILE accumulates TENSOR_CONV_TILE consecutive outputs starting at output
 * index m0, every one of which has its whole kernel window inside the input.
 * With the kernel reversed, output m0 + u is the sum over reversed taps k of
 * va[m0 - p + k + u] * vbr[k], and both operands advance by one as u does, so
 * the innermost loop is over the tile rather than over the kernel. Its trip
 * count is a compile-time constant, so it unrolls into a block of vector
 * multiply-adds against a single broadcast tap. The caller guarantees
 * m0 >= p, which is what makes the lowest address touched, va[m0 - p],
 * non-negative.
 *
 * Note that the padding appears here only as that offset, and never as a
 * zero-valued tap. It moves where the output *starts and stops* and, with it,
 * where a given output reads from; the window a particular output sees is
 * otherwise the same window it would have seen in the full convolution, and a
 * tap that has slid past the end of the input is dropped by the caller rather
 * than contributing a zero.
 *
 * DOT is the per-output fallback. `base` is where the reversed kernel's first
 * contributing tap sits, so both operands again advance by one as j does: va[j]
 * against vbr[base + j], with the valid range already clamped by the caller.
 * Four accumulators rather than one, because a single accumulator makes the sum a
 * chain of adds each waiting on the last, and that latency rather than the
 * multiply is what this loop is paying. Four give the compiler four independent
 * chains to interleave. */
#define TENSOR_CONV_1D_TILE_BODY                                                  \
	double acc[TENSOR_CONV_TILE];                                             \
	const double * restrict base_ptr = va + m0 - p;                           \
	zend_long k, u;                                                           \
	                                                                             \
	for (u = 0; u < TENSOR_CONV_TILE; ++u) {                                   \
		acc[u] = 0.0;                                                        \
	}                                                                          \
	                                                                             \
	for (k = 0; k < nb; ++k) {                                                 \
		const double * restrict row = base_ptr + k;                          \
		const double kt = vbr[k];                                             \
	                                                                             \
		for (u = 0; u < TENSOR_CONV_TILE; ++u) {                              \
			acc[u] = TENSOR_CONV_ACC(acc[u], row[u], kt);                  \
		}                                                                      \
	}                                                                          \
	                                                                             \
	for (u = 0; u < TENSOR_CONV_TILE; ++u) {                                   \
		out[u] = acc[u];                                                      \
	}

#define TENSOR_CONV_1D_DOT_BODY                                                  \
	double a0 = 0.0, a1 = 0.0, a2 = 0.0, a3 = 0.0;                           \
	zend_long j;                                                              \
	                                                                             \
	for (j = jmin; j + 3 <= jmax; j += 4) {                                    \
		a0 = TENSOR_CONV_ACC(a0, va[j], vbr[base + j]);                      \
		a1 = TENSOR_CONV_ACC(a1, va[j + 1], vbr[base + j + 1]);              \
		a2 = TENSOR_CONV_ACC(a2, va[j + 2], vbr[base + j + 2]);              \
		a3 = TENSOR_CONV_ACC(a3, va[j + 3], vbr[base + j + 3]);              \
	}                                                                          \
	                                                                             \
	for (; j <= jmax; ++j) {                                                  \
		a0 = TENSOR_CONV_ACC(a0, va[j], vbr[base + j]);                      \
	}                                                                          \
	                                                                             \
	*out = (a0 + a1) + (a2 + a3);

#define TENSOR_CONV_2D_TILE_BODY                                                  \
	double acc[TENSOR_CONV_TILE];                                             \
	const double * restrict row = img;                                        \
	zend_long k, m, u;                                                        \
	                                                                             \
	for (u = 0; u < TENSOR_CONV_TILE; ++u) {                                   \
		acc[u] = 0.0;                                                        \
	}                                                                          \
	                                                                             \
	for (k = 0; k < mb; ++k, row -= ncol) {                                    \
		const double * restrict krow = vbr + k * nb;                         \
	                                                                             \
		for (m = 0; m < nb; ++m) {                                            \
			const double kv = krow[m];                                      \
			const double * restrict irow = row + jbase + m;                 \
	                                                                             \
			for (u = 0; u < TENSOR_CONV_TILE; ++u) {                          \
				acc[u] = TENSOR_CONV_ACC(acc[u], irow[u], kv);              \
			}                                                                  \
		}                                                                      \
	}                                                                          \
	                                                                             \
	for (u = 0; u < TENSOR_CONV_TILE; ++u) {                                   \
		out[u] = acc[u];                                                      \
	}

#define TENSOR_CONV_2D_DOT_BODY                                                  \
	double a0 = 0.0, a1 = 0.0, a2 = 0.0, a3 = 0.0;                           \
	zend_long k, m;                                                            \
	                                                                             \
	for (k = klo; k <= khi; ++k) {                                             \
		const double * restrict row = va + (xbase - k) * ncol + jcol;        \
		const double * restrict krow = vbr + k * nb;                         \
	                                                                             \
		for (m = mlo; m + 3 <= mhi; m += 4) {                                 \
			a0 = TENSOR_CONV_ACC(a0, row[m], krow[m]);                      \
			a1 = TENSOR_CONV_ACC(a1, row[m + 1], krow[m + 1]);              \
			a2 = TENSOR_CONV_ACC(a2, row[m + 2], krow[m + 2]);              \
			a3 = TENSOR_CONV_ACC(a3, row[m + 3], krow[m + 3]);              \
		}                                                                      \
	                                                                             \
		for (; m <= mhi; ++m) {                                              \
			a0 = TENSOR_CONV_ACC(a0, row[m], krow[m]);                      \
		}                                                                      \
	}                                                                          \
	                                                                             \
	*out = (a0 + a1) + (a2 + a3);

typedef void (*tensor_conv_1d_tile_fn)(double * out, const double * restrict va, const double * restrict vbr, zend_long m0, zend_long nb, zend_long p);
typedef void (*tensor_conv_1d_dot_fn)(double * out, const double * restrict va, const double * restrict vbr, zend_long jmin, zend_long jmax, zend_long base);
typedef void (*tensor_conv_2d_tile_fn)(double * out, const double * restrict img, const double * restrict vbr, zend_long ncol, zend_long jbase, zend_long mb, zend_long nb);
typedef void (*tensor_conv_2d_dot_fn)(double * out, const double * restrict va, const double * restrict vbr, zend_long ncol, zend_long xbase, zend_long jcol, zend_long nb, zend_long klo, zend_long khi, zend_long mlo, zend_long mhi);

static void tensor_conv_1d_tile_sse(double * out, const double * restrict va, const double * restrict vbr, zend_long m0, zend_long nb, zend_long p)
{
	TENSOR_CONV_1D_TILE_BODY
}

static void tensor_conv_1d_dot_sse(double * out, const double * restrict va, const double * restrict vbr, zend_long jmin, zend_long jmax, zend_long base)
{
	TENSOR_CONV_1D_DOT_BODY
}

TENSOR_TARGET_AVX
static void tensor_conv_1d_tile_avx(double * out, const double * restrict va, const double * restrict vbr, zend_long m0, zend_long nb, zend_long p)
{
	TENSOR_CONV_1D_TILE_BODY
}

TENSOR_TARGET_AVX
static void tensor_conv_1d_dot_avx(double * out, const double * restrict va, const double * restrict vbr, zend_long jmin, zend_long jmax, zend_long base)
{
	TENSOR_CONV_1D_DOT_BODY
}

static tensor_conv_1d_tile_fn tensor_conv_1d_tile_route = tensor_conv_1d_tile_sse;
static tensor_conv_1d_dot_fn tensor_conv_1d_dot_route = tensor_conv_1d_dot_sse;

static void tensor_conv_2d_tile_sse(double * out, const double * restrict img, const double * restrict vbr, zend_long ncol, zend_long jbase, zend_long mb, zend_long nb)
{
	TENSOR_CONV_2D_TILE_BODY
}

static void tensor_conv_2d_dot_sse(double * out, const double * restrict va, const double * restrict vbr, zend_long ncol, zend_long xbase, zend_long jcol, zend_long nb, zend_long klo, zend_long khi, zend_long mlo, zend_long mhi)
{
	TENSOR_CONV_2D_DOT_BODY
}

TENSOR_TARGET_AVX
static void tensor_conv_2d_tile_avx(double * out, const double * restrict img, const double * restrict vbr, zend_long ncol, zend_long jbase, zend_long mb, zend_long nb)
{
	TENSOR_CONV_2D_TILE_BODY
}

TENSOR_TARGET_AVX
static void tensor_conv_2d_dot_avx(double * out, const double * restrict va, const double * restrict vbr, zend_long ncol, zend_long xbase, zend_long jcol, zend_long nb, zend_long klo, zend_long khi, zend_long mlo, zend_long mhi)
{
	TENSOR_CONV_2D_DOT_BODY
}

static tensor_conv_2d_tile_fn tensor_conv_2d_tile_route = tensor_conv_2d_tile_sse;
static tensor_conv_2d_dot_fn tensor_conv_2d_dot_route = tensor_conv_2d_dot_sse;

/* The two FMA routes. A redefined accumulate step is the only difference between
 * these and the four above; see the note on TENSOR_CONV_ACC. */
#undef TENSOR_CONV_ACC
#define TENSOR_CONV_ACC(acc, x, y) (fma((x), (y), (acc)))

TENSOR_TARGET_FMA
static void tensor_conv_1d_tile_fma(double * out, const double * restrict va, const double * restrict vbr, zend_long m0, zend_long nb, zend_long p)
{
	TENSOR_CONV_1D_TILE_BODY
}

TENSOR_TARGET_FMA
static void tensor_conv_1d_dot_fma(double * out, const double * restrict va, const double * restrict vbr, zend_long jmin, zend_long jmax, zend_long base)
{
	TENSOR_CONV_1D_DOT_BODY
}

TENSOR_TARGET_FMA
static void tensor_conv_2d_tile_fma(double * out, const double * restrict img, const double * restrict vbr, zend_long ncol, zend_long jbase, zend_long mb, zend_long nb)
{
	TENSOR_CONV_2D_TILE_BODY
}

TENSOR_TARGET_FMA
static void tensor_conv_2d_dot_fma(double * out, const double * restrict va, const double * restrict vbr, zend_long ncol, zend_long xbase, zend_long jcol, zend_long nb, zend_long klo, zend_long khi, zend_long mlo, zend_long mhi)
{
	TENSOR_CONV_2D_DOT_BODY
}

TENSOR_TARGET_AVX512_FMA
static void tensor_conv_1d_tile_avx512(double * out, const double * restrict va, const double * restrict vbr, zend_long m0, zend_long nb, zend_long p)
{
	TENSOR_CONV_1D_TILE_BODY
}

TENSOR_TARGET_AVX512_FMA
static void tensor_conv_1d_dot_avx512(double * out, const double * restrict va, const double * restrict vbr, zend_long jmin, zend_long jmax, zend_long base)
{
	TENSOR_CONV_1D_DOT_BODY
}

TENSOR_TARGET_AVX512_FMA
static void tensor_conv_2d_tile_avx512(double * out, const double * restrict img, const double * restrict vbr, zend_long ncol, zend_long jbase, zend_long mb, zend_long nb)
{
	TENSOR_CONV_2D_TILE_BODY
}

TENSOR_TARGET_AVX512_FMA
static void tensor_conv_2d_dot_avx512(double * out, const double * restrict va, const double * restrict vbr, zend_long ncol, zend_long xbase, zend_long jcol, zend_long nb, zend_long klo, zend_long khi, zend_long mlo, zend_long mhi)
{
	TENSOR_CONV_2D_DOT_BODY
}

#undef TENSOR_CONV_ACC
#undef TENSOR_CONV_1D_TILE_BODY
#undef TENSOR_CONV_1D_DOT_BODY
#undef TENSOR_CONV_2D_TILE_BODY
#undef TENSOR_CONV_2D_DOT_BODY

/**
 * Accumulate the 1D outputs with indices in [from, to), one at a time.
 *
 * Takes a contiguous run so the traversal can name the two runs the tiles did not
 * cover without testing every output to see which run it belongs to.
 */
static void tensor_conv_1d_range(
	zend_long from, zend_long to, zend_long s, zend_long nout, zend_long na, zend_long nb, zend_long p,
	double * restrict vc, const double * restrict va, const double * restrict vbr)
{
	zend_long m;

	for (m = from; m < to; ++m) {
		/* Where this output sits in the full convolution. The padding slides the
		 * whole window down by p samples, so the p zeros it adds at the front of
		 * the input take part exactly as an input sample would, and an output
		 * that hangs off either end of the input gets a shorter window rather
		 * than being dropped. */
		zend_long i = m * s + (nb - 1) - p;
		zend_long jmin, jmax, base;

		if (UNEXPECTED(m >= nout)) {
			break;
		}

		jmin = i >= nb - 1 ? i - (nb - 1) : 0;
		jmax = i < na ? i : na - 1;

		base = nb - 1 - i;

		tensor_conv_1d_dot_route(&vc[m], va, vbr, jmin, jmax, base);
	}
}

/**
 * Accumulate the 2D outputs of row ii in the column range [from, to), one at a
 * time.
 */
static void tensor_conv_2d_range(
	zend_long ii, zend_long from, zend_long to, zend_long s,
	zend_long nrows, zend_long ncol, zend_long mb, zend_long nb,
	zend_long p, zend_long on, zend_long nout,
	double * restrict vc, const double * restrict va, const double * restrict vbr)
{
	zend_long c0 = mb > 0 ? (mb - 1) / 2 : 0;
	zend_long c1 = nb > 0 ? (nb - 1) / 2 : 0;
	zend_long jj;

	for (jj = from; jj < to; ++jj) {
		zend_long idx = ii * on + jj;
		zend_long xbase, jcol, klo, khi, mlo, mhi;

		if (UNEXPECTED(idx >= nout)) {
			break;
		}

		xbase = ii * s - p + c0;
		jcol = jj * s - p + c1 - (nb - 1);

		klo = xbase >= nrows ? xbase - (nrows - 1) : 0;
		khi = xbase < mb ? xbase : mb - 1;
		mlo = jcol < 0 ? -jcol : 0;
		mhi = nb - 1 < ncol - 1 - jcol ? nb - 1 : ncol - 1 - jcol;

		tensor_conv_2d_dot_route(&vc[idx], va, vbr, ncol, xbase, jcol, nb, klo, khi, mlo, mhi);
	}
}

/**
 * 1D convolution between a vector A and B (kernel) with a given stride and
 * `padding` zeros added to both ends of A.
 *
 * @param return_value
 * @param a
 * @param b
 * @param stride
 * @param padding
 */
void tensor_convolve_1d(zval * return_value, zval * a, zval * b, zval * stride, zval * padding)
{
	zend_long na = 0, nb = 0;
	int ok_a = 0, ok_b = 0;

	ZVAL_NULL(return_value);

	double * restrict va = tensor_tensorbuffer_doubles(a, &na, &ok_a);

	if (UNEXPECTED(!ok_a)) {
		return;
	}

	double * restrict vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);

	if (UNEXPECTED(!ok_b)) {
		return;
	}

	zend_long s = zephir_get_intval(stride);
	zend_long p = zephir_get_intval(padding);

	zend_long span = tensor_conv_add(tensor_conv_add(na, p), p) - nb;
	zend_long nout = span >= 0 ? span / s + 1 : 0;

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, nout, &c) == FAILURE)) {
		return;
	}

	double * restrict vc = zephir_buffer_doubles(&c);

	if (nout > 0) {
		double scratch[TENSOR_CONV_SCRATCH_DOUBLES];
		double * restrict vbr = scratch;
		double * heap = NULL;

		if (UNEXPECTED(nb > TENSOR_CONV_SCRATCH_DOUBLES)) {
			heap = emalloc(sizeof(double) * nb);
			vbr = heap;
		}

		tensor_conv_reverse(vbr, vb, 1, nb);

		/* Only a unit stride leaves consecutive outputs reading consecutive
		 * samples, which is the precondition for the tiles. A tile of TILE
		 * outputs starting at m0 reads va[m0 - p] through
		 * va[m0 - p + nb - 1 + TILE - 1], so its band is the set of m0 keeping
		 * that whole span inside the input, rounded to the tile grid. */
		zend_long first = tensor_conv_round_up(p, TENSOR_CONV_TILE);
		zend_long last = tensor_conv_round_down(na + p - nb - TENSOR_CONV_TILE + 1, TENSOR_CONV_TILE);

		/* The band above stops the last tile reading off the end of the input,
		 * which is all it implied while every convolution was the full one and so
		 * ran past the input by the kernel's own length. Padding removes that
		 * slack: the output can now be shorter than the input, and a last tile
		 * chosen from the input alone would write TILE outputs past the end of
		 * the buffer. Clamping the band to the output is what keeps every
		 * unchecked load preceded by one of these, both in bounds. */
		zend_long lastOut = tensor_conv_round_down(nout - TENSOR_CONV_TILE, TENSOR_CONV_TILE);

		if (last > lastOut) {
			last = lastOut;
		}

		if (s == 1 && first <= last) {
			for (zend_long m0 = first; m0 <= last; m0 += TENSOR_CONV_TILE) {
				tensor_conv_1d_tile_route(vc + m0, va, vbr, m0, nb, p);
			}

			tensor_conv_1d_range(0, first, s, nout, na, nb, p, vc, va, vbr);
			tensor_conv_1d_range(last + TENSOR_CONV_TILE, nout, s, nout, na, nb, p, vc, va, vbr);
		} else {
			tensor_conv_1d_range(0, nout, s, nout, na, nb, p, vc, va, vbr);
		}

		if (UNEXPECTED(heap != NULL)) {
			efree(heap);
		}
	}

	zval_ptr_dtor(&c);
}

/**
 * 2D convolution between a matrix A and B (kernel) with a given stride and
 * `padding` zeros added to all four sides of A.
 *
 * @param return_value
 * @param a
 * @param b
 * @param stride
 * @param padding
 * @param ma
 * @param na
 * @param mb
 * @param nb
 */
void tensor_convolve_2d(zval * return_value, zval * a, zval * b, zval * stride, zval * padding, zval * ma, zval * na, zval * mb, zval * nb)
{
	zend_long nbufa = 0, nbufb = 0;
	int ok_a = 0, ok_b = 0;

	ZVAL_NULL(return_value);

	zend_long s = zephir_get_intval(stride);
	zend_long p = zephir_get_intval(padding);
	zend_long ma_ = zephir_get_intval(ma);
	zend_long na_ = zephir_get_intval(na);
	zend_long mb_ = zephir_get_intval(mb);
	zend_long nb_ = zephir_get_intval(nb);

	double * restrict va = tensor_tensorbuffer_doubles(a, &nbufa, &ok_a);

	if (UNEXPECTED(!ok_a)) {
		return;
	}

	double * restrict vb = tensor_tensorbuffer_doubles(b, &nbufb, &ok_b);

	if (UNEXPECTED(!ok_b)) {
		return;
	}

	zend_long span_m = tensor_conv_add(tensor_conv_add(ma_, p), p) - mb_;
	zend_long span_n = tensor_conv_add(tensor_conv_add(na_, p), p) - nb_;

	zend_long om = span_m >= 0 ? span_m / s + 1 : 0;
	zend_long on = span_n >= 0 ? span_n / s + 1 : 0;

	zend_long nout = om != 0 && on > ZEND_LONG_MAX / om ? ZEND_LONG_MAX : om * on;

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, nout, &c) == FAILURE)) {
		return;
	}

	double * restrict vc = zephir_buffer_doubles(&c);

	if (mb_ == 0 || nb_ == 0) {
		zend_long i;

		for (i = 0; i < nout; ++i) {
			vc[i] = 0.0;
		}
	} else if (nout > 0) {
		zend_long ksize = mb_ * nb_;
		double scratch[TENSOR_CONV_SCRATCH_DOUBLES];
		double * restrict vbr = scratch;
		double * heap = NULL;

		if (UNEXPECTED(ksize > TENSOR_CONV_SCRATCH_DOUBLES)) {
			heap = emalloc(sizeof(double) * ksize);
			vbr = heap;
		}

		tensor_conv_reverse(vbr, vb, mb_, nb_);

		zend_long i_first = 0, i_last = -1, j_first = 0, j_last = -1;
		zend_long c0 = mb_ > 0 ? (mb_ - 1) / 2 : 0;
		zend_long c1 = nb_ > 0 ? (nb_ - 1) / 2 : 0;
		zend_long o0 = mb_ - 1 - c0;
		zend_long o1 = nb_ - 1 - c1;

		if (s == 1) {
			i_first = tensor_conv_round_up(p + o0, TENSOR_CONV_TILE);
			i_last = tensor_conv_round_down(ma_ - 1 + p - c0, TENSOR_CONV_TILE);
			j_first = tensor_conv_round_up(p + o1, TENSOR_CONV_TILE);
			j_last = tensor_conv_round_down(na_ + p - c1 - TENSOR_CONV_TILE, TENSOR_CONV_TILE);

			zend_long j_lastOut = tensor_conv_round_down(on - TENSOR_CONV_TILE, TENSOR_CONV_TILE);

			if (j_last > j_lastOut) {
				j_last = j_lastOut;
			}
		}

		int col_tiled = (s == 1) && j_first <= j_last;

		for (zend_long ii = 0; ii < om; ++ii) {
			if (EXPECTED(col_tiled && ii >= i_first && ii <= i_last)) {
				const double * restrict img = va + (ii - p + c0) * na_;

				tensor_conv_2d_range(ii, 0, j_first, s,
					ma_, na_, mb_, nb_, p, on, nout, vc, va, vbr);

				for (zend_long jj = j_first; jj <= j_last; jj += TENSOR_CONV_TILE) {
					tensor_conv_2d_tile_route(vc + ii * on + jj, img, vbr, na_,
						jj - p + c1 - (nb_ - 1), mb_, nb_);
				}

				tensor_conv_2d_range(ii, j_last + TENSOR_CONV_TILE, on, s,
					ma_, na_, mb_, nb_, p, on, nout, vc, va, vbr);
			} else {
				tensor_conv_2d_range(ii, 0, on, s,
					ma_, na_, mb_, nb_, p, on, nout, vc, va, vbr);
			}
		}

		if (UNEXPECTED(heap != NULL)) {
			efree(heap);
		}
	}

	zval_ptr_dtor(&c);
}

/**
 * Point every dispatched convolution kernel in this file back at its
 * baseline variant.
 */
void tensor_signal_processing_dispatch_sse_init(void)
{
	tensor_conv_1d_tile_route = tensor_conv_1d_tile_sse;
	tensor_conv_1d_dot_route = tensor_conv_1d_dot_sse;
	tensor_conv_2d_tile_route = tensor_conv_2d_tile_sse;
	tensor_conv_2d_dot_route = tensor_conv_2d_dot_sse;
}

/**
 * Point every dispatched convolution kernel in this file at its AVX variant.
 */
void tensor_signal_processing_dispatch_avx_init(void)
{
	tensor_conv_1d_tile_route = tensor_conv_1d_tile_avx;
	tensor_conv_1d_dot_route = tensor_conv_1d_dot_avx;
	tensor_conv_2d_tile_route = tensor_conv_2d_tile_avx;
	tensor_conv_2d_dot_route = tensor_conv_2d_dot_avx;
}

/**
 * Point every dispatched convolution kernel in this file at its FMA variant.
 */
void tensor_signal_processing_dispatch_fma_init(void)
{
	tensor_conv_1d_tile_route = tensor_conv_1d_tile_fma;
	tensor_conv_1d_dot_route = tensor_conv_1d_dot_fma;
	tensor_conv_2d_tile_route = tensor_conv_2d_tile_fma;
	tensor_conv_2d_dot_route = tensor_conv_2d_dot_fma;
}

/**
 * Point every dispatched convolution kernel in this file at its AVX-512 variant.
 */
void tensor_signal_processing_dispatch_avx512_init(void)
{
	tensor_conv_1d_tile_route = tensor_conv_1d_tile_avx512;
	tensor_conv_1d_dot_route = tensor_conv_1d_dot_avx512;
	tensor_conv_2d_tile_route = tensor_conv_2d_tile_avx512;
	tensor_conv_2d_dot_route = tensor_conv_2d_dot_avx512;
}
