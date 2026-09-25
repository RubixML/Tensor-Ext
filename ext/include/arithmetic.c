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

/* Dispatched elementwise binary kernels.
 *
 * One body, compiled twice: once at the extension's baseline ISA and once under
 * TENSOR_TARGET_AVX so the loop is widened to four doubles per vector. Which one
 * runs is decided once, at module init, by the route pointer.
 *
 * The body is a macro rather than a shared helper so that the baseline and the
 * AVX copy are the same text -- there is no second definition that could drift.
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
	static tensor_binary_fn tensor_##name##_route = tensor_##name##_baseline;   \
	                                                                             \
	void tensor_##name(zval * return_value, zval * a, zval * b)                 \
	{                                                                            \
		tensor_##name##_route(return_value, a, b);                                \
	}

TENSOR_BINARY_DISPATCH(multiply, va[i] * vb[i])
TENSOR_BINARY_DISPATCH(divide, va[i] / vb[i])
TENSOR_BINARY_DISPATCH(add, va[i] + vb[i])
TENSOR_BINARY_DISPATCH(subtract, va[i] - vb[i])

/* pow and fmod are per-element libm calls and there is no vector math library
 * behind them, so a 256-bit register would buy nothing. They reuse the same body
 * but keep a single route rather than being compiled twice. */
#define TENSOR_BINARY(name, op)                                                   \
	void tensor_##name(zval * return_value, zval * a, zval * b)                 \
	{                                                                            \
		TENSOR_BINARY_BODY(op)                                                   \
	}

TENSOR_BINARY(pow, pow(va[i], vb[i]))
TENSOR_BINARY(mod, fmod(va[i], vb[i]))

/* Elementwise operation of a buffer against a single scalar. Takes the same
 * route signature as the plain binary kernels: the scalar arrives as a zval, so
 * these are tensor_binary_fn as well. */

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
	static tensor_binary_fn tensor_##name##_route = tensor_##name##_baseline;   \
	                                                                             \
	void tensor_##name(zval * return_value, zval * a, zval * b)                 \
	{                                                                            \
		tensor_##name##_route(return_value, a, b);                                \
	}

TENSOR_SCALAR_DISPATCH(multiply_scalar, va[i] * ab)
TENSOR_SCALAR_DISPATCH(divide_scalar, va[i] / ab)
TENSOR_SCALAR_DISPATCH(add_scalar, va[i] + ab)
TENSOR_SCALAR_DISPATCH(subtract_scalar, va[i] - ab)

/* As above, the libm operations keep a single route. */
#define TENSOR_SCALAR(name, op)                                                   \
	void tensor_##name(zval * return_value, zval * a, zval * b)                 \
	{                                                                            \
		TENSOR_SCALAR_BODY(op)                                                   \
	}

TENSOR_SCALAR(pow_scalar, pow(va[i], ab))
TENSOR_SCALAR(mod_scalar, fmod(va[i], ab))


/* Scalar operation applied to every element of a matrix row. The matrix is
 * wrapped up in `a` (m * n doubles in row-major order) and the column vector
 * in `b` (m doubles) so that element (i, j) of the result is op(a[i * n + j],
 * b[i]).  Each operation expands into its own dedicated pair of loops so that
 * the optimizer can vectorize the elementwise mapping instead of being blocked
 * by an indirect call. */

#define TENSOR_COL_APPLY(name, expr)                                             \
void tensor_##name(zval * return_value, zval * a, zval * b, zval * n_zval)      \
{                                                                                \
	zend_long n = 0, m = 0, total = 0;                                           \
	int ok_a = 0, ok_b = 0;                                                      \
	                                                                             \
	double * va = tensor_tensorbuffer_doubles(a, &total, &ok_a);                 \
	double * vb = tensor_tensorbuffer_doubles(b, &m, &ok_b);                     \
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
	double * vc = zephir_buffer_doubles(&c);                                     \
	                                                                             \
	zend_long i, j;                                                              \
	                                                                             \
	for (i = 0; i < m; ++i) {                                                    \
		for (j = 0; j < nHat; ++j) {                                             \
			vc[i * nHat + j] = expr;                                             \
		}                                                                        \
	}                                                                            \
	                                                                             \
	zval_ptr_dtor(&c);                                                           \
}

/* Only the per-element libm operations live here. The plain floating point ones
 * are defined further down through TENSOR_COL_DISPATCH, which adds the AVX
 * variant; naming them here as well would define each kernel twice. */
TENSOR_COL_APPLY(pow_col, pow(va[i * nHat + j], vb[i]))
TENSOR_COL_APPLY(pow_col_reverse, pow(vb[i], va[i * nHat + j]))
TENSOR_COL_APPLY(mod_col, fmod(va[i * nHat + j], vb[i]))
TENSOR_COL_APPLY(mod_col_reverse, fmod(vb[i], va[i * nHat + j]))

#undef TENSOR_COL_APPLY

/* Binary operation applied to every element of a matrix using a shared
 * row vector. The matrix is wrapped up in `a` (m * n doubles in row-major
 * order) and the row vector in `b` (n doubles) so that element (i, j) of the
 * result is op(a[i * n + j], b[j]).  Each operation expands into its own
 * dedicated pair of loops so that the optimizer can vectorize the elementwise
 * mapping instead of being blocked by an indirect call. */

#define TENSOR_ROW_APPLY(name, expr)                                             \
void tensor_##name(zval * return_value, zval * a, zval * b, zval * n_zval)      \
{                                                                                \
	zend_long nHat = 0, m = 0, total = 0, nb = 0;                                \
	int ok_a = 0, ok_b = 0;                                                      \
	                                                                             \
	double * va = tensor_tensorbuffer_doubles(a, &total, &ok_a);                 \
	double * vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);                    \
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
	double * vc = zephir_buffer_doubles(&c);                                     \
	                                                                             \
	zend_long i, j;                                                              \
	                                                                             \
	for (i = 0; i < m; ++i) {                                                    \
		for (j = 0; j < nHat; ++j) {                                             \
			vc[i * nHat + j] = expr;                                             \
		}                                                                        \
	}                                                                            \
	                                                                             \
	zval_ptr_dtor(&c);                                                           \
}

/* As above, the libm operations only -- see TENSOR_COL_APPLY. */
TENSOR_ROW_APPLY(pow_row, pow(va[i * nHat + j], vb[j]))
TENSOR_ROW_APPLY(pow_row_reverse, pow(vb[j], va[i * nHat + j]))
TENSOR_ROW_APPLY(mod_row, fmod(va[i * nHat + j], vb[j]))
TENSOR_ROW_APPLY(mod_row_reverse, fmod(vb[j], va[i * nHat + j]))

#undef TENSOR_ROW_APPLY

/* Dispatched column and row variants.
 *
 * Same shape as the macros above, with the AVX copy added. The inner loop over
 * the columns is what gets widened: for the _col forms the row's scalar operand
 * is loop-invariant and is broadcast, and for the _row forms the operand is a
 * contiguous load, so the compiler turns both into 256-bit work. Only the plain
 * floating point operations are dispatched -- pow and fmod are per-element libm
 * calls with no vector math library behind them, so a wider register would buy
 * nothing and only duplicate code. */

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
	static tensor_dim_fn tensor_##name##_route = tensor_##name##_baseline;      \
	                                                                             \
	void tensor_##name(                                                         \
		zval * return_value, zval * a, zval * b, zval * n_zval)                  \
	{                                                                            \
		tensor_##name##_route(return_value, a, b, n_zval);                        \
	}

TENSOR_COL_DISPATCH(multiply_col, va[i * nHat + j] * vb[i])
TENSOR_COL_DISPATCH(add_col, va[i * nHat + j] + vb[i])
TENSOR_COL_DISPATCH(divide_col, va[i * nHat + j] / vb[i])
TENSOR_COL_DISPATCH(divide_col_reverse, vb[i] / va[i * nHat + j])
TENSOR_COL_DISPATCH(subtract_col, va[i * nHat + j] - vb[i])
TENSOR_COL_DISPATCH(subtract_col_reverse, vb[i] - va[i * nHat + j])

#undef TENSOR_COL_DISPATCH
#undef TENSOR_COL_DISPATCH_BODY

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
	static tensor_dim_fn tensor_##name##_route = tensor_##name##_baseline;      \
	                                                                             \
	void tensor_##name(                                                         \
		zval * return_value, zval * a, zval * b, zval * n_zval)                  \
	{                                                                            \
		tensor_##name##_route(return_value, a, b, n_zval);                        \
	}

TENSOR_ROW_DISPATCH(multiply_row, va[i * nHat + j] * vb[j])
TENSOR_ROW_DISPATCH(add_row, va[i * nHat + j] + vb[j])
TENSOR_ROW_DISPATCH(divide_row, va[i * nHat + j] / vb[j])
TENSOR_ROW_DISPATCH(divide_row_reverse, vb[j] / va[i * nHat + j])
TENSOR_ROW_DISPATCH(subtract_row, va[i * nHat + j] - vb[j])
TENSOR_ROW_DISPATCH(subtract_row_reverse, vb[j] - va[i * nHat + j])

#undef TENSOR_ROW_DISPATCH
#undef TENSOR_ROW_DISPATCH_BODY

#undef TENSOR_SCALAR
#undef TENSOR_SCALAR_DISPATCH
#undef TENSOR_SCALAR_BODY
#undef TENSOR_BINARY
#undef TENSOR_BINARY_DISPATCH
#undef TENSOR_BINARY_BODY

/**
 * Point every dispatched kernel in this file at its AVX variant.
 *
 * Called once from tensor_cpu_init() in include/cpu.c, which gates it on the
 * CPU actually supporting AVX. The routes are all already pointing at the
 * baseline variants before this runs, so the effect is strictly an upgrade.
 */
void tensor_arithmetic_dispatch_avx_init(void)
{
	tensor_multiply_route = tensor_multiply_avx;
	tensor_divide_route = tensor_divide_avx;
	tensor_add_route = tensor_add_avx;
	tensor_subtract_route = tensor_subtract_avx;

	tensor_multiply_scalar_route = tensor_multiply_scalar_avx;
	tensor_divide_scalar_route = tensor_divide_scalar_avx;
	tensor_add_scalar_route = tensor_add_scalar_avx;
	tensor_subtract_scalar_route = tensor_subtract_scalar_avx;

	tensor_multiply_col_route = tensor_multiply_col_avx;
	tensor_add_col_route = tensor_add_col_avx;
	tensor_divide_col_route = tensor_divide_col_avx;
	tensor_divide_col_reverse_route = tensor_divide_col_reverse_avx;
	tensor_subtract_col_route = tensor_subtract_col_avx;
	tensor_subtract_col_reverse_route = tensor_subtract_col_reverse_avx;

	tensor_multiply_row_route = tensor_multiply_row_avx;
	tensor_add_row_route = tensor_add_row_avx;
	tensor_divide_row_route = tensor_divide_row_avx;
	tensor_divide_row_reverse_route = tensor_divide_row_reverse_avx;
	tensor_subtract_row_route = tensor_subtract_row_avx;
	tensor_subtract_row_reverse_route = tensor_subtract_row_reverse_avx;
}
