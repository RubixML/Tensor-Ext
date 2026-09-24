#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <stdlib.h>
#include <ext/spl/spl_exceptions.h>
#include "kernel/main.h"
#include "kernel/buffer.h"
#include "kernel/exception.h"
#include "include/buffer.h"
#include "include/reductions.h"

#ifdef ZEPHIR_BUFFER_ENABLED

/**
 * Resolve the underlying raw buffer for the reduction operations, accepting
 * either a `TensorBuffer` decorator (returns a reference to its wrapped
 * buffer) or a raw `Buffer` object as-is (returns a reference to it). The
 * caller owns the reference written to `buf` and must release it with
 * `zval_ptr_dtor()`.
 *
 * @return 1 on success, 0 on failure (throwing).
 */
static int tensor_resolve_underlying_buffer(zval * obj, zval * buf)
{
	if (Z_TYPE_P(obj) == IS_OBJECT && Z_OBJCE_P(obj) == tensor_tensorbuffer_ce) {
		zval rv;
		zval *prop;

		ZVAL_UNDEF(&rv);

		prop = zend_read_property(tensor_tensorbuffer_ce, Z_OBJ_P(obj), "buffer", sizeof("buffer") - 1, 1, &rv);

		if (prop != NULL && prop != &rv) {
			ZVAL_COPY(buf, prop);
		} else {
			ZVAL_UNDEF(buf);
		}

		zval_ptr_dtor(&rv);
	} else {
		ZVAL_COPY(buf, obj);
	}

	if (UNEXPECTED(!zephir_is_buffer(buf))) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer or TensorBuffer object."));
		zval_ptr_dtor(buf);
		ZVAL_UNDEF(buf);
		return 0;
	}

	return 1;
}

static double tensor_buffer_sum_values(const uint8_t kind, const void * data, const zend_long len)
{
	double acc = 0.0;
	zend_long i;

	if (kind == ZEPHIR_BUFFER_LONG) {
		const zend_long * ptr = (const zend_long *) data;

		for (i = 0; i < len; ++i) {
			acc += (double) ptr[i];
		}
	} else {
		const double * ptr = (const double *) data;

		for (i = 0; i < len; ++i) {
			acc += ptr[i];
		}
	}

	return acc;
}

static double tensor_buffer_product_values(const uint8_t kind, const void * data, const zend_long len)
{
	double acc = 1.0;
	zend_long i;

	if (kind == ZEPHIR_BUFFER_LONG) {
		const zend_long * ptr = (const zend_long *) data;

		for (i = 0; i < len; ++i) {
			acc *= (double) ptr[i];
		}
	} else {
		const double * ptr = (const double *) data;

		for (i = 0; i < len; ++i) {
			acc *= ptr[i];
		}
	}

	return acc;
}

static void tensor_buffer_extreme(const uint8_t kind, const void * data, const zend_long len, const int find_min, double * value, zend_long * index)
{
	zend_long i;

	if (kind == ZEPHIR_BUFFER_LONG) {
		const zend_long * ptr = (const zend_long *) data;
		zend_long best = ptr[0];

		for (i = 1; i < len; ++i) {
			if (find_min ? ptr[i] < best : ptr[i] > best) {
				best = ptr[i];
				*index = i;
			}
		}

		if (value != NULL) {
			*value = (double) best;
		}
	} else {
		const double * ptr = (const double *) data;
		double best = ptr[0];

		for (i = 1; i < len; ++i) {
			if (find_min ? ptr[i] < best : ptr[i] > best) {
				best = ptr[i];
				*index = i;
			}
		}

		if (value != NULL) {
			*value = best;
		}
	}
}

/**
 * Return the sum of the elements of a Buffer.
 *
 * @param return_value
 * @param obj
 */
void tensor_buffer_sum(zval * return_value, zval * obj)
{
	zval buffer;
	uint8_t kind;
	zend_long len;

	if (!tensor_resolve_underlying_buffer(obj, &buffer)) {
		return;
	}

	kind = zephir_buffer_kind(&buffer);
	len = zephir_buffer_len(&buffer);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		zval_ptr_dtor(&buffer);
		return;
	}

	if (len == 0) {
		RETVAL_DOUBLE(0.0);
		zval_ptr_dtor(&buffer);
		return;
	}

	RETVAL_DOUBLE(tensor_buffer_sum_values(kind, kind == ZEPHIR_BUFFER_LONG
		? (const void *) zephir_buffer_longs(&buffer) : (const void *) zephir_buffer_doubles(&buffer), len));
	zval_ptr_dtor(&buffer);
}

/**
 * Return the product of the elements of a Buffer.
 *
 * @param return_value
 * @param obj
 */
void tensor_buffer_product(zval * return_value, zval * obj)
{
	zval buffer;
	uint8_t kind;
	zend_long len;

	if (!tensor_resolve_underlying_buffer(obj, &buffer)) {
		return;
	}

	kind = zephir_buffer_kind(&buffer);
	len = zephir_buffer_len(&buffer);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		zval_ptr_dtor(&buffer);
		return;
	}

	if (len == 0) {
		RETVAL_DOUBLE(1.0);
		zval_ptr_dtor(&buffer);
		return;
	}

	RETVAL_DOUBLE(tensor_buffer_product_values(kind, kind == ZEPHIR_BUFFER_LONG
		? (const void *) zephir_buffer_longs(&buffer) : (const void *) zephir_buffer_doubles(&buffer), len));
	zval_ptr_dtor(&buffer);
}

/**
 * Return the minimum element of a Buffer.
 *
 * @param return_value
 * @param obj
 */
void tensor_buffer_min(zval * return_value, zval * obj)
{
	zval buffer;
	uint8_t kind;
	zend_long len;
	double value = 0.0;
	zend_long index = 0;

	if (!tensor_resolve_underlying_buffer(obj, &buffer)) {
		return;
	}

	kind = zephir_buffer_kind(&buffer);
	len = zephir_buffer_len(&buffer);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		zval_ptr_dtor(&buffer);
		return;
	}

	if (UNEXPECTED(len == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Cannot compute the minimum of an empty buffer."));
		zval_ptr_dtor(&buffer);
		return;
	}

	tensor_buffer_extreme(kind, kind == ZEPHIR_BUFFER_LONG
		? (const void *) zephir_buffer_longs(&buffer) : (const void *) zephir_buffer_doubles(&buffer), len, 1, &value, &index);

	zval_ptr_dtor(&buffer);

	RETVAL_DOUBLE(value);
}

/**
 * Return the maximum element of a Buffer.
 *
 * @param return_value
 * @param obj
 */
void tensor_buffer_max(zval * return_value, zval * obj)
{
	zval buffer;
	uint8_t kind;
	zend_long len;
	double value = 0.0;
	zend_long index = 0;

	if (!tensor_resolve_underlying_buffer(obj, &buffer)) {
		return;
	}

	kind = zephir_buffer_kind(&buffer);
	len = zephir_buffer_len(&buffer);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		zval_ptr_dtor(&buffer);
		return;
	}

	if (UNEXPECTED(len == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Cannot compute the maximum of an empty buffer."));
		zval_ptr_dtor(&buffer);
		return;
	}

	tensor_buffer_extreme(kind, kind == ZEPHIR_BUFFER_LONG
		? (const void *) zephir_buffer_longs(&buffer) : (const void *) zephir_buffer_doubles(&buffer), len, 0, &value, &index);

	zval_ptr_dtor(&buffer);

	RETVAL_DOUBLE(value);
}

/**
 * Return the index of the minimum element of a Buffer.
 *
 * @param return_value
 * @param obj
 */
void tensor_buffer_argmin(zval * return_value, zval * obj)
{
	zval buffer;
	uint8_t kind;
	zend_long len;
	zend_long index = 0;

	if (!tensor_resolve_underlying_buffer(obj, &buffer)) {
		return;
	}

	kind = zephir_buffer_kind(&buffer);
	len = zephir_buffer_len(&buffer);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		zval_ptr_dtor(&buffer);
		return;
	}

	if (UNEXPECTED(len == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Cannot compute the argmin of an empty buffer."));
		zval_ptr_dtor(&buffer);
		return;
	}

	tensor_buffer_extreme(kind, kind == ZEPHIR_BUFFER_LONG
		? (const void *) zephir_buffer_longs(&buffer) : (const void *) zephir_buffer_doubles(&buffer), len, 1, NULL, &index);

	zval_ptr_dtor(&buffer);

	RETVAL_LONG(index);
}

/**
 * Return the index of the maximum element of a Buffer.
 *
 * @param return_value
 * @param obj
 */
void tensor_buffer_argmax(zval * return_value, zval * obj)
{
	zval buffer;
	uint8_t kind;
	zend_long len;
	zend_long index = 0;

	if (!tensor_resolve_underlying_buffer(obj, &buffer)) {
		return;
	}

	kind = zephir_buffer_kind(&buffer);
	len = zephir_buffer_len(&buffer);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		zval_ptr_dtor(&buffer);
		return;
	}

	if (UNEXPECTED(len == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Cannot compute the argmax of an empty buffer."));
		zval_ptr_dtor(&buffer);
		return;
	}

	tensor_buffer_extreme(kind, kind == ZEPHIR_BUFFER_LONG
		? (const void *) zephir_buffer_longs(&buffer) : (const void *) zephir_buffer_doubles(&buffer), len, 0, NULL, &index);

	zval_ptr_dtor(&buffer);

	RETVAL_LONG(index);
}

#endif