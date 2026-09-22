#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <math.h>
#include <ext/spl/spl_exceptions.h>
#include "kernel/operators.h"
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
 * b[i]). `reverse` swaps the operands so that the same value function can
 * serve both the matrix and the column vector as the left-hand side. */

typedef double (*tensor_col_op)(double x, double y);

static double tensor_col_multiply_value(double x, double y)
{
	return x * y;
}

static double tensor_col_divide_value(double x, double y)
{
	return x / y;
}

static double tensor_col_add_value(double x, double y)
{
	return x + y;
}

static double tensor_col_subtract_value(double x, double y)
{
	return x - y;
}

static double tensor_col_pow_value(double x, double y)
{
	return pow(x, y);
}

static double tensor_col_mod_value(double x, double y)
{
	return fmod(x, y);
}

static void tensor_col_apply(zval * return_value, zval * a, zval * b, zval * n_zval, tensor_col_op fn, int reverse)
{
	zend_long n = 0, m = 0, total = 0;
	int ok_a = 0, ok_b = 0;

	double * va = tensor_tensorbuffer_doubles(a, &total, &ok_a);
	double * vb = tensor_tensorbuffer_doubles(b, &m, &ok_b);

	if (UNEXPECTED(!ok_a || !ok_b)) {
		return;
	}

	zend_long nHat = zephir_get_intval(n_zval);

	if (UNEXPECTED(nHat < 1 || total != m * nHat)) {
		zephir_throw_exception_string(spl_ce_LengthException,
			SL("Matrix and vector dimensions must agree."));
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, total, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	zend_long i, j;

	for (i = 0; i < m; ++i) {
		for (j = 0; j < nHat; ++j) {
			vc[i * nHat + j] = reverse
				? fn(vb[i], va[i * nHat + j])
				: fn(va[i * nHat + j], vb[i]);
		}
	}

	zval_ptr_dtor(&c);
}

void tensor_multiply_col(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_multiply_value, 0);
}

void tensor_add_col(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_add_value, 0);
}

void tensor_divide_col(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_divide_value, 0);
}

void tensor_divide_col_reverse(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_divide_value, 1);
}

void tensor_subtract_col(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_subtract_value, 0);
}

void tensor_subtract_col_reverse(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_subtract_value, 1);
}

void tensor_pow_col(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_pow_value, 0);
}

void tensor_pow_col_reverse(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_pow_value, 1);
}

void tensor_mod_col(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_mod_value, 0);
}

void tensor_mod_col_reverse(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_mod_value, 1);
}

/* Binary operation applied to every element of a matrix using a shared
 * row vector. The matrix is wrapped up in `a` (m * n doubles in row-major
 * order) and the row vector in `b` (n doubles) so that element (i, j) of the
 * result is op(a[i * n + j], b[j]). */

typedef double (*tensor_row_op)(double x, double y);

static double tensor_row_multiply_value(double x, double y)
{
	return x * y;
}

static double tensor_row_add_value(double x, double y)
{
	return x + y;
}

static double tensor_row_divide_value(double x, double y)
{
	return x / y;
}

static double tensor_row_subtract_value(double x, double y)
{
	return x - y;
}

static double tensor_row_pow_value(double x, double y)
{
	return pow(x, y);
}

static double tensor_row_mod_value(double x, double y)
{
	return fmod(x, y);
}

static void tensor_row_apply(zval * return_value, zval * a, zval * b, zval * n_zval, tensor_row_op fn)
{
	zend_long nHat = 0, m = 0, total = 0, nb = 0;
	int ok_a = 0, ok_b = 0;

	double * va = tensor_tensorbuffer_doubles(a, &total, &ok_a);
	double * vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);

	if (UNEXPECTED(!ok_a || !ok_b)) {
		return;
	}

	nHat = zephir_get_intval(n_zval);

	if (UNEXPECTED(nHat < 1 || nb != nHat || total < nHat || total % nHat != 0)) {
		zephir_throw_exception_string(spl_ce_LengthException,
			SL("Matrix and vector dimensions must agree."));
		return;
	}

	m = total / nHat;

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, total, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	zend_long i, j;

	for (i = 0; i < m; ++i) {
		for (j = 0; j < nHat; ++j) {
			vc[i * nHat + j] = fn(va[i * nHat + j], vb[j]);
		}
	}

	zval_ptr_dtor(&c);
}

void tensor_multiply_row(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_row_apply(return_value, a, b, n, tensor_row_multiply_value);
}

void tensor_add_row(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_row_apply(return_value, a, b, n, tensor_row_add_value);
}

void tensor_divide_row(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_row_apply(return_value, a, b, n, tensor_row_divide_value);
}

void tensor_subtract_row(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_row_apply(return_value, a, b, n, tensor_row_subtract_value);
}

void tensor_pow_row(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_row_apply(return_value, a, b, n, tensor_row_pow_value);
}

void tensor_mod_row(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_row_apply(return_value, a, b, n, tensor_row_mod_value);
}