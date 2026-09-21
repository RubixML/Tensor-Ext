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