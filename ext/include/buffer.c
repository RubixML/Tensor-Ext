#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <stdlib.h>
#include <ext/spl/spl_exceptions.h>
#include "kernel/main.h"
#include "kernel/buffer.h"
#include "kernel/exception.h"
#include "kernel/operators.h"

#ifdef ZEPHIR_BUFFER_ENABLED

zend_class_entry * tensor_buffer_ce;

static int tensor_double_cmp(const void * a, const void * b)
{
	double da = *(const double *) a;
	double db = *(const double *) b;

	return (da > db) - (da < db);
}

static int tensor_double_cmp_desc(const void * a, const void * b)
{
	return tensor_double_cmp(b, a);
}

static int tensor_long_cmp(const void * a, const void * b)
{
	zend_long la = *(const zend_long *) a;
	zend_long lb = *(const zend_long *) b;

	return (la > lb) - (la < lb);
}

static int tensor_long_cmp_desc(const void * a, const void * b)
{
	return tensor_long_cmp(b, a);
}

/**
 * Sort the elements of a Buffer in place.
 *
 * @param return_value
 * @param obj
 * @param ascending
 */
void tensor_buffer_sort(zval * return_value, zval * obj, zval * ascending)
{
	uint8_t kind = zephir_buffer_kind(obj);
	zend_long len = zephir_buffer_len(obj);
	int ascending_order = zend_is_true(ascending);

	if (UNEXPECTED(kind == 0 || len < 2)) {
		RETURN_NULL();
	}

	if (kind == ZEPHIR_BUFFER_LONG) {
		qsort((void *) zephir_buffer_longs(obj), (size_t) len, sizeof(zend_long),
			ascending_order ? tensor_long_cmp : tensor_long_cmp_desc);
	} else {
		qsort((void *) zephir_buffer_doubles(obj), (size_t) len, sizeof(double),
			ascending_order ? tensor_double_cmp : tensor_double_cmp_desc);
	}

	RETURN_NULL();
}

/**
 * Return a new Buffer populated with a slice of the given buffer.
 *
 * @param return_value
 * @param obj
 * @param offset
 * @param length
 */
void tensor_buffer_slice(zval * return_value, zval * obj, zval * offset, zval * length)
{
	uint8_t kind = zephir_buffer_kind(obj);
	zend_long len = zephir_buffer_len(obj);
	zend_long offsetHat = zephir_get_intval(offset);
	zend_long lengthHat = zephir_get_intval(length);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		return;
	}

	if (UNEXPECTED(offsetHat < 0 || lengthHat < 0 || offsetHat > len - lengthHat)) {
		zephir_throw_exception_string(spl_ce_OutOfBoundsException,
			SL("Slice offset and length must be within the bounds of the buffer."));
		return;
	}

	if (UNEXPECTED(zephir_buffer_create(return_value, lengthHat, kind) == FAILURE)) {
		return;
	}

	if (lengthHat == 0) {
		return;
	}

	if (kind == ZEPHIR_BUFFER_LONG) {
		memcpy(zephir_buffer_longs(return_value), zephir_buffer_longs(obj) + offsetHat,
			(size_t) lengthHat * sizeof(zend_long));
	} else {
		memcpy(zephir_buffer_doubles(return_value), zephir_buffer_doubles(obj) + offsetHat,
			(size_t) lengthHat * sizeof(double));
	}
}

#endif