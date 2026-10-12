
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
#include "kernel/memory.h"
#include "kernel/array.h"
#include "kernel/fcall.h"
#include "kernel/operators.h"
#include "kernel/object.h"
#include "kernel/exception.h"
#include "kernel/concat.h"
#include "kernel/string.h"
#include "include/buffer.h"


/**
 * TensorBuffer
 *
 * A decorator that wraps the kernel Buffer class and provides structural
 * operations such as sorting, slicing, splitting, concatenating, and
 * repeating.
 *
 * @internal
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
	/**
	 * The element offset of a view into the underlying buffer.
	 *
	 * @var int
	 */
	zend_declare_property_null(tensor_tensorbuffer_ce, SL("offset"), ZEND_ACC_PROTECTED);
	/**
	 * The logical shape of a view, or null for a plain dense buffer.
	 *
	 * @var int[]|null
	 */
	zend_declare_property_null(tensor_tensorbuffer_ce, SL("shape"), ZEND_ACC_PROTECTED);
	/**
	 * The stride of each shape dimension, or null for a plain dense buffer.
	 *
	 * @var int[]|null
	 */
	zend_declare_property_null(tensor_tensorbuffer_ce, SL("strides"), ZEND_ACC_PROTECTED);
	zephir_declare_class_constant_long(tensor_tensorbuffer_ce, SL("TYPE_DOUBLE"), 1);

	zephir_declare_class_constant_long(tensor_tensorbuffer_ce, SL("TYPE_LONG"), 2);

	return SUCCESS;
}

/**
 * Build a buffer by concatenating an array of buffers into a single
 * contiguous buffer.
 *
 * @param \Tensor\TensorBuffer[] buffers
 * @return self
 */
PHP_METHOD(Tensor_TensorBuffer, fromBuffers)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS, rows;
	zval *buffers_param = NULL, _2, _3, _4, _0$$3, _1$$4;
	zval buffers;

	ZVAL_UNDEF(&buffers);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$4);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		ZEPHIR_Z_PARAM_ARRAY(buffers, buffers_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &buffers_param);
	zephir_get_arrval(&buffers, buffers_param);
	rows = zephir_fast_count_int(&buffers);
	if (UNEXPECTED(rows < 1)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		array_init(&_0$$3);
		tensor_buffer_from_array(return_value, &_0$$3);
		RETURN_MM();
	}
	if (UNEXPECTED(rows == 1)) {
		zephir_memory_observe(&_1$$4);
		zephir_array_fetch_long(&_1$$4, &buffers, 0, PH_NOISY, "tensor/tensorbuffer.zep", 68);
		RETURN_CCTOR(&_1$$4);
	}
	zephir_memory_observe(&_2);
	zephir_array_fetch_long(&_2, &buffers, 0, PH_NOISY, "tensor/tensorbuffer.zep", 71);
	ZVAL_LONG(&_3, 1);
	ZEPHIR_CALL_FUNCTION(&_4, "array_slice", NULL, 24, &buffers, &_3);
	zephir_check_call_status();
	ZEPHIR_RETURN_CALL_METHOD(&_2, "concat", NULL, 0, &_4);
	zephir_check_call_status();
	RETURN_MM();
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
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_0, 21, buffer);
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

	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 21, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_METHOD(&_0, "type", NULL, 0);
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
	zval *index_param = NULL, _0, value, _3, _4, _5, _6, _7, _1$$3, _2$$3;
	zend_long index;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&value);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	static zend_string *_zephir_prop_3 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("shape", 5, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("buffer", 6, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("offset", 6, 1);
	}
	if (UNEXPECTED(!_zephir_prop_3)) {
		_zephir_prop_3 = zend_string_init("strides", 7, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(index)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &index_param);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 22, PH_NOISY_CC | PH_READONLY);
	if (Z_TYPE_P(&_0) == IS_NULL) {
		zephir_read_property_cached(&_1$$3, this_ptr, _zephir_prop_1, 21, PH_NOISY_CC | PH_READONLY);
		zephir_memory_observe(&_2$$3);
		zephir_array_fetch_long(&_2$$3, &_1$$3, index, PH_NOISY, "tensor/tensorbuffer.zep", 101);
		RETURN_CCTOR(&_2$$3);
	}
	ZEPHIR_INIT_VAR(&value);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 21, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 23, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_0, 22, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_3, 24, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_7, index);
	tensor_tensorbuffer_get(&value, &_3, &_4, &_5, &_6, &_7);
	RETURN_CCTOR(&value);
}

/**
 * Set the element at the given index. Views are read-only and reject the
 * write.
 *
 * @param int index
 * @param mixed value
 * @return void
 */
PHP_METHOD(Tensor_TensorBuffer, set)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *index_param = NULL, *value, value_sub, status, _0, _1, _2, _3, _4;
	zend_long index;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&value_sub);
	ZVAL_UNDEF(&status);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	static zend_string *_zephir_prop_3 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("offset", 6, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("shape", 5, 1);
	}
	if (UNEXPECTED(!_zephir_prop_3)) {
		_zephir_prop_3 = zend_string_init("strides", 7, 1);
	}

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(index)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 2, 0, &index_param, &value);
	ZEPHIR_INIT_VAR(&status);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 21, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_1, 23, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_2, 22, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_3, 24, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_4, index);
	tensor_tensorbuffer_set(&status, &_0, &_1, &_2, &_3, &_4, value);
	ZEPHIR_MM_RESTORE();
}

/**
 * Sort the buffer in place. Views are read-only and reject the sort.
 *
 * @param bool ascending
 * @return void
 */
PHP_METHOD(Tensor_TensorBuffer, sort)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *ascending_param = NULL, _0, status, _1, _2;
	zend_bool ascending;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&status);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("shape", 5, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("buffer", 6, 1);
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
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 22, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(Z_TYPE_P(&_0) != IS_NULL)) {
		ZEPHIR_THROW_EXCEPTION_DEBUG_STR(tensor_exceptions_invalidargumentexception_ce, "Cannot sort a read-only view.", "tensor/tensorbuffer.zep", 131);
		return;
	}
	ZEPHIR_INIT_VAR(&status);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_1, 21, PH_NOISY_CC | PH_READONLY);
	if (ascending) {
		ZVAL_BOOL(&_2, 1);
	} else {
		ZVAL_BOOL(&_2, 0);
	}
	tensor_buffer_sort(&status, &_1, &_2);
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
	zval _6$$3;
	zend_bool _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *offset_param = NULL, *length_param = NULL, _2, _3, _4, view, _10, _11, _12, _13, _14, _15, _5$$3, b$$4, _7$$4, _8$$4, _9$$4;
	zend_long offset, length, ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&view);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_15);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&b$$4);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_8$$4);
	ZVAL_UNDEF(&_9$$4);
	ZVAL_UNDEF(&_6$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	static zend_string *_zephir_prop_3 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("offset", 6, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("shape", 5, 1);
	}
	if (UNEXPECTED(!_zephir_prop_3)) {
		_zephir_prop_3 = zend_string_init("strides", 7, 1);
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
		ZEPHIR_CALL_METHOD(&_2, this_ptr, "count", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_3);
		ZVAL_LONG(&_3, length);
		ZEPHIR_INIT_VAR(&_4);
		zephir_sub_function(&_4, &_2, &_3);
		_1 = ZEPHIR_LT_LONG(&_4, offset);
	}
	if (UNEXPECTED(_1)) {
		ZEPHIR_INIT_VAR(&_5$$3);
		object_init_ex(&_5$$3, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_INIT_VAR(&_6$$3);
		ZEPHIR_CONCAT_SS(&_6$$3, "Offset and length", " must be within the bounds of the buffer.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$3, "__construct", NULL, 2, &_6$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$3, "tensor/tensorbuffer.zep", 149);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&_2, this_ptr, "type", NULL, 0);
	zephir_check_call_status();
	if (!ZEPHIR_IS_LONG_IDENTICAL(&_2, 1)) {
		ZEPHIR_INIT_VAR(&b$$4);
		zephir_read_property_cached(&_7$$4, this_ptr, _zephir_prop_0, 21, PH_NOISY_CC | PH_READONLY);
		ZVAL_LONG(&_8$$4, offset);
		ZVAL_LONG(&_9$$4, length);
		tensor_buffer_slice(&b$$4, &_7$$4, &_8$$4, &_9$$4);
		object_init_ex(return_value, tensor_tensorbuffer_ce);
		ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 25, &b$$4);
		zephir_check_call_status();
		RETURN_MM();
	}
	ZEPHIR_INIT_VAR(&view);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_0, 21, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_1, 23, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_12, this_ptr, _zephir_prop_2, 22, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_3, 24, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_14, offset);
	ZVAL_LONG(&_15, length);
	tensor_buffer_slice_view(&view, &_10, &_11, &_12, &_13, &_14, &_15);
	RETURN_CCTOR(&view);
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
	zval _10$$10;
	zend_bool invalid, _0, _1, _2, _7$$3, _8$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *offset_param = NULL, *length_param = NULL, *stride_param = NULL, _11, view, _16, _17, _18, _19, _20, _21, _22, _3$$3, _4$$3, _5$$3, _6$$3, _9$$10, b$$11, _12$$11, _13$$11, _14$$11, _15$$11;
	zend_long offset, length, stride, ZEPHIR_LAST_CALL_STATUS, limit, a, acc, product;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&view);
	ZVAL_UNDEF(&_16);
	ZVAL_UNDEF(&_17);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19);
	ZVAL_UNDEF(&_20);
	ZVAL_UNDEF(&_21);
	ZVAL_UNDEF(&_22);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_9$$10);
	ZVAL_UNDEF(&b$$11);
	ZVAL_UNDEF(&_12$$11);
	ZVAL_UNDEF(&_13$$11);
	ZVAL_UNDEF(&_14$$11);
	ZVAL_UNDEF(&_15$$11);
	ZVAL_UNDEF(&_10$$10);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	static zend_string *_zephir_prop_3 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("offset", 6, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("shape", 5, 1);
	}
	if (UNEXPECTED(!_zephir_prop_3)) {
		_zephir_prop_3 = zend_string_init("strides", 7, 1);
	}

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(offset)
		Z_PARAM_LONG(length)
		Z_PARAM_LONG(stride)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 3, 0, &offset_param, &length_param, &stride_param);
	limit = 0;
	a = 0;
	acc = 0;
	product = 0;
	_0 = offset < 0;
	if (!(_0)) {
		_0 = length < 0;
	}
	_1 = _0;
	if (!(_1)) {
		_1 = stride < 1;
	}
	invalid = _1;
	_2 = !invalid;
	if (_2) {
		_2 = length > 0;
	}
	if (EXPECTED(_2)) {
		ZEPHIR_CALL_METHOD(&_3$$3, this_ptr, "count", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_4$$3);
		ZVAL_LONG(&_4$$3, 1);
		ZEPHIR_INIT_VAR(&_5$$3);
		zephir_sub_function(&_5$$3, &_3$$3, &_4$$3);
		ZEPHIR_INIT_NVAR(&_4$$3);
		ZVAL_LONG(&_4$$3, offset);
		ZEPHIR_INIT_VAR(&_6$$3);
		zephir_sub_function(&_6$$3, &_5$$3, &_4$$3);
		limit = zephir_get_intval(&_6$$3);
		_7$$3 = limit < 0;
		if (!(_7$$3)) {
			_8$$3 = length > 1;
			if (_8$$3) {
				_8$$3 = stride > limit;
			}
			_7$$3 = _8$$3;
		}
		invalid = _7$$3;
		if (EXPECTED(!invalid)) {
			a = (length - 1);
			acc = stride;
			while (1) {
				if (!(a > 0)) {
					break;
				}
				if ((a & 1)) {
					product += acc;
					if (product > limit) {
						invalid = 1;
						break;
					}
				}
				a = (a >> 1);
				if (a > 0) {
					if (acc > ((limit >> 1))) {
						invalid = 1;
						break;
					}
					acc = (acc << 1);
				}
			}
		}
	}
	if (UNEXPECTED(invalid)) {
		ZEPHIR_INIT_VAR(&_9$$10);
		object_init_ex(&_9$$10, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_INIT_VAR(&_10$$10);
		ZEPHIR_CONCAT_SS(&_10$$10, "Offset, length, and", " stride must be within the bounds of the buffer.");
		ZEPHIR_CALL_METHOD(NULL, &_9$$10, "__construct", NULL, 2, &_10$$10);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_9$$10, "tensor/tensorbuffer.zep", 218);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&_11, this_ptr, "type", NULL, 0);
	zephir_check_call_status();
	if (!ZEPHIR_IS_LONG_IDENTICAL(&_11, 1)) {
		ZEPHIR_INIT_VAR(&b$$11);
		zephir_read_property_cached(&_12$$11, this_ptr, _zephir_prop_0, 21, PH_NOISY_CC | PH_READONLY);
		ZVAL_LONG(&_13$$11, offset);
		ZVAL_LONG(&_14$$11, length);
		ZVAL_LONG(&_15$$11, stride);
		tensor_buffer_slice_strided(&b$$11, &_12$$11, &_13$$11, &_14$$11, &_15$$11);
		object_init_ex(return_value, tensor_tensorbuffer_ce);
		ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 25, &b$$11);
		zephir_check_call_status();
		RETURN_MM();
	}
	ZEPHIR_INIT_VAR(&view);
	zephir_read_property_cached(&_16, this_ptr, _zephir_prop_0, 21, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_17, this_ptr, _zephir_prop_1, 23, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_18, this_ptr, _zephir_prop_2, 22, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_19, this_ptr, _zephir_prop_3, 24, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_20, offset);
	ZVAL_LONG(&_21, length);
	ZVAL_LONG(&_22, stride);
	tensor_buffer_slice_strided_view(&view, &_16, &_17, &_18, &_19, &_20, &_21, &_22);
	RETURN_CCTOR(&view);
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
	zval *buffers_param = NULL, buffer, *_0, _2, self, b, _1$$3, _4$$4;
	zval buffers, unwrapped;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&buffers);
	ZVAL_UNDEF(&unwrapped);
	ZVAL_UNDEF(&buffer);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&self);
	ZVAL_UNDEF(&b);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_4$$4);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		ZEPHIR_Z_PARAM_ARRAY(buffers, buffers_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &buffers_param);
	zephir_get_arrval(&buffers, buffers_param);
	ZEPHIR_INIT_VAR(&unwrapped);
	array_init(&unwrapped);
	zephir_is_iterable(&buffers, 0, "tensor/tensorbuffer.zep", 249);
	if (Z_TYPE_P(&buffers) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(&buffers), _0)
		{
			ZEPHIR_INIT_NVAR(&buffer);
			ZVAL_COPY(&buffer, _0);
			ZEPHIR_CALL_METHOD(&_1$$3, &buffer, "asBuffer", NULL, 0);
			zephir_check_call_status();
			zephir_array_append(&unwrapped, &_1$$3, PH_SEPARATE, "tensor/tensorbuffer.zep", 246);
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
				zephir_array_append(&unwrapped, &_4$$4, PH_SEPARATE, "tensor/tensorbuffer.zep", 246);
		}
	}
	ZEPHIR_INIT_NVAR(&buffer);
	ZEPHIR_CALL_METHOD(&self, this_ptr, "asBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&b);
	tensor_buffer_concat(&b, &self, &unwrapped);
	object_init_ex(return_value, tensor_tensorbuffer_ce);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 25, &b);
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
	zend_bool _13$$4;
	zval tensorBuffers$$4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_11 = NULL;
	zval *chunkLength_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, buffer, _4, views, _15, _16, _17, _18, _19, buffers$$4, _5$$4, _6$$4, *_7$$4, _8$$4, *_9$$4, _12$$4, _10$$5, _14$$6;
	zend_long chunkLength, ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&buffer);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&views);
	ZVAL_UNDEF(&_15);
	ZVAL_UNDEF(&_16);
	ZVAL_UNDEF(&_17);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19);
	ZVAL_UNDEF(&buffers$$4);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_6$$4);
	ZVAL_UNDEF(&_8$$4);
	ZVAL_UNDEF(&_12$$4);
	ZVAL_UNDEF(&_10$$5);
	ZVAL_UNDEF(&_14$$6);
	ZVAL_UNDEF(&tensorBuffers$$4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	static zend_string *_zephir_prop_3 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("offset", 6, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("shape", 5, 1);
	}
	if (UNEXPECTED(!_zephir_prop_3)) {
		_zephir_prop_3 = zend_string_init("strides", 7, 1);
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
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", NULL, 5, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SSVS(&_3$$3, "Chunk length must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/tensorbuffer.zep", 268);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&_4, this_ptr, "type", NULL, 0);
	zephir_check_call_status();
	if (!ZEPHIR_IS_LONG_IDENTICAL(&_4, 1)) {
		ZEPHIR_INIT_VAR(&buffers$$4);
		zephir_read_property_cached(&_5$$4, this_ptr, _zephir_prop_0, 21, PH_NOISY_CC | PH_READONLY);
		ZVAL_LONG(&_6$$4, chunkLength);
		tensor_buffer_split(&buffers$$4, &_5$$4, &_6$$4);
		ZEPHIR_INIT_VAR(&tensorBuffers$$4);
		array_init(&tensorBuffers$$4);
		if (Z_TYPE_P(&buffers$$4) == IS_STRING) {
			ZEPHIR_INIT_VAR(&_8$$4);
			zephir_string_to_char_array(&_8$$4, &buffers$$4);
			_7$$4 = &_8$$4;
		} else {
			_7$$4 = &buffers$$4;
		}
		zephir_is_iterable(_7$$4, 0, "tensor/tensorbuffer.zep", 282);
		if (Z_TYPE_P(_7$$4) == IS_ARRAY) {
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_7$$4), _9$$4)
			{
				ZEPHIR_INIT_NVAR(&buffer);
				ZVAL_COPY(&buffer, _9$$4);
				ZEPHIR_INIT_NVAR(&_10$$5);
				object_init_ex(&_10$$5, tensor_tensorbuffer_ce);
				ZEPHIR_CALL_METHOD(NULL, &_10$$5, "__construct", &_11, 25, &buffer);
				zephir_check_call_status();
				zephir_array_append(&tensorBuffers$$4, &_10$$5, PH_SEPARATE, "tensor/tensorbuffer.zep", 279);
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEPHIR_CALL_METHOD(NULL, _7$$4, "rewind", NULL, 0);
			zephir_check_call_status();
			_13$$4 = 1;
			while (1) {
				if (_13$$4) {
					_13$$4 = 0;
				} else {
					ZEPHIR_CALL_METHOD(NULL, _7$$4, "next", NULL, 0);
					zephir_check_call_status();
				}
				ZEPHIR_CALL_METHOD(&_12$$4, _7$$4, "valid", NULL, 0);
				zephir_check_call_status();
				if (!zend_is_true(&_12$$4)) {
					break;
				}
				ZEPHIR_CALL_METHOD(&buffer, _7$$4, "current", NULL, 0);
				zephir_check_call_status();
					ZEPHIR_INIT_NVAR(&_14$$6);
					object_init_ex(&_14$$6, tensor_tensorbuffer_ce);
					ZEPHIR_CALL_METHOD(NULL, &_14$$6, "__construct", &_11, 25, &buffer);
					zephir_check_call_status();
					zephir_array_append(&tensorBuffers$$4, &_14$$6, PH_SEPARATE, "tensor/tensorbuffer.zep", 279);
			}
		}
		ZEPHIR_INIT_NVAR(&buffer);
		RETURN_CTOR(&tensorBuffers$$4);
	}
	ZEPHIR_INIT_VAR(&views);
	zephir_read_property_cached(&_15, this_ptr, _zephir_prop_0, 21, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_16, this_ptr, _zephir_prop_1, 23, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_17, this_ptr, _zephir_prop_2, 22, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_18, this_ptr, _zephir_prop_3, 24, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_19, chunkLength);
	tensor_buffer_split_view(&views, &_15, &_16, &_17, &_18, &_19);
	RETURN_CCTOR(&views);
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
	zval *times_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, self, b, _4;
	zend_long times, ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&self);
	ZVAL_UNDEF(&b);
	ZVAL_UNDEF(&_4);
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
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", NULL, 5, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SSVS(&_3$$3, "Times must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/tensorbuffer.zep", 302);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&self, this_ptr, "asBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&b);
	ZVAL_LONG(&_4, times);
	tensor_buffer_repeat(&b, &self, &_4);
	object_init_ex(return_value, tensor_tensorbuffer_ce);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 25, &b);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the underlying buffer.
 *
 * @return \Tensor\Buffer
 */
PHP_METHOD(Tensor_TensorBuffer, asBuffer)
{
	zval _0, b, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&b);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	static zend_string *_zephir_prop_3 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("shape", 5, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("buffer", 6, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("offset", 6, 1);
	}
	if (UNEXPECTED(!_zephir_prop_3)) {
		_zephir_prop_3 = zend_string_init("strides", 7, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 22, PH_NOISY_CC | PH_READONLY);
	if (Z_TYPE_P(&_0) == IS_NULL) {
		RETURN_MM_MEMBER(getThis(), "buffer");
	}
	ZEPHIR_INIT_VAR(&b);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_1, 21, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_2, 23, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_0, 22, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_3, 24, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_view_materialize(&b, &_1, &_2, &_3, &_4);
	RETURN_CCTOR(&b);
}

/**
 * Return the number of elements in the buffer.
 *
 * @return int
 */
PHP_METHOD(Tensor_TensorBuffer, count)
{
	zval _0, _2, _3, _1$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_1$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("shape", 5, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("buffer", 6, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 22, PH_NOISY_CC | PH_READONLY);
	if (Z_TYPE_P(&_0) == IS_NULL) {
		zephir_read_property_cached(&_1$$3, this_ptr, _zephir_prop_1, 21, PH_NOISY_CC | PH_READONLY);
		ZEPHIR_RETURN_CALL_METHOD(&_1$$3, "count", NULL, 0);
		zephir_check_call_status();
		RETURN_MM();
	}
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 21, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_0, 22, PH_NOISY_CC | PH_READONLY);
	tensor_tensorbuffer_count(return_value, &_2, &_3);
	RETURN_MM();
}

/**
 * Return the buffer as a PHP array.
 *
 * @return list<float>
 */
PHP_METHOD(Tensor_TensorBuffer, toArray)
{
	zval _0, _2, _3, _4, _5, _1$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_1$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	static zend_string *_zephir_prop_3 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("shape", 5, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("buffer", 6, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("offset", 6, 1);
	}
	if (UNEXPECTED(!_zephir_prop_3)) {
		_zephir_prop_3 = zend_string_init("strides", 7, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 22, PH_NOISY_CC | PH_READONLY);
	if (Z_TYPE_P(&_0) == IS_NULL) {
		zephir_read_property_cached(&_1$$3, this_ptr, _zephir_prop_1, 21, PH_NOISY_CC | PH_READONLY);
		ZEPHIR_RETURN_CALL_METHOD(&_1$$3, "toArray", NULL, 0);
		zephir_check_call_status();
		RETURN_MM();
	}
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 21, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 23, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_0, 22, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_3, 24, PH_NOISY_CC | PH_READONLY);
	tensor_tensorbuffer_to_array(return_value, &_2, &_3, &_4, &_5);
	RETURN_MM();
}

