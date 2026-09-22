
#ifdef HAVE_CONFIG_H
#include "../ext_config.h"
#endif

#include <php.h>
#include "../php_ext.h"
#include "../ext.h"

#include <Zend/zend_operators.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>

#include "kernel/main.h"
#include "kernel/object.h"
#include "kernel/memory.h"
#include "kernel/fcall.h"
#include "kernel/array.h"
#include "kernel/operators.h"
#include "kernel/exception.h"
#include "kernel/concat.h"
#include "kernel/string.h"
#include "include/buffer.h"


/**
 * TensorBuffer
 *
 * A decorator that wraps the kernel Buffer class and provides additional
 * operations such as sorting and slicing.
 *
 * @category    Scientific Computing
 * @package     Rubix/Tensor
 * @author      Andrew DalPino
 */
ZEPHIR_INIT_CLASS(Tensor_TensorBuffer)
{
	ZEPHIR_REGISTER_CLASS(Tensor, TensorBuffer, tensor, tensorbuffer, tensor_tensorbuffer_method_entry, 0);

	/**
	 * The underlying buffer being decorated.
	 *
	 * @var \Tensor\Buffer
	 */
	zend_declare_property_null(tensor_tensorbuffer_ce, SL("buffer"), ZEND_ACC_PROTECTED);
	return SUCCESS;
}

/**
 * @param \Tensor\Buffer buffer
 */
PHP_METHOD(Tensor_TensorBuffer, __construct)
{
	zval *buffer, buffer_sub;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&buffer_sub);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(buffer, zephir_get_internal_ce(SL("tensor\\buffer")))
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(1, 0, &buffer);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_0, 20, buffer);
}

/**
 * Return the underlying buffer.
 *
 * @return \Tensor\Buffer
 */
PHP_METHOD(Tensor_TensorBuffer, asBuffer)
{

	RETURN_MEMBER(getThis(), "buffer");
}

/**
 * Return the number of elements in the buffer.
 *
 * @return int
 */
PHP_METHOD(Tensor_TensorBuffer, count)
{
	zval _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_METHOD(&_0, "count", NULL, 0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element type of the buffer.
 *
 * @return int
 */
PHP_METHOD(Tensor_TensorBuffer, type)
{
	zval _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_METHOD(&_0, "type", NULL, 0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the buffer as a PHP array.
 *
 * @return list<float>
 */
PHP_METHOD(Tensor_TensorBuffer, toArray)
{
	zval _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_METHOD(&_0, "toArray", NULL, 0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element at the given index.
 *
 * @param int index
 * @return mixed
 */
PHP_METHOD(Tensor_TensorBuffer, get)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *index_param = NULL, _0, _1;
	zend_long index;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(index)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &index_param);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	zephir_memory_observe(&_1);
	zephir_array_fetch_long(&_1, &_0, index, PH_NOISY, "tensor/tensorbuffer.zep", 80);
	RETURN_CCTOR(&_1);
}

/**
 * Set the element at the given index.
 *
 * @param int index
 * @param mixed value
 * @return void
 */
PHP_METHOD(Tensor_TensorBuffer, set)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *index_param = NULL, *value, value_sub, _0;
	zend_long index;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&value_sub);
	ZVAL_UNDEF(&_0);
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(index)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 2, 0, &index_param, &value);
	ZEPHIR_INIT_VAR(&_0);
	ZVAL_LONG(&_0, index);
	zephir_update_property_array(this_ptr, SL("buffer"), &_0, value);
	ZEPHIR_MM_RESTORE();
}

/**
 * Sort the buffer in place.
 *
 * @param bool ascending
 * @return void
 */
PHP_METHOD(Tensor_TensorBuffer, sort)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *ascending_param = NULL, status, _0, _1;
	zend_bool ascending;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&status);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(ascending)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 0, 1, &ascending_param);
	if (!ascending_param) {
		ascending = 1;
	} else {
		}
	ZEPHIR_INIT_VAR(&status);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	if (ascending) {
		ZVAL_BOOL(&_1, 1);
	} else {
		ZVAL_BOOL(&_1, 0);
	}
	tensor_buffer_sort(&status, &_0, &_1);
	ZEPHIR_MM_RESTORE();
}

/**
 * Return a slice of the buffer as a new decorator.
 *
 * @param int offset
 * @param int length
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_TensorBuffer, slice)
{
	zval _5$$3;
	zend_bool _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *offset_param = NULL, *length_param = NULL, _2, _3, b, _6, _7, _8, _4$$3;
	zend_long offset, length, ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&b);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(offset)
		Z_PARAM_LONG(length)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 2, 0, &offset_param, &length_param);
	_0 = offset < 0;
	if (!(_0)) {
		_0 = length < 0;
	}
	_1 = _0;
	if (!(_1)) {
		zephir_read_property_cached(&_2, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
		ZEPHIR_CALL_METHOD(&_3, &_2, "count", NULL, 0);
		zephir_check_call_status();
		_1 = offset > (zephir_get_numberval(&_3) - length);
	}
	if (UNEXPECTED(_1)) {
		ZEPHIR_INIT_VAR(&_4$$3);
		object_init_ex(&_4$$3, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_INIT_VAR(&_5$$3);
		ZEPHIR_CONCAT_SS(&_5$$3, "Offset and length", " must be within the bounds of the buffer.");
		ZEPHIR_CALL_METHOD(NULL, &_4$$3, "__construct", NULL, 2, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_4$$3, "tensor/tensorbuffer.zep", 118);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&b);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_7, offset);
	ZVAL_LONG(&_8, length);
	tensor_buffer_slice(&b, &_6, &_7, &_8);
	object_init_ex(return_value, tensor_tensorbuffer_ce);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &b);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Map a function over the elements in the buffer and return a new decorator.
 *
 * @internal
 *
 * @param callable callback
 * @return self
 */
PHP_METHOD(Tensor_TensorBuffer, map)
{
	zend_bool _6;
	zval b;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *callback, callback_sub, value, a, _0, *_1, _2, *_3, _5, buffer, _4$$3, _7$$4;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&callback_sub);
	ZVAL_UNDEF(&value);
	ZVAL_UNDEF(&a);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&buffer);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&b);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(callback)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &callback);
	ZEPHIR_INIT_VAR(&b);
	array_init(&b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&a, &_0, "toArray", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&a) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_2);
		zephir_string_to_char_array(&_2, &a);
		_1 = &_2;
	} else {
		_1 = &a;
	}
	zephir_is_iterable(_1, 0, "tensor/tensorbuffer.zep", 146);
	if (Z_TYPE_P(_1) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_1), _3)
		{
			ZEPHIR_INIT_NVAR(&value);
			ZVAL_COPY(&value, _3);
			ZEPHIR_CALL_ZVAL_FUNCTION(&_4$$3, callback, NULL, 0, &value);
			zephir_check_call_status();
			zephir_array_append(&b, &_4$$3, PH_SEPARATE, "tensor/tensorbuffer.zep", 143);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _1, "rewind", NULL, 0);
		zephir_check_call_status();
		_6 = 1;
		while (1) {
			if (_6) {
				_6 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _1, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_5, _1, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_5)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&value, _1, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_CALL_ZVAL_FUNCTION(&_7$$4, callback, NULL, 0, &value);
				zephir_check_call_status();
				zephir_array_append(&b, &_7$$4, PH_SEPARATE, "tensor/tensorbuffer.zep", 143);
		}
	}
	ZEPHIR_INIT_NVAR(&value);
	ZEPHIR_INIT_VAR(&buffer);
	tensor_buffer_from_array(&buffer, &b);
	object_init_ex(return_value, tensor_tensorbuffer_ce);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &buffer);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the sum of the elements in the buffer.
 *
 * @return float
 */
PHP_METHOD(Tensor_TensorBuffer, sum)
{
	zval _0;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_sum(return_value, &_0);
	return;
}

/**
 * Return the product of the elements in the buffer.
 *
 * @return float
 */
PHP_METHOD(Tensor_TensorBuffer, product)
{
	zval _0;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_product(return_value, &_0);
	return;
}

/**
 * Return the minimum element in the buffer.
 *
 * @return float
 */
PHP_METHOD(Tensor_TensorBuffer, min)
{
	zval _0;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_min(return_value, &_0);
	return;
}

/**
 * Return the maximum element in the buffer.
 *
 * @return float
 */
PHP_METHOD(Tensor_TensorBuffer, max)
{
	zval _0;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_max(return_value, &_0);
	return;
}

/**
 * Return the index of the minimum element in the buffer.
 *
 * @return int
 */
PHP_METHOD(Tensor_TensorBuffer, argmin)
{
	zval _0;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_argmin(return_value, &_0);
	return;
}

/**
 * Return the index of the maximum element in the buffer.
 *
 * @return int
 */
PHP_METHOD(Tensor_TensorBuffer, argmax)
{
	zval _0;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_argmax(return_value, &_0);
	return;
}

/**
 * Return a slice of the buffer with a given stride as a new decorator.
 *
 * @param int offset
 * @param int length
 * @param int stride
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_TensorBuffer, sliceStrided)
{
	zval _7$$3;
	zend_bool _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *offset_param = NULL, *length_param = NULL, *stride_param = NULL, _4, _5, b, _8, _9, _10, _11, _6$$3;
	zend_long offset, length, stride, ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&b);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(offset)
		Z_PARAM_LONG(length)
		Z_PARAM_LONG(stride)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 3, 0, &offset_param, &length_param, &stride_param);
	_0 = offset < 0;
	if (!(_0)) {
		_0 = length < 0;
	}
	_1 = _0;
	if (!(_1)) {
		_1 = stride < 1;
	}
	_2 = _1;
	if (!(_2)) {
		_3 = length > 0;
		if (_3) {
			zephir_read_property_cached(&_4, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
			ZEPHIR_CALL_METHOD(&_5, &_4, "count", NULL, 0);
			zephir_check_call_status();
			_3 = offset > ((zephir_get_numberval(&_5) - (((length - 1)) * stride)) - 1);
		}
		_2 = _3;
	}
	if (UNEXPECTED(_2)) {
		ZEPHIR_INIT_VAR(&_6$$3);
		object_init_ex(&_6$$3, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SS(&_7$$3, "Offset, length, and", " stride must be within the bounds of the buffer.");
		ZEPHIR_CALL_METHOD(NULL, &_6$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_6$$3, "tensor/tensorbuffer.zep", 225);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&b);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_9, offset);
	ZVAL_LONG(&_10, length);
	ZVAL_LONG(&_11, stride);
	tensor_buffer_slice_strided(&b, &_8, &_9, &_10, &_11);
	object_init_ex(return_value, tensor_tensorbuffer_ce);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &b);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return a new decorator wrapping a new buffer containing the elements of
 * this buffer concatenated with the given buffers.
 *
 * @param \Tensor\TensorBuffer[] buffers
 * @return self
 */
PHP_METHOD(Tensor_TensorBuffer, concat)
{
	zend_bool _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *buffers_param = NULL, buffer, *_0, _2, b, _5, _1$$3, _4$$4;
	zval buffers, unwrapped;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&buffers);
	ZVAL_UNDEF(&unwrapped);
	ZVAL_UNDEF(&buffer);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&b);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_4$$4);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		ZEPHIR_Z_PARAM_ARRAY(buffers, buffers_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &buffers_param);
	zephir_get_arrval(&buffers, buffers_param);
	ZEPHIR_INIT_VAR(&unwrapped);
	array_init(&unwrapped);
	zephir_is_iterable(&buffers, 0, "tensor/tensorbuffer.zep", 250);
	if (Z_TYPE_P(&buffers) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(&buffers), _0)
		{
			ZEPHIR_INIT_NVAR(&buffer);
			ZVAL_COPY(&buffer, _0);
			ZEPHIR_CALL_METHOD(&_1$$3, &buffer, "asBuffer", NULL, 0);
			zephir_check_call_status();
			zephir_array_append(&unwrapped, &_1$$3, PH_SEPARATE, "tensor/tensorbuffer.zep", 247);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, &buffers, "rewind", NULL, 0);
		zephir_check_call_status();
		_3 = 1;
		while (1) {
			if (_3) {
				_3 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, &buffers, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_2, &buffers, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_2)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&buffer, &buffers, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_CALL_METHOD(&_4$$4, &buffer, "asBuffer", NULL, 0);
				zephir_check_call_status();
				zephir_array_append(&unwrapped, &_4$$4, PH_SEPARATE, "tensor/tensorbuffer.zep", 247);
		}
	}
	ZEPHIR_INIT_NVAR(&buffer);
	ZEPHIR_INIT_VAR(&b);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_concat(&b, &_5, &unwrapped);
	object_init_ex(return_value, tensor_tensorbuffer_ce);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &b);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return an array of new decorators each wrapping a chunk of this buffer
 * of the given length.
 *
 * @param int chunkLength
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return \Tensor\TensorBuffer[]
 */
PHP_METHOD(Tensor_TensorBuffer, split)
{
	zend_bool _12;
	zval tensorBuffers;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_10 = NULL;
	zval *chunkLength_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, buffer, buffers, _4, _5, *_6, _7, *_8, _11, _9$$4, _13$$5;
	zend_long chunkLength, ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&buffer);
	ZVAL_UNDEF(&buffers);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_9$$4);
	ZVAL_UNDEF(&_13$$5);
	ZVAL_UNDEF(&tensorBuffers);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(chunkLength)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &chunkLength_param);
	if (UNEXPECTED(chunkLength < 1)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		object_init_ex(&_0$$3, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_1$$3, chunkLength);
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", NULL, 3, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SSVS(&_3$$3, "Chunk length must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/tensorbuffer.zep", 267);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&buffers);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_5, chunkLength);
	tensor_buffer_split(&buffers, &_4, &_5);
	ZEPHIR_INIT_VAR(&tensorBuffers);
	array_init(&tensorBuffers);
	if (Z_TYPE_P(&buffers) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_7);
		zephir_string_to_char_array(&_7, &buffers);
		_6 = &_7;
	} else {
		_6 = &buffers;
	}
	zephir_is_iterable(_6, 0, "tensor/tensorbuffer.zep", 280);
	if (Z_TYPE_P(_6) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_6), _8)
		{
			ZEPHIR_INIT_NVAR(&buffer);
			ZVAL_COPY(&buffer, _8);
			ZEPHIR_INIT_NVAR(&_9$$4);
			object_init_ex(&_9$$4, tensor_tensorbuffer_ce);
			ZEPHIR_CALL_METHOD(NULL, &_9$$4, "__construct", &_10, 14, &buffer);
			zephir_check_call_status();
			zephir_array_append(&tensorBuffers, &_9$$4, PH_SEPARATE, "tensor/tensorbuffer.zep", 277);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _6, "rewind", NULL, 0);
		zephir_check_call_status();
		_12 = 1;
		while (1) {
			if (_12) {
				_12 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _6, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_11, _6, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_11)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&buffer, _6, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_13$$5);
				object_init_ex(&_13$$5, tensor_tensorbuffer_ce);
				ZEPHIR_CALL_METHOD(NULL, &_13$$5, "__construct", &_10, 14, &buffer);
				zephir_check_call_status();
				zephir_array_append(&tensorBuffers, &_13$$5, PH_SEPARATE, "tensor/tensorbuffer.zep", 277);
		}
	}
	ZEPHIR_INIT_NVAR(&buffer);
	RETURN_CTOR(&tensorBuffers);
}

/**
 * Return a new decorator wrapping a new buffer with the elements of this
 * buffer repeated the given number of times.
 *
 * @param int times
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_TensorBuffer, repeat)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *times_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, b, _4, _5;
	zend_long times, ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&b);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(times)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &times_param);
	if (UNEXPECTED(times < 1)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		object_init_ex(&_0$$3, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_1$$3, times);
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", NULL, 3, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SSVS(&_3$$3, "Times must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/tensorbuffer.zep", 295);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&b);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_0, 20, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_5, times);
	tensor_buffer_repeat(&b, &_4, &_5);
	object_init_ex(return_value, tensor_tensorbuffer_ce);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &b);
	zephir_check_call_status();
	RETURN_MM();
}

