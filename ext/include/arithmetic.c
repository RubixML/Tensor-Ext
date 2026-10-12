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
	tensor_double_view v_a, v_b;                                                 \
	                                                                             \
	if (UNEXPECTED(!tensor_view_resolve_arg(a, &v_a) ||                          \
		!tensor_view_resolve_arg(b, &v_b))) {                                    \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	zend_long na = v_a.len;                                                      \
	zval c;                                                                      \
	                                                                             \
	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na, &c) == FAILURE)) { \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	double * restrict vc = zephir_buffer_doubles(&c);                            \
	                                                                             \
	if (v_a.contiguous && v_b.contiguous) {                                      \
		const double * restrict va = v_a.base + v_a.offset;                      \
		const double * restrict vb = v_b.base + v_b.offset;                      \
	                                                                             \
		for (i = 0; i < na; ++i) {                                               \
			double x = va[i];                                                    \
			double y = vb[i];                                                    \
			vc[i] = op;                                                          \
		}                                                                        \
	} else {                                                                     \
		tensor_view_reader ra, rb;                                               \
		tensor_view_reader_init(&ra, &v_a);                                      \
		tensor_view_reader_init(&rb, &v_b);                                      \
	                                                                             \
		for (i = 0; i < na; ++i) {                                               \
			double x = tensor_view_reader_value(&ra);                            \
			double y = tensor_view_reader_value(&rb);                            \
			vc[i] = op;                                                          \
			tensor_view_reader_advance(&ra);                                     \
			tensor_view_reader_advance(&rb);                                     \
		}                                                                        \
	}                                                                            \
	                                                                             \
	zval_ptr_dtor(&c);

#define TENSOR_BINARY_DISPATCH(name, op)                                          \
	static void tensor_##name##_sse(zval * return_value, zval * a, zval * b) \
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
	static tensor_binary_fn tensor_##name##_route = tensor_##name##_sse;   \
	                                                                             \
	void tensor_##name(zval * return_value, zval * a, zval * b)                 \
	{                                                                            \
		tensor_##name##_route(return_value, a, b);                                \
	}

TENSOR_BINARY_DISPATCH(multiply, x * y)
TENSOR_BINARY_DISPATCH(divide, x / y)
TENSOR_BINARY_DISPATCH(add, x + y)
TENSOR_BINARY_DISPATCH(subtract, x - y)

#define TENSOR_BINARY(name, op)                                                   \
	void tensor_##name(zval * return_value, zval * a, zval * b)                 \
	{                                                                            \
		TENSOR_BINARY_BODY(op)                                                   \
	}

TENSOR_BINARY(pow, pow(x, y))
TENSOR_BINARY(mod, fmod(x, y))

/* Elementwise operation of a buffer against a single scalar. Takes the same
 * route signature as the plain binary kernels: the scalar arrives as a zval, so
 * these are tensor_binary_fn as well. */

#define TENSOR_SCALAR_BODY(op)                                                    \
	zend_long i;                                                                 \
	tensor_double_view v_a;                                                      \
	                                                                             \
	if (UNEXPECTED(!tensor_view_resolve_arg(a, &v_a))) {                         \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	zend_long na = v_a.len;                                                      \
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
	if (v_a.contiguous) {                                                        \
		const double * restrict va = v_a.base + v_a.offset;                      \
	                                                                             \
		for (i = 0; i < na; ++i) {                                               \
			double x = va[i];                                                    \
			vc[i] = op;                                                          \
		}                                                                        \
	} else {                                                                     \
		tensor_view_reader ra;                                                   \
		tensor_view_reader_init(&ra, &v_a);                                      \
	                                                                             \
		for (i = 0; i < na; ++i) {                                               \
			double x = tensor_view_reader_value(&ra);                            \
			vc[i] = op;                                                          \
			tensor_view_reader_advance(&ra);                                     \
		}                                                                        \
	}                                                                            \
	                                                                             \
	zval_ptr_dtor(&c);

#define TENSOR_SCALAR_DISPATCH(name, op)                                         \
	static void tensor_##name##_sse(zval * return_value, zval * a, zval * b) \
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
	static tensor_binary_fn tensor_##name##_route = tensor_##name##_sse;   \
	                                                                             \
	void tensor_##name(zval * return_value, zval * a, zval * b)                 \
	{                                                                            \
		tensor_##name##_route(return_value, a, b);                                \
	}

TENSOR_SCALAR_DISPATCH(multiply_scalar, x * ab)
TENSOR_SCALAR_DISPATCH(divide_scalar, x / ab)
TENSOR_SCALAR_DISPATCH(add_scalar, x + ab)
TENSOR_SCALAR_DISPATCH(subtract_scalar, x - ab)

/* As above, the libm operations keep a single route. */
#define TENSOR_SCALAR(name, op)                                                   \
	void tensor_##name(zval * return_value, zval * a, zval * b)                 \
	{                                                                            \
		TENSOR_SCALAR_BODY(op)                                                   \
	}

TENSOR_SCALAR(pow_scalar, pow(x, ab))
TENSOR_SCALAR(mod_scalar, fmod(x, ab))


/* Scalar operation applied to every element of a matrix row. The matrix is
 * wrapped up in `a` (m * n doubles in row-major order) and the column vector
 * in `b` (m doubles) so that element (i, j) of the result is op(a[i * n + j],
 * b[i]).  Each operation expands into its own dedicated pair of loops so that
 * the optimizer can vectorize the elementwise mapping instead of being blocked
 * by an indirect call. */

#define TENSOR_COL_APPLY(name, expr)                                             \
void tensor_##name(zval * return_value, zval * a, zval * b, zval * n_zval)      \
{                                                                                \
	tensor_double_view v_a, v_b;                                                 \
	zend_long m = 0;                                                             \
	double * owned_b = NULL;                                                     \
	const double * vb = NULL;                                                    \
	                                                                             \
	if (UNEXPECTED(!tensor_view_resolve_arg(a, &v_a) ||                          \
		!tensor_view_doubles(b, &vb, &m, &owned_b))) {                           \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	zend_long total = v_a.len;                                                   \
	zend_long nHat = zephir_get_intval(n_zval);                                  \
	                                                                             \
	if (UNEXPECTED(nHat < 1)) {                                                  \
		zephir_throw_exception_string(spl_ce_LengthException,                    \
			SL("Matrix and vector dimensions must agree."));                     \
		if (owned_b != NULL) { efree(owned_b); }                                 \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	zval c;                                                                      \
	                                                                             \
	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, total, &c) == FAILURE)) { \
		if (owned_b != NULL) { efree(owned_b); }                                 \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	double * vc = zephir_buffer_doubles(&c);                                     \
	                                                                             \
	zend_long i, j;                                                              \
	                                                                             \
	if (v_a.contiguous) {                                                        \
		const double * va = v_a.base + v_a.offset;                               \
	                                                                             \
		for (i = 0; i < m; ++i) {                                                \
			for (j = 0; j < nHat; ++j) {                                         \
				double x = va[i * nHat + j];                                     \
				double y = vb[i];                                                \
				vc[i * nHat + j] = expr;                                         \
			}                                                                    \
		}                                                                        \
	} else {                                                                     \
		tensor_view_reader ra;                                                   \
		tensor_view_reader_init(&ra, &v_a);                                      \
	                                                                             \
		for (i = 0; i < m; ++i) {                                                \
			for (j = 0; j < nHat; ++j) {                                         \
				double x = tensor_view_reader_value(&ra);                        \
				double y = vb[i];                                                \
				vc[i * nHat + j] = expr;                                         \
				tensor_view_reader_advance(&ra);                                 \
			}                                                                    \
		}                                                                        \
	}                                                                            \
	                                                                             \
	if (owned_b != NULL) { efree(owned_b); }                                     \
	zval_ptr_dtor(&c);                                                           \
}

TENSOR_COL_APPLY(pow_col, pow(x, y))
TENSOR_COL_APPLY(pow_col_reverse, pow(y, x))
TENSOR_COL_APPLY(mod_col, fmod(x, y))
TENSOR_COL_APPLY(mod_col_reverse, fmod(y, x))

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
	tensor_double_view v_a;                                                      \
	zend_long nb = 0;                                                            \
	double * owned_b = NULL;                                                     \
	const double * vb = NULL;                                                    \
	                                                                             \
	if (UNEXPECTED(!tensor_view_resolve_arg(a, &v_a) ||                          \
		!tensor_view_doubles(b, &vb, &nb, &owned_b))) {                          \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	zend_long total = v_a.len;                                                   \
	zend_long nHat = zephir_get_intval(n_zval);                                  \
	                                                                             \
	if (UNEXPECTED(nHat < 1 || total < nHat)) {                                  \
		zephir_throw_exception_string(spl_ce_LengthException,                    \
			SL("Matrix and vector dimensions must agree."));                     \
		if (owned_b != NULL) { efree(owned_b); }                                 \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	zend_long m = total / nHat;                                                  \
	                                                                             \
	zval c;                                                                      \
	                                                                             \
	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, total, &c) == FAILURE)) { \
		if (owned_b != NULL) { efree(owned_b); }                                 \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	double * vc = zephir_buffer_doubles(&c);                                     \
	                                                                             \
	zend_long i, j;                                                              \
	                                                                             \
	if (v_a.contiguous) {                                                        \
		const double * va = v_a.base + v_a.offset;                               \
	                                                                             \
		for (i = 0; i < m; ++i) {                                                \
			for (j = 0; j < nHat; ++j) {                                         \
				double x = va[i * nHat + j];                                     \
				double y = vb[j];                                                \
				vc[i * nHat + j] = expr;                                         \
			}                                                                    \
		}                                                                        \
	} else {                                                                     \
		tensor_view_reader ra;                                                   \
		tensor_view_reader_init(&ra, &v_a);                                      \
	                                                                             \
		for (i = 0; i < m; ++i) {                                                \
			for (j = 0; j < nHat; ++j) {                                         \
				double x = tensor_view_reader_value(&ra);                        \
				double y = vb[j];                                                \
				vc[i * nHat + j] = expr;                                         \
				tensor_view_reader_advance(&ra);                                 \
			}                                                                    \
		}                                                                        \
	}                                                                            \
	                                                                             \
	if (owned_b != NULL) { efree(owned_b); }                                     \
	zval_ptr_dtor(&c);                                                           \
}

/* As above, the libm operations only -- see TENSOR_COL_APPLY. */
TENSOR_ROW_APPLY(pow_row, pow(x, y))
TENSOR_ROW_APPLY(pow_row_reverse, pow(y, x))
TENSOR_ROW_APPLY(mod_row, fmod(x, y))
TENSOR_ROW_APPLY(mod_row_reverse, fmod(y, x))

#undef TENSOR_ROW_APPLY

#define TENSOR_COL_DISPATCH_BODY(op)                                             \
	tensor_double_view v_a;                                                      \
	zend_long m = 0;                                                             \
	double * owned_b = NULL;                                                     \
	const double * vb = NULL;                                                    \
	                                                                             \
	if (UNEXPECTED(!tensor_view_resolve_arg(a, &v_a) ||                          \
		!tensor_view_doubles(b, &vb, &m, &owned_b))) {                           \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	zend_long total = v_a.len;                                                   \
	zend_long nHat = zephir_get_intval(n_zval);                                  \
	                                                                             \
	if (UNEXPECTED(nHat < 1)) {                                                  \
		zephir_throw_exception_string(spl_ce_LengthException,                    \
			SL("Matrix and vector dimensions must agree."));                     \
		if (owned_b != NULL) { efree(owned_b); }                                 \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	zval c;                                                                      \
	                                                                             \
	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, total, &c) == FAILURE)) { \
		if (owned_b != NULL) { efree(owned_b); }                                 \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	double * restrict vc = zephir_buffer_doubles(&c);                           \
	                                                                             \
	zend_long i, j;                                                              \
	                                                                             \
	if (v_a.contiguous) {                                                        \
		const double * restrict va = v_a.base + v_a.offset;                      \
	                                                                             \
		for (i = 0; i < m; ++i) {                                                \
			for (j = 0; j < nHat; ++j) {                                         \
				double x = va[i * nHat + j];                                     \
				double y = vb[i];                                                \
				vc[i * nHat + j] = op;                                           \
			}                                                                    \
		}                                                                        \
	} else {                                                                     \
		tensor_view_reader ra;                                                   \
		tensor_view_reader_init(&ra, &v_a);                                      \
	                                                                             \
		for (i = 0; i < m; ++i) {                                                \
			for (j = 0; j < nHat; ++j) {                                         \
				double x = tensor_view_reader_value(&ra);                        \
				double y = vb[i];                                                \
				vc[i * nHat + j] = op;                                           \
				tensor_view_reader_advance(&ra);                                 \
			}                                                                    \
		}                                                                        \
	}                                                                            \
	                                                                             \
	if (owned_b != NULL) { efree(owned_b); }                                     \
	zval_ptr_dtor(&c);

#define TENSOR_COL_DISPATCH(name, op)                                            \
	static void tensor_##name##_sse(                                         \
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
	static tensor_dim_fn tensor_##name##_route = tensor_##name##_sse;      \
	                                                                             \
	void tensor_##name(                                                         \
		zval * return_value, zval * a, zval * b, zval * n_zval)                  \
	{                                                                            \
		tensor_##name##_route(return_value, a, b, n_zval);                        \
	}

TENSOR_COL_DISPATCH(multiply_col, x * y)
TENSOR_COL_DISPATCH(add_col, x + y)
TENSOR_COL_DISPATCH(divide_col, x / y)
TENSOR_COL_DISPATCH(divide_col_reverse, y / x)
TENSOR_COL_DISPATCH(subtract_col, x - y)
TENSOR_COL_DISPATCH(subtract_col_reverse, y - x)

#undef TENSOR_COL_DISPATCH
#undef TENSOR_COL_DISPATCH_BODY

#define TENSOR_ROW_DISPATCH_BODY(op)                                             \
	tensor_double_view v_a;                                                      \
	zend_long nb = 0;                                                            \
	double * owned_b = NULL;                                                     \
	const double * vb = NULL;                                                    \
	                                                                             \
	if (UNEXPECTED(!tensor_view_resolve_arg(a, &v_a) ||                          \
		!tensor_view_doubles(b, &vb, &nb, &owned_b))) {                          \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	zend_long total = v_a.len;                                                   \
	zend_long nHat = zephir_get_intval(n_zval);                                  \
	                                                                             \
	if (UNEXPECTED(nHat < 1 || total < nHat)) {                                  \
		zephir_throw_exception_string(spl_ce_LengthException,                    \
			SL("Matrix and vector dimensions must agree."));                     \
		if (owned_b != NULL) { efree(owned_b); }                                 \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	zend_long m = total / nHat;                                                  \
	                                                                             \
	zval c;                                                                      \
	                                                                             \
	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, total, &c) == FAILURE)) { \
		if (owned_b != NULL) { efree(owned_b); }                                 \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	double * restrict vc = zephir_buffer_doubles(&c);                           \
	                                                                             \
	zend_long i, j;                                                              \
	                                                                             \
	if (v_a.contiguous) {                                                        \
		const double * restrict va = v_a.base + v_a.offset;                      \
	                                                                             \
		for (i = 0; i < m; ++i) {                                                \
			for (j = 0; j < nHat; ++j) {                                         \
				double x = va[i * nHat + j];                                     \
				double y = vb[j];                                                \
				vc[i * nHat + j] = op;                                           \
			}                                                                    \
		}                                                                        \
	} else {                                                                     \
		tensor_view_reader ra;                                                   \
		tensor_view_reader_init(&ra, &v_a);                                      \
	                                                                             \
		for (i = 0; i < m; ++i) {                                                \
			for (j = 0; j < nHat; ++j) {                                         \
				double x = tensor_view_reader_value(&ra);                        \
				double y = vb[j];                                                \
				vc[i * nHat + j] = op;                                           \
				tensor_view_reader_advance(&ra);                                 \
			}                                                                    \
		}                                                                        \
	}                                                                            \
	                                                                             \
	if (owned_b != NULL) { efree(owned_b); }                                     \
	zval_ptr_dtor(&c);

#define TENSOR_ROW_DISPATCH(name, op)                                            \
	static void tensor_##name##_sse(                                         \
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
	static tensor_dim_fn tensor_##name##_route = tensor_##name##_sse;      \
	                                                                             \
	void tensor_##name(                                                         \
		zval * return_value, zval * a, zval * b, zval * n_zval)                  \
	{                                                                            \
		tensor_##name##_route(return_value, a, b, n_zval);                        \
	}

TENSOR_ROW_DISPATCH(multiply_row, x * y)
TENSOR_ROW_DISPATCH(add_row, x + y)
TENSOR_ROW_DISPATCH(divide_row, x / y)
TENSOR_ROW_DISPATCH(divide_row_reverse, y / x)
TENSOR_ROW_DISPATCH(subtract_row, x - y)
TENSOR_ROW_DISPATCH(subtract_row_reverse, y - x)

#undef TENSOR_ROW_DISPATCH
#undef TENSOR_ROW_DISPATCH_BODY

#undef TENSOR_SCALAR
#undef TENSOR_SCALAR_DISPATCH
#undef TENSOR_SCALAR_BODY
#undef TENSOR_BINARY
#undef TENSOR_BINARY_DISPATCH
#undef TENSOR_BINARY_BODY

/**
 * Point every dispatched kernel in this file back at its SSE variant.
 */
void tensor_arithmetic_dispatch_sse_init(void)
{
	tensor_multiply_route = tensor_multiply_sse;
	tensor_divide_route = tensor_divide_sse;
	tensor_add_route = tensor_add_sse;
	tensor_subtract_route = tensor_subtract_sse;

	tensor_multiply_scalar_route = tensor_multiply_scalar_sse;
	tensor_divide_scalar_route = tensor_divide_scalar_sse;
	tensor_add_scalar_route = tensor_add_scalar_sse;
	tensor_subtract_scalar_route = tensor_subtract_scalar_sse;

	tensor_multiply_col_route = tensor_multiply_col_sse;
	tensor_add_col_route = tensor_add_col_sse;
	tensor_divide_col_route = tensor_divide_col_sse;
	tensor_divide_col_reverse_route = tensor_divide_col_reverse_sse;
	tensor_subtract_col_route = tensor_subtract_col_sse;
	tensor_subtract_col_reverse_route = tensor_subtract_col_reverse_sse;

	tensor_multiply_row_route = tensor_multiply_row_sse;
	tensor_add_row_route = tensor_add_row_sse;
	tensor_divide_row_route = tensor_divide_row_sse;
	tensor_divide_row_reverse_route = tensor_divide_row_reverse_sse;
	tensor_subtract_row_route = tensor_subtract_row_sse;
	tensor_subtract_row_reverse_route = tensor_subtract_row_reverse_sse;
}

/**
 * Point every dispatched kernel in this file at its AVX variant.
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

/**
 * Point every dispatched kernel in this file at its AVX-512 variant.
 */
void tensor_arithmetic_dispatch_avx512_init(void)
{
	tensor_multiply_route = tensor_multiply_avx512;
	tensor_divide_route = tensor_divide_avx512;
	tensor_add_route = tensor_add_avx512;
	tensor_subtract_route = tensor_subtract_avx512;

	tensor_multiply_scalar_route = tensor_multiply_scalar_avx512;
	tensor_divide_scalar_route = tensor_divide_scalar_avx512;
	tensor_add_scalar_route = tensor_add_scalar_avx512;
	tensor_subtract_scalar_route = tensor_subtract_scalar_avx512;

	tensor_multiply_col_route = tensor_multiply_col_avx512;
	tensor_add_col_route = tensor_add_col_avx512;
	tensor_divide_col_route = tensor_divide_col_avx512;
	tensor_divide_col_reverse_route = tensor_divide_col_reverse_avx512;
	tensor_subtract_col_route = tensor_subtract_col_avx512;
	tensor_subtract_col_reverse_route = tensor_subtract_col_reverse_avx512;

	tensor_multiply_row_route = tensor_multiply_row_avx512;
	tensor_add_row_route = tensor_add_row_avx512;
	tensor_divide_row_route = tensor_divide_row_avx512;
	tensor_divide_row_reverse_route = tensor_divide_row_reverse_avx512;
	tensor_subtract_row_route = tensor_subtract_row_avx512;
	tensor_subtract_row_reverse_route = tensor_subtract_row_reverse_avx512;
}
