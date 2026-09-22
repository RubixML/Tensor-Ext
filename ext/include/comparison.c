#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <ext/spl/spl_exceptions.h>
#include "kernel/operators.h"
#include "kernel/buffer.h"
#include "include/buffer.h"

void tensor_equal(zval * return_value, zval * a, zval * b)
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
		vc[i] = va[i] == vb[i] ? 1.0 : 0.0;
	}

	zval_ptr_dtor(&c);
}

void tensor_not_equal(zval * return_value, zval * a, zval * b)
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
		vc[i] = va[i] != vb[i] ? 1.0 : 0.0;
	}

	zval_ptr_dtor(&c);
}

void tensor_greater(zval * return_value, zval * a, zval * b)
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
		vc[i] = va[i] > vb[i] ? 1.0 : 0.0;
	}

	zval_ptr_dtor(&c);
}

void tensor_greater_equal(zval * return_value, zval * a, zval * b)
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
		vc[i] = va[i] >= vb[i] ? 1.0 : 0.0;
	}

	zval_ptr_dtor(&c);
}

void tensor_less(zval * return_value, zval * a, zval * b)
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
		vc[i] = va[i] < vb[i] ? 1.0 : 0.0;
	}

	zval_ptr_dtor(&c);
}

void tensor_less_equal(zval * return_value, zval * a, zval * b)
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
		vc[i] = va[i] <= vb[i] ? 1.0 : 0.0;
	}

	zval_ptr_dtor(&c);
}

void tensor_equal_scalar(zval * return_value, zval * a, zval * b)
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
		vc[i] = va[i] == ab ? 1.0 : 0.0;
	}

	zval_ptr_dtor(&c);
}

void tensor_not_equal_scalar(zval * return_value, zval * a, zval * b)
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
		vc[i] = va[i] != ab ? 1.0 : 0.0;
	}

	zval_ptr_dtor(&c);
}

void tensor_greater_scalar(zval * return_value, zval * a, zval * b)
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
		vc[i] = va[i] > ab ? 1.0 : 0.0;
	}

	zval_ptr_dtor(&c);
}

void tensor_greater_equal_scalar(zval * return_value, zval * a, zval * b)
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
		vc[i] = va[i] >= ab ? 1.0 : 0.0;
	}

	zval_ptr_dtor(&c);
}

void tensor_less_scalar(zval * return_value, zval * a, zval * b)
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
		vc[i] = va[i] < ab ? 1.0 : 0.0;
	}

	zval_ptr_dtor(&c);
}

void tensor_less_equal_scalar(zval * return_value, zval * a, zval * b)
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
		vc[i] = va[i] <= ab ? 1.0 : 0.0;
	}

	zval_ptr_dtor(&c);
}

/* Comparison applied to every element of a matrix row. The matrix is wrapped
 * up in `a` (m * n doubles in row-major order) and the column vector in `b`
 * (m doubles) so that element (i, j) of the result is 1.0 when the comparison
 * op(a[i * n + j], b[i]) holds and 0.0 otherwise. `reverse` swaps the operands
 * so that the same predicate can serve both the matrix and the column vector
 * as the left-hand side. */

typedef double (*tensor_col_op)(double x, double y);

static double tensor_col_equal_value(double x, double y)
{
	return x == y ? 1.0 : 0.0;
}

static double tensor_col_not_equal_value(double x, double y)
{
	return x != y ? 1.0 : 0.0;
}

static double tensor_col_greater_value(double x, double y)
{
	return x > y ? 1.0 : 0.0;
}

static double tensor_col_greater_equal_value(double x, double y)
{
	return x >= y ? 1.0 : 0.0;
}

static double tensor_col_less_value(double x, double y)
{
	return x < y ? 1.0 : 0.0;
}

static double tensor_col_less_equal_value(double x, double y)
{
	return x <= y ? 1.0 : 0.0;
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

void tensor_equal_col(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_equal_value, 0);
}

void tensor_not_equal_col(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_not_equal_value, 0);
}

void tensor_greater_col(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_greater_value, 0);
}

void tensor_greater_col_reverse(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_greater_value, 1);
}

void tensor_greater_equal_col(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_greater_equal_value, 0);
}

void tensor_greater_equal_col_reverse(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_greater_equal_value, 1);
}

void tensor_less_col(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_less_value, 0);
}

void tensor_less_col_reverse(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_less_value, 1);
}

void tensor_less_equal_col(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_less_equal_value, 0);
}

void tensor_less_equal_col_reverse(zval * return_value, zval * a, zval * b, zval * n)
{
	tensor_col_apply(return_value, a, b, n, tensor_col_less_equal_value, 1);
}