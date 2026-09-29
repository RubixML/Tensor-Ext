#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <ext/spl/spl_exceptions.h>
#include <cblas.h>
#include <lapacke.h>
#include "kernel/operators.h"
#include "php_ext.h"
#include "kernel/buffer.h"
#include "include/buffer.h"
#include "include/dispatch.h"
#include "include/reductions.h"

/**
 * Dispatched row kernels for the row reductions and the outer product.
 *
 * The reasoning behind the three routes is the same as for the elementwise
 * kernels in include/arithmetic.c -- the extension is built with no ISA flags,
 * so the baseline tops out at the SSE2 width of two doubles per vector, and the
 * wider kernels are only ever reached through a pointer that cpu.c installs once
 * it has found the CPU to support the ISA (see the note in include/dispatch.h).
 * The shape is different in one respect: these are not whole zval entry points
 * but the innermost loop of a larger algorithm, so the route pointer replaces
 * the loop and the caller keeps its own control flow.
 *
 * Each body is a macro expanded once per route, so the copies cannot drift.
 *
 * Every body takes restrict pointers, and that is what makes the loops vectorize
 * at all. Each reads and writes `w`, a private scratch block copied out of the
 * caller's buffer, through one `double *`, so with no restrict the vectorizer
 * had to assume the store could collide with the load and bailed out.
 *
 * The accumulate step of the row update is the only difference between the FMA
 * routes and the rest. `fma()` is spelled out on those and nowhere else, for the
 * reason given in include/signal_processing.c: on the baseline route the
 * compiler would emit a call to fma() for every element, which is far slower
 * than the two-rounded multiply and subtract. The two roundings it collapses
 * differ by at most one unit in the last place, which is several orders of
 * magnitude below the tolerance the tests assert against.
 */

/* dst[j] -= alpha * src[j] */
#define TENSOR_ROW_UPDATE_BODY                                                    \
	unsigned int j;                                                               \
	                                                                               \
	for (j = 0; j < len; ++j) {                                                  \
		dst[j] = TENSOR_ROW_UPDATE_ACC(alpha, src[j], dst[j]);                    \
	}

#define TENSOR_ROW_UPDATE_ACC(a, x, acc) ((acc) - (a) * (x))

/* row[j] /= pivot. A division cannot be fused, so there is no FMA route for
 * this one and `1 / pivot` is deliberately not used to make one -- that would
 * change the result. */
#define TENSOR_ROW_SCALE_BODY                                                     \
	unsigned int j;                                                               \
	                                                                               \
	for (j = 0; j < len; ++j) {                                                  \
		row[j] /= pivot;                                                          \
	}

/* vc = va (x) vb, i.e. every row of the product is one element of va broadcast
 * across vb. */
#define TENSOR_OUTER_FILL_BODY                                                    \
	unsigned int i, j;                                                            \
	                                                                               \
	for (i = 0; i < na; ++i) {                                                   \
		const double a = va[i];                                                  \
		double * out = vc + i * nb;                                              \
	                                                                               \
		for (j = 0; j < nb; ++j) {                                               \
			out[j] = a * vb[j];                                                  \
		}                                                                          \
	}

typedef void (*tensor_row_update_fn)(double * restrict dst, const double * restrict src, double alpha, unsigned int len);
typedef void (*tensor_row_scale_fn)(double * restrict row, double pivot, unsigned int len);
typedef void (*tensor_outer_fill_fn)(double * restrict vc, const double * restrict va, const double * restrict vb, unsigned int na, unsigned int nb);

#define TENSOR_ROW_UPDATE_BASELINE(name)                                           \
	static void name##_baseline(double * restrict dst, const double * restrict src, double alpha, unsigned int len) \
	{                                                                             \
		TENSOR_ROW_UPDATE_BODY                                                    \
	}

#define TENSOR_ROW_UPDATE_ROUTE(name)                                              \
	TENSOR_TARGET_AVX                                                             \
	static void name##_avx(double * restrict dst, const double * restrict src, double alpha, unsigned int len) \
	{                                                                             \
		TENSOR_ROW_UPDATE_BODY                                                    \
	}                                                                             \
	                                                                               \
	TENSOR_TARGET_AVX512_FMA                                                     \
	static void name##_avx512(double * restrict dst, const double * restrict src, double alpha, unsigned int len) \
	{                                                                             \
		TENSOR_ROW_UPDATE_BODY                                                    \
	}

TENSOR_ROW_UPDATE_BASELINE(tensor_row_update)
TENSOR_ROW_UPDATE_ROUTE(tensor_row_update)

static tensor_row_update_fn tensor_row_update_route = tensor_row_update_baseline;

/* The FMA route is a separate function rather than a separate route pointer: a
 * route can only point at one variant, and cpu.c picks exactly one of the three
 * initializers, so the FMA variant is installed in place of the plain AVX one. */
#undef TENSOR_ROW_UPDATE_ACC
#define TENSOR_ROW_UPDATE_ACC(a, x, acc) fma(-(a), (x), (acc))

TENSOR_TARGET_FMA
static void tensor_row_update_fma(double * restrict dst, const double * restrict src, double alpha, unsigned int len)
{
	TENSOR_ROW_UPDATE_BODY
}

#define TENSOR_ROW_SCALE_BASELINE(name)                                            \
	static void name##_baseline(double * restrict row, double pivot, unsigned int len) \
	{                                                                             \
		TENSOR_ROW_SCALE_BODY                                                     \
	}

TENSOR_ROW_SCALE_BASELINE(tensor_row_scale)

TENSOR_TARGET_AVX
static void tensor_row_scale_avx(double * restrict row, double pivot, unsigned int len)
{
	TENSOR_ROW_SCALE_BODY
}

TENSOR_TARGET_AVX512
static void tensor_row_scale_avx512(double * restrict row, double pivot, unsigned int len)
{
	TENSOR_ROW_SCALE_BODY
}

static tensor_row_scale_fn tensor_row_scale_route = tensor_row_scale_baseline;

#define TENSOR_OUTER_FILL_BASELINE(name)                                           \
	static void name##_baseline(double * restrict vc, const double * restrict va, const double * restrict vb, unsigned int na, unsigned int nb) \
	{                                                                             \
		TENSOR_OUTER_FILL_BODY                                                    \
	}

TENSOR_OUTER_FILL_BASELINE(tensor_outer_fill)

TENSOR_TARGET_AVX
static void tensor_outer_fill_avx(double * restrict vc, const double * restrict va, const double * restrict vb, unsigned int na, unsigned int nb)
{
	TENSOR_OUTER_FILL_BODY
}

TENSOR_TARGET_AVX512
static void tensor_outer_fill_avx512(double * restrict vc, const double * restrict va, const double * restrict vb, unsigned int na, unsigned int nb)
{
	TENSOR_OUTER_FILL_BODY
}

static tensor_outer_fill_fn tensor_outer_fill_route = tensor_outer_fill_baseline;

/**
 * Matrix-matrix multiplication i.e. linear transformation of matrices A and B.
 * 
 * @param return_value
 * @param a
 * @param b
 * @param m
 * @param p
 * @param n
 */
void tensor_matmul(zval * return_value, zval * a, zval * b, zval * m, zval * p, zval * n)
{
    zend_long i;
    zend_long ma = zephir_get_intval(m);
    zend_long pa = zephir_get_intval(p);
    zend_long nb = zephir_get_intval(n);
    zend_long na = 0, nbb = 0;
    int ok_a = 0, ok_b = 0;

    double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);
    double * vb = tensor_tensorbuffer_doubles(b, &nbb, &ok_b);

    if (UNEXPECTED(!ok_a || !ok_b)) {
        return;
    }

    zval c;

    if (UNEXPECTED(tensor_tensorbuffer_create(return_value, ma * nb, &c) == FAILURE)) {
        return;
    }

    double * vc = zephir_buffer_doubles(&c);

    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, ma, nb, pa, 1.0, va, pa, vb, nb, 0.0, vc, nb);

    zval_ptr_dtor(&c);
}

/**
 * Matrix-vector product i.e. the dot product of matrix A and vector B.
 *
 * @param return_value
 * @param a
 * @param b
 * @param m
 * @param p
 */
void tensor_matrix_dot(zval * return_value, zval * a, zval * b, zval * m, zval * p)
{
    zend_long ma = zephir_get_intval(m);
    zend_long pc = zephir_get_intval(p);
    zend_long na = 0, nb = 0;
    int ok_a = 0, ok_b = 0;

    double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);
    double * vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);

    if (UNEXPECTED(!ok_a || !ok_b)) {
        return;
    }

    zval c;

    if (UNEXPECTED(tensor_tensorbuffer_create(return_value, ma, &c) == FAILURE)) {
        return;
    }

    double * vc = zephir_buffer_doubles(&c);

    cblas_dgemv(CblasRowMajor, CblasNoTrans, (blasint) ma, (blasint) pc, 1.0, va, (blasint) pc, vb, 1, 0.0, vc, 1);

    zval_ptr_dtor(&c);
}

/**
 * Dot product between vectors A and B.
 * 
 * @param return_value
 * @param a
 * @param b
 */
void tensor_dot(zval * return_value, zval * a, zval * b)
{
	zend_long na = 0, nb = 0;
	int ok_a = 0, ok_b = 0;

	double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);
	double * vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);

	if (UNEXPECTED(!ok_a || !ok_b)) {
		return;
	}

	RETVAL_DOUBLE(cblas_ddot((blasint) na, va, 1, vb, 1));
}

/**
 * Return the multiplicative inverse of a square matrix A.
 *
 * @param return_value
 * @param a
 * @param n
 */
void tensor_inverse(zval * return_value, zval * a, zval * n)
{
    zend_long i;
    zend_long nn = zephir_get_intval(n);
    zend_long na = 0;
    int ok_a = 0;

    double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    double * w = emalloc(na * sizeof(double));
    int * pivots = emalloc(nn * sizeof(int));

    for (i = 0; i < na; ++i) {
        w[i] = va[i];
    }

    lapack_int status;

    status = LAPACKE_dgetrf(LAPACK_ROW_MAJOR, nn, nn, w, nn, pivots);

    if (status != 0) {
        efree(w);
        efree(pivots);

        RETURN_NULL();
    }

    status = LAPACKE_dgetri(LAPACK_ROW_MAJOR, nn, w, nn, pivots);

    if (status != 0) {
        efree(w);
        efree(pivots);

        RETURN_NULL();
    }

    zval c;

    if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na, &c) == FAILURE)) {
        efree(w);
        efree(pivots);

        return;
    }

    double * vc = zephir_buffer_doubles(&c);

    for (i = 0; i < na; ++i) {
        vc[i] = w[i];
    }

    zval_ptr_dtor(&c);

    efree(w);
    efree(pivots);
}

/**
 * Return the tolerance below which a value of an m-by-n matrix is treated as
 * zero, given the largest of them in `scale`.
 * 
 * @param m
 * @param n
 * @param scale
 * @return double
 */
static double tensor_relative_tolerance(unsigned int m, unsigned int n, double scale)
{
    return (double) MAX(m, n) * DBL_EPSILON * scale;
}

/**
 * Return the largest absolute value among the first `count` elements of `a`,
 * which is the scale a tolerance is measured against.
 *
 * @param a
 * @param count
 * @return double
 */
static double tensor_absolute_max(const double * a, zend_long count)
{
    zend_long i;

    double max = 0.0;

    for (i = 0; i < count; ++i) {
        double v = fabs(a[i]);

        if (v > max) {
            max = v;
        }
    }

    return max;
}

/**
 * Return the (Moore-Penrose) pseudoinverse of a general matrix A.
 * 
 * @param return_value
 * @param a
 * @param m
 * @param n
 */
void tensor_pseudoinverse(zval * return_value, zval * a, zval * m, zval * n)
{
    zend_long i;
    zend_long ma = zephir_get_intval(m);
    zend_long na = zephir_get_intval(n);
    zend_long nbuf = 0;
    int ok_a = 0;

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    unsigned int k = MIN(ma, na);

    double * w = emalloc(nbuf * sizeof(double));
    double * vu = safe_emalloc((size_t) ma * (size_t) ma, sizeof(double), 0);
    double * vs = emalloc(k * sizeof(double));
    double * vvt = safe_emalloc((size_t) na * (size_t) na, sizeof(double), 0);
    double * vb = safe_emalloc((size_t) na * (size_t) ma, sizeof(double), 0);

    for (i = 0; i < nbuf; ++i) {
        w[i] = va[i];
    }

    lapack_int status = LAPACKE_dgesdd(LAPACK_ROW_MAJOR, 'A', ma, na, w, na, vs, vu, ma, vvt, na);

    if (status != 0) {
        efree(w);
        efree(vu);
        efree(vs);
        efree(vvt);
        efree(vb);

        RETURN_NULL();
    }

    double tolerance = tensor_relative_tolerance(ma, na, vs[0]);

    for (i = 0; i < k; ++i) {
        cblas_dscal(ma, vs[i] > tolerance ? 1.0 / vs[i] : 0.0, &vu[i], ma);
    }

    cblas_dgemm(CblasRowMajor, CblasTrans, CblasTrans, na, ma, k, 1.0, vvt, na, vu, ma, 0.0, vb, ma);

    zval c;

    if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na * ma, &c) == FAILURE)) {
        efree(w);
        efree(vu);
        efree(vs);
        efree(vvt);
        efree(vb);

        return;
    }

    double * vc = zephir_buffer_doubles(&c);

    for (i = 0; i < na * ma; ++i) {
        vc[i] = vb[i];
    }

    zval_ptr_dtor(&c);

    efree(w);
    efree(vu);
    efree(vs);
    efree(vvt);
    efree(vb);
}

/**
 * Reduce a (possibly singular) row echelon matrix stored in `w` in place,
 * mirroring the pure-PHP row reduction path. Pivot rows are not normalized so
 * the output matches the non-singular (LAPACK dgetrf) path. An entry counts as
 * a pivot only if it is strictly above `tolerance`; returns the number of row
 * swaps performed.
 *
 * The comparison is strict because the tolerance of a matrix of zeros is zero,
 * and a threshold that only rejects what is *below* it would find every entry of
 * the zero matrix above it. Treating the tolerance as the largest value that is
 * still zero -- the reading `tensor_pseudoinverse` already gives a singular
 * value -- leaves a zero matrix with no pivots and hence a rank of zero, which
 * is what it is.
 *
 * @param w
 * @param m
 * @param n
 * @param tolerance
 * @return long
 */
static long tensor_ref_singular(double * w, unsigned int m, unsigned int n, double tolerance)
{
    unsigned int i, j;

    double pivot, scale, tmp;
    unsigned int r = 0;
    unsigned int c = 0;
    long swaps = 0;

    while (r < m && c < n) {
        double * pivotRow = w + r * n;

        if (fabs(pivotRow[c]) <= tolerance) {
            for (i = r + 1; i < m; ++i) {
                if (fabs(w[i * n + c]) > tolerance) {
                    for (j = 0; j < n; ++j) {
                        tmp = pivotRow[j];
                        pivotRow[j] = w[i * n + j];
                        w[i * n + j] = tmp;
                    }

                    ++swaps;

                    break;
                }
            }
        }

        if (fabs(pivotRow[c]) <= tolerance) {
            ++c;

            continue;
        }

        pivot = pivotRow[c];

        for (i = r + 1; i < m; ++i) {
            scale = w[i * n + c] / pivot;

            if (fabs(scale) > tolerance) {
                tensor_row_update_route(w + i * n, pivotRow, scale, n);
            }
        }

        ++r;
        ++c;
    }

    return swaps;
}

/**
 * Bring a matrix in row-echelon form on a scratch buffer in place, mirroring
 * the pure-PHP forward elimination path, with a pivot taken to be one that is
 * above `tolerance`. Returns the number of row swaps and sets `*status` to the
 * LAPACK result (negative on failure).
 */
static long tensor_ref_step(double * w, const double * orig, unsigned int m, unsigned int n, double tolerance, lapack_int * status);

/**
 * Compute the row echelon form (REF) of matrix A, reading A out of a
 * TensorBuffer, and return a tuple with the reduced matrix (as a TensorBuffer)
 * and the number of row swaps performed.
 *
 * @param return_value
 * @param a
 * @param m
 * @param n
 */
void tensor_ref(zval * return_value, zval * a, zval * m, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;
    unsigned int i;

    unsigned int ma = (unsigned int) zephir_get_intval(m);
    unsigned int na = (unsigned int) zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    double * w = safe_emalloc((size_t) ma * (size_t) na, sizeof(double), 0);

    for (i = 0; i < ma * na; ++i) {
        w[i] = va[i];
    }

    lapack_int status;

    long swaps = tensor_ref_step(
        w,
        va,
        ma,
        na,
        tensor_relative_tolerance(ma, na, tensor_absolute_max(va, nbuf)),
        &status
    );

    if (status < 0) {
        efree(w);

        RETURN_NULL();
    }

    zval matrix, buf;

    if (UNEXPECTED(tensor_tensorbuffer_create(&matrix, (zend_long) ma * na, &buf) == FAILURE)) {
        efree(w);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&buf);

        for (i = 0; i < (zend_ulong)(ma * na); ++i) {
            vc[i] = w[i];
        }
    }

    zval_ptr_dtor(&buf);

    zval tuple;

    array_init_size(&tuple, 2);

    add_next_index_zval(&tuple, &matrix);
    add_next_index_long(&tuple, swaps);

    RETVAL_ARR(Z_ARR(tuple));

    efree(w);
}

/**
 * Compute the Cholesky decomposition of matrix A (read from a TensorBuffer)
 * and return the lower triangular matrix as a TensorBuffer.
 *
 * @param return_value
 * @param a
 * @param n
 */
void tensor_cholesky(zval * return_value, zval * a, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;
    unsigned int i, j;

    unsigned int na = (unsigned int) zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    double * w = safe_emalloc((size_t) na * (size_t) na, sizeof(double), 0);

    for (i = 0; i < na * na; ++i) {
        w[i] = va[i];
    }

    lapack_int status = LAPACKE_dpotrf(LAPACK_ROW_MAJOR, 'L', na, w, na);

    if (status != 0) {
        efree(w);

        RETURN_NULL();
    }

    /* zero the upper triangle so the result is a proper lower triangular matrix */
    for (i = 0; i < na; ++i) {
        for (j = i + 1; j < na; ++j) {
            w[i * na + j] = 0.0;
        }
    }

    zval l, buf;

    if (UNEXPECTED(tensor_tensorbuffer_create(&l, (zend_long) na * na, &buf) == FAILURE)) {
        efree(w);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&buf);

        for (i = 0; i < (zend_ulong)(na * na); ++i) {
            vc[i] = w[i];
        }
    }

    zval_ptr_dtor(&buf);

    *return_value = l;

    efree(w);
}

/**
 * Compute the LU factorization of matrix A (read from a TensorBuffer) and
 * return a tuple with the lower, upper, and permutation matrices (each as a
 * TensorBuffer).
 *
 * @param return_value
 * @param a
 * @param n
 */
void tensor_lu(zval * return_value, zval * a, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;
    unsigned int i, j;

    unsigned int na = (unsigned int) zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    unsigned int * perm;
    double * va_ = safe_emalloc((size_t) na * (size_t) na, sizeof(double), 0);
    int * pivots = emalloc(na * sizeof(int));

    for (i = 0; i < na * na; ++i) {
        va_[i] = va[i];
    }

    lapack_int status = LAPACKE_dgetrf(LAPACK_ROW_MAJOR, na, na, va_, na, pivots);

    if (status != 0) {
        efree(va_);
        efree(pivots);

        RETURN_NULL();
    }

    double * lbuf = safe_emalloc((size_t) na * (size_t) na, sizeof(double), 0);
    double * ubuf = safe_emalloc((size_t) na * (size_t) na, sizeof(double), 0);
    double * pbuf = safe_emalloc((size_t) na * (size_t) na, sizeof(double), 0);

    for (i = 0; i < na; ++i) {
        for (j = 0; j < i; ++j) {
            lbuf[i * na + j] = va_[i * na + j];
        }

        lbuf[i * na + i] = 1.0;

        for (j = i + 1; j < na; ++j) {
            lbuf[i * na + j] = 0.0;
        }

        for (j = 0; j < i; ++j) {
            ubuf[i * na + j] = 0.0;
        }

        for (j = i; j < na; ++j) {
            ubuf[i * na + j] = va_[i * na + j];
        }
    }

    perm = emalloc(na * sizeof(unsigned int));

    for (i = 0; i < na; ++i) {
        perm[i] = i;
    }

    for (i = 0; i < na; ++i) {
        unsigned int r = (unsigned int)(pivots[i] - 1);

        if (r != i) {
            unsigned int t = perm[i];

            perm[i] = perm[r];
            perm[r] = t;
        }
    }

    for (i = 0; i < na; ++i) {
        for (j = 0; j < na; ++j) {
            pbuf[i * na + j] = (j == perm[i]) ? 1.0 : 0.0;
        }
    }

    zval l, u, p, tuple;
    zval bufL, bufU, bufP;

    if (UNEXPECTED(tensor_tensorbuffer_create(&l, (zend_long) na * na, &bufL) == FAILURE)) {
        efree(perm);
        efree(lbuf);
        efree(ubuf);
        efree(pbuf);
        efree(va_);
        efree(pivots);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&bufL);

        for (i = 0; i < (zend_ulong)(na * na); ++i) {
            vc[i] = lbuf[i];
        }
    }

    zval_ptr_dtor(&bufL);

    if (UNEXPECTED(tensor_tensorbuffer_create(&u, (zend_long) na * na, &bufU) == FAILURE)) {
        zval_ptr_dtor(&l);
        
        efree(perm);
        efree(lbuf);
        efree(ubuf);
        efree(pbuf);
        efree(va_);
        efree(pivots);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&bufU);

        for (i = 0; i < (zend_ulong)(na * na); ++i) {
            vc[i] = ubuf[i];
        }
    }

    zval_ptr_dtor(&bufU);

    if (UNEXPECTED(tensor_tensorbuffer_create(&p, (zend_long) na * na, &bufP) == FAILURE)) {
        zval_ptr_dtor(&l);
        zval_ptr_dtor(&u);

        efree(perm);
        efree(lbuf);
        efree(ubuf);
        efree(pbuf);
        efree(va_);
        efree(pivots);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&bufP);

        for (i = 0; i < (zend_ulong)(na * na); ++i) {
            vc[i] = pbuf[i];
        }
    }

    zval_ptr_dtor(&bufP);

    array_init_size(&tuple, 3);

    add_next_index_zval(&tuple, &l);
    add_next_index_zval(&tuple, &u);
    add_next_index_zval(&tuple, &p);

    RETVAL_ARR(Z_ARR(tuple));

    efree(perm);
    efree(lbuf);
    efree(ubuf);
    efree(pbuf);
    efree(va_);
    efree(pivots);
}

/**
 * Compute the eigendecomposition of a general matrix A and return the real part of the
 * eigenvalues (TensorBuffer), the imaginary part of the eigenvalues (TensorBuffer), and
 * the eigenvectors (TensorBuffer) in a 3-element tuple, in that order. For a complex
 * conjugate pair the two eigenvector columns are the real and imaginary parts of a
 * single complex eigenvector.
 *
 * @param return_value
 * @param a
 */
void tensor_eig(zval * return_value, zval * a, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;
    unsigned int i;

    unsigned int na = (unsigned int) zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    double * w = safe_emalloc((size_t) na * (size_t) na, sizeof(double), 0);
    double * wr = emalloc(na * sizeof(double));
    double * wi = emalloc(na * sizeof(double));
    double * vr = safe_emalloc((size_t) na * (size_t) na, sizeof(double), 0);

    for (i = 0; i < na * na; ++i) {
        w[i] = va[i];
    }

    lapack_int status = LAPACKE_dgeev(LAPACK_ROW_MAJOR, 'N', 'V', na, w, na, wr, wi, NULL, na, vr, na);

    if (status != 0) {
        efree(w);
        efree(wr);
        efree(wi);
        efree(vr);

        RETURN_NULL();
    }

    zval eigenvalues, bufRe;

    if (UNEXPECTED(tensor_tensorbuffer_create(&eigenvalues, (zend_long) na, &bufRe) == FAILURE)) {
        efree(w);
        efree(wr);
        efree(wi);
        efree(vr);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&bufRe);

        for (i = 0; i < na; ++i) {
            vc[i] = wr[i];
        }
    }

    zval_ptr_dtor(&bufRe);

    zval eigenvaluesImaginary, bufIm;

    if (UNEXPECTED(tensor_tensorbuffer_create(&eigenvaluesImaginary, (zend_long) na, &bufIm) == FAILURE)) {
        zval_ptr_dtor(&eigenvalues);

        efree(w);
        efree(wr);
        efree(wi);
        efree(vr);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&bufIm);

        for (i = 0; i < na; ++i) {
            vc[i] = wi[i];
        }
    }

    zval_ptr_dtor(&bufIm);

    zval eigenvectors, buf;

    if (UNEXPECTED(tensor_tensorbuffer_create(&eigenvectors, (zend_long) na * na, &buf) == FAILURE)) {
        zval_ptr_dtor(&eigenvalues);
        zval_ptr_dtor(&eigenvaluesImaginary);

        efree(w);
        efree(wr);
        efree(wi);
        efree(vr);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&buf);

        for (i = 0; i < (zend_ulong)(na * na); ++i) {
            vc[i] = vr[i];
        }
    }

    zval_ptr_dtor(&buf);

    zval tuple;

    array_init_size(&tuple, 3);

    add_next_index_zval(&tuple, &eigenvalues);
    add_next_index_zval(&tuple, &eigenvaluesImaginary);
    add_next_index_zval(&tuple, &eigenvectors);

    RETVAL_ARR(Z_ARR(tuple));

    efree(w);
    efree(wr);
    efree(wi);
    efree(vr);
}

/**
 * Compute the eigendecomposition of a symmetric matrix A and return the (real) eigenvalues,
 * the eigenvectors, and a zero-filled imaginary eigenvalue list in a tuple. The tuple
 * shape matches tensor_eig so callers can treat both results uniformly.
 *
 * @param return_value
 * @param a
 */
void tensor_eig_symmetric(zval * return_value, zval * a, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;
    unsigned int i;

    unsigned int na = (unsigned int) zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    double * w = safe_emalloc((size_t) na * (size_t) na, sizeof(double), 0);
    double * wr = emalloc(na * sizeof(double));

    for (i = 0; i < na * na; ++i) {
        w[i] = va[i];
    }

    lapack_int status = LAPACKE_dsyev(LAPACK_ROW_MAJOR, 'V', 'U', na, w, na, wr);

    if (status != 0) {
        efree(w);
        efree(wr);

        RETURN_NULL();
    }

    zval eigenvalues, bufRe;

    if (UNEXPECTED(tensor_tensorbuffer_create(&eigenvalues, (zend_long) na, &bufRe) == FAILURE)) {
        efree(w);
        efree(wr);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&bufRe);

        for (i = 0; i < na; ++i) {
            vc[i] = wr[i];
        }
    }

    zval_ptr_dtor(&bufRe);

    zval eigenvaluesImaginary, bufIm;

    if (UNEXPECTED(tensor_tensorbuffer_create(&eigenvaluesImaginary, (zend_long) na, &bufIm) == FAILURE)) {
        zval_ptr_dtor(&eigenvalues);

        efree(w);
        efree(wr);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&bufIm);

        memset(vc, 0, na * sizeof(double));
    }

    zval_ptr_dtor(&bufIm);

    zval eigenvectors, buf;

    if (UNEXPECTED(tensor_tensorbuffer_create(&eigenvectors, (zend_long) na * na, &buf) == FAILURE)) {
        zval_ptr_dtor(&eigenvalues);
        zval_ptr_dtor(&eigenvaluesImaginary);

        efree(w);
        efree(wr);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&buf);

        for (i = 0; i < (zend_ulong)(na * na); ++i) {
            vc[i] = w[i];
        }
    }

    zval_ptr_dtor(&buf);

    zval tuple;

    array_init_size(&tuple, 3);

    add_next_index_zval(&tuple, &eigenvalues);
    add_next_index_zval(&tuple, &eigenvaluesImaginary);
    add_next_index_zval(&tuple, &eigenvectors);

    RETVAL_ARR(Z_ARR(tuple));

    efree(w);
    efree(wr);
}

/**
 * Compute the singular value decomposition of a matrix A and return the singular values and unitary matrices U and VT in a tuple.
 * 
 * @param return_value
 * @param a
 */
void tensor_svd(zval * return_value, zval * a, zval * m, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;
    unsigned int i;

    unsigned int ma = (unsigned int) zephir_get_intval(m);
    unsigned int na = (unsigned int) zephir_get_intval(n);
    unsigned int k = MIN(ma, na);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    double * w = safe_emalloc((size_t) ma * (size_t) na, sizeof(double), 0);
    double * vu = safe_emalloc((size_t) ma * (size_t) ma, sizeof(double), 0);
    double * vs = emalloc(k * sizeof(double));
    double * vvt = safe_emalloc((size_t) na * (size_t) na, sizeof(double), 0);

    for (i = 0; i < ma * na; ++i) {
        w[i] = va[i];
    }

    lapack_int status = LAPACKE_dgesdd(LAPACK_ROW_MAJOR, 'A', ma, na, w, na, vs, vu, ma, vvt, na);

    if (status != 0) {
        efree(w);
        efree(vu);
        efree(vs);
        efree(vvt);

        RETURN_NULL();
    }

    zval u, bufU;

    if (UNEXPECTED(tensor_tensorbuffer_create(&u, (zend_long) ma * ma, &bufU) == FAILURE)) {
        efree(w);
        efree(vu);
        efree(vs);
        efree(vvt);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&bufU);

        for (i = 0; i < (zend_ulong)(ma * ma); ++i) {
            vc[i] = vu[i];
        }
    }

    zval_ptr_dtor(&bufU);

    zval s, bufS;

    if (UNEXPECTED(tensor_tensorbuffer_create(&s, (zend_long) k, &bufS) == FAILURE)) {
        zval_ptr_dtor(&u);

        efree(w);
        efree(vu);
        efree(vs);
        efree(vvt);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&bufS);

        for (i = 0; i < k; ++i) {
            vc[i] = vs[i];
        }
    }

    zval_ptr_dtor(&bufS);

    zval vt, bufVt;

    if (UNEXPECTED(tensor_tensorbuffer_create(&vt, (zend_long) na * na, &bufVt) == FAILURE)) {
        zval_ptr_dtor(&u);
        zval_ptr_dtor(&s);

        efree(w);
        efree(vu);
        efree(vs);
        efree(vvt);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&bufVt);

        for (i = 0; i < (zend_ulong)(na * na); ++i) {
            vc[i] = vvt[i];
        }
    }

    zval_ptr_dtor(&bufVt);

    zval tuple;

    array_init_size(&tuple, 3);

    add_next_index_zval(&tuple, &u);
    add_next_index_zval(&tuple, &s);
    add_next_index_zval(&tuple, &vt);

    RETVAL_ARR(Z_ARR(tuple));

    efree(w);
    efree(vu);
    efree(vs);
    efree(vvt);
}

/**
 * Run the forward elimination + singular row reduction used by both
 * `tensor_ref` and `tensor_rref`. On failure (LAPACK error) sets *status to a
 * large negative value. On success returns the number of row swaps. A pivot is
 * only a pivot if it is above `tolerance`; the reduction leaves the rows below
 * the last such one at zero, whatever roundoff they were left with.
 */
static long tensor_ref_step(double * w, const double * orig, unsigned int m, unsigned int n, double tolerance, lapack_int * status)
{
    unsigned int i, j;

    if (m == 0 || n == 0) {
        *status = 0;

        return 0;
    }

    int * pivots = emalloc(MIN(m, n) * sizeof(int));

    *status = LAPACKE_dgetrf(LAPACK_ROW_MAJOR, m, n, w, n, pivots);

    long swaps = 0;

    if (*status > 0) {
        /* Singular: `dgetrf` left `w` partially eliminated. The pure-PHP REF
         * fallback must operate on the original (unmodified) matrix to match
         * the previous behaviour, so restore `w` from `orig` first. */
        for (i = 0; i < m * n; ++i) {
            w[i] = orig[i];
        }

        swaps = tensor_ref_singular(w, m, n, tolerance);
    } else if (*status != 0) {
        efree(pivots);

        return 0;
    } else {
        for (i = 0; i < MIN(m, n); ++i) {
            if (i + 1 != (unsigned int) pivots[i]) {
                ++swaps;
            }
        }

        /* `dgetrf` returns the `L + U` factor in `w`. Extract the upper
         * triangular `U` (row-echelon form) by clearing the strictly-lower
         * triangle, matching the original `tensor_ref` output shape. */
        for (i = 0; i < m; ++i) {
            unsigned int lim = i < n ? i : n;

            for (j = 0; j < lim; ++j) {
                w[i * n + j] = 0.0;
            }
        }
    }

    for (i = 0; i < MIN(m, n); ++i) {
        if (fabs(w[i * n + i]) <= tolerance) {
            w[i * n + i] = 0.0;
        }
    }

    efree(pivots);

    return swaps;
}

/**
 * Return whether the row echelon form in `w` is that of a matrix of full column
 * rank, so that the reduced form is forced to be `[I; 0]` and need not be
 * computed. Only meaningful for `m >= n`, where `dgetrf` having found a pivot in
 * every column means every diagonal entry below is a real pivot.
 *
 * The `dgetrf` status alone is not enough to answer this. Its test is for a
 * pivot that is exactly zero, and floating-point roundoff in the factorization
 * of a matrix that is singular in exact arithmetic leaves a pivot of order 1e-16
 * instead -- the reduced form of a 4x4 whose fourth column is a fixed
 * combination of the first three comes back from `dgetrf` as full rank. So the
 * diagonal is measured against the same tolerance the reductions themselves use
 * before the shortcut is taken; below it, the matrix is treated as the
 * rank-deficient one it is and the reduction below decides its rank, which is
 * what `rank()`, `fullRank()` and `det()` all report.
 *
 * @param w
 * @param n
 * @param tolerance
 * @return int
 */
static int tensor_rref_is_full_column_rank(const double * w, unsigned int n, double tolerance)
{
    unsigned int i;

    for (i = 0; i < n; ++i) {
        if (fabs(w[i * n + i]) <= tolerance) {
            return 0;
        }
    }

    return 1;
}

/**
 * Complete the row echelon form in `w` in place by normalizing each pivot to 1
 * and eliminating the entries above it, turning a row echelon form into the
 * reduced one. A pivot is only a pivot if it is above `tolerance`. The scan is
 * the pure-PHP `Rref::reduce` path: it advances past a column whose pivot is
 * below the tolerance, zeroing a row that has no entry at or to the right of
 * that column, and skipping a column whose pivot is small but whose row does
 * have one to the right.
 *
 * A row zeroed for want of a pivot is zeroed whole, and not only from that
 * column on. A reduced row echelon form has a zero row there -- the columns to
 * the left of it are pivot columns, which the rows below have already been
 * eliminated from -- so an entry of roundoff left in one of them is not a
 * nonzero row of the form, and `tensor_rank` counts rows of the form. It is the
 * same reason the rows `tensor_ref_step` could not pivot on are exactly zero
 * there.
 *
 * Only reached when the row echelon form is not already known to be `[I; 0]`;
 * see `tensor_rref` for the shortcut that skips this entirely.
 *
 * @param w
 * @param m
 * @param n
 * @param tolerance
 */
static void tensor_rref_step(double * w, unsigned int m, unsigned int n, double tolerance)
{
    unsigned int i, j;

    unsigned int r = 0, c = 0;

    while (r < m && c < n) {
        double pivot = w[r * n + c];

        if (fabs(pivot) <= tolerance) {
            int found = 0;

            for (i = c; i < n; ++i) {
                if (fabs(w[r * n + i]) > tolerance) {
                    found = 1;
                    break;
                }
            }

            if (!found) {
                for (j = 0; j < n; ++j) {
                    w[r * n + j] = 0.0;
                }

                ++r;

                continue;
            }

            ++c;

            continue;
        }

        if (pivot != 1.0) {
            tensor_row_scale_route(w + r * n, pivot, n);
        }

        for (i = 0; i < r; ++i) {
            double scale = w[i * n + c];

            if (fabs(scale) > tolerance) {
                tensor_row_update_route(w + i * n, w + r * n, scale, n);
            }
        }

        ++r;
        ++c;
    }
}

/**
 * Compute the reduced row echelon form (RREF) of matrix A (read from a
 * TensorBuffer). The matrix is first brought to row-echelon form via LAPACK
 * `dgetrf` (with a singular-matrix fallback), and then Gauss-Jordan
 * normalization and elimination are applied to produce unit pivots, matching
 * the pure-PHP `Rref::reduce` path.
 *
 * When the echelon form comes back with full column rank -- `dgetrf` found a
 * pivot in every one of the `n` columns and each is above the tolerance the
 * reduction uses -- the reduced row echelon form is already known and is
 * written directly: the identity block over a zero block, with the `m - n`
 * surplus rows all zero. The uniqueness of RREF makes that forced, and taking it
 * skips the O(m * n * min(m, n)) of elimination that would otherwise have been
 * spent computing an answer already in hand. A matrix with more rows than
 * columns can only be in this case; a wide one reduces to `[I | X]` and still
 * has to be computed. See `tensor_rref_is_full_column_rank` for what "full
 * column rank" means here, and why it is not the `dgetrf` status on its own.
 *
 * @param return_value
 * @param a
 * @param m
 * @param n
 */
void tensor_rref(zval * return_value, zval * a, zval * m, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;
    unsigned int i;

    unsigned int ma = (unsigned int) zephir_get_intval(m);
    unsigned int na = (unsigned int) zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    double * w = safe_emalloc((size_t) ma * (size_t) na, sizeof(double), 0);

    for (i = 0; i < ma * na; ++i) {
        w[i] = va[i];
    }

    lapack_int status;

    double tolerance = tensor_relative_tolerance(ma, na, tensor_absolute_max(va, nbuf));

    (void) tensor_ref_step(w, va, ma, na, tolerance, &status);

    if (status < 0) {
        efree(w);

        RETURN_NULL();
    }

    if (status == 0 && ma >= na && tensor_rref_is_full_column_rank(w, na, tolerance)) {
        memset(w, 0, (size_t) ma * na * sizeof(double));

        for (i = 0; i < na; ++i) {
            w[i * na + i] = 1.0;
        }
    } else {
        tensor_rref_step(w, ma, na, tolerance);
    }

    zval result, buf;

    if (UNEXPECTED(tensor_tensorbuffer_create(&result, (zend_long) ma * na, &buf) == FAILURE)) {
        efree(w);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&buf);

        for (i = 0; i < (zend_ulong)(ma * na); ++i) {
            vc[i] = w[i];
        }
    }

    zval_ptr_dtor(&buf);

    *return_value = result;

    efree(w);
}

/**
 * Return the rank of an already-reduced matrix stored in a TensorBuffer, i.e.
 * the number of rows containing at least one non-zero element.
 *
 * The input is a reduced row echelon form, and the reduction that produced it has
 * already decided which of its rows are zero: a row it could not pivot on is
 * zeroed whole, and a pivot that is not above the tolerance is not a pivot, so
 * the row it heads is zeroed too. Comparing against zero exactly is therefore
 * counting those rows, and re-measuring them against a second threshold would
 * only disagree with the reduction that wrote them. The tolerance belongs to the
 * reduction, in one place, and this is where the decision it makes is read back.
 *
 * @param return_value
 * @param a
 * @param m
 * @param n
 */
void tensor_rank(zval * return_value, zval * a, zval * m, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;
    unsigned int i, j;

    unsigned int ma = (unsigned int) zephir_get_intval(m);
    unsigned int na = (unsigned int) zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    unsigned int rank = 0;

    for (i = 0; i < ma; ++i) {
        for (j = 0; j < na; ++j) {
            if (va[i * na + j] != 0.0) {
                ++rank;

                break;
            }
        }
    }

    RETVAL_LONG((zend_long) rank);
}

/**
 * Return whether the square matrix A (read from a TensorBuffer) is symmetric
 * with respect to a strict element-wise equality (matching PHP `!=`).
 *
 * @param return_value
 * @param a
 * @param n
 */
void tensor_is_symmetric(zval * return_value, zval * a, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;

    unsigned int i, j;

    unsigned int na = (unsigned int) zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    if (na < 2) {
        RETVAL_TRUE;

        return;
    }

    for (i = 0; i < na - 1; ++i) {
        for (j = i + 1; j < na; ++j) {
            if (va[i * na + j] != va[j * na + i]) {
                RETVAL_FALSE;

                return;
            }
        }
    }

    RETVAL_TRUE;
}

/**
 * Compute the outer product of two vectors (read from TensorBuffers) and
 * return the resulting matrix as a TensorBuffer.
 *
 * @param return_value
 * @param a
 * @param b
 * @param na
 * @param nb
 */
void tensor_outer(zval * return_value, zval * a, zval * b, zval * na, zval * nb)
{
    zend_long nbufa = 0, nbufb = 0;
    int ok_a = 0, ok_b = 0;

    unsigned int naHat = (unsigned int) zephir_get_intval(na);
    unsigned int nbHat = (unsigned int) zephir_get_intval(nb);

    double * va = tensor_tensorbuffer_doubles(a, &nbufa, &ok_a);
    double * vb = tensor_tensorbuffer_doubles(b, &nbufb, &ok_b);

    if (UNEXPECTED(!ok_a || !ok_b)) {
        return;
    }

    zval product, buf;

    if (UNEXPECTED(tensor_tensorbuffer_create(&product, (zend_long) naHat * nbHat, &buf) == FAILURE)) {
        return;
    }
    {
        double * vc = (double *) zephir_buffer_doubles(&buf);

        tensor_outer_fill_route(vc, va, vb, naHat, nbHat);
    }

    zval_ptr_dtor(&buf);

    *return_value = product;
}

/**
 * Compute the covariance of matrix A over its columns, centring each row on that
 * row's mean. The means are computed in a single pass and never materialised as
 * a separate object.
 *
 * @param return_value
 * @param a
 * @param m
 * @param n
 */
void tensor_covariance(zval * return_value, zval * a, zval * m, zval * n)
{
    zend_long i, j;
    zend_long nbuf = 0;
    int ok_a = 0;

    zend_long ma = zephir_get_intval(m);
    zend_long na = zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    /* The composed implementation this replaces reached both of these failures
     * by way of the row reduction and the column broadcast respectively, so the
     * exception types and messages are reproduced exactly. */
    if (UNEXPECTED(ma < 1)) {
        zephir_throw_exception_string(spl_ce_InvalidArgumentException,
            SL("Number of groups must be greater than 0."));
        return;
    }

    if (UNEXPECTED(na < 1)) {
        zephir_throw_exception_string(spl_ce_LengthException,
            SL("Matrix and vector dimensions must agree."));
        return;
    }

    /* Centre the rows into a scratch block. This is a plain block rather than a
     * TensorBuffer because it never escapes this call, and materialising the
     * centred matrix up front also keeps the product free of the cancellation
     * that folding the correction in as a rank-1 update would introduce. */
    double * vb = safe_emalloc((size_t) ma * (size_t) na, sizeof(double), 0);

    for (i = 0; i < ma; ++i) {
        const double * src = va + i * na;
        double * dst = vb + i * na;
        double mu = tensor_sum_doubles(src, na) / (double) na;

        for (j = 0; j < na; ++j) {
            dst[j] = src[j] - mu;
        }
    }

    zval c;

    if (UNEXPECTED(tensor_tensorbuffer_create(return_value, ma * ma, &c) == FAILURE)) {
        efree(vb);

        return;
    }

    double * vc = zephir_buffer_doubles(&c);

    /* B'B is symmetric, so dsyrk performs half the multiplies a dgemm would and
     * the 1/n folds into the scale factor, leaving no separate division pass.
     * B is already row-major and N x K as dsyrk wants it, and only the upper
     * triangle of the output is written. */
    cblas_dsyrk(CblasRowMajor, CblasUpper, CblasNoTrans, (blasint) ma, (blasint) na,
        1.0 / (double) na, vb, (blasint) na, 0.0, vc, (blasint) ma);

    /* Copy the untouched triangle across so the whole buffer is populated. Which
     * triangle dsyrk filled is immaterial: the product is symmetric, so either
     * half already holds a full set of pairwise covariances. */
    for (i = 0; i < ma; ++i) {
        for (j = i + 1; j < ma; ++j) {
            vc[j * ma + i] = vc[i * ma + j];
        }
    }

    efree(vb);

    zval_ptr_dtor(&c);
}

/* Point every dispatched row kernel in this file at its AVX variant.
 */
void tensor_linear_algebra_dispatch_avx_init(void)
{
	tensor_row_update_route = tensor_row_update_avx;
	tensor_row_scale_route = tensor_row_scale_avx;
	tensor_outer_fill_route = tensor_outer_fill_avx;
}

/**
 * Point every dispatched row kernel in this file at its FMA variant.
 *
 * The row update is the only kernel here with a multiply-accumulate to fuse. The
 * row scale is a division and the outer product a bare multiply, neither of
 * which FMA can improve, so those two are pointed at the same AVX variants the
 * plain AVX initializer would have used -- cpu.c runs this in place of that one
 * rather than alongside it.
 */
void tensor_linear_algebra_dispatch_fma_init(void)
{
	tensor_row_update_route = tensor_row_update_fma;
	tensor_row_scale_route = tensor_row_scale_avx;
	tensor_outer_fill_route = tensor_outer_fill_avx;
}

/**
 * Point every dispatched row kernel in this file at its AVX-512 variant.
 */
void tensor_linear_algebra_dispatch_avx512_init(void)
{
	tensor_row_update_route = tensor_row_update_avx512;
	tensor_row_scale_route = tensor_row_scale_avx512;
	tensor_outer_fill_route = tensor_outer_fill_avx512;
}
