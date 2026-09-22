
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
#include "kernel/fcall.h"
#include "kernel/memory.h"
#include "kernel/operators.h"
#include "kernel/object.h"
#include "kernel/exception.h"
#include "kernel/concat.h"
#include "include/buffer_ops.h"


/**
 * BufferVector
 *
 * A rank-1 tensor whose elements live in a contiguous C array -- a
 * <Ns>\Buffer, the kernel class added for zephir-lang/zephir#2721 -- instead
 * of a PHP array. A chain of operations therefore never materialises an
 * intermediate PHP array: the elements stay in C from `build()` to
 * `asArray()`.
 *
 * This is a measurement prototype, not a replacement for Vector. It carries
 * only the three operations the issue names as dominated by pack/unpack
 * overhead, so that the two representations can be benchmarked against each
 * other on identical work.
 *
 * Note that no method here names the Buffer class: the compiler has no
 * definition for a kernel class, so the buffer is produced by
 * tensor_buffer_from_array() and thereafter only ever passed along.
 */
ZEPHIR_INIT_CLASS(Tensor_BufferVector)
{
	ZEPHIR_REGISTER_CLASS(Tensor, BufferVector, tensor, buffervector, tensor_buffervector_method_entry, 0);

	/**
	 * The elements of the vector, as a contiguous buffer of doubles.
	 *
	 * @var \Tensor\Buffer
	 */
	zend_declare_property_null(tensor_buffervector_ce, SL("a"), ZEND_ACC_PROTECTED);
	/**
	 * The number of elements in the vector.
	 *
	 * @var int
	 */
	zend_declare_property_null(tensor_buffervector_ce, SL("n"), ZEND_ACC_PROTECTED);
	return SUCCESS;
}

/**
 * Build a vector from a PHP array. This is the only point at which the
 * elements are converted.
 *
 * @param float[] a
 * @return self
 */
PHP_METHOD(Tensor_BufferVector, build)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *a_param = NULL, _0;
	zval a;

	ZVAL_UNDEF(&a);
	ZVAL_UNDEF(&_0);
	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		ZEPHIR_Z_PARAM_ARRAY(a, a_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 0, 1, &a_param);
	if (!a_param) {
		ZEPHIR_INIT_VAR(&a);
		array_init(&a);
	} else {
		zephir_get_arrval(&a, a_param);
	}
	object_init_ex(return_value, tensor_buffervector_ce);
	ZEPHIR_INIT_VAR(&_0);
	tensor_buffer_from_array(&_0, &a);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 17, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Wrap an existing buffer without copying it.
 *
 * @return self
 */
PHP_METHOD(Tensor_BufferVector, quick)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *a, a_sub;

	ZVAL_UNDEF(&a_sub);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(a)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &a);
	object_init_ex(return_value, tensor_buffervector_ce);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 17, a);
	zephir_check_call_status();
	RETURN_MM();
}

PHP_METHOD(Tensor_BufferVector, __construct)
{
	zval *a, a_sub, _0;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&a_sub);
	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(a)
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(1, 0, &a);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_0, 3, a);
	ZVAL_UNDEF(&_0);
	ZVAL_LONG(&_0, zephir_fast_count_int(a));
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_1, 4, &_0);
}

/**
 * Return the underlying buffer.
 */
PHP_METHOD(Tensor_BufferVector, buffer)
{

	RETURN_MEMBER(getThis(), "a");
}

/**
 * Return the number of elements in the vector.
 */
PHP_METHOD(Tensor_BufferVector, n)
{

	RETURN_MEMBER_TYPED(getThis(), "n", IS_LONG);
}

/**
 * Materialise the elements as a PHP array. The whole point of the class is
 * that this is called once at the end, not once per operation.
 *
 * @return list<float>
 */
PHP_METHOD(Tensor_BufferVector, asArray)
{
	zval _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 3, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_METHOD(&_0, "toArray", NULL, 0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Elementwise addition.
 *
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_BufferVector, add)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, _8, _9, _10, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_buffervector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 4, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 4, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/buffervector.zep", 105);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 3, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_10, b, "buffer", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_add(&_8, &_9, &_10);
	ZEPHIR_RETURN_CALL_SELF("quick", NULL, 0, &_8);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Elementwise product.
 *
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_BufferVector, multiply)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, _8, _9, _10, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_buffervector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 4, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 4, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/buffervector.zep", 122);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 3, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_10, b, "buffer", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_multiply(&_8, &_9, &_10);
	ZEPHIR_RETURN_CALL_SELF("quick", NULL, 0, &_8);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Inner product.
 *
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return float
 */
PHP_METHOD(Tensor_BufferVector, dot)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, _8, _9, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_buffervector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 4, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 4, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/buffervector.zep", 139);
		ZEPHIR_MM_RESTORE();
		return;
	}
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 3, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_9, b, "buffer", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_dot(return_value, &_8, &_9);
	RETURN_MM();
}

