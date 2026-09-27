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
 * Divide n by s, rounding the result up.
 *
 * Written as `n / s + (n % s ? 1 : 0)` rather than the usual `(n + s - 1) / s`
 * on purpose: the latter overflows zend_long for a stride near ZEND_LONG_MAX and
 * yields a length that disagrees with the number of samples the convolve loops
 * actually emit, which previously led to a write through a NULL buffer pointer.
 * This form cannot overflow because s >= 1 is enforced by the caller.
 */
static zend_long tensor_conv_ceil_div(zend_long n, zend_long s)
{
	return n / s + (n % s ? 1 : 0);
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
 * TILE accumulates TENSOR_CONV_TILE consecutive outputs of the full convolution
 * starting at output index m0, every one of which has its whole kernel window
 * inside the input. With the kernel reversed, output m0 + u is the sum over
 * reversed taps k of va[m0 - (nb - 1) + k + u] * vbr[k], and both operands
 * advance by one as u does, so the innermost loop is over the tile rather than
 * over the kernel. Its trip count is a compile-time constant, so it unrolls into
 * a block of vector multiply-adds against a single broadcast tap. The caller
 * guarantees m0 >= nb - 1, which is what makes the lowest address touched,
 * va[m0 - nb + 1], non-negative.
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
	const double * restrict base_ptr = va + m0 - (nb - 1);                    \
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

typedef void (*tensor_conv_1d_tile_fn)(double * out, const double * restrict va, const double * restrict vbr, zend_long m0, zend_long nb);
typedef void (*tensor_conv_1d_dot_fn)(double * out, const double * restrict va, const double * restrict vbr, zend_long jmin, zend_long jmax, zend_long base);
typedef void (*tensor_conv_2d_tile_fn)(double * out, const double * restrict img, const double * restrict vbr, zend_long ncol, zend_long jbase, zend_long mb, zend_long nb);
typedef void (*tensor_conv_2d_dot_fn)(double * out, const double * restrict va, const double * restrict vbr, zend_long ncol, zend_long xbase, zend_long jcol, zend_long nb, zend_long klo, zend_long khi, zend_long mlo, zend_long mhi);

static void tensor_conv_1d_tile_baseline(double * out, const double * restrict va, const double * restrict vbr, zend_long m0, zend_long nb)
{
	TENSOR_CONV_1D_TILE_BODY
}

static void tensor_conv_1d_dot_baseline(double * out, const double * restrict va, const double * restrict vbr, zend_long jmin, zend_long jmax, zend_long base)
{
	TENSOR_CONV_1D_DOT_BODY
}

TENSOR_TARGET_AVX
static void tensor_conv_1d_tile_avx(double * out, const double * restrict va, const double * restrict vbr, zend_long m0, zend_long nb)
{
	TENSOR_CONV_1D_TILE_BODY
}

TENSOR_TARGET_AVX
static void tensor_conv_1d_dot_avx(double * out, const double * restrict va, const double * restrict vbr, zend_long jmin, zend_long jmax, zend_long base)
{
	TENSOR_CONV_1D_DOT_BODY
}

static tensor_conv_1d_tile_fn tensor_conv_1d_tile_route = tensor_conv_1d_tile_baseline;
static tensor_conv_1d_dot_fn tensor_conv_1d_dot_route = tensor_conv_1d_dot_baseline;

static void tensor_conv_2d_tile_baseline(double * out, const double * restrict img, const double * restrict vbr, zend_long ncol, zend_long jbase, zend_long mb, zend_long nb)
{
	TENSOR_CONV_2D_TILE_BODY
}

static void tensor_conv_2d_dot_baseline(double * out, const double * restrict va, const double * restrict vbr, zend_long ncol, zend_long xbase, zend_long jcol, zend_long nb, zend_long klo, zend_long khi, zend_long mlo, zend_long mhi)
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

static tensor_conv_2d_tile_fn tensor_conv_2d_tile_route = tensor_conv_2d_tile_baseline;
static tensor_conv_2d_dot_fn tensor_conv_2d_dot_route = tensor_conv_2d_dot_baseline;

/* The two FMA routes. A redefined accumulate step is the only difference between
 * these and the four above; see the note on TENSOR_CONV_ACC. */
#undef TENSOR_CONV_ACC
#define TENSOR_CONV_ACC(acc, x, y) (fma((x), (y), (acc)))

TENSOR_TARGET_FMA
static void tensor_conv_1d_tile_fma(double * out, const double * restrict va, const double * restrict vbr, zend_long m0, zend_long nb)
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
static void tensor_conv_1d_tile_avx512(double * out, const double * restrict va, const double * restrict vbr, zend_long m0, zend_long nb)
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
	zend_long from, zend_long to, zend_long s, zend_long na, zend_long nb, zend_long nc,
	double * restrict vc, const double * restrict va, const double * restrict vbr)
{
	zend_long m;

	for (m = from; m < to; ++m) {
		zend_long i = m * s;
		zend_long jmin, jmax, base;

		/* The traversal below trusts that every output index it reaches is a
		 * sample of the full convolution. This is what keeps that true if the
		 * stride arithmetic above ever disagrees with the length it allocated. */
		if (UNEXPECTED(i >= nc)) {
			break;
		}

		/* The taps that land inside the input: these were two tests per element
		 * of the original inner loop, and here they are two clamps per output. */
		jmin = i >= nb - 1 ? i - (nb - 1) : 0;
		jmax = i < na ? i : na - 1;

		/* Where the reversed kernel's first contributing tap sits. */
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
	zend_long p, zend_long q, zend_long on, zend_long nout,
	double * restrict vc, const double * restrict va, const double * restrict vbr)
{
	zend_long jj;

	for (jj = from; jj < to; ++jj) {
		zend_long j = jj * s;
		zend_long idx = ii * on + jj;
		zend_long xbase, jcol, klo, khi, mlo, mhi;

		/* As in 1D: the guard that makes the write provably in bounds whatever
		 * the stride arithmetic did. */
		if (UNEXPECTED(idx >= nout)) {
			break;
		}

		/* "Same" padding puts the kernel's centre sample on the output sample, so
		 * tap k reads input row (xbase - k) and reversed tap column m reads
		 * input column (jcol + m). Both ranges below are the two tests the
		 * original inner loop made per element, hoisted to here. */
		xbase = ii * s + p;
		jcol = j + q - (nb - 1);

		klo = xbase >= nrows ? xbase - (nrows - 1) : 0;
		khi = xbase < mb ? xbase : mb - 1;
		mlo = jcol < 0 ? -jcol : 0;
		mhi = nb - 1 < ncol - 1 - jcol ? nb - 1 : ncol - 1 - jcol;

		tensor_conv_2d_dot_route(&vc[idx], va, vbr, ncol, xbase, jcol, nb, klo, khi, mlo, mhi);
	}
}

/**
 * 1D convolution between a vector A and B (kernel) with a given stride.
 *
 * @param return_value
 * @param a
 * @param b
 * @param stride
 */
void tensor_convolve_1d(zval * return_value, zval * a, zval * b, zval * stride)
{
	zend_long na = 0, nb = 0;
	int ok_a = 0, ok_b = 0;

	/* The optimizers emit this call as a statement and then pass the result
	 * straight to a constructor, so every path out of here has to leave
	 * *return_value defined even when it is about to throw. */
	ZVAL_NULL(return_value);

	double * restrict va = tensor_tensorbuffer_doubles(a, &na, &ok_a);

	/* Checked one at a time: unwrapping the second buffer while the first
	 * failure is already pending would only do work that gets discarded. */
	if (UNEXPECTED(!ok_a)) {
		return;
	}

	double * restrict vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);

	if (UNEXPECTED(!ok_b)) {
		return;
	}

	zend_long s = zephir_get_intval(stride);

	if (UNEXPECTED(s < 1)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Stride must be at least 1."));
		return;
	}

	/* Two empty buffers convolve to nothing, so guard the full-convolution
	 * length against going negative before it reaches the allocation. */
	zend_long nc = na + nb - 1;
	zend_long nout = nc > 0 ? tensor_conv_ceil_div(nc, s) : 0;

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, nout, &c) == FAILURE)) {
		return;
	}

	double * restrict vc = zephir_buffer_doubles(&c);

	if (nb == 0) {
		zend_long m;

		for (m = 0; m < nout; ++m) {
			vc[m] = 0.0;
		}
	} else if (nout > 0) {
		double scratch[TENSOR_CONV_SCRATCH_DOUBLES];
		double * restrict vbr = scratch;
		double * heap = NULL;

		if (UNEXPECTED(nb > TENSOR_CONV_SCRATCH_DOUBLES)) {
			heap = emalloc(sizeof(double) * nb);
			vbr = heap;
		}

		tensor_conv_reverse(vbr, vb, 1, nb);

		/* Only a unit stride leaves consecutive outputs reading consecutive
		 * samples, which is the precondition for the tiles. */
		zend_long first = tensor_conv_round_up(nb - 1, TENSOR_CONV_TILE);
		zend_long last = tensor_conv_round_down(na - 1 - TENSOR_CONV_TILE, TENSOR_CONV_TILE);

		/* A tile of TILE outputs starting at m0 has no padding tap at all when
		 * every one of its outputs has the whole kernel inside the input, and the
		 * kernel has to fit in the input as well, so the whole-tile outputs are
		 * the ones in [nb - 1, na - 1] whose index is a multiple of TILE.
		 *
		 * first > last is the common case, not the exceptional one: a kernel
		 * narrower than the tile, or an input narrower than it, leaves no whole
		 * tile at all. The two ranges that bracket the tiles only partition the
		 * output between them when there *are* tiles, so the no-tile case has to
		 * fall through to the per-output path for the whole result rather than
		 * relying on those two ranges to happen to meet. */
		if (s == 1 && first <= last) {
			for (zend_long m0 = first; m0 <= last; m0 += TENSOR_CONV_TILE) {
				tensor_conv_1d_tile_route(vc + m0, va, vbr, m0, nb);
			}

			tensor_conv_1d_range(0, first, s, na, nb, nc, vc, va, vbr);
			tensor_conv_1d_range(last + TENSOR_CONV_TILE, nout, s, na, nb, nc, vc, va, vbr);
		} else {
			tensor_conv_1d_range(0, nout, s, na, nb, nc, vc, va, vbr);
		}

		if (UNEXPECTED(heap != NULL)) {
			efree(heap);
		}
	}

	zval_ptr_dtor(&c);
}

/**
 * 2D convolution between a matrix A and B (kernel) with a given stride using the "same" method for zero padding.
 *
 * @param return_value
 * @param a
 * @param b
 * @param stride
 * @param ma
 * @param na
 * @param mb
 * @param nb
 */
void tensor_convolve_2d(zval * return_value, zval * a, zval * b, zval * stride, zval * ma, zval * na, zval * mb, zval * nb)
{
	zend_long nbufa = 0, nbufb = 0;
	int ok_a = 0, ok_b = 0;

	ZVAL_NULL(return_value);

	zend_long s = zephir_get_intval(stride);
	zend_long ma_ = zephir_get_intval(ma);
	zend_long na_ = zephir_get_intval(na);
	zend_long mb_ = zephir_get_intval(mb);
	zend_long nb_ = zephir_get_intval(nb);

	if (UNEXPECTED(s < 1)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Stride must be at least 1."));
		return;
	}

	double * restrict va = tensor_tensorbuffer_doubles(a, &nbufa, &ok_a);

	if (UNEXPECTED(!ok_a)) {
		return;
	}

	double * restrict vb = tensor_tensorbuffer_doubles(b, &nbufb, &ok_b);

	if (UNEXPECTED(!ok_b)) {
		return;
	}

	if (UNEXPECTED(ma_ < 0 || na_ < 0 || mb_ < 0 || nb_ < 0)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Dimensions must be non-negative."));
		return;
	}

	if (UNEXPECTED(nbufa != ma_ * na_ || nbufb != mb_ * nb_)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Input buffers must match the given dimensions."));
		return;
	}

	/* "Same" padding anchors the kernel's centre sample on the output sample.
	 * Using (n - 1) / 2 rather than n / 2 keeps even-sized kernels aligned the
	 * way numpy and scipy's mode='same' align them. */
	zend_long p = mb_ > 0 ? (mb_ - 1) / 2 : 0;
	zend_long q = nb_ > 0 ? (nb_ - 1) / 2 : 0;

	zend_long om = ma_ > 0 ? tensor_conv_ceil_div(ma_, s) : 0;
	zend_long on = na_ > 0 ? tensor_conv_ceil_div(na_, s) : 0;

	/* om <= ma_ and on <= na_ for s >= 1, so this product cannot overflow and
	 * is bounded by the input size. */
	zend_long nout = om * on;

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

		/* See tensor_convolve_1d: the tiles need a unit stride. */
		zend_long i_first = 0, i_last = -1, j_first = 0, j_last = -1;

		if (s == 1) {
			/* A whole tile of TILE output columns starting at jj0 needs every
			 * one of its TILE * nb_ taps to land inside the input, in both
			 * directions: the first output column must sit past the kernel's
			 * left overhang and the last must sit short of the right one. The
			 * same argument on the rows gives the row bounds, and each is
			 * rounded to the tile grid so the tiles neither overlap nor run off
			 * either end. The last tile also has to fit inside the output row,
			 * which the -TILE in j_last is what guarantees. */
			i_first = tensor_conv_round_up(mb_ - 1 - p, TENSOR_CONV_TILE);
			i_last = tensor_conv_round_down(ma_ - 1 - p, TENSOR_CONV_TILE);
			j_first = tensor_conv_round_up(nb_ - 1 - q, TENSOR_CONV_TILE);
			j_last = tensor_conv_round_down(na_ - 1 - TENSOR_CONV_TILE - q, TENSOR_CONV_TILE);
		}

		/* The two bounds are independent, so a row can be in the tiled row band
		 * while no whole column tile exists at all -- and again that is the common
		 * case, since a kernel or an input narrower than the tile produces
		 * neither. The column test has to be made first, and the per-output path
		 * has to cover a whole row whenever it fails: the two ranges bracketing
		 * the column tiles only partition the row between them when there are
		 * tiles to put between them. */
		int col_tiled = (s == 1) && j_first <= j_last;

		for (zend_long ii = 0; ii < om; ++ii) {
			if (EXPECTED(col_tiled && ii >= i_first && ii <= i_last)) {
				/* The image row the tile kernel starts from: the first image row
				 * the kernel's topmost tap reads. It walks the rows downward,
				 * because kernel row k reads image row (ii + p - k) and the
				 * reversed kernel leaves the row index alone. */
				const double * restrict img = va + (ii + p) * na_;

				tensor_conv_2d_range(ii, 0, j_first, s,
					ma_, na_, mb_, nb_, p, q, on, nout, vc, va, vbr);

				for (zend_long jj = j_first; jj <= j_last; jj += TENSOR_CONV_TILE) {
					tensor_conv_2d_tile_route(vc + ii * on + jj, img, vbr, na_,
						jj + q - (nb_ - 1), mb_, nb_);
				}

				tensor_conv_2d_range(ii, j_last + TENSOR_CONV_TILE, on, s,
					ma_, na_, mb_, nb_, p, q, on, nout, vc, va, vbr);
			} else {
				tensor_conv_2d_range(ii, 0, on, s,
					ma_, na_, mb_, nb_, p, q, on, nout, vc, va, vbr);
			}
		}

		if (UNEXPECTED(heap != NULL)) {
			efree(heap);
		}
	}

	zval_ptr_dtor(&c);
}

void tensor_signal_processing_dispatch_avx_init(void)
{
	tensor_conv_1d_tile_route = tensor_conv_1d_tile_avx;
	tensor_conv_1d_dot_route = tensor_conv_1d_dot_avx;
	tensor_conv_2d_tile_route = tensor_conv_2d_tile_avx;
	tensor_conv_2d_dot_route = tensor_conv_2d_dot_avx;
}

void tensor_signal_processing_dispatch_fma_init(void)
{
	tensor_conv_1d_tile_route = tensor_conv_1d_tile_fma;
	tensor_conv_1d_dot_route = tensor_conv_1d_dot_fma;
	tensor_conv_2d_tile_route = tensor_conv_2d_tile_fma;
	tensor_conv_2d_dot_route = tensor_conv_2d_dot_fma;
}

void tensor_signal_processing_dispatch_avx512_init(void)
{
	tensor_conv_1d_tile_route = tensor_conv_1d_tile_avx512;
	tensor_conv_1d_dot_route = tensor_conv_1d_dot_avx512;
	tensor_conv_2d_tile_route = tensor_conv_2d_tile_avx512;
	tensor_conv_2d_dot_route = tensor_conv_2d_dot_avx512;
}
