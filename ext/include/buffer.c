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
#include "include/buffer.h"
#include "../tensor/exceptions/invalidargumentexception.zep.h"

#ifdef ZEPHIR_BUFFER_ENABLED

zend_class_entry * tensor_buffer_ce;

/**
 * Allocate a new `Tensor\TensorBuffer` wrapping a fresh zero-filled double
 * buffer of `len` elements (see include/buffer.h).
 */
int tensor_tensorbuffer_create(zval * ret, zend_long len, zval * buffer)
{
	if (UNEXPECTED(zephir_buffer_create(buffer, len, ZEPHIR_BUFFER_DOUBLE) == FAILURE)) {
		ZVAL_UNDEF(buffer);
		return FAILURE;
	}

	object_init_ex(ret, tensor_tensorbuffer_ce);

	/* zend_update_property() sets the engine's fake scope to the owner class
	 * so protected property writes from C pass the PHP 8.4 access check. */
	zend_update_property(tensor_tensorbuffer_ce, Z_OBJ_P(ret), "buffer", sizeof("buffer") - 1, buffer);

	if (UNEXPECTED(EG(exception))) {
		zval_ptr_dtor(ret);
		ZVAL_UNDEF(ret);
		zval_ptr_dtor(buffer);
		ZVAL_UNDEF(buffer);
		return FAILURE;
	}

	return SUCCESS;
}

/**
 * Allocate a new `Tensor\TensorBuffer` decorator wrapping a `Tensor\Buffer`
 * built from a PHP array of values, casting every element to a double (see
 * include/buffer.h).
 */
void tensor_buffer_from_array(zval * ret, zval * arr)
{
	zval buffer;

	if (UNEXPECTED(zephir_buffer_create_from_array(&buffer, arr, ZEPHIR_BUFFER_DOUBLE) == FAILURE)) {
		ZVAL_NULL(ret);
		return;
	}

	object_init_ex(ret, tensor_tensorbuffer_ce);

	zend_update_property(tensor_tensorbuffer_ce, Z_OBJ_P(ret), "buffer", sizeof("buffer") - 1, &buffer);

	if (UNEXPECTED(EG(exception))) {
		zval_ptr_dtor(ret);
		ZVAL_UNDEF(ret);
		zval_ptr_dtor(&buffer);
		ZVAL_UNDEF(&buffer);
		return;
	}

	zval_ptr_dtor(&buffer);
}

/**
 * Unwrap the double buffer hidden inside a `Tensor\TensorBuffer` object
 * (see include/buffer.h).
 */
double * tensor_tensorbuffer_doubles(zval * obj, zend_long * len, int * success)
{
	zval rv;

	ZVAL_UNDEF(&rv);

	if (success != NULL) {
		*success = 0;
	}

	if (len != NULL) {
		*len = 0;
	}

	if (UNEXPECTED(Z_TYPE_P(obj) != IS_OBJECT || Z_OBJCE_P(obj) != tensor_tensorbuffer_ce)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Tensor\\TensorBuffer object."));
		return NULL;
	}

	zval *prop = zend_read_property(tensor_tensorbuffer_ce, Z_OBJ_P(obj), "buffer", sizeof("buffer") - 1, 1, &rv);

	if (prop != NULL && prop != &rv) {
		ZVAL_COPY(&rv, prop);
	}

	if (UNEXPECTED(!zephir_is_buffer(&rv))) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument is not wrapping a Buffer object."));
		goto cleanup;
	}

	if (UNEXPECTED(zephir_buffer_kind(&rv) != ZEPHIR_BUFFER_DOUBLE)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must wrap a buffer of type double."));
		goto cleanup;
	}

	zend_long n = zephir_buffer_len(&rv);
	double * ptr = zephir_buffer_doubles(&rv);

	zval_ptr_dtor(&rv);

	if (len != NULL) {
		*len = n;
	}

	if (success != NULL) {
		*success = 1;
	}

	return ptr;

cleanup:
	zval_ptr_dtor(&rv);

	return NULL;
}

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

/**
 * Return a new Buffer populated with a strided slice of the given buffer.
 *
 * @param return_value
 * @param obj
 * @param offset
 * @param length
 * @param stride
 */
void tensor_buffer_slice_strided(zval * return_value, zval * obj, zval * offset, zval * length, zval * stride)
{
	uint8_t kind = zephir_buffer_kind(obj);
	zend_long len = zephir_buffer_len(obj);
	zend_long offsetHat = zephir_get_intval(offset);
	zend_long lengthHat = zephir_get_intval(length);
	zend_long strideHat = zephir_get_intval(stride);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		return;
	}

	if (UNEXPECTED(offsetHat < 0 || lengthHat < 0 || strideHat < 1)) {
		zephir_throw_exception_string(spl_ce_OutOfBoundsException,
			SL("Slice offset, length, and stride must be within the bounds of the buffer."));
		return;
	}

	if (lengthHat > 0) {
		zend_long last = len - 1 - offsetHat;

		/* Integer division on non-negative operands is exact and immune to the
		 * signed overflow of (lengthHat - 1) * strideHat, which wraps for huge
		 * arguments and previously let the guard pass while the copy below read
		 * far out of bounds. */
		if (UNEXPECTED(last < 0 || (lengthHat - 1) > last / strideHat)) {
			zephir_throw_exception_string(spl_ce_OutOfBoundsException,
				SL("Slice offset, length, and stride must be within the bounds of the buffer."));
			return;
		}
	}

	if (UNEXPECTED(zephir_buffer_create(return_value, lengthHat, kind) == FAILURE)) {
		return;
	}

	if (lengthHat == 0) {
		return;
	}

	zend_long i;

	if (kind == ZEPHIR_BUFFER_LONG) {
		const zend_long * src = zephir_buffer_longs(obj);
		zend_long * dst = zephir_buffer_longs(return_value);
		zend_long index = offsetHat;

		for (i = 0; i < lengthHat; ++i) {
			dst[i] = src[index];

			if (i + 1 < lengthHat) {
				index += strideHat;
			}
		}
	} else {
		const double * src = zephir_buffer_doubles(obj);
		double * dst = zephir_buffer_doubles(return_value);
		zend_long index = offsetHat;

		for (i = 0; i < lengthHat; ++i) {
			dst[i] = src[index];

			if (i + 1 < lengthHat) {
				index += strideHat;
			}
		}
	}
}

/**
 * Return a new Buffer populated with a copy of this buffer concatenated with
 * the given buffers.
 *
 * @param return_value
 * @param obj
 * @param others
 */
void tensor_buffer_concat(zval * return_value, zval * obj, zval * others)
{
	uint8_t kind = zephir_buffer_kind(obj);
	zend_long len = zephir_buffer_len(obj);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		return;
	}

	HashTable * ht;

	if (UNEXPECTED((ht = Z_ARRVAL_P(others)) == NULL)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be an array of Buffer objects."));
		return;
	}

	zend_long total = len;
	zval * other;

	ZEND_HASH_FOREACH_VAL(ht, other) {
		if (UNEXPECTED(!zephir_is_buffer(other) || zephir_buffer_kind(other) != kind)) {
			zephir_throw_exception_string(spl_ce_InvalidArgumentException,
				SL("All buffers must be the same type as the given buffer."));
			return;
		}

		total += zephir_buffer_len(other);
	} ZEND_HASH_FOREACH_END();

	if (UNEXPECTED(zephir_buffer_create(return_value, total, kind) == FAILURE)) {
		return;
	}

	if (len > 0) {
		if (kind == ZEPHIR_BUFFER_LONG) {
			memcpy(zephir_buffer_longs(return_value), zephir_buffer_longs(obj),
				(size_t) len * sizeof(zend_long));
		} else {
			memcpy(zephir_buffer_doubles(return_value), zephir_buffer_doubles(obj),
				(size_t) len * sizeof(double));
		}
	}

	zend_long pos = len;

	ZEND_HASH_FOREACH_VAL(ht, other) {
		zend_long other_len = zephir_buffer_len(other);

		if (other_len > 0) {
			if (kind == ZEPHIR_BUFFER_LONG) {
				memcpy(zephir_buffer_longs(return_value) + pos, zephir_buffer_longs(other),
					(size_t) other_len * sizeof(zend_long));
			} else {
				memcpy(zephir_buffer_doubles(return_value) + pos, zephir_buffer_doubles(other),
					(size_t) other_len * sizeof(double));
			}
		}

		pos += other_len;
	} ZEND_HASH_FOREACH_END();
}

/**
 * Return a new array of Buffers splitting the given buffer into chunks of
 * the given length.
 *
 * @param return_value
 * @param obj
 * @param chunk_length
 */
void tensor_buffer_split(zval * return_value, zval * obj, zval * chunk_length)
{
	uint8_t kind = zephir_buffer_kind(obj);
	zend_long len = zephir_buffer_len(obj);
	zend_long chunkHat = zephir_get_intval(chunk_length);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		return;
	}

	if (UNEXPECTED(chunkHat < 1)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Chunk length must be greater than 0."));
		return;
	}

	zend_long chunks;

	if (UNEXPECTED(len == 0)) {
		chunks = 0;
	} else if (UNEXPECTED(chunkHat > len)) {
		/* The whole buffer fits in one chunk; avoids the overflow of
		 * len + chunkHat - 1 for a chunk length near LONG_MAX. */
		chunks = 1;
	} else {
		/* Both operands are at most 2 * len, so the sum cannot overflow. */
		chunks = (len + chunkHat - 1) / chunkHat;
	}

	zend_long i;

	array_init_size(return_value, (zend_ulong) chunks);

	for (i = 0; i < chunks; ++i) {
		zend_long start = i * chunkHat;
		zend_long chunk_len = len - start < chunkHat ? len - start : chunkHat;
		zval buffer;

		if (UNEXPECTED(zephir_buffer_create(&buffer, chunk_len, kind) == FAILURE)) {
			return;
		}

		if (chunk_len > 0) {
			if (kind == ZEPHIR_BUFFER_LONG) {
				memcpy(zephir_buffer_longs(&buffer), zephir_buffer_longs(obj) + start,
					(size_t) chunk_len * sizeof(zend_long));
			} else {
				memcpy(zephir_buffer_doubles(&buffer), zephir_buffer_doubles(obj) + start,
					(size_t) chunk_len * sizeof(double));
			}
		}

		add_next_index_zval(return_value, &buffer);
	}
}

/**
 * Return a new Buffer populated with the elements of the given buffer
 * repeated the given number of times.
 *
 * @param return_value
 * @param obj
 * @param times
 */
void tensor_buffer_repeat(zval * return_value, zval * obj, zval * times)
{
	uint8_t kind = zephir_buffer_kind(obj);
	zend_long len = zephir_buffer_len(obj);
	zend_long timesHat = zephir_get_intval(times);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		return;
	}

	if (UNEXPECTED(timesHat < 1)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Times must be greater than 0."));
		return;
	}

	/* Reject before len * timesHat wraps; the product is then guaranteed to
	 * fit, so every i * len destination offset stays in bounds. */
	if (UNEXPECTED(len > 0 && timesHat > ZEND_LONG_MAX / len)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Repeat count must not overflow the buffer length."));
		return;
	}

	zend_long total = len * timesHat;
	zend_long i;

	if (UNEXPECTED(zephir_buffer_create(return_value, total, kind) == FAILURE)) {
		return;
	}

	if (len > 0) {
		for (i = 0; i < timesHat; ++i) {
			if (kind == ZEPHIR_BUFFER_LONG) {
				memcpy(zephir_buffer_longs(return_value) + i * len, zephir_buffer_longs(obj),
					(size_t) len * sizeof(zend_long));
			} else {
				memcpy(zephir_buffer_doubles(return_value) + i * len, zephir_buffer_doubles(obj),
					(size_t) len * sizeof(double));
			}
		}
	}
}

#endif