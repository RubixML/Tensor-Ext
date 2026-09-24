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

void tensor_multiply(zval * return_value, zval * a, zval * b)
{
	zend_long i;
	zend_long na = 0, nb = 0;
	int ok_a = 0, ok_b = 0;

	double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);
	double * vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);

	if (UNEXPECTED(!ok_a || !ok_b)) {
		return;
	}

	if (UNEXPECTED(na != nb)) {
		zephir_throw_exception_string(spl_ce_LengthException,
			SL("Input buffers must be the same length."));
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < na; ++i) {
		vc[i] = va[i] * vb[i];
	}

	zval_ptr_dtor(&c);
}

void tensor_divide(zval * return_value, zval * a, zval * b)
{
	zend_long i;
	zend_long na = 0, nb = 0;
	int ok_a = 0, ok_b = 0;

	double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);
	double * vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);

	if (UNEXPECTED(!ok_a || !ok_b)) {
		return;
	}

	if (UNEXPECTED(na != nb)) {
		zephir_throw_exception_string(spl_ce_LengthException,
			SL("Input buffers must be the same length."));
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < na; ++i) {
		vc[i] = va[i] / vb[i];
	}

	zval_ptr_dtor(&c);
}

void tensor_add(zval * return_value, zval * a, zval * b)
{
	zend_long i;
	zend_long na = 0, nb = 0;
	int ok_a = 0, ok_b = 0;

	double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);
	double * vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);

	if (UNEXPECTED(!ok_a || !ok_b)) {
		return;
	}

	if (UNEXPECTED(na != nb)) {
		zephir_throw_exception_string(spl_ce_LengthException,
			SL("Input buffers must be the same length."));
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < na; ++i) {
		vc[i] = va[i] + vb[i];
	}

	zval_ptr_dtor(&c);
}

void tensor_subtract(zval * return_value, zval * a, zval * b)
{
	zend_long i;
	zend_long na = 0, nb = 0;
	int ok_a = 0, ok_b = 0;

	double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);
	double * vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);

	if (UNEXPECTED(!ok_a || !ok_b)) {
		return;
	}

	if (UNEXPECTED(na != nb)) {
		zephir_throw_exception_string(spl_ce_LengthException,
			SL("Input buffers must be the same length."));
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < na; ++i) {
		vc[i] = va[i] - vb[i];
	}

	zval_ptr_dtor(&c);
}

void tensor_pow(zval * return_value, zval * a, zval * b)
{
	zend_long i;
	zend_long na = 0, nb = 0;
	int ok_a = 0, ok_b = 0;

	double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);
	double * vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);

	if (UNEXPECTED(!ok_a || !ok_b)) {
		return;
	}

	if (UNEXPECTED(na != nb)) {
		zephir_throw_exception_string(spl_ce_LengthException,
			SL("Input buffers must be the same length."));
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < na; ++i) {
		vc[i] = pow(va[i], vb[i]);
	}

	zval_ptr_dtor(&c);
}

void tensor_mod(zval * return_value, zval * a, zval * b)
{
	zend_long i;
	zend_long na = 0, nb = 0;
	int ok_a = 0, ok_b = 0;

	double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);
	double * vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);

	if (UNEXPECTED(!ok_a || !ok_b)) {
		return;
	}

	if (UNEXPECTED(na != nb)) {
		zephir_throw_exception_string(spl_ce_LengthException,
			SL("Input buffers must be the same length."));
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < na; ++i) {
		vc[i] = fmod(va[i], vb[i]);
	}

	zval_ptr_dtor(&c);
}

void tensor_multiply_scalar(zval * return_value, zval * a, zval * b)
{
	zend_long i;
	zend_long na = 0;
	int ok_a = 0;

	double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);

	if (UNEXPECTED(!ok_a)) {
		return;
	}

	double ab = zephir_get_doubleval(b);

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < na; ++i) {
		vc[i] = va[i] * ab;
	}

	zval_ptr_dtor(&c);
}

void tensor_divide_scalar(zval * return_value, zval * a, zval * b)
{
	zend_long i;
	zend_long na = 0;
	int ok_a = 0;

	double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);

	if (UNEXPECTED(!ok_a)) {
		return;
	}

	double ab = zephir_get_doubleval(b);

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < na; ++i) {
		vc[i] = va[i] / ab;
	}

	zval_ptr_dtor(&c);
}

void tensor_add_scalar(zval * return_value, zval * a, zval * b)
{
	zend_long i;
	zend_long na = 0;
	int ok_a = 0;

	double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);

	if (UNEXPECTED(!ok_a)) {
		return;
	}

	double ab = zephir_get_doubleval(b);

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < na; ++i) {
		vc[i] = va[i] + ab;
	}

	zval_ptr_dtor(&c);
}

void tensor_subtract_scalar(zval * return_value, zval * a, zval * b)
{
	zend_long i;
	zend_long na = 0;
	int ok_a = 0;

	double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);

	if (UNEXPECTED(!ok_a)) {
		return;
	}

	double ab = zephir_get_doubleval(b);

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < na; ++i) {
		vc[i] = va[i] - ab;
	}

	zval_ptr_dtor(&c);
}

void tensor_pow_scalar(zval * return_value, zval * a, zval * b)
{
	zend_long i;
	zend_long na = 0;
	int ok_a = 0;

	double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);

	if (UNEXPECTED(!ok_a)) {
		return;
	}

	double ab = zephir_get_doubleval(b);

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < na; ++i) {
		vc[i] = pow(va[i], ab);
	}

	zval_ptr_dtor(&c);
}

void tensor_mod_scalar(zval * return_value, zval * a, zval * b)
{
	zend_long i;
	zend_long na = 0;
	int ok_a = 0;

	double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);

	if (UNEXPECTED(!ok_a)) {
		return;
	}

	double ab = zephir_get_doubleval(b);

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, na, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < na; ++i) {
		vc[i] = fmod(va[i], ab);
	}

	zval_ptr_dtor(&c);
}

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

TENSOR_COL_APPLY(multiply_col, va[i * nHat + j] * vb[i])
TENSOR_COL_APPLY(add_col, va[i * nHat + j] + vb[i])
TENSOR_COL_APPLY(divide_col, va[i * nHat + j] / vb[i])
TENSOR_COL_APPLY(divide_col_reverse, vb[i] / va[i * nHat + j])
TENSOR_COL_APPLY(subtract_col, va[i * nHat + j] - vb[i])
TENSOR_COL_APPLY(subtract_col_reverse, vb[i] - va[i * nHat + j])
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

TENSOR_ROW_APPLY(multiply_row, va[i * nHat + j] * vb[j])
TENSOR_ROW_APPLY(add_row, va[i * nHat + j] + vb[j])
TENSOR_ROW_APPLY(divide_row, va[i * nHat + j] / vb[j])
TENSOR_ROW_APPLY(divide_row_reverse, vb[j] / va[i * nHat + j])
TENSOR_ROW_APPLY(subtract_row, va[i * nHat + j] - vb[j])
TENSOR_ROW_APPLY(subtract_row_reverse, vb[j] - va[i * nHat + j])
TENSOR_ROW_APPLY(pow_row, pow(va[i * nHat + j], vb[j]))
TENSOR_ROW_APPLY(pow_row_reverse, pow(vb[j], va[i * nHat + j]))
TENSOR_ROW_APPLY(mod_row, fmod(va[i * nHat + j], vb[j]))
TENSOR_ROW_APPLY(mod_row_reverse, fmod(vb[j], va[i * nHat + j]))

#undef TENSOR_ROW_APPLY