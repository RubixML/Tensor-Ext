#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <ext/spl/spl_exceptions.h>
#include "kernel/operators.h"
#include "php_ext.h"
#include "kernel/buffer.h"
#include "include/buffer.h"
#include "include/dispatch.h"

/* Dispatched elementwise binary comparison kernels. Each one returns 1.0 when
 * the comparison holds and 0.0 otherwise.
 *
 * One body, compiled three times: once at the extension's baseline ISA, once
 * under TENSOR_TARGET_AVX, and once under TENSOR_TARGET_AVX512 so the loop is
 * widened to four or eight doubles per vector. Which one runs is decided once,
 * at module init, by the route pointer.
 *
 * The body is a macro rather than a shared helper so that the baseline and the
 * AVX and AVX-512 copies are the same text -- there is no second definition
 * that could drift.
 *
 * All three buffers are marked restrict: the inputs are fixed-size Buffers that
 * are never reallocated and the output was allocated microseconds ago, so they
 * cannot overlap, and telling the compiler so spares it a runtime alias check on
 * every call. Passing the same buffer as both `a` and `b` stays legal, because
 * those are only ever read through.
 */

#define TENSOR_BINARY_BODY(op)                                                    \
	zend_long i;                                                                 \
	zend_long na = 0, nb = 0;                                                    \
	int ok_a = 0, ok_b = 0;                                                      \
	                                                                             \
	double * restrict va = tensor_tensorbuffer_doubles(a, &na, &ok_a);          \
	double * restrict vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);          \
	                                                                             \
	if (UNEXPECTED(!ok_a || !ok_b)) {                                            \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	if (UNEXPECTED(na != nb)) {                                                  \
		zephir_throw_exception_string(spl_ce_LengthException,                    \
			SL("Input buffers must be the same length."));                     \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	zval c;                                                                      \
	                                                                             \
	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na, &c) == FAILURE)) { \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	double * restrict vc = zephir_buffer_doubles(&c);                            \
	                                                                             \
	for (i = 0; i < na; ++i) {                                                   \
		vc[i] = op;                                                              \
	}                                                                            \
	                                                                             \
	zval_ptr_dtor(&c);

#define TENSOR_BINARY_DISPATCH(name, op)                                          \
	static void tensor_##name##_baseline(zval * return_value, zval * a, zval * b) \
	{                                                                            \
		TENSOR_BINARY_BODY(op)                                                   \
	}                                                                            \
	                                                                             \
	TENSOR_TARGET_AVX                                                            \
	static void tensor_##name##_avx(zval * return_value, zval * a, zval * b)     \
	{                                                                            \
		TENSOR_BINARY_BODY(op)                                                   \
	}                                                                            \
	                                                                             \
	TENSOR_TARGET_AVX512                                                       \
	static void tensor_##name##_avx512(zval * return_value, zval * a, zval * b) \
	{                                                                            \
		TENSOR_BINARY_BODY(op)                                                   \
	}                                                                            \
	                                                                             \
	static tensor_binary_fn tensor_##name##_route = tensor_##name##_baseline;   \
	                                                                             \
	void tensor_##name(zval * return_value, zval * a, zval * b)                 \
	{                                                                            \
		tensor_##name##_route(return_value, a, b);                                \
	}

TENSOR_BINARY_DISPATCH(equal, va[i] == vb[i] ? 1.0 : 0.0)
TENSOR_BINARY_DISPATCH(not_equal, va[i] != vb[i] ? 1.0 : 0.0)
TENSOR_BINARY_DISPATCH(greater, va[i] > vb[i] ? 1.0 : 0.0)
TENSOR_BINARY_DISPATCH(greater_equal, va[i] >= vb[i] ? 1.0 : 0.0)
TENSOR_BINARY_DISPATCH(less, va[i] < vb[i] ? 1.0 : 0.0)
TENSOR_BINARY_DISPATCH(less_equal, va[i] <= vb[i] ? 1.0 : 0.0)

/* Elementwise comparison of a buffer against a single scalar. Returns 1.0 when
 * the comparison holds and 0.0 otherwise. Takes the same route signature as the
 * plain binary kernels: the scalar arrives as a zval, so these are
 * tensor_binary_fn as well. */

#define TENSOR_SCALAR_BODY(op)                                                    \
	zend_long i;                                                                 \
	zend_long na = 0;                                                            \
	int ok_a = 0;                                                                \
	                                                                             \
	double * restrict va = tensor_tensorbuffer_doubles(a, &na, &ok_a);          \
	                                                                             \
	if (UNEXPECTED(!ok_a)) {                                                     \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	double ab = zephir_get_doubleval(b);                                         \
	                                                                             \
	zval c;                                                                      \
	                                                                             \
	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na, &c) == FAILURE)) { \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	double * restrict vc = zephir_buffer_doubles(&c);                            \
	                                                                             \
	for (i = 0; i < na; ++i) {                                                   \
		vc[i] = op;                                                              \
	}                                                                            \
	                                                                             \
	zval_ptr_dtor(&c);

#define TENSOR_SCALAR_DISPATCH(name, op)                                         \
	static void tensor_##name##_baseline(zval * return_value, zval * a, zval * b) \
	{                                                                            \
		TENSOR_SCALAR_BODY(op)                                                   \
	}                                                                            \
	                                                                             \
	TENSOR_TARGET_AVX                                                            \
	static void tensor_##name##_avx(zval * return_value, zval * a, zval * b)     \
	{                                                                            \
		TENSOR_SCALAR_BODY(op)                                                   \
	}                                                                            \
	                                                                             \
	TENSOR_TARGET_AVX512                                                       \
	static void tensor_##name##_avx512(zval * return_value, zval * a, zval * b) \
	{                                                                            \
		TENSOR_SCALAR_BODY(op)                                                   \
	}                                                                            \
	                                                                             \
	static tensor_binary_fn tensor_##name##_route = tensor_##name##_baseline;   \
	                                                                             \
	void tensor_##name(zval * return_value, zval * a, zval * b)                 \
	{                                                                            \
		tensor_##name##_route(return_value, a, b);                                \
	}

TENSOR_SCALAR_DISPATCH(equal_scalar, va[i] == ab ? 1.0 : 0.0)
TENSOR_SCALAR_DISPATCH(not_equal_scalar, va[i] != ab ? 1.0 : 0.0)
TENSOR_SCALAR_DISPATCH(greater_scalar, va[i] > ab ? 1.0 : 0.0)
TENSOR_SCALAR_DISPATCH(greater_equal_scalar, va[i] >= ab ? 1.0 : 0.0)
TENSOR_SCALAR_DISPATCH(less_scalar, va[i] < ab ? 1.0 : 0.0)
TENSOR_SCALAR_DISPATCH(less_equal_scalar, va[i] <= ab ? 1.0 : 0.0)

/* Comparison applied to every element of a matrix against a shared column
 * vector. The matrix is wrapped up in `a` (m * n doubles in row-major order)
 * and the column vector in `b` (m doubles) so that element (i, j) of the
 * result is 1.0 when the comparison op(a[i * n + j], b[i]) holds and 0.0
 * otherwise. Each operation expands into its own dedicated pair of loops so
 * that the optimizer can vectorize the elementwise mapping instead of being
 * blocked by an indirect call. */

#define TENSOR_COL_DISPATCH_BODY(op)                                             \
	zend_long n = 0, m = 0, total = 0;                                           \
	int ok_a = 0, ok_b = 0;                                                      \
	                                                                             \
	double * restrict va = tensor_tensorbuffer_doubles(a, &total, &ok_a);       \
	double * restrict vb = tensor_tensorbuffer_doubles(b, &m, &ok_b);           \
	                                                                             \
	if (UNEXPECTED(!ok_a || !ok_b)) {                                            \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	zend_long nHat = zephir_get_intval(n_zval);                                  \
	                                                                             \
	if (UNEXPECTED(nHat < 1 || total != m * nHat)) {                             \
		zephir_throw_exception_string(spl_ce_LengthException,                    \
			SL("Matrix and vector dimensions must agree."));                     \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	zval c;                                                                      \
	                                                                             \
	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, total, &c) == FAILURE)) { \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	double * restrict vc = zephir_buffer_doubles(&c);                           \
	                                                                             \
	zend_long i, j;                                                              \
	                                                                             \
	for (i = 0; i < m; ++i) {                                                    \
		for (j = 0; j < nHat; ++j) {                                             \
			vc[i * nHat + j] = op;                                               \
		}                                                                        \
	}                                                                            \
	                                                                             \
	zval_ptr_dtor(&c);

#define TENSOR_COL_DISPATCH(name, op)                                            \
	static void tensor_##name##_baseline(                                         \
		zval * return_value, zval * a, zval * b, zval * n_zval)                  \
	{                                                                            \
		TENSOR_COL_DISPATCH_BODY(op)                                             \
	}                                                                            \
	                                                                             \
	TENSOR_TARGET_AVX                                                            \
	static void tensor_##name##_avx(                                             \
		zval * return_value, zval * a, zval * b, zval * n_zval)                  \
	{                                                                            \
		TENSOR_COL_DISPATCH_BODY(op)                                             \
	}                                                                            \
	                                                                             \
	TENSOR_TARGET_AVX512                                                       \
	static void tensor_##name##_avx512(                                          \
		zval * return_value, zval * a, zval * b, zval * n_zval)                  \
	{                                                                            \
		TENSOR_COL_DISPATCH_BODY(op)                                             \
	}                                                                            \
	                                                                             \
	static tensor_dim_fn tensor_##name##_route = tensor_##name##_baseline;      \
	                                                                             \
	void tensor_##name(                                                         \
		zval * return_value, zval * a, zval * b, zval * n_zval)                  \
	{                                                                            \
		tensor_##name##_route(return_value, a, b, n_zval);                        \
	}

TENSOR_COL_DISPATCH(equal_col, va[i * nHat + j] == vb[i] ? 1.0 : 0.0)
TENSOR_COL_DISPATCH(not_equal_col, va[i * nHat + j] != vb[i] ? 1.0 : 0.0)
TENSOR_COL_DISPATCH(greater_col, va[i * nHat + j] > vb[i] ? 1.0 : 0.0)
TENSOR_COL_DISPATCH(greater_col_reverse, vb[i] > va[i * nHat + j] ? 1.0 : 0.0)
TENSOR_COL_DISPATCH(greater_equal_col, va[i * nHat + j] >= vb[i] ? 1.0 : 0.0)
TENSOR_COL_DISPATCH(greater_equal_col_reverse, vb[i] >= va[i * nHat + j] ? 1.0 : 0.0)
TENSOR_COL_DISPATCH(less_col, va[i * nHat + j] < vb[i] ? 1.0 : 0.0)
TENSOR_COL_DISPATCH(less_col_reverse, vb[i] < va[i * nHat + j] ? 1.0 : 0.0)
TENSOR_COL_DISPATCH(less_equal_col, va[i * nHat + j] <= vb[i] ? 1.0 : 0.0)
TENSOR_COL_DISPATCH(less_equal_col_reverse, vb[i] <= va[i * nHat + j] ? 1.0 : 0.0)

/* Comparison applied to every element of a matrix against a shared row vector.
 * The matrix is wrapped up in `a` (m * n doubles in row-major order) and the
 * row vector in `b` (n doubles) so that element (i, j) of the result is 1.0
 * when the comparison op(a[i * n + j], b[j]) holds and 0.0 otherwise. Each
 * operation expands into its own dedicated pair of loops so that the optimizer
 * can vectorize the elementwise mapping instead of being blocked by an
 * indirect call. */

#define TENSOR_ROW_DISPATCH_BODY(op)                                             \
	zend_long nHat = 0, m = 0, total = 0, nb = 0;                                \
	int ok_a = 0, ok_b = 0;                                                      \
	                                                                             \
	double * restrict va = tensor_tensorbuffer_doubles(a, &total, &ok_a);       \
	double * restrict vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);          \
	                                                                             \
	if (UNEXPECTED(!ok_a || !ok_b)) {                                            \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	nHat = zephir_get_intval(n_zval);                                            \
	                                                                             \
	if (UNEXPECTED(nHat < 1 || nb != nHat || total < nHat || total % nHat != 0)) { \
		zephir_throw_exception_string(spl_ce_LengthException,                    \
			SL("Matrix and vector dimensions must agree."));                     \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	m = total / nHat;                                                            \
	                                                                             \
	zval c;                                                                      \
	                                                                             \
	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, total, &c) == FAILURE)) { \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	double * restrict vc = zephir_buffer_doubles(&c);                           \
	                                                                             \
	zend_long i, j;                                                              \
	                                                                             \
	for (i = 0; i < m; ++i) {                                                    \
		for (j = 0; j < nHat; ++j) {                                             \
			vc[i * nHat + j] = op;                                               \
		}                                                                        \
	}                                                                            \
	                                                                             \
	zval_ptr_dtor(&c);

#define TENSOR_ROW_DISPATCH(name, op)                                            \
	static void tensor_##name##_baseline(                                         \
		zval * return_value, zval * a, zval * b, zval * n_zval)                  \
	{                                                                            \
		TENSOR_ROW_DISPATCH_BODY(op)                                             \
	}                                                                            \
	                                                                             \
	TENSOR_TARGET_AVX                                                            \
	static void tensor_##name##_avx(                                             \
		zval * return_value, zval * a, zval * b, zval * n_zval)                  \
	{                                                                            \
		TENSOR_ROW_DISPATCH_BODY(op)                                             \
	}                                                                            \
	                                                                             \
	TENSOR_TARGET_AVX512                                                       \
	static void tensor_##name##_avx512(                                          \
		zval * return_value, zval * a, zval * b, zval * n_zval)                  \
	{                                                                            \
		TENSOR_ROW_DISPATCH_BODY(op)                                             \
	}                                                                            \
	                                                                             \
	static tensor_dim_fn tensor_##name##_route = tensor_##name##_baseline;      \
	                                                                             \
	void tensor_##name(                                                         \
		zval * return_value, zval * a, zval * b, zval * n_zval)                  \
	{                                                                            \
		tensor_##name##_route(return_value, a, b, n_zval);                        \
	}

TENSOR_ROW_DISPATCH(equal_row, va[i * nHat + j] == vb[j] ? 1.0 : 0.0)
TENSOR_ROW_DISPATCH(not_equal_row, va[i * nHat + j] != vb[j] ? 1.0 : 0.0)
TENSOR_ROW_DISPATCH(greater_row, va[i * nHat + j] > vb[j] ? 1.0 : 0.0)
TENSOR_ROW_DISPATCH(greater_row_reverse, vb[j] > va[i * nHat + j] ? 1.0 : 0.0)
TENSOR_ROW_DISPATCH(greater_equal_row, va[i * nHat + j] >= vb[j] ? 1.0 : 0.0)
TENSOR_ROW_DISPATCH(greater_equal_row_reverse, vb[j] >= va[i * nHat + j] ? 1.0 : 0.0)
TENSOR_ROW_DISPATCH(less_row, va[i * nHat + j] < vb[j] ? 1.0 : 0.0)
TENSOR_ROW_DISPATCH(less_row_reverse, vb[j] < va[i * nHat + j] ? 1.0 : 0.0)
TENSOR_ROW_DISPATCH(less_equal_row, va[i * nHat + j] <= vb[j] ? 1.0 : 0.0)
TENSOR_ROW_DISPATCH(less_equal_row_reverse, vb[j] <= va[i * nHat + j] ? 1.0 : 0.0)

/**
 * Point every dispatched kernel in this file at its AVX variant.
 *
 * Called once from tensor_cpu_init() in include/cpu.c, which gates it on the
 * CPU actually supporting AVX. The routes are all already pointing at the
 * baseline variants before this runs, so the effect is strictly an upgrade.
 */
void tensor_comparison_dispatch_avx_init(void)
{
	tensor_equal_route = tensor_equal_avx;
	tensor_not_equal_route = tensor_not_equal_avx;
	tensor_greater_route = tensor_greater_avx;
	tensor_greater_equal_route = tensor_greater_equal_avx;
	tensor_less_route = tensor_less_avx;
	tensor_less_equal_route = tensor_less_equal_avx;

	tensor_equal_scalar_route = tensor_equal_scalar_avx;
	tensor_not_equal_scalar_route = tensor_not_equal_scalar_avx;
	tensor_greater_scalar_route = tensor_greater_scalar_avx;
	tensor_greater_equal_scalar_route = tensor_greater_equal_scalar_avx;
	tensor_less_scalar_route = tensor_less_scalar_avx;
	tensor_less_equal_scalar_route = tensor_less_equal_scalar_avx;

	tensor_equal_col_route = tensor_equal_col_avx;
	tensor_not_equal_col_route = tensor_not_equal_col_avx;
	tensor_greater_col_route = tensor_greater_col_avx;
	tensor_greater_col_reverse_route = tensor_greater_col_reverse_avx;
	tensor_greater_equal_col_route = tensor_greater_equal_col_avx;
	tensor_greater_equal_col_reverse_route = tensor_greater_equal_col_reverse_avx;
	tensor_less_col_route = tensor_less_col_avx;
	tensor_less_col_reverse_route = tensor_less_col_reverse_avx;
	tensor_less_equal_col_route = tensor_less_equal_col_avx;
	tensor_less_equal_col_reverse_route = tensor_less_equal_col_reverse_avx;

	tensor_equal_row_route = tensor_equal_row_avx;
	tensor_not_equal_row_route = tensor_not_equal_row_avx;
	tensor_greater_row_route = tensor_greater_row_avx;
	tensor_greater_row_reverse_route = tensor_greater_row_reverse_avx;
	tensor_greater_equal_row_route = tensor_greater_equal_row_avx;
	tensor_greater_equal_row_reverse_route = tensor_greater_equal_row_reverse_avx;
	tensor_less_row_route = tensor_less_row_avx;
	tensor_less_row_reverse_route = tensor_less_row_reverse_avx;
	tensor_less_equal_row_route = tensor_less_equal_row_avx;
	tensor_less_equal_row_reverse_route = tensor_less_equal_row_reverse_avx;
}

/**
 * Point every dispatched kernel in this file at its AVX-512 variant.
 *
 * Called once from tensor_cpu_init() in include/cpu.c, which gates it on the
 * CPU actually supporting AVX-512 -- and on AVX, since only one of the two
 * initializers runs. The effect is strictly an upgrade to the widest route.
 */
void tensor_comparison_dispatch_avx512_init(void)
{
	tensor_equal_route = tensor_equal_avx512;
	tensor_not_equal_route = tensor_not_equal_avx512;
	tensor_greater_route = tensor_greater_avx512;
	tensor_greater_equal_route = tensor_greater_equal_avx512;
	tensor_less_route = tensor_less_avx512;
	tensor_less_equal_route = tensor_less_equal_avx512;

	tensor_equal_scalar_route = tensor_equal_scalar_avx512;
	tensor_not_equal_scalar_route = tensor_not_equal_scalar_avx512;
	tensor_greater_scalar_route = tensor_greater_scalar_avx512;
	tensor_greater_equal_scalar_route = tensor_greater_equal_scalar_avx512;
	tensor_less_scalar_route = tensor_less_scalar_avx512;
	tensor_less_equal_scalar_route = tensor_less_equal_scalar_avx512;

	tensor_equal_col_route = tensor_equal_col_avx512;
	tensor_not_equal_col_route = tensor_not_equal_col_avx512;
	tensor_greater_col_route = tensor_greater_col_avx512;
	tensor_greater_col_reverse_route = tensor_greater_col_reverse_avx512;
	tensor_greater_equal_col_route = tensor_greater_equal_col_avx512;
	tensor_greater_equal_col_reverse_route = tensor_greater_equal_col_reverse_avx512;
	tensor_less_col_route = tensor_less_col_avx512;
	tensor_less_col_reverse_route = tensor_less_col_reverse_avx512;
	tensor_less_equal_col_route = tensor_less_equal_col_avx512;
	tensor_less_equal_col_reverse_route = tensor_less_equal_col_reverse_avx512;

	tensor_equal_row_route = tensor_equal_row_avx512;
	tensor_not_equal_row_route = tensor_not_equal_row_avx512;
	tensor_greater_row_route = tensor_greater_row_avx512;
	tensor_greater_row_reverse_route = tensor_greater_row_reverse_avx512;
	tensor_greater_equal_row_route = tensor_greater_equal_row_avx512;
	tensor_greater_equal_row_reverse_route = tensor_greater_equal_row_reverse_avx512;
	tensor_less_row_route = tensor_less_row_avx512;
	tensor_less_row_reverse_route = tensor_less_row_reverse_avx512;
	tensor_less_equal_row_route = tensor_less_equal_row_avx512;
	tensor_less_equal_row_reverse_route = tensor_less_equal_row_reverse_avx512;
}
