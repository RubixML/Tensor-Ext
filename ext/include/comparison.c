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