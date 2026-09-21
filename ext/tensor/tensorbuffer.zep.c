
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
		Z_PARAM_OBJECT_OF_CLASS(buffer, tensor_buffer_ce)
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
		ZEPHIR_CALL_METHOD(NULL, &_4$$3, "__construct", NULL, 3, &_5$$3);
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
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 35, &b);
	zephir_check_call_status();
	RETURN_MM();
}

