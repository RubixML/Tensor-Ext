
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
#include "kernel/operators.h"
#include "kernel/array.h"
#include "kernel/object.h"
#include "include/linear_algebra.h"


/**
 * SVD
 *
 * @category    Scientific Computing
 * @package     Rubix/Tensor
 * @author      Andrew DalPino
 */
ZEPHIR_INIT_CLASS(Tensor_Decompositions_Svd)
{
	ZEPHIR_REGISTER_CLASS(Tensor\\Decompositions, Svd, tensor, decompositions_svd, tensor_decompositions_svd_method_entry, 0);

	/**
	 * The U matrix.
	 *
	 * @var \Tensor\Matrix
	 */
	zend_declare_property_null(tensor_decompositions_svd_ce, SL("u"), ZEND_ACC_PROTECTED);
	/**
	 * The singular values of the matrix A.
	 *
	 * @var \Tensor\Vector
	 */
	zend_declare_property_null(tensor_decompositions_svd_ce, SL("singularValues"), ZEND_ACC_PROTECTED);
	/**
	 * The transposed V matrix.
	 *
	 * @var \Tensor\Matrix
	 */
	zend_declare_property_null(tensor_decompositions_svd_ce, SL("vT"), ZEND_ACC_PROTECTED);
	return SUCCESS;
}

/**
 * Factory method to decompose a matrix.
 *
 * @param \Tensor\Matrix a
 * @throws \Tensor\Exceptions\RuntimeException
 * @return self
 */
PHP_METHOD(Tensor_Decompositions_Svd, decompose)
{
	zval _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *a, a_sub, result, _0, _1, _2, _3, u, _5, _6, _7, singularValues, _8, vT, _9, _10, _11;

	ZVAL_UNDEF(&a_sub);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&u);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&singularValues);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&vT);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_4);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(a, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &a);
	ZEPHIR_INIT_VAR(&result);
	ZEPHIR_CALL_METHOD(&_0, a, "buffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, a, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_2, a, "n", NULL, 0);
	zephir_check_call_status();
	tensor_svd(&result, &_0, &_1, &_2);
	if (Z_TYPE_P(&result) == IS_NULL) {
		ZEPHIR_THROW_EXCEPTION_DEBUG_STR(tensor_exceptions_runtimeexception_ce, "Failed to decompose matrix.", "tensor/decompositions/svd.zep", 49);
		return;
	}
	ZEPHIR_CPY_WRT(&_3, &result);
	zephir_get_arrval(&_4, &_3);
	ZEPHIR_CPY_WRT(&result, &_4);
	zephir_memory_observe(&_5);
	zephir_array_fetch_long(&_5, &result, 0, PH_NOISY, "tensor/decompositions/svd.zep", 54);
	ZEPHIR_CALL_METHOD(&_6, a, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_7, a, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_CE_STATIC(&u, tensor_matrix_ce, "fromBuffer", NULL, 0, &_5, &_6, &_7);
	zephir_check_call_status();
	zephir_memory_observe(&_8);
	zephir_array_fetch_long(&_8, &result, 1, PH_NOISY, "tensor/decompositions/svd.zep", 55);
	ZEPHIR_CALL_CE_STATIC(&singularValues, tensor_vector_ce, "fromBuffer", NULL, 0, &_8);
	zephir_check_call_status();
	zephir_memory_observe(&_9);
	zephir_array_fetch_long(&_9, &result, 2, PH_NOISY, "tensor/decompositions/svd.zep", 56);
	ZEPHIR_CALL_METHOD(&_10, a, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_11, a, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_CE_STATIC(&vT, tensor_matrix_ce, "fromBuffer", NULL, 0, &_9, &_10, &_11);
	zephir_check_call_status();
	object_init_ex(return_value, tensor_decompositions_svd_ce);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &u, &singularValues, &vT);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * @param \Tensor\Matrix u
 * @param \Tensor\Vector singularValues
 * @param \Tensor\Matrix vT
 */
PHP_METHOD(Tensor_Decompositions_Svd, __construct)
{
	zval *u, u_sub, *singularValues, singularValues_sub, *vT, vT_sub;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&u_sub);
	ZVAL_UNDEF(&singularValues_sub);
	ZVAL_UNDEF(&vT_sub);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("u", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("singularValues", 14, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("vT", 2, 1);
	}

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_OBJECT_OF_CLASS(u, tensor_matrix_ce)
		Z_PARAM_OBJECT_OF_CLASS(singularValues, tensor_vector_ce)
		Z_PARAM_OBJECT_OF_CLASS(vT, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(3, 0, &u, &singularValues, &vT);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_0, 12, u);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_1, 13, singularValues);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_2, 14, vT);
}

/**
 * Return the U matrix.
 *
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Decompositions_Svd, u)
{

	RETURN_MEMBER(getThis(), "u");
}

/**
 * Return the singular values of matrix A.
 *
 * @return \Tensor\Vector
 */
PHP_METHOD(Tensor_Decompositions_Svd, singularValues)
{

	RETURN_MEMBER(getThis(), "singularValues");
}

/**
 * Return the singular value matrix.
 *
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Decompositions_Svd, s)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("singularValues", 14, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 13, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, &_0, "asArray", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_RETURN_CALL_CE_STATIC(tensor_matrix_ce, "diagonal", NULL, 0, &_1);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the V matrix.
 *
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Decompositions_Svd, v)
{
	zval _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("vT", 2, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_METHOD(&_0, "transpose", NULL, 0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the V transposed matrix.
 *
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Decompositions_Svd, vT)
{

	RETURN_MEMBER(getThis(), "vT");
}

