
#ifdef HAVE_CONFIG_H
#include "../../ext_config.h"
#endif

#include <php.h>
#include "../../php_ext.h"
#include "../../ext.h"

#include <Zend/zend_operators.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/fcall.h"
#include "kernel/exception.h"
#include "kernel/object.h"
#include "include/linear_algebra.h"


/**
 * RREF
 *
 * The reduced row echelon form (RREF) of a matrix.
 *
 * @category    Scientific Computing
 * @package     Rubix/Tensor
 * @author      Andrew DalPino
 */
ZEPHIR_INIT_CLASS(Tensor_Reductions_Rref)
{
	ZEPHIR_REGISTER_CLASS(Tensor\\Reductions, Rref, tensor, reductions_rref, tensor_reductions_rref_method_entry, 0);

	/**
	 * The reduced matrix in reduced row echelon form.
	 *
	 * @var \Tensor\Matrix
	 */
	zend_declare_property_null(tensor_reductions_rref_ce, SL("a"), ZEND_ACC_PROTECTED);
	return SUCCESS;
}

/**
 * Factory method to decompose a matrix.
 *
 * @param \Tensor\Matrix a
 * @return self
 */
PHP_METHOD(Tensor_Reductions_Rref, reduce)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *a, a_sub, result, _0, _1, _2, _3, _4, _5;

	ZVAL_UNDEF(&a_sub);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(a, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &a);
	ZEPHIR_INIT_VAR(&result);
	ZEPHIR_CALL_METHOD(&_0, a, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, a, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_2, a, "n", NULL, 0);
	zephir_check_call_status();
	tensor_rref(&result, &_0, &_1, &_2);
	if (Z_TYPE_P(&result) == IS_NULL) {
		ZEPHIR_THROW_EXCEPTION_DEBUG_STR(tensor_exceptions_runtimeexception_ce, "Failed to reduce matrix.", "tensor/reductions/rref.zep", 36);
		return;
	}
	object_init_ex(return_value, tensor_reductions_rref_ce);
	ZEPHIR_INIT_VAR(&_3);
	object_init_ex(&_3, tensor_matrix_ce);
	ZEPHIR_CALL_METHOD(&_4, a, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_5, a, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, &_3, "__construct", NULL, 15, &result, &_4, &_5);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 33, &_3);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * @param \Tensor\Matrix a
 * @throws \InvalidArgumentException
 */
PHP_METHOD(Tensor_Reductions_Rref, __construct)
{
	zval *a, a_sub;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&a_sub);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(a, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(1, 0, &a);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_0, 19, a);
}

/**
 * Return the reduced matrix in reduced row echelon form.
 *
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Reductions_Rref, a)
{

	RETURN_MEMBER(getThis(), "a");
}

