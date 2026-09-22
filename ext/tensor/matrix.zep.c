
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
#include "kernel/array.h"
#include "math.h"
#include "kernel/string.h"
#include "kernel/math.h"
#include "ext/spl/spl_array.h"
#include "include/buffer_matrix.h"
#include "include/buffer_linalg.h"
#include "include/linear_algebra.h"
#include "include/signal_processing.h"
#include "include/buffer_unary.h"
#include "include/buffer_binary.h"
#include "include/buffer_ops.h"


/**
 * Matrix
 *
 * A two dimensional (rank 2) tensor with integer and/or floating point elements.
 *
 * @category    Scientific Computing
 * @package     Rubix/Tensor
 * @author      Andrew DalPino
 */
ZEPHIR_INIT_CLASS(Tensor_Matrix)
{
	ZEPHIR_REGISTER_CLASS(Tensor, Matrix, tensor, matrix, tensor_matrix_method_entry, 0);

	/**
	 * The elements of the matrix, row-major, in one contiguous buffer.
	 *
	 * Flat, not nested: element (i, j) lives at i * n + j. One allocation for
	 * the whole matrix rather than one per row, which is also what lets an
	 * element-wise operation be a single call over m * n instead of m calls
	 * over n.
	 *
	 * @var \Tensor\Buffer
	 */
	zend_declare_property_null(tensor_matrix_ce, SL("a"), ZEND_ACC_PROTECTED);
	/**
	 * The number of rows in the matrix.
	 *
	 * @var int
	 */
	zend_declare_property_null(tensor_matrix_ce, SL("m"), ZEND_ACC_PROTECTED);
	/**
	 * The number of columns in the matrix.
	 *
	 * @var int
	 */
	zend_declare_property_null(tensor_matrix_ce, SL("n"), ZEND_ACC_PROTECTED);
	zend_class_implements(tensor_matrix_ce, 1, tensor_tensor_ce);
	return SUCCESS;
}

/**
 * Factory method to build a new matrix from an array.
 *
 * @param array[] a
 * @return self
 */
PHP_METHOD(Tensor_Matrix, build)
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
	object_init_ex(return_value, tensor_matrix_ce);
	ZVAL_BOOL(&_0, 1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 22, &a, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Build a new matrix foregoing any validation for quicker instantiation.
 *
 * @param array[] a
 * @return self
 */
PHP_METHOD(Tensor_Matrix, quick)
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
	object_init_ex(return_value, tensor_matrix_ce);
	ZVAL_BOOL(&_0, 0);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 22, &a, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return an identity matrix with dimensionality n x n.
 *
 * @param int n
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Matrix, identity)
{
	zend_bool _4, _7$$4;
	zval a, rowA;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *n_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, _10$$5;
	zend_long n, ZEPHIR_LAST_CALL_STATUS, i = 0, j = 0, _5, _6, _8$$4, _9$$4;

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_10$$5);
	ZVAL_UNDEF(&a);
	ZVAL_UNDEF(&rowA);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &n_param);
	if (UNEXPECTED(n < 1)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		object_init_ex(&_0$$3, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_1$$3, n);
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", NULL, 4, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SSVS(&_3$$3, "N must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 3, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/matrix.zep", 84);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&a);
	array_init(&a);
	ZEPHIR_INIT_VAR(&rowA);
	array_init(&rowA);
	_6 = (n - 1);
	_5 = 0;
	_4 = 0;
	if (_5 <= _6) {
		while (1) {
			if (_4) {
				_5++;
				if (!(_5 <= _6)) {
					break;
				}
			} else {
				_4 = 1;
			}
			i = _5;
			ZEPHIR_INIT_NVAR(&rowA);
			array_init(&rowA);
			_9$$4 = (n - 1);
			_8$$4 = 0;
			_7$$4 = 0;
			if (_8$$4 <= _9$$4) {
				while (1) {
					if (_7$$4) {
						_8$$4++;
						if (!(_8$$4 <= _9$$4)) {
							break;
						}
					} else {
						_7$$4 = 1;
					}
					j = _8$$4;
					ZEPHIR_INIT_NVAR(&_10$$5);
					if (i == j) {
						ZEPHIR_INIT_NVAR(&_10$$5);
						ZVAL_DOUBLE(&_10$$5, 1.0);
					} else {
						ZEPHIR_INIT_NVAR(&_10$$5);
						ZVAL_DOUBLE(&_10$$5, 0.0);
					}
					zephir_array_append(&rowA, &_10$$5, PH_SEPARATE, "tensor/matrix.zep", 96);
				}
			}
			zephir_array_append(&a, &rowA, PH_SEPARATE, "tensor/matrix.zep", 99);
		}
	}
	ZEPHIR_RETURN_CALL_SELF("quick", NULL, 0, &a);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return a zero matrix with the specified dimensionality.
 *
 * @param int m
 * @param int n
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Matrix, zeros)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *m_param = NULL, *n_param = NULL, _0, _1, _2;
	zend_long m, n, ZEPHIR_LAST_CALL_STATUS;

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(m)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 2, 0, &m_param, &n_param);
	ZVAL_DOUBLE(&_0, 0.0);
	ZVAL_LONG(&_1, m);
	ZVAL_LONG(&_2, n);
	ZEPHIR_RETURN_CALL_SELF("fill", NULL, 0, &_0, &_1, &_2);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return a one matrix with the given dimensions.
 *
 * @param int m
 * @param int n
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Matrix, ones)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *m_param = NULL, *n_param = NULL, _0, _1, _2;
	zend_long m, n, ZEPHIR_LAST_CALL_STATUS;

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(m)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 2, 0, &m_param, &n_param);
	ZVAL_DOUBLE(&_0, 1.0);
	ZVAL_LONG(&_1, m);
	ZVAL_LONG(&_2, n);
	ZEPHIR_RETURN_CALL_SELF("fill", NULL, 0, &_0, &_1, &_2);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Build a matrix with the value of each element along the diagonal
 * and zeros everywhere else.
 *
 * @param float[] elements
 * @return self
 */
PHP_METHOD(Tensor_Matrix, diagonal)
{
	zend_bool _1, _4$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS, n, i = 0, j = 0, _2, _3, _5$$3, _6$$3;
	zval *elements_param = NULL, _0, _7$$4;
	zval elements, a, rowA;

	ZVAL_UNDEF(&elements);
	ZVAL_UNDEF(&a);
	ZVAL_UNDEF(&rowA);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_7$$4);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		ZEPHIR_Z_PARAM_ARRAY(elements, elements_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &elements_param);
	zephir_get_arrval(&elements, elements_param);
	n = zephir_fast_count_int(&elements);
	ZEPHIR_CALL_FUNCTION(&_0, "array_values", NULL, 23, &elements);
	zephir_check_call_status();
	ZEPHIR_CPY_WRT(&elements, &_0);
	ZEPHIR_INIT_VAR(&a);
	array_init(&a);
	ZEPHIR_INIT_VAR(&rowA);
	array_init(&rowA);
	_3 = (n - 1);
	_2 = 0;
	_1 = 0;
	if (_2 <= _3) {
		while (1) {
			if (_1) {
				_2++;
				if (!(_2 <= _3)) {
					break;
				}
			} else {
				_1 = 1;
			}
			i = _2;
			ZEPHIR_INIT_NVAR(&rowA);
			array_init(&rowA);
			_6$$3 = (n - 1);
			_5$$3 = 0;
			_4$$3 = 0;
			if (_5$$3 <= _6$$3) {
				while (1) {
					if (_4$$3) {
						_5$$3++;
						if (!(_5$$3 <= _6$$3)) {
							break;
						}
					} else {
						_4$$3 = 1;
					}
					j = _5$$3;
					ZEPHIR_INIT_NVAR(&_7$$4);
					if (i == j) {
						ZEPHIR_OBS_NVAR(&_7$$4);
						zephir_array_fetch_long(&_7$$4, &elements, i, PH_NOISY, "tensor/matrix.zep", 153);
					} else {
						ZEPHIR_INIT_NVAR(&_7$$4);
						ZVAL_DOUBLE(&_7$$4, 0.0);
					}
					zephir_array_append(&rowA, &_7$$4, PH_SEPARATE, "tensor/matrix.zep", 153);
				}
			}
			zephir_array_append(&a, &rowA, PH_SEPARATE, "tensor/matrix.zep", 156);
		}
	}
	ZEPHIR_RETURN_CALL_SELF("quick", NULL, 0, &a);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Fill a matrix with a given value at each element.
 *
 * @param float value
 * @param int m
 * @param int n
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Matrix, fill)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_3 = NULL, *_13 = NULL;
	zend_long m, n, ZEPHIR_LAST_CALL_STATUS;
	zval *value_param = NULL, *m_param = NULL, *n_param = NULL, _0$$3, _1$$3, _2$$3, _4$$3, _5$$4, _6$$4, _7$$4, _8$$4, _9, _10, _11, _12, _14;
	double value;

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_6$$4);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_8$$4);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_14);
	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_ZVAL(value_param)
		Z_PARAM_LONG(m)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 3, 0, &value_param, &m_param, &n_param);
	value = zephir_get_doubleval(value_param);
	if (UNEXPECTED(m < 1)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		object_init_ex(&_0$$3, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_1$$3, m);
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", &_3, 4, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_4$$3);
		ZEPHIR_CONCAT_SSVS(&_4$$3, "M must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 3, &_4$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/matrix.zep", 175);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(n < 1)) {
		ZEPHIR_INIT_VAR(&_5$$4);
		object_init_ex(&_5$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_6$$4, n);
		ZEPHIR_CALL_FUNCTION(&_7$$4, "strval", &_3, 4, &_6$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_8$$4);
		ZEPHIR_CONCAT_SSVS(&_8$$4, "N must be", " greater than 0, ", &_7$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 3, &_8$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/matrix.zep", 180);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZVAL_LONG(&_9, 0);
	ZVAL_LONG(&_10, n);
	ZVAL_DOUBLE(&_11, value);
	ZEPHIR_CALL_FUNCTION(&_12, "array_fill", &_13, 24, &_9, &_10, &_11);
	zephir_check_call_status();
	ZVAL_LONG(&_9, 0);
	ZVAL_LONG(&_10, m);
	ZEPHIR_CALL_FUNCTION(&_14, "array_fill", &_13, 24, &_9, &_10, &_12);
	zephir_check_call_status();
	ZEPHIR_RETURN_CALL_SELF("quick", NULL, 0, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return a random uniform matrix with values between 0 and 1.
 *
 * @param int m
 * @param int n
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Matrix, rand)
{
	zval a, rowA;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_3 = NULL, *_11 = NULL;
	zval *m_param = NULL, *n_param = NULL, _0$$3, _1$$3, _2$$3, _4$$3, _5$$4, _6$$4, _7$$4, _8$$4, _9, _10$$6, _12$$6;
	zend_long m, n, ZEPHIR_LAST_CALL_STATUS, max;

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_6$$4);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_8$$4);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10$$6);
	ZVAL_UNDEF(&_12$$6);
	ZVAL_UNDEF(&a);
	ZVAL_UNDEF(&rowA);
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(m)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 2, 0, &m_param, &n_param);
	if (UNEXPECTED(m < 1)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		object_init_ex(&_0$$3, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_1$$3, m);
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", &_3, 4, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_4$$3);
		ZEPHIR_CONCAT_SSVS(&_4$$3, "M must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 3, &_4$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/matrix.zep", 198);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(n < 1)) {
		ZEPHIR_INIT_VAR(&_5$$4);
		object_init_ex(&_5$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_6$$4, n);
		ZEPHIR_CALL_FUNCTION(&_7$$4, "strval", &_3, 4, &_6$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_8$$4);
		ZEPHIR_CONCAT_SSVS(&_8$$4, "N must be", " greater than 0, ", &_7$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 3, &_8$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/matrix.zep", 203);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&a);
	array_init(&a);
	ZEPHIR_INIT_VAR(&rowA);
	array_init(&rowA);
	ZEPHIR_CALL_FUNCTION(&_9, "getrandmax", NULL, 5);
	zephir_check_call_status();
	max = zephir_get_intval(&_9);
	while (1) {
		if (!(zephir_fast_count_int(&a) < m)) {
			break;
		}
		ZEPHIR_INIT_NVAR(&rowA);
		array_init(&rowA);
		while (1) {
			if (!(zephir_fast_count_int(&rowA) < n)) {
				break;
			}
			ZEPHIR_CALL_FUNCTION(&_10$$6, "rand", &_11, 6);
			zephir_check_call_status();
			ZEPHIR_INIT_NVAR(&_12$$6);
			ZVAL_DOUBLE(&_12$$6, zephir_safe_div_zval_long(&_10$$6, max));
			zephir_array_append(&rowA, &_12$$6, PH_SEPARATE, "tensor/matrix.zep", 215);
		}
		zephir_array_append(&a, &rowA, PH_SEPARATE, "tensor/matrix.zep", 218);
	}
	ZEPHIR_RETURN_CALL_SELF("quick", NULL, 0, &a);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return a standard normally (Gaussian( distributed random matrix of specified dimensionality.
 *
 * @param int m
 * @param int n
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Matrix, gaussian)
{
	zval a, rowA, extras;
	double r = 0, phi = 0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_3 = NULL, *_11 = NULL, *_13 = NULL, *_16 = NULL;
	zval *m_param = NULL, *n_param = NULL, _0$$3, _1$$3, _2$$3, _4$$3, _5$$4, _6$$4, _7$$4, _8$$4, _9, _10$$6, _12$$7, _14$$7, _15$$7, _17$$7, _18$$7, _19$$7, _20$$7, _21$$8;
	zend_long m, n, ZEPHIR_LAST_CALL_STATUS, max;

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_6$$4);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_8$$4);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10$$6);
	ZVAL_UNDEF(&_12$$7);
	ZVAL_UNDEF(&_14$$7);
	ZVAL_UNDEF(&_15$$7);
	ZVAL_UNDEF(&_17$$7);
	ZVAL_UNDEF(&_18$$7);
	ZVAL_UNDEF(&_19$$7);
	ZVAL_UNDEF(&_20$$7);
	ZVAL_UNDEF(&_21$$8);
	ZVAL_UNDEF(&a);
	ZVAL_UNDEF(&rowA);
	ZVAL_UNDEF(&extras);
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(m)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 2, 0, &m_param, &n_param);
	if (UNEXPECTED(m < 1)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		object_init_ex(&_0$$3, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_1$$3, m);
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", &_3, 4, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_4$$3);
		ZEPHIR_CONCAT_SSVS(&_4$$3, "M must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 3, &_4$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/matrix.zep", 236);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(n < 1)) {
		ZEPHIR_INIT_VAR(&_5$$4);
		object_init_ex(&_5$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_6$$4, n);
		ZEPHIR_CALL_FUNCTION(&_7$$4, "strval", &_3, 4, &_6$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_8$$4);
		ZEPHIR_CONCAT_SSVS(&_8$$4, "N must be", " greater than 0, ", &_7$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 3, &_8$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/matrix.zep", 241);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&a);
	array_init(&a);
	ZEPHIR_INIT_VAR(&rowA);
	array_init(&rowA);
	ZEPHIR_INIT_VAR(&extras);
	array_init(&extras);
	ZEPHIR_CALL_FUNCTION(&_9, "getrandmax", NULL, 5);
	zephir_check_call_status();
	max = zephir_get_intval(&_9);
	while (1) {
		if (!(zephir_fast_count_int(&a) < m)) {
			break;
		}
		ZEPHIR_INIT_NVAR(&rowA);
		array_init(&rowA);
		if (!(ZEPHIR_IS_EMPTY(&extras))) {
			ZEPHIR_MAKE_REF(&extras);
			ZEPHIR_CALL_FUNCTION(&_10$$6, "array_pop", &_11, 8, &extras);
			ZEPHIR_UNREF(&extras);
			zephir_check_call_status();
			zephir_array_append(&rowA, &_10$$6, PH_SEPARATE, "tensor/matrix.zep", 256);
		}
		while (1) {
			if (!(zephir_fast_count_int(&rowA) < n)) {
				break;
			}
			ZEPHIR_CALL_FUNCTION(&_12$$7, "rand", &_13, 6);
			zephir_check_call_status();
			ZVAL_DOUBLE(&_14$$7, zephir_safe_div_zval_long(&_12$$7, max));
			ZEPHIR_CALL_FUNCTION(&_15$$7, "log", &_16, 7, &_14$$7);
			zephir_check_call_status();
			ZVAL_DOUBLE(&_14$$7, (-2.0 * zephir_get_numberval(&_15$$7)));
			r =  (sqrt((-2.0 * zephir_get_numberval(&_15$$7))));
			ZEPHIR_CALL_FUNCTION(&_17$$7, "rand", &_13, 6);
			zephir_check_call_status();
			phi =  ((zephir_safe_div_zval_long(&_17$$7, max) * 6.28318530718));
			ZVAL_DOUBLE(&_18$$7, phi);
			ZEPHIR_INIT_NVAR(&_19$$7);
			ZVAL_DOUBLE(&_19$$7, (r * sin(phi)));
			zephir_array_append(&rowA, &_19$$7, PH_SEPARATE, "tensor/matrix.zep", 264);
			ZVAL_DOUBLE(&_20$$7, phi);
			ZEPHIR_INIT_NVAR(&_19$$7);
			ZVAL_DOUBLE(&_19$$7, (r * cos(phi)));
			zephir_array_append(&rowA, &_19$$7, PH_SEPARATE, "tensor/matrix.zep", 265);
		}
		if (zephir_fast_count_int(&rowA) > n) {
			ZEPHIR_MAKE_REF(&rowA);
			ZEPHIR_CALL_FUNCTION(&_21$$8, "array_pop", &_11, 8, &rowA);
			ZEPHIR_UNREF(&rowA);
			zephir_check_call_status();
			zephir_array_append(&extras, &_21$$8, PH_SEPARATE, "tensor/matrix.zep", 269);
		}
		zephir_array_append(&a, &rowA, PH_SEPARATE, "tensor/matrix.zep", 272);
	}
	ZEPHIR_RETURN_CALL_SELF("quick", NULL, 0, &a);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Generate a m x n matrix with elements from a Poisson distribution.
 *
 * @param int m
 * @param int n
 * @param float lambda
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Matrix, poisson)
{
	zval a, rowA;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_3 = NULL, *_20 = NULL;
	double lambda, l = 0, p = 0, k = 0;
	zval *m_param = NULL, *n_param = NULL, *lambda_param = NULL, _0$$3, _1$$3, _2$$3, _4$$3, _5$$4, _6$$4, _7$$4, _8$$4, _9$$5, _10$$5, _11$$5, _12$$5, _13$$6, _14$$6, _15$$6, _16, _17, _18, _19$$9, _21$$8;
	zend_long m, n, ZEPHIR_LAST_CALL_STATUS, max;

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_6$$4);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_8$$4);
	ZVAL_UNDEF(&_9$$5);
	ZVAL_UNDEF(&_10$$5);
	ZVAL_UNDEF(&_11$$5);
	ZVAL_UNDEF(&_12$$5);
	ZVAL_UNDEF(&_13$$6);
	ZVAL_UNDEF(&_14$$6);
	ZVAL_UNDEF(&_15$$6);
	ZVAL_UNDEF(&_16);
	ZVAL_UNDEF(&_17);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19$$9);
	ZVAL_UNDEF(&_21$$8);
	ZVAL_UNDEF(&a);
	ZVAL_UNDEF(&rowA);
	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_LONG(m)
		Z_PARAM_LONG(n)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(lambda_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 2, 1, &m_param, &n_param, &lambda_param);
	if (!lambda_param) {
		lambda = 1.0;
	} else {
		lambda = zephir_get_doubleval(lambda_param);
	}
	if (UNEXPECTED(m < 1)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		object_init_ex(&_0$$3, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_1$$3, m);
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", &_3, 4, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_4$$3);
		ZEPHIR_CONCAT_SSVS(&_4$$3, "M must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 3, &_4$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/matrix.zep", 291);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(n < 1)) {
		ZEPHIR_INIT_VAR(&_5$$4);
		object_init_ex(&_5$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_6$$4, n);
		ZEPHIR_CALL_FUNCTION(&_7$$4, "strval", &_3, 4, &_6$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_8$$4);
		ZEPHIR_CONCAT_SSVS(&_8$$4, "N must be", " greater than 0, ", &_7$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 3, &_8$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/matrix.zep", 296);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(lambda < 0.0)) {
		ZEPHIR_INIT_VAR(&_9$$5);
		object_init_ex(&_9$$5, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_DOUBLE(&_10$$5, lambda);
		ZEPHIR_CALL_FUNCTION(&_11$$5, "strval", &_3, 4, &_10$$5);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_12$$5);
		ZEPHIR_CONCAT_SSVS(&_12$$5, "Lambda must be", " greater than or equal to 0, ", &_11$$5, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_9$$5, "__construct", NULL, 3, &_12$$5);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_9$$5, "tensor/matrix.zep", 301);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(lambda == 0.0)) {
		ZVAL_DOUBLE(&_13$$6, 0.0);
		ZVAL_LONG(&_14$$6, m);
		ZVAL_LONG(&_15$$6, n);
		ZEPHIR_RETURN_CALL_SELF("fill", NULL, 0, &_13$$6, &_14$$6, &_15$$6);
		zephir_check_call_status();
		RETURN_MM();
	}
	ZEPHIR_INIT_VAR(&a);
	array_init(&a);
	ZEPHIR_INIT_VAR(&rowA);
	array_init(&rowA);
	ZVAL_DOUBLE(&_16, -lambda);
	ZEPHIR_CALL_FUNCTION(&_17, "exp", NULL, 9, &_16);
	zephir_check_call_status();
	l =  (zephir_get_doubleval(&_17));
	ZEPHIR_CALL_FUNCTION(&_18, "getrandmax", NULL, 5);
	zephir_check_call_status();
	max = zephir_get_intval(&_18);
	while (1) {
		if (!(zephir_fast_count_int(&a) < m)) {
			break;
		}
		ZEPHIR_INIT_NVAR(&rowA);
		array_init(&rowA);
		while (1) {
			if (!(zephir_fast_count_int(&rowA) < n)) {
				break;
			}
			k =  (0.0);
			p =  (1.0);
			while (1) {
				if (!(p > l)) {
					break;
				}
				k++;
				ZEPHIR_CALL_FUNCTION(&_19$$9, "rand", &_20, 6);
				zephir_check_call_status();
				p *=  (zephir_safe_div_zval_long(&_19$$9, max));
			}
			ZEPHIR_INIT_NVAR(&_21$$8);
			ZVAL_DOUBLE(&_21$$8, (k - 1.0));
			zephir_array_append(&rowA, &_21$$8, PH_SEPARATE, "tensor/matrix.zep", 330);
		}
		zephir_array_append(&a, &rowA, PH_SEPARATE, "tensor/matrix.zep", 333);
	}
	ZEPHIR_RETURN_CALL_SELF("quick", NULL, 0, &a);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return a random uniformly distributed matrix with values between -1 and 1.
 *
 * @param int m
 * @param int n
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Matrix, uniform)
{
	zval a, rowA;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_3 = NULL, *_13 = NULL;
	zval *m_param = NULL, *n_param = NULL, _0$$3, _1$$3, _2$$3, _4$$3, _5$$4, _6$$4, _7$$4, _8$$4, _9, _10$$6, _11$$6, _12$$6, _14$$6;
	zend_long m, n, ZEPHIR_LAST_CALL_STATUS, max;

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_6$$4);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_8$$4);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10$$6);
	ZVAL_UNDEF(&_11$$6);
	ZVAL_UNDEF(&_12$$6);
	ZVAL_UNDEF(&_14$$6);
	ZVAL_UNDEF(&a);
	ZVAL_UNDEF(&rowA);
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(m)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 2, 0, &m_param, &n_param);
	if (UNEXPECTED(m < 1)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		object_init_ex(&_0$$3, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_1$$3, m);
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", &_3, 4, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_4$$3);
		ZEPHIR_CONCAT_SSVS(&_4$$3, "M must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 3, &_4$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/matrix.zep", 351);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(n < 1)) {
		ZEPHIR_INIT_VAR(&_5$$4);
		object_init_ex(&_5$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_6$$4, n);
		ZEPHIR_CALL_FUNCTION(&_7$$4, "strval", &_3, 4, &_6$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_8$$4);
		ZEPHIR_CONCAT_SSVS(&_8$$4, "N must be", " greater than 0, ", &_7$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 3, &_8$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/matrix.zep", 356);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&a);
	array_init(&a);
	ZEPHIR_INIT_VAR(&rowA);
	array_init(&rowA);
	ZEPHIR_CALL_FUNCTION(&_9, "getrandmax", NULL, 5);
	zephir_check_call_status();
	max = zephir_get_intval(&_9);
	while (1) {
		if (!(zephir_fast_count_int(&a) < m)) {
			break;
		}
		ZEPHIR_INIT_NVAR(&rowA);
		array_init(&rowA);
		while (1) {
			if (!(zephir_fast_count_int(&rowA) < n)) {
				break;
			}
			ZVAL_LONG(&_10$$6, -max);
			ZVAL_LONG(&_11$$6, max);
			ZEPHIR_CALL_FUNCTION(&_12$$6, "rand", &_13, 6, &_10$$6, &_11$$6);
			zephir_check_call_status();
			ZEPHIR_INIT_NVAR(&_14$$6);
			ZVAL_DOUBLE(&_14$$6, zephir_safe_div_zval_long(&_12$$6, max));
			zephir_array_append(&rowA, &_14$$6, PH_SEPARATE, "tensor/matrix.zep", 368);
		}
		zephir_array_append(&a, &rowA, PH_SEPARATE, "tensor/matrix.zep", 371);
	}
	ZEPHIR_RETURN_CALL_SELF("quick", NULL, 0, &a);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * @param array[] a
 * @param bool validate
 * @throws \Tensor\Exceptions\InvalidArgumentException
 */
PHP_METHOD(Tensor_Matrix, __construct)
{
	zend_string *_8$$6;
	zend_ulong _7$$6;
	zval b$$6, rowB$$6;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_12 = NULL, *_15 = NULL, *_21 = NULL;
	zend_long rows, columns, ZEPHIR_LAST_CALL_STATUS, m, n;
	zend_bool validate, _27$$6, _23$$7, _39$$11;
	zval *a = NULL, a_sub, *validate_param = NULL, *rows_param = NULL, *columns_param = NULL, i, rowA, valueA, _1, _2, _42, _43, _0$$3, _3$$6, *_4$$6, _5$$6, *_6$$6, _26$$6, _9$$8, _10$$8, _11$$8, _13$$8, _14$$8, *_16$$7, _17$$7, *_18$$7, _22$$7, _19$$9, _20$$9, _24$$10, _25$$10, _28$$12, _29$$12, _30$$12, _31$$12, _32$$12, *_33$$11, _34$$11, *_35$$11, _38$$11, _36$$13, _37$$13, _40$$14, _41$$14;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&a_sub);
	ZVAL_UNDEF(&i);
	ZVAL_UNDEF(&rowA);
	ZVAL_UNDEF(&valueA);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_42);
	ZVAL_UNDEF(&_43);
	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_3$$6);
	ZVAL_UNDEF(&_5$$6);
	ZVAL_UNDEF(&_26$$6);
	ZVAL_UNDEF(&_9$$8);
	ZVAL_UNDEF(&_10$$8);
	ZVAL_UNDEF(&_11$$8);
	ZVAL_UNDEF(&_13$$8);
	ZVAL_UNDEF(&_14$$8);
	ZVAL_UNDEF(&_17$$7);
	ZVAL_UNDEF(&_22$$7);
	ZVAL_UNDEF(&_19$$9);
	ZVAL_UNDEF(&_20$$9);
	ZVAL_UNDEF(&_24$$10);
	ZVAL_UNDEF(&_25$$10);
	ZVAL_UNDEF(&_28$$12);
	ZVAL_UNDEF(&_29$$12);
	ZVAL_UNDEF(&_30$$12);
	ZVAL_UNDEF(&_31$$12);
	ZVAL_UNDEF(&_32$$12);
	ZVAL_UNDEF(&_34$$11);
	ZVAL_UNDEF(&_38$$11);
	ZVAL_UNDEF(&_36$$13);
	ZVAL_UNDEF(&_37$$13);
	ZVAL_UNDEF(&_40$$14);
	ZVAL_UNDEF(&_41$$14);
	ZVAL_UNDEF(&b$$6);
	ZVAL_UNDEF(&rowB$$6);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(0, 4)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(a)
		Z_PARAM_BOOL(validate)
		Z_PARAM_LONG(rows)
		Z_PARAM_LONG(columns)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 0, 4, &a, &validate_param, &rows_param, &columns_param);
	if (!a) {
		a = &a_sub;
		ZEPHIR_INIT_VAR(a);
		array_init(a);
	} else {
		ZEPHIR_SEPARATE_PARAM(a);
	}
	if (!validate_param) {
		validate = 1;
	} else {
		}
	if (!rows_param) {
		rows = -1;
	} else {
		}
	if (!columns_param) {
		columns = -1;
	} else {
		}
	if (Z_TYPE_P(a) != IS_ARRAY) {
		zephir_update_property_zval_cached(this_ptr, _zephir_prop_0, 16, a);
		ZVAL_UNDEF(&_0$$3);
		ZVAL_LONG(&_0$$3, rows);
		zephir_update_property_zval_cached(this_ptr, _zephir_prop_1, 17, &_0$$3);
		ZVAL_UNDEF(&_0$$3);
		ZVAL_LONG(&_0$$3, columns);
		zephir_update_property_zval_cached(this_ptr, _zephir_prop_2, 18, &_0$$3);
		RETURN_MM_NULL();
	}
	m = zephir_fast_count_int(a);
	ZEPHIR_INIT_VAR(&_1);
	ZEPHIR_CALL_FUNCTION(&_2, "current", NULL, 25, a);
	zephir_check_call_status();
	if (!(zephir_is_true(&_2))) {
		ZEPHIR_INIT_NVAR(&_1);
		array_init(&_1);
	} else {
		ZEPHIR_CALL_FUNCTION(&_1, "current", NULL, 25, a);
		zephir_check_call_status();
	}
	n = zephir_fast_count_int(&_1);
	if (validate) {
		ZEPHIR_INIT_VAR(&b$$6);
		array_init(&b$$6);
		ZEPHIR_INIT_VAR(&rowB$$6);
		array_init(&rowB$$6);
		ZEPHIR_CALL_FUNCTION(&_3$$6, "array_values", NULL, 23, a);
		zephir_check_call_status();
		ZEPHIR_CPY_WRT(a, &_3$$6);
		if (Z_TYPE_P(a) == IS_STRING) {
			ZEPHIR_INIT_VAR(&_5$$6);
			zephir_string_to_char_array(&_5$$6, a);
			_4$$6 = &_5$$6;
		} else {
			_4$$6 = a;
		}
		zephir_is_iterable(_4$$6, 0, "tensor/matrix.zep", 423);
		if (Z_TYPE_P(_4$$6) == IS_ARRAY) {
			ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(_4$$6), _7$$6, _8$$6, _6$$6)
			{
				ZEPHIR_INIT_NVAR(&i);
				if (_8$$6 != NULL) { 
					ZVAL_STR_COPY(&i, _8$$6);
				} else {
					ZVAL_LONG(&i, _7$$6);
				}
				ZEPHIR_INIT_NVAR(&rowA);
				ZVAL_COPY(&rowA, _6$$6);
				if (UNEXPECTED(zephir_fast_count_int(&rowA) != n)) {
					ZEPHIR_INIT_NVAR(&_9$$8);
					object_init_ex(&_9$$8, tensor_exceptions_invalidargumentexception_ce);
					ZVAL_LONG(&_10$$8, n);
					ZEPHIR_CALL_FUNCTION(&_11$$8, "strval", &_12, 4, &_10$$8);
					zephir_check_call_status();
					ZEPHIR_INIT_NVAR(&_13$$8);
					ZVAL_LONG(&_13$$8, zephir_fast_count_int(&rowA));
					ZEPHIR_INIT_NVAR(&_14$$8);
					ZEPHIR_CONCAT_SSVSVSSVS(&_14$$8, "The number of columns", " must be equal for all rows, ", &_11$$8, " needed but ", &_13$$8, " given", " at row offset ", &i, ".");
					ZEPHIR_CALL_METHOD(NULL, &_9$$8, "__construct", &_15, 3, &_14$$8);
					zephir_check_call_status();
					zephir_throw_exception_debug(&_9$$8, "tensor/matrix.zep", 411);
					ZEPHIR_MM_RESTORE();
					return;
				}
				ZEPHIR_INIT_NVAR(&rowB$$6);
				array_init(&rowB$$6);
				ZEPHIR_CPY_WRT(&rowB$$6, &rowB$$6);
				if (Z_TYPE_P(&rowA) == IS_STRING) {
					ZEPHIR_INIT_NVAR(&_17$$7);
					zephir_string_to_char_array(&_17$$7, &rowA);
					_16$$7 = &_17$$7;
				} else {
					_16$$7 = &rowA;
				}
				zephir_is_iterable(_16$$7, 0, "tensor/matrix.zep", 420);
				if (Z_TYPE_P(_16$$7) == IS_ARRAY) {
					ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_16$$7), _18$$7)
					{
						ZEPHIR_INIT_NVAR(&valueA);
						ZVAL_COPY(&valueA, _18$$7);
						ZEPHIR_INIT_NVAR(&_19$$9);
						ZEPHIR_CALL_FUNCTION(&_20$$9, "is_float", &_21, 2, &valueA);
						zephir_check_call_status();
						if (zephir_is_true(&_20$$9)) {
							ZEPHIR_CPY_WRT(&_19$$9, &valueA);
						} else {
							ZEPHIR_INIT_NVAR(&_19$$9);
							ZVAL_DOUBLE(&_19$$9, zephir_get_doubleval(&valueA));
						}
						zephir_array_append(&rowB$$6, &_19$$9, PH_SEPARATE, "tensor/matrix.zep", 417);
					} ZEND_HASH_FOREACH_END();
				} else {
					ZEPHIR_CALL_METHOD(NULL, _16$$7, "rewind", NULL, 0);
					zephir_check_call_status();
					_23$$7 = 1;
					while (1) {
						if (_23$$7) {
							_23$$7 = 0;
						} else {
							ZEPHIR_CALL_METHOD(NULL, _16$$7, "next", NULL, 0);
							zephir_check_call_status();
						}
						ZEPHIR_CALL_METHOD(&_22$$7, _16$$7, "valid", NULL, 0);
						zephir_check_call_status();
						if (!zend_is_true(&_22$$7)) {
							break;
						}
						ZEPHIR_CALL_METHOD(&valueA, _16$$7, "current", NULL, 0);
						zephir_check_call_status();
							ZEPHIR_INIT_NVAR(&_24$$10);
							ZEPHIR_CALL_FUNCTION(&_25$$10, "is_float", &_21, 2, &valueA);
							zephir_check_call_status();
							if (zephir_is_true(&_25$$10)) {
								ZEPHIR_CPY_WRT(&_24$$10, &valueA);
							} else {
								ZEPHIR_INIT_NVAR(&_24$$10);
								ZVAL_DOUBLE(&_24$$10, zephir_get_doubleval(&valueA));
							}
							zephir_array_append(&rowB$$6, &_24$$10, PH_SEPARATE, "tensor/matrix.zep", 417);
					}
				}
				ZEPHIR_INIT_NVAR(&valueA);
				zephir_array_append(&b$$6, &rowB$$6, PH_SEPARATE, "tensor/matrix.zep", 420);
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEPHIR_CALL_METHOD(NULL, _4$$6, "rewind", NULL, 0);
			zephir_check_call_status();
			_27$$6 = 1;
			while (1) {
				if (_27$$6) {
					_27$$6 = 0;
				} else {
					ZEPHIR_CALL_METHOD(NULL, _4$$6, "next", NULL, 0);
					zephir_check_call_status();
				}
				ZEPHIR_CALL_METHOD(&_26$$6, _4$$6, "valid", NULL, 0);
				zephir_check_call_status();
				if (!zend_is_true(&_26$$6)) {
					break;
				}
				ZEPHIR_CALL_METHOD(&i, _4$$6, "key", NULL, 0);
				zephir_check_call_status();
				ZEPHIR_CALL_METHOD(&rowA, _4$$6, "current", NULL, 0);
				zephir_check_call_status();
					if (UNEXPECTED(zephir_fast_count_int(&rowA) != n)) {
						ZEPHIR_INIT_NVAR(&_28$$12);
						object_init_ex(&_28$$12, tensor_exceptions_invalidargumentexception_ce);
						ZVAL_LONG(&_29$$12, n);
						ZEPHIR_CALL_FUNCTION(&_30$$12, "strval", &_12, 4, &_29$$12);
						zephir_check_call_status();
						ZEPHIR_INIT_NVAR(&_31$$12);
						ZVAL_LONG(&_31$$12, zephir_fast_count_int(&rowA));
						ZEPHIR_INIT_NVAR(&_32$$12);
						ZEPHIR_CONCAT_SSVSVSSVS(&_32$$12, "The number of columns", " must be equal for all rows, ", &_30$$12, " needed but ", &_31$$12, " given", " at row offset ", &i, ".");
						ZEPHIR_CALL_METHOD(NULL, &_28$$12, "__construct", &_15, 3, &_32$$12);
						zephir_check_call_status();
						zephir_throw_exception_debug(&_28$$12, "tensor/matrix.zep", 411);
						ZEPHIR_MM_RESTORE();
						return;
					}
					ZEPHIR_INIT_NVAR(&rowB$$6);
					array_init(&rowB$$6);
					ZEPHIR_CPY_WRT(&rowB$$6, &rowB$$6);
					if (Z_TYPE_P(&rowA) == IS_STRING) {
						ZEPHIR_INIT_NVAR(&_34$$11);
						zephir_string_to_char_array(&_34$$11, &rowA);
						_33$$11 = &_34$$11;
					} else {
						_33$$11 = &rowA;
					}
					zephir_is_iterable(_33$$11, 0, "tensor/matrix.zep", 420);
					if (Z_TYPE_P(_33$$11) == IS_ARRAY) {
						ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_33$$11), _35$$11)
						{
							ZEPHIR_INIT_NVAR(&valueA);
							ZVAL_COPY(&valueA, _35$$11);
							ZEPHIR_INIT_NVAR(&_36$$13);
							ZEPHIR_CALL_FUNCTION(&_37$$13, "is_float", &_21, 2, &valueA);
							zephir_check_call_status();
							if (zephir_is_true(&_37$$13)) {
								ZEPHIR_CPY_WRT(&_36$$13, &valueA);
							} else {
								ZEPHIR_INIT_NVAR(&_36$$13);
								ZVAL_DOUBLE(&_36$$13, zephir_get_doubleval(&valueA));
							}
							zephir_array_append(&rowB$$6, &_36$$13, PH_SEPARATE, "tensor/matrix.zep", 417);
						} ZEND_HASH_FOREACH_END();
					} else {
						ZEPHIR_CALL_METHOD(NULL, _33$$11, "rewind", NULL, 0);
						zephir_check_call_status();
						_39$$11 = 1;
						while (1) {
							if (_39$$11) {
								_39$$11 = 0;
							} else {
								ZEPHIR_CALL_METHOD(NULL, _33$$11, "next", NULL, 0);
								zephir_check_call_status();
							}
							ZEPHIR_CALL_METHOD(&_38$$11, _33$$11, "valid", NULL, 0);
							zephir_check_call_status();
							if (!zend_is_true(&_38$$11)) {
								break;
							}
							ZEPHIR_CALL_METHOD(&valueA, _33$$11, "current", NULL, 0);
							zephir_check_call_status();
								ZEPHIR_INIT_NVAR(&_40$$14);
								ZEPHIR_CALL_FUNCTION(&_41$$14, "is_float", &_21, 2, &valueA);
								zephir_check_call_status();
								if (zephir_is_true(&_41$$14)) {
									ZEPHIR_CPY_WRT(&_40$$14, &valueA);
								} else {
									ZEPHIR_INIT_NVAR(&_40$$14);
									ZVAL_DOUBLE(&_40$$14, zephir_get_doubleval(&valueA));
								}
								zephir_array_append(&rowB$$6, &_40$$14, PH_SEPARATE, "tensor/matrix.zep", 417);
						}
					}
					ZEPHIR_INIT_NVAR(&valueA);
					zephir_array_append(&b$$6, &rowB$$6, PH_SEPARATE, "tensor/matrix.zep", 420);
			}
		}
		ZEPHIR_INIT_NVAR(&rowA);
		ZEPHIR_INIT_NVAR(&i);
		ZEPHIR_CPY_WRT(a, &b$$6);
	}
	ZEPHIR_INIT_VAR(&_42);
	tensor_buffer_from_matrix(&_42, a);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_0, 16, &_42);
	ZVAL_UNDEF(&_43);
	ZVAL_LONG(&_43, m);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_1, 17, &_43);
	ZVAL_UNDEF(&_43);
	ZVAL_LONG(&_43, n);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_2, 18, &_43);
	ZEPHIR_MM_RESTORE();
}

/**
 * Wrap a buffer a C handler has just allocated, without copying it.
 *
 * Protected, so it stays off the public surface and out of the parity
 * harness's coverage obligation.
 */
PHP_METHOD(Tensor_Matrix, wrap)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long rows, columns, ZEPHIR_LAST_CALL_STATUS;
	zval *buffer, buffer_sub, *rows_param = NULL, *columns_param = NULL, _0, _1, _2;

	ZVAL_UNDEF(&buffer_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_ZVAL(buffer)
		Z_PARAM_LONG(rows)
		Z_PARAM_LONG(columns)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 3, 0, &buffer, &rows_param, &columns_param);
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZVAL_BOOL(&_0, 0);
	ZVAL_LONG(&_1, rows);
	ZVAL_LONG(&_2, columns);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 22, buffer, &_0, &_1, &_2);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return a tuple with the dimensionality of the tensor.
 *
 * @return int[]
 */
PHP_METHOD(Tensor_Matrix, shape)
{
	zval _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	zephir_create_array(return_value, 2, 0);
	zephir_memory_observe(&_0);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
	zephir_array_fast_append(return_value, &_0);
	ZEPHIR_OBS_NVAR(&_0);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC);
	zephir_array_fast_append(return_value, &_0);
	RETURN_MM();
}

/**
 * Return the shape of the tensor as a string.
 *
 * @return string
 */
PHP_METHOD(Tensor_Matrix, shapeString)
{
	zval _1, _3;
	zval _0, _2;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	zephir_memory_observe(&_0);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
	zephir_cast_to_string(&_1, &_0);
	zephir_memory_observe(&_2);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC);
	zephir_cast_to_string(&_3, &_2);
	ZEPHIR_CONCAT_VSV(return_value, &_1, " x ", &_3);
	RETURN_MM();
}

/**
 * Is this a square matrix?
 *
 * @return bool
 */
PHP_METHOD(Tensor_Matrix, isSquare)
{
	zval _0, _1;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	RETURN_BOOL(ZEPHIR_IS_IDENTICAL(&_0, &_1));
}

/**
 * Return the number of elements in the tensor.
 *
 * @return int
 */
PHP_METHOD(Tensor_Matrix, size)
{
	zval _0, _1;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	mul_function(return_value, &_0, &_1);
	return;
}

/**
 * Return the number of rows in the matrix.
 *
 * @return int
 */
PHP_METHOD(Tensor_Matrix, m)
{

	RETURN_MEMBER_TYPED(getThis(), "m", IS_LONG);
}

/**
 * Return the number of columns in the matrix.
 *
 * @return int
 */
PHP_METHOD(Tensor_Matrix, n)
{

	RETURN_MEMBER_TYPED(getThis(), "n", IS_LONG);
}

/**
 * Return a row as a vector from the matrix.
 *
 * @param int index
 * @return \Tensor\Vector
 */
PHP_METHOD(Tensor_Matrix, rowAsVector)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *index_param = NULL, _0;
	zend_long index, ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(index)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &index_param);
	ZVAL_LONG(&_0, index);
	ZEPHIR_RETURN_CALL_METHOD(this_ptr, "offsetGet", NULL, 0, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return a column as a vector from the matrix.
 *
 * @param int index
 * @return \Tensor\ColumnVector
 */
PHP_METHOD(Tensor_Matrix, columnAsVector)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *index_param = NULL, _0, _1, _2, _3, _4, _5;
	zend_long index, ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(index)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &index_param);
	object_init_ex(return_value, tensor_columnvector_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_4, index);
	tensor_buffer_stride(&_0, &_1, &_4, &_2, &_3);
	ZVAL_BOOL(&_5, 0);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 1, &_0, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the diagonal elements of a square matrix as a vector.
 *
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return \Tensor\Vector
 */
PHP_METHOD(Tensor_Matrix, diagonalAsVector)
{
	zval _0, _4, _5, _6, _7, _8, _9, _10, _1$$3, _2$$3, _3$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_CALL_METHOD(&_0, this_ptr, "isSquare", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!zephir_is_true(&_0))) {
		ZEPHIR_INIT_VAR(&_1$$3);
		object_init_ex(&_1$$3, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_CALL_METHOD(&_2$$3, this_ptr, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SSVS(&_3$$3, "Matrix must be", " square, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_1$$3, "__construct", NULL, 3, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_1$$3, "tensor/matrix.zep", 534);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, tensor_vector_ce);
	ZEPHIR_INIT_VAR(&_4);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_8, 0);
	ZVAL_LONG(&_9, (zephir_get_numberval(&_6) + 1));
	tensor_buffer_stride(&_4, &_5, &_8, &_9, &_7);
	ZVAL_BOOL(&_10, 0);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 1, &_4, &_10);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the elements of the matrix in a 2-d array.
 *
 * @return list<list<float>>
 */
PHP_METHOD(Tensor_Matrix, asArray)
{
	zval _0, _1;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_to_matrix(return_value, &_0, &_1);
	return;
}

/**
 * Return each row as a vector in an array.
 *
 * @return \Tensor\Vector[]
 */
PHP_METHOD(Tensor_Matrix, asVectors)
{
	zval _1, _2;
	zval _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_create_array(&_0, 2, 0);
	ZEPHIR_INIT_VAR(&_1);
	ZVAL_STRING(&_1, "Tensor\\Vector");
	zephir_array_fast_append(&_0, &_1);
	ZEPHIR_INIT_NVAR(&_1);
	ZVAL_STRING(&_1, "quick");
	zephir_array_fast_append(&_0, &_1);
	ZEPHIR_CALL_METHOD(&_2, this_ptr, "asArray", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_RETURN_CALL_FUNCTION("array_map", NULL, 11, &_0, &_2);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return each column as a column vector in an array.
 *
 * @return \Tensor\ColumnVector[]
 */
PHP_METHOD(Tensor_Matrix, asColumnVectors)
{
	zend_bool _1;
	zval _0, _4$$3, _5$$3;
	zval vectors;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_6 = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS, i = 0, _2, _3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&vectors);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&vectors);
	array_init(&vectors);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	_3 = (zephir_get_numberval(&_0) - 1);
	_2 = 0;
	_1 = 0;
	if (_2 <= _3) {
		while (1) {
			if (_1) {
				_2++;
				if (!(_2 <= _3)) {
					break;
				}
			} else {
				_1 = 1;
			}
			i = _2;
			ZVAL_LONG(&_5$$3, i);
			ZEPHIR_CALL_METHOD(&_4$$3, this_ptr, "columnAsVector", &_6, 0, &_5$$3);
			zephir_check_call_status();
			zephir_array_append(&vectors, &_4$$3, PH_SEPARATE, "tensor/matrix.zep", 573);
		}
	}
	RETURN_CTOR(&vectors);
}

/**
 * Flatten i.e unravel the matrix into a vector.
 *
 * @return \Tensor\Vector
 */
PHP_METHOD(Tensor_Matrix, flatten)
{
	zval _0, _1, _2, _3, _4, _5;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, tensor_vector_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_4);
	mul_function(&_4, &_2, &_3);
	ZVAL_LONG(&_2, 0);
	ZVAL_LONG(&_3, 1);
	tensor_buffer_stride(&_0, &_1, &_2, &_3, &_4);
	ZVAL_BOOL(&_5, 0);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 1, &_0, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Run a function over all of the elements in the matrix.
 *
 * @internal
 *
 * @param callable callback
 * @return self
 */
PHP_METHOD(Tensor_Matrix, map)
{
	zend_bool _7;
	zval b;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_5 = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *callback, callback_sub, rowA, _0, *_1, _2, *_3, _6, _4$$3, _8$$4;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&callback_sub);
	ZVAL_UNDEF(&rowA);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_8$$4);
	ZVAL_UNDEF(&b);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(callback)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &callback);
	ZEPHIR_INIT_VAR(&b);
	array_init(&b);
	ZEPHIR_CALL_METHOD(&_0, this_ptr, "asArray", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_0) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_2);
		zephir_string_to_char_array(&_2, &_0);
		_1 = &_2;
	} else {
		_1 = &_0;
	}
	zephir_is_iterable(_1, 0, "tensor/matrix.zep", 607);
	if (Z_TYPE_P(_1) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_1), _3)
		{
			ZEPHIR_INIT_NVAR(&rowA);
			ZVAL_COPY(&rowA, _3);
			ZEPHIR_CALL_FUNCTION(&_4$$3, "array_map", &_5, 11, callback, &rowA);
			zephir_check_call_status();
			zephir_array_append(&b, &_4$$3, PH_SEPARATE, "tensor/matrix.zep", 604);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _1, "rewind", NULL, 0);
		zephir_check_call_status();
		_7 = 1;
		while (1) {
			if (_7) {
				_7 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _1, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_6, _1, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_6)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowA, _1, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_CALL_FUNCTION(&_8$$4, "array_map", &_5, 11, callback, &rowA);
				zephir_check_call_status();
				zephir_array_append(&b, &_8$$4, PH_SEPARATE, "tensor/matrix.zep", 604);
		}
	}
	ZEPHIR_INIT_NVAR(&rowA);
	ZEPHIR_RETURN_CALL_SELF("quick", NULL, 0, &b);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Reduce the matrix down to a scalar using a callback function.
 *
 * @internal
 *
 * @param callable callback function (float carry, float value): float
 * @param float initial
 * @return float
 */
PHP_METHOD(Tensor_Matrix, reduce)
{
	zend_bool _12, _9$$3, _18$$6;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	double initial;
	zval *callback, callback_sub, *initial_param = NULL, rowA, valueA, carry, _0, *_1, _2, *_3, _11, *_4$$3, _5$$3, *_6$$3, _8$$3, _7$$4, _10$$5, *_13$$6, _14$$6, *_15$$6, _17$$6, _16$$7, _19$$8;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&callback_sub);
	ZVAL_UNDEF(&rowA);
	ZVAL_UNDEF(&valueA);
	ZVAL_UNDEF(&carry);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_8$$3);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_10$$5);
	ZVAL_UNDEF(&_14$$6);
	ZVAL_UNDEF(&_17$$6);
	ZVAL_UNDEF(&_16$$7);
	ZVAL_UNDEF(&_19$$8);
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(callback)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(initial_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 1, &callback, &initial_param);
	if (!initial_param) {
		initial = 0.0;
	} else {
		initial = zephir_get_doubleval(initial_param);
	}
	ZEPHIR_INIT_VAR(&carry);
	ZVAL_DOUBLE(&carry, initial);
	ZEPHIR_CALL_METHOD(&_0, this_ptr, "asArray", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_0) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_2);
		zephir_string_to_char_array(&_2, &_0);
		_1 = &_2;
	} else {
		_1 = &_0;
	}
	zephir_is_iterable(_1, 0, "tensor/matrix.zep", 631);
	if (Z_TYPE_P(_1) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_1), _3)
		{
			ZEPHIR_INIT_NVAR(&rowA);
			ZVAL_COPY(&rowA, _3);
			if (Z_TYPE_P(&rowA) == IS_STRING) {
				ZEPHIR_INIT_NVAR(&_5$$3);
				zephir_string_to_char_array(&_5$$3, &rowA);
				_4$$3 = &_5$$3;
			} else {
				_4$$3 = &rowA;
			}
			zephir_is_iterable(_4$$3, 0, "tensor/matrix.zep", 629);
			if (Z_TYPE_P(_4$$3) == IS_ARRAY) {
				ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_4$$3), _6$$3)
				{
					ZEPHIR_INIT_NVAR(&valueA);
					ZVAL_COPY(&valueA, _6$$3);
					ZEPHIR_CALL_ZVAL_FUNCTION(&_7$$4, callback, NULL, 0, &carry, &valueA);
					zephir_check_call_status();
					ZEPHIR_CPY_WRT(&carry, &_7$$4);
				} ZEND_HASH_FOREACH_END();
			} else {
				ZEPHIR_CALL_METHOD(NULL, _4$$3, "rewind", NULL, 0);
				zephir_check_call_status();
				_9$$3 = 1;
				while (1) {
					if (_9$$3) {
						_9$$3 = 0;
					} else {
						ZEPHIR_CALL_METHOD(NULL, _4$$3, "next", NULL, 0);
						zephir_check_call_status();
					}
					ZEPHIR_CALL_METHOD(&_8$$3, _4$$3, "valid", NULL, 0);
					zephir_check_call_status();
					if (!zend_is_true(&_8$$3)) {
						break;
					}
					ZEPHIR_CALL_METHOD(&valueA, _4$$3, "current", NULL, 0);
					zephir_check_call_status();
						ZEPHIR_CALL_ZVAL_FUNCTION(&_10$$5, callback, NULL, 0, &carry, &valueA);
						zephir_check_call_status();
						ZEPHIR_CPY_WRT(&carry, &_10$$5);
				}
			}
			ZEPHIR_INIT_NVAR(&valueA);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _1, "rewind", NULL, 0);
		zephir_check_call_status();
		_12 = 1;
		while (1) {
			if (_12) {
				_12 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _1, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_11, _1, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_11)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowA, _1, "current", NULL, 0);
			zephir_check_call_status();
				if (Z_TYPE_P(&rowA) == IS_STRING) {
					ZEPHIR_INIT_NVAR(&_14$$6);
					zephir_string_to_char_array(&_14$$6, &rowA);
					_13$$6 = &_14$$6;
				} else {
					_13$$6 = &rowA;
				}
				zephir_is_iterable(_13$$6, 0, "tensor/matrix.zep", 629);
				if (Z_TYPE_P(_13$$6) == IS_ARRAY) {
					ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_13$$6), _15$$6)
					{
						ZEPHIR_INIT_NVAR(&valueA);
						ZVAL_COPY(&valueA, _15$$6);
						ZEPHIR_CALL_ZVAL_FUNCTION(&_16$$7, callback, NULL, 0, &carry, &valueA);
						zephir_check_call_status();
						ZEPHIR_CPY_WRT(&carry, &_16$$7);
					} ZEND_HASH_FOREACH_END();
				} else {
					ZEPHIR_CALL_METHOD(NULL, _13$$6, "rewind", NULL, 0);
					zephir_check_call_status();
					_18$$6 = 1;
					while (1) {
						if (_18$$6) {
							_18$$6 = 0;
						} else {
							ZEPHIR_CALL_METHOD(NULL, _13$$6, "next", NULL, 0);
							zephir_check_call_status();
						}
						ZEPHIR_CALL_METHOD(&_17$$6, _13$$6, "valid", NULL, 0);
						zephir_check_call_status();
						if (!zend_is_true(&_17$$6)) {
							break;
						}
						ZEPHIR_CALL_METHOD(&valueA, _13$$6, "current", NULL, 0);
						zephir_check_call_status();
							ZEPHIR_CALL_ZVAL_FUNCTION(&_19$$8, callback, NULL, 0, &carry, &valueA);
							zephir_check_call_status();
							ZEPHIR_CPY_WRT(&carry, &_19$$8);
					}
				}
				ZEPHIR_INIT_NVAR(&valueA);
		}
	}
	ZEPHIR_INIT_NVAR(&rowA);
	RETURN_CCTOR(&carry);
}

/**
 * Transpose the matrix i.e row become columns and columns become rows.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, transpose)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_transpose(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Compute the inverse of the square matrix.
 *
 * @throws \Tensor\Exceptions\RuntimeException
 * @return self
 */
PHP_METHOD(Tensor_Matrix, inverse)
{
	zval _6$$4, _10$$5;
	zval _0, _4, result, _7, _8, _11, _12, _1$$3, _2$$3, _3$$3, _5$$4, _9$$5;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_9$$5);
	ZVAL_UNDEF(&_6$$4);
	ZVAL_UNDEF(&_10$$5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_CALL_METHOD(&_0, this_ptr, "isSquare", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!zephir_is_true(&_0))) {
		ZEPHIR_INIT_VAR(&_1$$3);
		object_init_ex(&_1$$3, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_CALL_METHOD(&_2$$3, this_ptr, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SSVS(&_3$$3, "Matrix must be", " square, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_1$$3, "__construct", NULL, 3, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_1$$3, "tensor/matrix.zep", 656);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&_4, this_ptr, "fullRank", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!zephir_is_true(&_4))) {
		ZEPHIR_INIT_VAR(&_5$$4);
		object_init_ex(&_5$$4, tensor_exceptions_runtimeexception_ce);
		ZEPHIR_INIT_VAR(&_6$$4);
		ZEPHIR_CONCAT_SS(&_6$$4, "Failed to compute the inverse", " of a singular matrix.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 26, &_6$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/matrix.zep", 661);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_inverse(&result, &_7, &_8);
	if (Z_TYPE_P(&result) == IS_NULL) {
		ZEPHIR_INIT_VAR(&_9$$5);
		object_init_ex(&_9$$5, tensor_exceptions_runtimeexception_ce);
		ZEPHIR_INIT_VAR(&_10$$5);
		ZEPHIR_CONCAT_SS(&_10$$5, "Failed to compute the inverse", " of a singular matrix.");
		ZEPHIR_CALL_METHOD(NULL, &_9$$5, "__construct", NULL, 26, &_10$$5);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_9$$5, "tensor/matrix.zep", 668);
		ZEPHIR_MM_RESTORE();
		return;
	}
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_12, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &result, &_11, &_12);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Compute the Moore-Penrose pseudoinverse of a general matrix.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, pseudoinverse)
{
	zval _2$$3;
	zval result, _0, _1$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&result);
	ZEPHIR_CALL_METHOD(&_0, this_ptr, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_pseudoinverse(&result, &_0);
	if (Z_TYPE_P(&result) == IS_NULL) {
		ZEPHIR_INIT_VAR(&_1$$3);
		object_init_ex(&_1$$3, tensor_exceptions_runtimeexception_ce);
		ZEPHIR_INIT_VAR(&_2$$3);
		ZEPHIR_CONCAT_SS(&_2$$3, "Failed to compute the pseudoinverse", " of the matrix.");
		ZEPHIR_CALL_METHOD(NULL, &_1$$3, "__construct", NULL, 26, &_2$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_1$$3, "tensor/matrix.zep", 685);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_RETURN_CALL_SELF("quick", NULL, 0, &result);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Calculate the determinant of the matrix.
 *
 * @throws \Tensor\Exceptions\RuntimeException
 * @return float
 */
PHP_METHOD(Tensor_Matrix, det)
{
	zval _0, ref, pi, _4, _5, _6, _7, _8, _1$$3, _2$$3, _3$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&ref);
	ZVAL_UNDEF(&pi);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_CALL_METHOD(&_0, this_ptr, "isSquare", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!zephir_is_true(&_0))) {
		ZEPHIR_INIT_VAR(&_1$$3);
		object_init_ex(&_1$$3, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_CALL_METHOD(&_2$$3, this_ptr, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SSVS(&_3$$3, "Matrix must be", " square, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_1$$3, "__construct", NULL, 3, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_1$$3, "tensor/matrix.zep", 701);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&ref, this_ptr, "ref", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_4, &ref, "a", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_5, &_4, "diagonalAsVector", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&pi, &_5, "product", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_6, &ref, "swaps", NULL, 0);
	zephir_check_call_status();
	ZVAL_DOUBLE(&_7, -1.0);
	ZEPHIR_CALL_FUNCTION(&_8, "pow", NULL, 13, &_7, &_6);
	zephir_check_call_status();
	mul_function(return_value, &pi, &_8);
	RETURN_MM();
}

/**
 * Return the trace of the matrix i.e the sum of all diagonal elements of a square matrix.
 *
 * @return float
 */
PHP_METHOD(Tensor_Matrix, trace)
{
	zval _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_CALL_METHOD(&_0, this_ptr, "diagonalAsVector", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_RETURN_CALL_METHOD(&_0, "sum", NULL, 0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Calculate the rank of the matrix i.e the number of pivots in its reduced row echelon form.
 *
 * @return int
 */
PHP_METHOD(Tensor_Matrix, rank)
{
	double epsilon;
	zend_bool stop = 0, _11$$3;
	zval a, _3;
	zval rowA, valueA, _0, _1, _2, *_4, *_5$$3, _6$$3, *_7$$3, _10$$3, _8$$4, _12$$7;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_9 = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS, pivots;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&rowA);
	ZVAL_UNDEF(&valueA);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_10$$3);
	ZVAL_UNDEF(&_8$$4);
	ZVAL_UNDEF(&_12$$7);
	ZVAL_UNDEF(&a);
	ZVAL_UNDEF(&_3);
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&a);
	array_init(&a);
	ZEPHIR_CALL_METHOD(&_0, this_ptr, "rref", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, &_0, "a", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_2, &_1, "asArray", NULL, 0);
	zephir_check_call_status();
	zephir_get_arrval(&_3, &_2);
	ZEPHIR_CPY_WRT(&a, &_3);
	pivots = 0;
	epsilon =  (0.00000001);
	zephir_is_iterable(&a, 0, "tensor/matrix.zep", 756);
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(&a), _4)
	{
		ZEPHIR_INIT_NVAR(&rowA);
		ZVAL_COPY(&rowA, _4);
		stop = 0;
		if (Z_TYPE_P(&rowA) == IS_STRING) {
			ZEPHIR_INIT_NVAR(&_6$$3);
			zephir_string_to_char_array(&_6$$3, &rowA);
			_5$$3 = &_6$$3;
		} else {
			_5$$3 = &rowA;
		}
		zephir_is_iterable(_5$$3, 0, "tensor/matrix.zep", 754);
		if (Z_TYPE_P(_5$$3) == IS_ARRAY) {
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_5$$3), _7$$3)
			{
				ZEPHIR_INIT_NVAR(&valueA);
				ZVAL_COPY(&valueA, _7$$3);
				if (stop) {
					continue;
				}
				ZEPHIR_CALL_FUNCTION(&_8$$4, "abs", &_9, 27, &valueA);
				zephir_check_call_status();
				if (!ZEPHIR_LT_DOUBLE(&_8$$4, epsilon)) {
					pivots++;
					stop = 1;
				}
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEPHIR_CALL_METHOD(NULL, _5$$3, "rewind", NULL, 0);
			zephir_check_call_status();
			_11$$3 = 1;
			while (1) {
				if (_11$$3) {
					_11$$3 = 0;
				} else {
					ZEPHIR_CALL_METHOD(NULL, _5$$3, "next", NULL, 0);
					zephir_check_call_status();
				}
				ZEPHIR_CALL_METHOD(&_10$$3, _5$$3, "valid", NULL, 0);
				zephir_check_call_status();
				if (!zend_is_true(&_10$$3)) {
					break;
				}
				ZEPHIR_CALL_METHOD(&valueA, _5$$3, "current", NULL, 0);
				zephir_check_call_status();
					if (stop) {
						continue;
					}
					ZEPHIR_CALL_FUNCTION(&_12$$7, "abs", &_9, 27, &valueA);
					zephir_check_call_status();
					if (!ZEPHIR_LT_DOUBLE(&_12$$7, epsilon)) {
						pivots++;
						stop = 1;
					}
			}
		}
		ZEPHIR_INIT_NVAR(&valueA);
	} ZEND_HASH_FOREACH_END();
	ZEPHIR_INIT_NVAR(&rowA);
	RETURN_MM_LONG(pivots);
}

/**
 * Is the matrix full rank?
 * 
 * @return bool
 */
PHP_METHOD(Tensor_Matrix, fullRank)
{
	zval _0, _1, _2;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_CALL_METHOD(&_0, this_ptr, "rank", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, this_ptr, "shape", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_FUNCTION(&_2, "min", NULL, 28, &_1);
	zephir_check_call_status();
	RETURN_MM_BOOL(ZEPHIR_IS_IDENTICAL(&_0, &_2));
}

/**
 * Is the matrix symmetric i.e. is it equal to its transpose.
 * 
 * @return bool
 */
PHP_METHOD(Tensor_Matrix, symmetric)
{
	zend_bool _2, _6$$4;
	zval _0, rowA, aHat, _1, _5$$4, _9$$5, _10$$5, _11$$5;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS, i = 0, j = 0, _3, _4, _7$$4, _8$$4;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&rowA);
	ZVAL_UNDEF(&aHat);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_9$$5);
	ZVAL_UNDEF(&_10$$5);
	ZVAL_UNDEF(&_11$$5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_CALL_METHOD(&_0, this_ptr, "isSquare", NULL, 0);
	zephir_check_call_status();
	if (!(zephir_is_true(&_0))) {
		RETURN_MM_BOOL(0);
	}
	ZEPHIR_CALL_METHOD(&aHat, this_ptr, "asArray", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	_4 = (zephir_get_numberval(&_1) - 2);
	_3 = 0;
	_2 = 0;
	if (_3 <= _4) {
		while (1) {
			if (_2) {
				_3++;
				if (!(_3 <= _4)) {
					break;
				}
			} else {
				_2 = 1;
			}
			i = _3;
			ZEPHIR_OBS_NVAR(&rowA);
			zephir_array_fetch_long(&rowA, &aHat, i, PH_NOISY, "tensor/matrix.zep", 787);
			zephir_read_property_cached(&_5$$4, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
			_8$$4 = (zephir_get_numberval(&_5$$4) - 1);
			_7$$4 = (i + 1);
			_6$$4 = 0;
			if (_7$$4 <= _8$$4) {
				while (1) {
					if (_6$$4) {
						_7$$4++;
						if (!(_7$$4 <= _8$$4)) {
							break;
						}
					} else {
						_6$$4 = 1;
					}
					j = _7$$4;
					ZEPHIR_OBS_NVAR(&_9$$5);
					zephir_array_fetch_long(&_9$$5, &rowA, j, PH_NOISY, "tensor/matrix.zep", 790);
					ZEPHIR_OBS_NVAR(&_10$$5);
					zephir_array_fetch_long(&_10$$5, &aHat, j, PH_NOISY, "tensor/matrix.zep", 790);
					ZEPHIR_OBS_NVAR(&_11$$5);
					zephir_array_fetch_long(&_11$$5, &_10$$5, i, PH_NOISY, "tensor/matrix.zep", 790);
					if (!ZEPHIR_IS_EQUAL(&_9$$5, &_11$$5)) {
						RETURN_MM_BOOL(0);
					}
				}
			}
		}
	}
	RETURN_MM_BOOL(1);
}

/**
 * Multiply this matrix with another matrix (matrix-matrix product).
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, matmul)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "m", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A requires ", &_4$$3, " rows but Matrix B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 811);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_12, b, "n", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_matmul(&_8, &_9, &_10, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_14, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_8, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Compute the dot product of this matrix and a vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return \Tensor\ColumnVector
 */
PHP_METHOD(Tensor_Matrix, dot)
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
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_vector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "size", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "size", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 829);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&_9, b, "asColumnMatrix", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_8, this_ptr, "matmul", NULL, 0, &_9);
	zephir_check_call_status();
	ZVAL_LONG(&_10, 0);
	ZEPHIR_RETURN_CALL_METHOD(&_8, "columnAsVector", NULL, 0, &_10);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the 2D convolution of this matrix and a kernel matrix with given stride using the "same" method for zero padding.
 *
 * @param \Tensor\Matrix b
 * @param int stride
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Matrix, convolve)
{
	zval _6$$3;
	zend_bool _2;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long stride, ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, *stride_param = NULL, _0, _1, _3, _4, _11, _12, _13, _14, _5$$3, _7$$4, _8$$4, _9$$4, _10$$4;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_8$$4);
	ZVAL_UNDEF(&_9$$4);
	ZVAL_UNDEF(&_10$$4);
	ZVAL_UNDEF(&_6$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(stride)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 1, &b, &stride_param);
	if (!stride_param) {
		stride = 1;
	} else {
		}
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	_2 = ZEPHIR_GT(&_0, &_1);
	if (!(_2)) {
		ZEPHIR_CALL_METHOD(&_3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
		_2 = ZEPHIR_GT(&_3, &_4);
	}
	if (UNEXPECTED(_2)) {
		ZEPHIR_INIT_VAR(&_5$$3);
		object_init_ex(&_5$$3, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_INIT_VAR(&_6$$3);
		ZEPHIR_CONCAT_SS(&_6$$3, "Matrix B cannot be", " larger than Matrix A.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$3, "__construct", NULL, 3, &_6$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$3, "tensor/matrix.zep", 847);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(stride < 1)) {
		ZEPHIR_INIT_VAR(&_7$$4);
		object_init_ex(&_7$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_8$$4, stride);
		ZEPHIR_CALL_FUNCTION(&_9$$4, "strval", NULL, 4, &_8$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_10$$4);
		ZEPHIR_CONCAT_SSVS(&_10$$4, "Stride cannot be", " less than 1, ", &_9$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_7$$4, "__construct", NULL, 3, &_10$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_7$$4, "tensor/matrix.zep", 852);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_11);
	ZEPHIR_CALL_METHOD(&_12, this_ptr, "asArray", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_13, b, "asArray", NULL, 0);
	zephir_check_call_status();
	ZVAL_LONG(&_14, stride);
	tensor_convolve_2d(&_11, &_12, &_13, &_14);
	ZEPHIR_RETURN_CALL_SELF("quick", NULL, 0, &_11);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Calculate the row echelon form (REF) of the matrix.
 *
 * @return \Tensor\Reductions\Ref
 */
PHP_METHOD(Tensor_Matrix, ref)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_RETURN_CALL_CE_STATIC(tensor_reductions_ref_ce, "reduce", NULL, 0, this_ptr);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the reduced row echelon (RREF) form of the matrix.
 *
 * @return \Tensor\Reductions\Rref
 */
PHP_METHOD(Tensor_Matrix, rref)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_RETURN_CALL_CE_STATIC(tensor_reductions_rref_ce, "reduce", NULL, 0, this_ptr);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the LU decomposition of the matrix in a tuple where l is
 * the lower triangular matrix, u is the upper triangular matrix,
 * and p is the permutation matrix.
 *
 * @return \Tensor\Decompositions\Lu
 */
PHP_METHOD(Tensor_Matrix, lu)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_RETURN_CALL_CE_STATIC(tensor_decompositions_lu_ce, "decompose", NULL, 0, this_ptr);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the lower triangular matrix of the Cholesky decomposition.
 * 
 * @return \Tensor\Decompositions\Cholesky;
 */
PHP_METHOD(Tensor_Matrix, cholesky)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_RETURN_CALL_CE_STATIC(tensor_decompositions_cholesky_ce, "decompose", NULL, 0, this_ptr);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Compute the eigenvalues and eigenvectors of the matrix and return them in a tuple.
 *
 * @param bool symmetric
 * @return \Tensor\Decompositions\Eigen
 */
PHP_METHOD(Tensor_Matrix, eig)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *symmetric_param = NULL, _0;
	zend_bool symmetric;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(symmetric)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 0, 1, &symmetric_param);
	if (!symmetric_param) {
		symmetric = 0;
	} else {
		}
	if (symmetric) {
		ZVAL_BOOL(&_0, 1);
	} else {
		ZVAL_BOOL(&_0, 0);
	}
	ZEPHIR_RETURN_CALL_CE_STATIC(tensor_decompositions_eigen_ce, "decompose", NULL, 0, this_ptr, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Compute the singluar value decomposition of this matrix.
 *
 * @return \Tensor\Decompositions\Svd
 */
PHP_METHOD(Tensor_Matrix, svd)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_RETURN_CALL_CE_STATIC(tensor_decompositions_svd_ce, "decompose", NULL, 0, this_ptr);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the L1 norm of the matrix.
 *
 * @return float
 */
PHP_METHOD(Tensor_Matrix, l1Norm)
{
	zval _0, _1, _2;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_CALL_METHOD(&_0, this_ptr, "transpose", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, &_0, "abs", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_2, &_1, "sum", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_RETURN_CALL_METHOD(&_2, "max", NULL, 0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the L2 norm of the matrix.
 *    
 * @return float
 */
PHP_METHOD(Tensor_Matrix, l2Norm)
{
	zval _0, _1, _2;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_CALL_METHOD(&_0, this_ptr, "square", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, &_0, "sum", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_2, &_1, "sum", NULL, 0);
	zephir_check_call_status();
	RETURN_MM_DOUBLE(zephir_sqrt(&_2));
}

/**
 * Retrn the infinity norm of the matrix.
 *
 * @return float
 */
PHP_METHOD(Tensor_Matrix, infinityNorm)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_CALL_METHOD(&_0, this_ptr, "abs", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, &_0, "sum", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_RETURN_CALL_METHOD(&_1, "max", NULL, 0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the max norm of the matrix.
 *
 * @return float
 */
PHP_METHOD(Tensor_Matrix, maxNorm)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_CALL_METHOD(&_0, this_ptr, "abs", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, &_0, "max", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_RETURN_CALL_METHOD(&_1, "max", NULL, 0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * A universal function to multiply this matrix with another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Matrix, multiply)
{
	zend_bool _1$$3;
	zval _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_INIT_VAR(&_0);
	zephir_gettype(&_0, b);
	if (ZEPHIR_IS_STRING(&_0, "object")) { goto zephir_switch_0_clause_0; }
	if (ZEPHIR_IS_STRING(&_0, "double")) { goto zephir_switch_0_clause_1; }
	if (ZEPHIR_IS_STRING(&_0, "integer")) { goto zephir_switch_0_clause_2; }
	goto zephir_switch_0_end;
	zephir_switch_0_clause_0: ;
		_1$$3 = 1;
		if (_1$$3 == zephir_instance_of_ev(b, tensor_matrix_ce)) { goto zephir_switch_1_clause_0; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_columnvector_ce)) { goto zephir_switch_1_clause_1; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_2; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "multiplyMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "multiplyColumnVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_2: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "multiplyVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_end: ;

		goto zephir_switch_0_end;
	zephir_switch_0_clause_1: ;
	zephir_switch_0_clause_2: ;
		ZEPHIR_RETURN_CALL_METHOD(this_ptr, "multiplyScalar", NULL, 0, b);
		zephir_check_call_status();
		RETURN_MM();
	zephir_switch_0_end: ;

	ZEPHIR_INIT_VAR(&_2);
	object_init_ex(&_2, tensor_exceptions_invalidargumentexception_ce);
	ZEPHIR_INIT_VAR(&_3);
	ZEPHIR_CONCAT_SS(&_3, "Cannot multiply", " matrix by the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 3, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 991);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to divide this matrix by another tensor sdfsdfelement-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Matrix, divide)
{
	zend_bool _1$$3;
	zval _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_INIT_VAR(&_0);
	zephir_gettype(&_0, b);
	if (ZEPHIR_IS_STRING(&_0, "object")) { goto zephir_switch_0_clause_0; }
	if (ZEPHIR_IS_STRING(&_0, "double")) { goto zephir_switch_0_clause_1; }
	if (ZEPHIR_IS_STRING(&_0, "integer")) { goto zephir_switch_0_clause_2; }
	goto zephir_switch_0_end;
	zephir_switch_0_clause_0: ;
		_1$$3 = 1;
		if (_1$$3 == zephir_instance_of_ev(b, tensor_matrix_ce)) { goto zephir_switch_1_clause_0; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_columnvector_ce)) { goto zephir_switch_1_clause_1; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_2; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "divideMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "divideColumnVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_2: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "divideVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_end: ;

		goto zephir_switch_0_end;
	zephir_switch_0_clause_1: ;
	zephir_switch_0_clause_2: ;
		ZEPHIR_RETURN_CALL_METHOD(this_ptr, "divideScalar", NULL, 0, b);
		zephir_check_call_status();
		RETURN_MM();
	zephir_switch_0_end: ;

	ZEPHIR_INIT_VAR(&_2);
	object_init_ex(&_2, tensor_exceptions_invalidargumentexception_ce);
	ZEPHIR_INIT_VAR(&_3);
	ZEPHIR_CONCAT_SS(&_3, "Cannot divide", " matrix by the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 3, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1024);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to add this matrix with another tensor
 * element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Matrix, add)
{
	zend_bool _1$$3;
	zval _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_INIT_VAR(&_0);
	zephir_gettype(&_0, b);
	if (ZEPHIR_IS_STRING(&_0, "object")) { goto zephir_switch_0_clause_0; }
	if (ZEPHIR_IS_STRING(&_0, "double")) { goto zephir_switch_0_clause_1; }
	if (ZEPHIR_IS_STRING(&_0, "integer")) { goto zephir_switch_0_clause_2; }
	goto zephir_switch_0_end;
	zephir_switch_0_clause_0: ;
		_1$$3 = 1;
		if (_1$$3 == zephir_instance_of_ev(b, tensor_matrix_ce)) { goto zephir_switch_1_clause_0; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_columnvector_ce)) { goto zephir_switch_1_clause_1; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_2; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "addMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "addColumnVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_2: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "addVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_end: ;

		goto zephir_switch_0_end;
	zephir_switch_0_clause_1: ;
	zephir_switch_0_clause_2: ;
		ZEPHIR_RETURN_CALL_METHOD(this_ptr, "addScalar", NULL, 0, b);
		zephir_check_call_status();
		RETURN_MM();
	zephir_switch_0_end: ;

	ZEPHIR_INIT_VAR(&_2);
	object_init_ex(&_2, tensor_exceptions_invalidargumentexception_ce);
	ZEPHIR_INIT_VAR(&_3);
	ZEPHIR_CONCAT_SS(&_3, "Cannot add", " matrix with the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 3, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1058);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to subtract this matrix from another tensor
 * element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Matrix, subtract)
{
	zend_bool _1$$3;
	zval _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_INIT_VAR(&_0);
	zephir_gettype(&_0, b);
	if (ZEPHIR_IS_STRING(&_0, "object")) { goto zephir_switch_0_clause_0; }
	if (ZEPHIR_IS_STRING(&_0, "double")) { goto zephir_switch_0_clause_1; }
	if (ZEPHIR_IS_STRING(&_0, "integer")) { goto zephir_switch_0_clause_2; }
	goto zephir_switch_0_end;
	zephir_switch_0_clause_0: ;
		_1$$3 = 1;
		if (_1$$3 == zephir_instance_of_ev(b, tensor_matrix_ce)) { goto zephir_switch_1_clause_0; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_columnvector_ce)) { goto zephir_switch_1_clause_1; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_2; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "subtractMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "subtractColumnVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_2: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "subtractVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_end: ;

		goto zephir_switch_0_end;
	zephir_switch_0_clause_1: ;
	zephir_switch_0_clause_2: ;
		ZEPHIR_RETURN_CALL_METHOD(this_ptr, "subtractScalar", NULL, 0, b);
		zephir_check_call_status();
		RETURN_MM();
	zephir_switch_0_end: ;

	ZEPHIR_INIT_VAR(&_2);
	object_init_ex(&_2, tensor_exceptions_invalidargumentexception_ce);
	ZEPHIR_INIT_VAR(&_3);
	ZEPHIR_CONCAT_SS(&_3, "Cannot subtract", " matrix with the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 3, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1092);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to raise this matrix to the power of another
 * tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Matrix, pow)
{
	zend_bool _1$$3;
	zval _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_INIT_VAR(&_0);
	zephir_gettype(&_0, b);
	if (ZEPHIR_IS_STRING(&_0, "object")) { goto zephir_switch_0_clause_0; }
	if (ZEPHIR_IS_STRING(&_0, "double")) { goto zephir_switch_0_clause_1; }
	if (ZEPHIR_IS_STRING(&_0, "integer")) { goto zephir_switch_0_clause_2; }
	goto zephir_switch_0_end;
	zephir_switch_0_clause_0: ;
		_1$$3 = 1;
		if (_1$$3 == zephir_instance_of_ev(b, tensor_matrix_ce)) { goto zephir_switch_1_clause_0; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_columnvector_ce)) { goto zephir_switch_1_clause_1; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_2; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "powMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "powColumnVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_2: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "powVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_end: ;

		goto zephir_switch_0_end;
	zephir_switch_0_clause_1: ;
	zephir_switch_0_clause_2: ;
		ZEPHIR_RETURN_CALL_METHOD(this_ptr, "powScalar", NULL, 0, b);
		zephir_check_call_status();
		RETURN_MM();
	zephir_switch_0_end: ;

	ZEPHIR_INIT_VAR(&_2);
	object_init_ex(&_2, tensor_exceptions_invalidargumentexception_ce);
	ZEPHIR_INIT_VAR(&_3);
	ZEPHIR_CONCAT_SS(&_3, "Cannot raise", " matrix to the power of the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 3, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1126);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to compute the integer modulus of this matrix
 * and another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Matrix, mod)
{
	zend_bool _1$$3;
	zval _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_INIT_VAR(&_0);
	zephir_gettype(&_0, b);
	if (ZEPHIR_IS_STRING(&_0, "object")) { goto zephir_switch_0_clause_0; }
	if (ZEPHIR_IS_STRING(&_0, "double")) { goto zephir_switch_0_clause_1; }
	if (ZEPHIR_IS_STRING(&_0, "integer")) { goto zephir_switch_0_clause_2; }
	goto zephir_switch_0_end;
	zephir_switch_0_clause_0: ;
		_1$$3 = 1;
		if (_1$$3 == zephir_instance_of_ev(b, tensor_matrix_ce)) { goto zephir_switch_1_clause_0; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_columnvector_ce)) { goto zephir_switch_1_clause_1; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_2; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "modMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "modColumnVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_2: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "modVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_end: ;

		goto zephir_switch_0_end;
	zephir_switch_0_clause_1: ;
	zephir_switch_0_clause_2: ;
		ZEPHIR_RETURN_CALL_METHOD(this_ptr, "modScalar", NULL, 0, b);
		zephir_check_call_status();
		RETURN_MM();
	zephir_switch_0_end: ;

	ZEPHIR_INIT_VAR(&_2);
	object_init_ex(&_2, tensor_exceptions_invalidargumentexception_ce);
	ZEPHIR_INIT_VAR(&_3);
	ZEPHIR_CONCAT_SS(&_3, "Cannot mod", " matrix with the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 3, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1160);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to compute the equality comparison of
 * this matrix and another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Matrix, equal)
{
	zend_bool _1$$3;
	zval _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_INIT_VAR(&_0);
	zephir_gettype(&_0, b);
	if (ZEPHIR_IS_STRING(&_0, "object")) { goto zephir_switch_0_clause_0; }
	if (ZEPHIR_IS_STRING(&_0, "double")) { goto zephir_switch_0_clause_1; }
	if (ZEPHIR_IS_STRING(&_0, "integer")) { goto zephir_switch_0_clause_2; }
	goto zephir_switch_0_end;
	zephir_switch_0_clause_0: ;
		_1$$3 = 1;
		if (_1$$3 == zephir_instance_of_ev(b, tensor_matrix_ce)) { goto zephir_switch_1_clause_0; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_columnvector_ce)) { goto zephir_switch_1_clause_1; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_2; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "equalMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "equalColumnVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_2: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "equalVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_end: ;

		goto zephir_switch_0_end;
	zephir_switch_0_clause_1: ;
	zephir_switch_0_clause_2: ;
		ZEPHIR_RETURN_CALL_METHOD(this_ptr, "equalScalar", NULL, 0, b);
		zephir_check_call_status();
		RETURN_MM();
	zephir_switch_0_end: ;

	ZEPHIR_INIT_VAR(&_2);
	object_init_ex(&_2, tensor_exceptions_invalidargumentexception_ce);
	ZEPHIR_INIT_VAR(&_3);
	ZEPHIR_CONCAT_SS(&_3, "Cannot compare", " matrix to the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 3, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1194);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to compute the not equal comparison of
 * this matrix and another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Matrix, notEqual)
{
	zend_bool _1$$3;
	zval _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_INIT_VAR(&_0);
	zephir_gettype(&_0, b);
	if (ZEPHIR_IS_STRING(&_0, "object")) { goto zephir_switch_0_clause_0; }
	if (ZEPHIR_IS_STRING(&_0, "double")) { goto zephir_switch_0_clause_1; }
	if (ZEPHIR_IS_STRING(&_0, "integer")) { goto zephir_switch_0_clause_2; }
	goto zephir_switch_0_end;
	zephir_switch_0_clause_0: ;
		_1$$3 = 1;
		if (_1$$3 == zephir_instance_of_ev(b, tensor_matrix_ce)) { goto zephir_switch_1_clause_0; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_columnvector_ce)) { goto zephir_switch_1_clause_1; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_2; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "notEqualMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "notEqualColumnVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_2: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "notEqualVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_end: ;

		goto zephir_switch_0_end;
	zephir_switch_0_clause_1: ;
	zephir_switch_0_clause_2: ;
		ZEPHIR_RETURN_CALL_METHOD(this_ptr, "notEqualScalar", NULL, 0, b);
		zephir_check_call_status();
		RETURN_MM();
	zephir_switch_0_end: ;

	ZEPHIR_INIT_VAR(&_2);
	object_init_ex(&_2, tensor_exceptions_invalidargumentexception_ce);
	ZEPHIR_INIT_VAR(&_3);
	ZEPHIR_CONCAT_SS(&_3, "Cannot compare", " matrix to the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 3, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1228);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to compute the greater than comparison of
 * this matrix and another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Matrix, greater)
{
	zend_bool _1$$3;
	zval _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_INIT_VAR(&_0);
	zephir_gettype(&_0, b);
	if (ZEPHIR_IS_STRING(&_0, "object")) { goto zephir_switch_0_clause_0; }
	if (ZEPHIR_IS_STRING(&_0, "double")) { goto zephir_switch_0_clause_1; }
	if (ZEPHIR_IS_STRING(&_0, "integer")) { goto zephir_switch_0_clause_2; }
	goto zephir_switch_0_end;
	zephir_switch_0_clause_0: ;
		_1$$3 = 1;
		if (_1$$3 == zephir_instance_of_ev(b, tensor_matrix_ce)) { goto zephir_switch_1_clause_0; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_columnvector_ce)) { goto zephir_switch_1_clause_1; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_2; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "greaterMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "greaterColumnVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_2: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "greaterVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_end: ;

		goto zephir_switch_0_end;
	zephir_switch_0_clause_1: ;
	zephir_switch_0_clause_2: ;
		ZEPHIR_RETURN_CALL_METHOD(this_ptr, "greaterScalar", NULL, 0, b);
		zephir_check_call_status();
		RETURN_MM();
	zephir_switch_0_end: ;

	ZEPHIR_INIT_VAR(&_2);
	object_init_ex(&_2, tensor_exceptions_invalidargumentexception_ce);
	ZEPHIR_INIT_VAR(&_3);
	ZEPHIR_CONCAT_SS(&_3, "Cannot compare", " matrix to the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 3, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1262);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to compute the greater than or equal to
 * comparison of this matrix and another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Matrix, greaterEqual)
{
	zend_bool _1$$3;
	zval _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_INIT_VAR(&_0);
	zephir_gettype(&_0, b);
	if (ZEPHIR_IS_STRING(&_0, "object")) { goto zephir_switch_0_clause_0; }
	if (ZEPHIR_IS_STRING(&_0, "double")) { goto zephir_switch_0_clause_1; }
	if (ZEPHIR_IS_STRING(&_0, "integer")) { goto zephir_switch_0_clause_2; }
	goto zephir_switch_0_end;
	zephir_switch_0_clause_0: ;
		_1$$3 = 1;
		if (_1$$3 == zephir_instance_of_ev(b, tensor_matrix_ce)) { goto zephir_switch_1_clause_0; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_columnvector_ce)) { goto zephir_switch_1_clause_1; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_2; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "greaterEqualMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "greaterEqualColumnVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_2: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "greaterEqualVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_end: ;

		goto zephir_switch_0_end;
	zephir_switch_0_clause_1: ;
	zephir_switch_0_clause_2: ;
		ZEPHIR_RETURN_CALL_METHOD(this_ptr, "greaterEqualScalar", NULL, 0, b);
		zephir_check_call_status();
		RETURN_MM();
	zephir_switch_0_end: ;

	ZEPHIR_INIT_VAR(&_2);
	object_init_ex(&_2, tensor_exceptions_invalidargumentexception_ce);
	ZEPHIR_INIT_VAR(&_3);
	ZEPHIR_CONCAT_SS(&_3, "Cannot compare", " matrix to the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 3, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1296);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to compute the less than comparison of
 * this matrix and another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Matrix, less)
{
	zend_bool _1$$3;
	zval _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_INIT_VAR(&_0);
	zephir_gettype(&_0, b);
	if (ZEPHIR_IS_STRING(&_0, "object")) { goto zephir_switch_0_clause_0; }
	if (ZEPHIR_IS_STRING(&_0, "double")) { goto zephir_switch_0_clause_1; }
	if (ZEPHIR_IS_STRING(&_0, "integer")) { goto zephir_switch_0_clause_2; }
	goto zephir_switch_0_end;
	zephir_switch_0_clause_0: ;
		_1$$3 = 1;
		if (_1$$3 == zephir_instance_of_ev(b, tensor_matrix_ce)) { goto zephir_switch_1_clause_0; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_columnvector_ce)) { goto zephir_switch_1_clause_1; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_2; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "lessMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "lessColumnVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_2: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "lessVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_end: ;

		goto zephir_switch_0_end;
	zephir_switch_0_clause_1: ;
	zephir_switch_0_clause_2: ;
		ZEPHIR_RETURN_CALL_METHOD(this_ptr, "lessScalar", NULL, 0, b);
		zephir_check_call_status();
		RETURN_MM();
	zephir_switch_0_end: ;

	ZEPHIR_INIT_VAR(&_2);
	object_init_ex(&_2, tensor_exceptions_invalidargumentexception_ce);
	ZEPHIR_INIT_VAR(&_3);
	ZEPHIR_CONCAT_SS(&_3, "Cannot compare", " matrix to the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 3, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1330);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to compute the less than or equal to
 * comparison of this matrix and another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Matrix, lessEqual)
{
	zend_bool _1$$3;
	zval _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_INIT_VAR(&_0);
	zephir_gettype(&_0, b);
	if (ZEPHIR_IS_STRING(&_0, "object")) { goto zephir_switch_0_clause_0; }
	if (ZEPHIR_IS_STRING(&_0, "double")) { goto zephir_switch_0_clause_1; }
	if (ZEPHIR_IS_STRING(&_0, "integer")) { goto zephir_switch_0_clause_2; }
	goto zephir_switch_0_end;
	zephir_switch_0_clause_0: ;
		_1$$3 = 1;
		if (_1$$3 == zephir_instance_of_ev(b, tensor_matrix_ce)) { goto zephir_switch_1_clause_0; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_columnvector_ce)) { goto zephir_switch_1_clause_1; }
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_2; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "lessEqualMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "lessEqualColumnVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_2: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "lessEqualVector", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_end: ;

		goto zephir_switch_0_end;
	zephir_switch_0_clause_1: ;
	zephir_switch_0_clause_2: ;
		ZEPHIR_RETURN_CALL_METHOD(this_ptr, "lessEqualScalar", NULL, 0, b);
		zephir_check_call_status();
		RETURN_MM();
	zephir_switch_0_end: ;

	ZEPHIR_INIT_VAR(&_2);
	object_init_ex(&_2, tensor_exceptions_invalidargumentexception_ce);
	ZEPHIR_INIT_VAR(&_3);
	ZEPHIR_CONCAT_SS(&_3, "Cannot compare", " matrix to the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 3, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1364);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * Return the element-wise reciprocal of the matrix.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, reciprocal)
{
	zval _0, _1, _2;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_SELF(&_0, "ones", NULL, 0, &_1, &_2);
	zephir_check_call_status();
	ZEPHIR_RETURN_CALL_METHOD(&_0, "divideMatrix", NULL, 0, this_ptr);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the absolute value of each element in the matrix.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, abs)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "abs");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the square of the matrix elementwise.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, square)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_RETURN_CALL_METHOD(this_ptr, "multiplyMatrix", NULL, 0, this_ptr);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the square root of the matrix.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, sqrt)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "sqrt");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the exponential of the matrix.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, exp)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "exp");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the exponential of the tensor minus 1.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, expm1)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "expm1");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the logarithm of the matrix in specified base.
 *
 * @param float base
 * @return self
 */
PHP_METHOD(Tensor_Matrix, log)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *base_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, _4$$3, _5, _6, _7, _8, _9, _10;
	double base;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(base_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 0, 1, &base_param);
	if (!base_param) {
		base = 2.7182818284590452354;
	} else {
		base = zephir_get_doubleval(base_param);
	}
	if (base == 2.7182818284590452354) {
		ZEPHIR_INIT_VAR(&_0$$3);
		zephir_read_property_cached(&_1$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
		ZEPHIR_INIT_VAR(&_2$$3);
		ZVAL_STRING(&_2$$3, "log");
		tensor_buffer_unary(&_0$$3, &_1$$3, &_2$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
		zephir_read_property_cached(&_4$$3, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
		ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0$$3, &_3$$3, &_4$$3);
		zephir_check_call_status();
		RETURN_MM();
	}
	ZEPHIR_INIT_VAR(&_5);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_7);
	ZVAL_STRING(&_7, "log");
	ZVAL_DOUBLE(&_8, base);
	tensor_buffer_unary_arg(&_5, &_6, &_7, &_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_5, &_9, &_10);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the log of 1 plus the tensor i.e. a transform.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, log1p)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "log1p");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the sine of the matrix.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, sin)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "sin");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Compute the arc sine of the matrix.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, asin)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "asin");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the cosine of the matrix.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, cos)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "cos");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Compute the arc cosine of the matrix.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, acos)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "acos");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the tangent of the matrix.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, tan)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "tan");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Compute the arc tangent of the matrix.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, atan)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "atan");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Convert angles from radians to degrees.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, rad2deg)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "rad2deg");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Convert angles from degrees to radians.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, deg2rad)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "deg2rad");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Sum the rows of the matrix and return a vector.
 *
 * @return \Tensor\ColumnVector
 */
PHP_METHOD(Tensor_Matrix, sum)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, tensor_columnvector_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_3);
	ZVAL_STRING(&_3, "sum");
	tensor_buffer_row_reduce(&_0, &_1, &_2, &_3);
	ZVAL_BOOL(&_4, 0);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 1, &_0, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Calculate the row product of the matrix.
 *
 * @return \Tensor\ColumnVector
 */
PHP_METHOD(Tensor_Matrix, product)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, tensor_columnvector_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_3);
	ZVAL_STRING(&_3, "product");
	tensor_buffer_row_reduce(&_0, &_1, &_2, &_3);
	ZVAL_BOOL(&_4, 0);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 1, &_0, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the minimum of each row in the matrix.
 *
 * @return \Tensor\ColumnVector
 */
PHP_METHOD(Tensor_Matrix, min)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, tensor_columnvector_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_3);
	ZVAL_STRING(&_3, "min");
	tensor_buffer_row_reduce(&_0, &_1, &_2, &_3);
	ZVAL_BOOL(&_4, 0);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 1, &_0, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the maximum of each row in the matrix.
 *
 * @return \Tensor\ColumnVector
 */
PHP_METHOD(Tensor_Matrix, max)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, tensor_columnvector_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_3);
	ZVAL_STRING(&_3, "max");
	tensor_buffer_row_reduce(&_0, &_1, &_2, &_3);
	ZVAL_BOOL(&_4, 0);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 1, &_0, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Compute the means of each row and return them in a vector.
 *
 * @return \Tensor\ColumnVector
 */
PHP_METHOD(Tensor_Matrix, mean)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_CALL_METHOD(&_0, this_ptr, "sum", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_METHOD(&_0, "divideScalar", NULL, 0, &_1);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the median vector of this matrix.
 *
 * @return \Tensor\ColumnVector
 */
PHP_METHOD(Tensor_Matrix, median)
{
	zend_bool odd, _12;
	zval b;
	zval rowA, median, _0, _1, _2, _3, *_4, _5, *_6, _11, _8$$5, _9$$5, _10$$5, _13$$8, _14$$8, _15$$8;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_7 = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS, mid;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&rowA);
	ZVAL_UNDEF(&median);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_8$$5);
	ZVAL_UNDEF(&_9$$5);
	ZVAL_UNDEF(&_10$$5);
	ZVAL_UNDEF(&_13$$8);
	ZVAL_UNDEF(&_14$$8);
	ZVAL_UNDEF(&_15$$8);
	ZVAL_UNDEF(&b);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&b);
	array_init(&b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_1, 2);
	ZEPHIR_CALL_FUNCTION(&_2, "intdiv", NULL, 14, &_0, &_1);
	zephir_check_call_status();
	mid = zephir_get_intval(&_2);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	odd = zephir_safe_mod_zval_long(&_1, 2) == 1;
	ZEPHIR_CALL_METHOD(&_3, this_ptr, "asArray", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_3) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_5);
		zephir_string_to_char_array(&_5, &_3);
		_4 = &_5;
	} else {
		_4 = &_3;
	}
	zephir_is_iterable(_4, 0, "tensor/matrix.zep", 1609);
	if (Z_TYPE_P(_4) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_4), _6)
		{
			ZEPHIR_INIT_NVAR(&rowA);
			ZVAL_COPY(&rowA, _6);
			ZEPHIR_MAKE_REF(&rowA);
			ZEPHIR_CALL_FUNCTION(NULL, "sort", &_7, 15, &rowA);
			ZEPHIR_UNREF(&rowA);
			zephir_check_call_status();
			if (odd) {
				ZEPHIR_OBS_NVAR(&median);
				zephir_array_fetch_long(&median, &rowA, mid, PH_NOISY, "tensor/matrix.zep", 1601);
			} else {
				ZEPHIR_OBS_NVAR(&_8$$5);
				zephir_array_fetch_long(&_8$$5, &rowA, (mid - 1), PH_NOISY, "tensor/matrix.zep", 1603);
				ZEPHIR_OBS_NVAR(&_9$$5);
				zephir_array_fetch_long(&_9$$5, &rowA, mid, PH_NOISY, "tensor/matrix.zep", 1603);
				ZEPHIR_INIT_NVAR(&_10$$5);
				zephir_add_function(&_10$$5, &_8$$5, &_9$$5);
				ZEPHIR_INIT_NVAR(&median);
				ZVAL_DOUBLE(&median, zephir_safe_div_zval_double(&_10$$5, 2.0));
			}
			zephir_array_append(&b, &median, PH_SEPARATE, "tensor/matrix.zep", 1606);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _4, "rewind", NULL, 0);
		zephir_check_call_status();
		_12 = 1;
		while (1) {
			if (_12) {
				_12 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _4, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_11, _4, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_11)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowA, _4, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_MAKE_REF(&rowA);
				ZEPHIR_CALL_FUNCTION(NULL, "sort", &_7, 15, &rowA);
				ZEPHIR_UNREF(&rowA);
				zephir_check_call_status();
				if (odd) {
					ZEPHIR_OBS_NVAR(&median);
					zephir_array_fetch_long(&median, &rowA, mid, PH_NOISY, "tensor/matrix.zep", 1601);
				} else {
					ZEPHIR_OBS_NVAR(&_13$$8);
					zephir_array_fetch_long(&_13$$8, &rowA, (mid - 1), PH_NOISY, "tensor/matrix.zep", 1603);
					ZEPHIR_OBS_NVAR(&_14$$8);
					zephir_array_fetch_long(&_14$$8, &rowA, mid, PH_NOISY, "tensor/matrix.zep", 1603);
					ZEPHIR_INIT_NVAR(&_15$$8);
					zephir_add_function(&_15$$8, &_13$$8, &_14$$8);
					ZEPHIR_INIT_NVAR(&median);
					ZVAL_DOUBLE(&median, zephir_safe_div_zval_double(&_15$$8, 2.0));
				}
				zephir_array_append(&b, &median, PH_SEPARATE, "tensor/matrix.zep", 1606);
		}
	}
	ZEPHIR_INIT_NVAR(&rowA);
	ZEPHIR_RETURN_CALL_CE_STATIC(tensor_columnvector_ce, "quick", NULL, 0, &b);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the q'th quantile of this matrix.
 *
 * @param float q
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return \Tensor\ColumnVector
 */
PHP_METHOD(Tensor_Matrix, quantile)
{
	zval b;
	zend_bool _0, _19;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_10 = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS, xHat;
	zval *q_param = NULL, rowA, _5, _6, *_7, _8, *_9, _18, _1$$3, _2$$3, _3$$3, _4$$3, _11$$4, _15$$4, _16$$4, _17$$4, _12$$5, _13$$5, _14$$5, _20$$6, _24$$6, _25$$6, _26$$6, _21$$7, _22$$7, _23$$7;
	double q, t = 0, x, remainder;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&rowA);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_11$$4);
	ZVAL_UNDEF(&_15$$4);
	ZVAL_UNDEF(&_16$$4);
	ZVAL_UNDEF(&_17$$4);
	ZVAL_UNDEF(&_12$$5);
	ZVAL_UNDEF(&_13$$5);
	ZVAL_UNDEF(&_14$$5);
	ZVAL_UNDEF(&_20$$6);
	ZVAL_UNDEF(&_24$$6);
	ZVAL_UNDEF(&_25$$6);
	ZVAL_UNDEF(&_26$$6);
	ZVAL_UNDEF(&_21$$7);
	ZVAL_UNDEF(&_22$$7);
	ZVAL_UNDEF(&_23$$7);
	ZVAL_UNDEF(&b);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(q_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &q_param);
	q = zephir_get_doubleval(q_param);
	_0 = q < 0.0;
	if (!(_0)) {
		_0 = q > 1.0;
	}
	if (UNEXPECTED(_0)) {
		ZEPHIR_INIT_VAR(&_1$$3);
		object_init_ex(&_1$$3, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_DOUBLE(&_2$$3, q);
		ZEPHIR_CALL_FUNCTION(&_3$$3, "strval", NULL, 4, &_2$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_4$$3);
		ZEPHIR_CONCAT_SSVS(&_4$$3, "Q must be between", " 0 and 1, ", &_3$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_1$$3, "__construct", NULL, 3, &_4$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_1$$3, "tensor/matrix.zep", 1623);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&b);
	array_init(&b);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	x =  (((q * (double) ((zephir_get_numberval(&_5) - 1))) + (double) (1)));
	xHat = (int) x;
	remainder =  ((x -  (double) xHat));
	ZEPHIR_CALL_METHOD(&_6, this_ptr, "asArray", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_6) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_8);
		zephir_string_to_char_array(&_8, &_6);
		_7 = &_8;
	} else {
		_7 = &_6;
	}
	zephir_is_iterable(_7, 0, "tensor/matrix.zep", 1651);
	if (Z_TYPE_P(_7) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_7), _9)
		{
			ZEPHIR_INIT_NVAR(&rowA);
			ZVAL_COPY(&rowA, _9);
			ZEPHIR_MAKE_REF(&rowA);
			ZEPHIR_CALL_FUNCTION(NULL, "sort", &_10, 15, &rowA);
			ZEPHIR_UNREF(&rowA);
			zephir_check_call_status();
			zephir_read_property_cached(&_11$$4, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
			if (ZEPHIR_LE_LONG(&_11$$4, xHat)) {
				ZEPHIR_OBS_NVAR(&_12$$5);
				zephir_read_property_cached(&_13$$5, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
				zephir_array_fetch_long(&_12$$5, &rowA, (zephir_get_numberval(&_13$$5) - 1), PH_NOISY, "tensor/matrix.zep", 1641);
				ZEPHIR_INIT_NVAR(&_14$$5);
				ZVAL_DOUBLE(&_14$$5, zephir_get_doubleval(&_12$$5));
				zephir_array_append(&b, &_14$$5, PH_SEPARATE, "tensor/matrix.zep", 1641);
				continue;
			}
			ZEPHIR_OBS_NVAR(&_15$$4);
			zephir_array_fetch_long(&_15$$4, &rowA, (xHat - 1), PH_NOISY, "tensor/matrix.zep", 1646);
			t =  (zephir_get_doubleval(&_15$$4));
			ZEPHIR_OBS_NVAR(&_16$$4);
			zephir_array_fetch_long(&_16$$4, &rowA, xHat, PH_NOISY, "tensor/matrix.zep", 1648);
			ZEPHIR_INIT_NVAR(&_17$$4);
			ZVAL_DOUBLE(&_17$$4, (t + (remainder * (zephir_get_numberval(&_16$$4) - t))));
			zephir_array_append(&b, &_17$$4, PH_SEPARATE, "tensor/matrix.zep", 1648);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _7, "rewind", NULL, 0);
		zephir_check_call_status();
		_19 = 1;
		while (1) {
			if (_19) {
				_19 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _7, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_18, _7, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_18)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowA, _7, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_MAKE_REF(&rowA);
				ZEPHIR_CALL_FUNCTION(NULL, "sort", &_10, 15, &rowA);
				ZEPHIR_UNREF(&rowA);
				zephir_check_call_status();
				zephir_read_property_cached(&_20$$6, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
				if (ZEPHIR_LE_LONG(&_20$$6, xHat)) {
					ZEPHIR_OBS_NVAR(&_21$$7);
					zephir_read_property_cached(&_22$$7, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
					zephir_array_fetch_long(&_21$$7, &rowA, (zephir_get_numberval(&_22$$7) - 1), PH_NOISY, "tensor/matrix.zep", 1641);
					ZEPHIR_INIT_NVAR(&_23$$7);
					ZVAL_DOUBLE(&_23$$7, zephir_get_doubleval(&_21$$7));
					zephir_array_append(&b, &_23$$7, PH_SEPARATE, "tensor/matrix.zep", 1641);
					continue;
				}
				ZEPHIR_OBS_NVAR(&_24$$6);
				zephir_array_fetch_long(&_24$$6, &rowA, (xHat - 1), PH_NOISY, "tensor/matrix.zep", 1646);
				t =  (zephir_get_doubleval(&_24$$6));
				ZEPHIR_OBS_NVAR(&_25$$6);
				zephir_array_fetch_long(&_25$$6, &rowA, xHat, PH_NOISY, "tensor/matrix.zep", 1648);
				ZEPHIR_INIT_NVAR(&_26$$6);
				ZVAL_DOUBLE(&_26$$6, (t + (remainder * (zephir_get_numberval(&_25$$6) - t))));
				zephir_array_append(&b, &_26$$6, PH_SEPARATE, "tensor/matrix.zep", 1648);
		}
	}
	ZEPHIR_INIT_NVAR(&rowA);
	ZEPHIR_RETURN_CALL_CE_STATIC(tensor_columnvector_ce, "quick", NULL, 0, &b);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Compute the row variance of the matrix.
 *
 * @param \Tensor\ColumnVector|null mean
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return \Tensor\ColumnVector
 */
PHP_METHOD(Tensor_Matrix, variance)
{
	zval _7$$5, _9$$5, _10$$5;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *mean = NULL, mean_sub, __$null, _0$$4, _1$$4, _2$$4, _3$$3, _4$$3, _5$$5, _6$$5, _8$$5, _11, _12, _13, _14;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&mean_sub);
	ZVAL_NULL(&__$null);
	ZVAL_UNDEF(&_0$$4);
	ZVAL_UNDEF(&_1$$4);
	ZVAL_UNDEF(&_2$$4);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$5);
	ZVAL_UNDEF(&_6$$5);
	ZVAL_UNDEF(&_8$$5);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_7$$5);
	ZVAL_UNDEF(&_9$$5);
	ZVAL_UNDEF(&_10$$5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}

	bool is_null_true = 1;
	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL_OR_NULL(mean)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 0, 1, &mean);
	if (!mean) {
		mean = &mean_sub;
		ZEPHIR_CPY_WRT(mean, &__$null);
	} else {
		ZEPHIR_SEPARATE_PARAM(mean);
	}
	if (!(Z_TYPE_P(mean) == IS_NULL)) {
		if (UNEXPECTED(!((zephir_instance_of_ev(mean, tensor_columnvector_ce))))) {
			ZEPHIR_INIT_VAR(&_0$$4);
			object_init_ex(&_0$$4, tensor_exceptions_invalidargumentexception_ce);
			ZEPHIR_INIT_VAR(&_1$$4);
			zephir_gettype(&_1$$4, mean);
			ZEPHIR_INIT_VAR(&_2$$4);
			ZEPHIR_CONCAT_SSVS(&_2$$4, "Mean must be a", " column vector ", &_1$$4, " given.");
			ZEPHIR_CALL_METHOD(NULL, &_0$$4, "__construct", NULL, 3, &_2$$4);
			zephir_check_call_status();
			zephir_throw_exception_debug(&_0$$4, "tensor/matrix.zep", 1667);
			ZEPHIR_MM_RESTORE();
			return;
		}
		ZEPHIR_CALL_METHOD(&_3$$3, mean, "m", NULL, 0);
		zephir_check_call_status();
		zephir_read_property_cached(&_4$$3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
		if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_3$$3, &_4$$3))) {
			ZEPHIR_INIT_VAR(&_5$$5);
			object_init_ex(&_5$$5, tensor_exceptions_dimensionalitymismatch_ce);
			zephir_memory_observe(&_6$$5);
			zephir_read_property_cached(&_6$$5, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
			zephir_cast_to_string(&_7$$5, &_6$$5);
			ZEPHIR_CALL_METHOD(&_8$$5, mean, "m", NULL, 0);
			zephir_check_call_status();
			zephir_cast_to_string(&_9$$5, &_8$$5);
			ZEPHIR_INIT_VAR(&_10$$5);
			ZEPHIR_CONCAT_SSVSVS(&_10$$5, "Mean vector must", " have ", &_7$$5, " rows, ", &_9$$5, " given.");
			ZEPHIR_CALL_METHOD(NULL, &_5$$5, "__construct", NULL, 3, &_10$$5);
			zephir_check_call_status();
			zephir_throw_exception_debug(&_5$$5, "tensor/matrix.zep", 1673);
			ZEPHIR_MM_RESTORE();
			return;
		}
	} else {
		ZEPHIR_CALL_METHOD(mean, this_ptr, "mean", NULL, 0);
		zephir_check_call_status();
	}
	ZEPHIR_CALL_METHOD(&_11, this_ptr, "subtractColumnVector", NULL, 0, mean);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_12, &_11, "square", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_13, &_12, "sum", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_METHOD(&_13, "divideScalar", NULL, 0, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Compute the covariance of the matrix.
 *
 * @param \Tensor\ColumnVector mean
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, covariance)
{
	zval _4$$4, _6$$4, _7$$4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *mean = NULL, mean_sub, __$null, _0$$3, _1$$3, _2$$4, _3$$4, _5$$4, b, _8, _9, _10;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&mean_sub);
	ZVAL_NULL(&__$null);
	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$4);
	ZVAL_UNDEF(&_3$$4);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&b);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_4$$4);
	ZVAL_UNDEF(&_6$$4);
	ZVAL_UNDEF(&_7$$4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}

	bool is_null_true = 1;
	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(mean, tensor_columnvector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 0, 1, &mean);
	if (!mean) {
		mean = &mean_sub;
		ZEPHIR_CPY_WRT(mean, &__$null);
	} else {
		ZEPHIR_SEPARATE_PARAM(mean);
	}
	if (!(Z_TYPE_P(mean) == IS_NULL)) {
		ZEPHIR_CALL_METHOD(&_0$$3, mean, "m", NULL, 0);
		zephir_check_call_status();
		zephir_read_property_cached(&_1$$3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
		if (!ZEPHIR_IS_IDENTICAL(&_0$$3, &_1$$3)) {
			ZEPHIR_INIT_VAR(&_2$$4);
			object_init_ex(&_2$$4, tensor_exceptions_dimensionalitymismatch_ce);
			zephir_memory_observe(&_3$$4);
			zephir_read_property_cached(&_3$$4, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
			zephir_cast_to_string(&_4$$4, &_3$$4);
			ZEPHIR_CALL_METHOD(&_5$$4, mean, "m", NULL, 0);
			zephir_check_call_status();
			zephir_cast_to_string(&_6$$4, &_5$$4);
			ZEPHIR_INIT_VAR(&_7$$4);
			ZEPHIR_CONCAT_SSVSVS(&_7$$4, "Mean vector must", " have ", &_4$$4, " rows, ", &_6$$4, " given.");
			ZEPHIR_CALL_METHOD(NULL, &_2$$4, "__construct", NULL, 3, &_7$$4);
			zephir_check_call_status();
			zephir_throw_exception_debug(&_2$$4, "tensor/matrix.zep", 1698);
			ZEPHIR_MM_RESTORE();
			return;
		}
	} else {
		ZEPHIR_CALL_METHOD(mean, this_ptr, "mean", NULL, 0);
		zephir_check_call_status();
	}
	ZEPHIR_CALL_METHOD(&b, this_ptr, "subtractColumnVector", NULL, 0, mean);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_9, &b, "transpose", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_8, &b, "matmul", NULL, 0, &_9);
	zephir_check_call_status();
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_METHOD(&_8, "divideScalar", NULL, 0, &_10);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Round the elements in the matrix to a given decimal place.
 *
 * @param int precision
 * @return self
 */
PHP_METHOD(Tensor_Matrix, round)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *precision_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, _4$$3, _5$$4, _6$$4, _7$$4, _8$$4, _9, _10, _11, _12, _13, _14;
	zend_long precision, ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_6$$4);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_8$$4);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(precision)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 0, 1, &precision_param);
	if (!precision_param) {
		precision = 0;
	} else {
		}
	if (precision == 0) {
		ZEPHIR_INIT_VAR(&_0$$3);
		zephir_read_property_cached(&_1$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
		ZEPHIR_INIT_VAR(&_2$$3);
		ZVAL_STRING(&_2$$3, "round");
		tensor_buffer_unary(&_0$$3, &_1$$3, &_2$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
		zephir_read_property_cached(&_4$$3, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
		ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0$$3, &_3$$3, &_4$$3);
		zephir_check_call_status();
		RETURN_MM();
	}
	if (UNEXPECTED(precision < 0)) {
		ZEPHIR_INIT_VAR(&_5$$4);
		object_init_ex(&_5$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_6$$4, precision);
		ZEPHIR_CALL_FUNCTION(&_7$$4, "strval", NULL, 4, &_6$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_8$$4);
		ZEPHIR_CONCAT_SSVS(&_8$$4, "Decimal precision cannot", " be less than 0, ", &_7$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 3, &_8$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/matrix.zep", 1724);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_11);
	ZVAL_STRING(&_11, "round");
	ZVAL_LONG(&_12, precision);
	tensor_buffer_unary_arg(&_9, &_10, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Round the elements in the matrix down to the nearest integer.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, floor)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "floor");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Round the elements in the matrix up to the nearest integer.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, ceil)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "ceil");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Clip the elements in the matrix to be between given minimum and maximum
 * and return a new matrix.
 *
 * @param float min
 * @param float max
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Matrix, clip)
{
	zval _1$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *min_param = NULL, *max_param = NULL, _0$$3, _2, _3, _4, _5, _6, _7;
	double min, max;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_1$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(min_param)
		Z_PARAM_ZVAL(max_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 2, 0, &min_param, &max_param);
	min = zephir_get_doubleval(min_param);
	max = zephir_get_doubleval(max_param);
	if (UNEXPECTED(min > max)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		object_init_ex(&_0$$3, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_INIT_VAR(&_1$$3);
		ZEPHIR_CONCAT_SS(&_1$$3, "Minimum cannot be", " greater than maximum.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 3, &_1$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/matrix.zep", 1763);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_4, min);
	ZVAL_DOUBLE(&_5, max);
	tensor_buffer_clip(&_2, &_3, &_4, &_5);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_2, &_6, &_7);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Clip the tensor to be lower bounded by a given minimum.
 *
 * @param float min
 * @return self
 */
PHP_METHOD(Tensor_Matrix, clipLower)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *min_param = NULL, _0, _1, _2, _3, _4, _5;
	double min;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(min_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &min_param);
	min = zephir_get_doubleval(min_param);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "clipLower");
	ZVAL_DOUBLE(&_3, min);
	tensor_buffer_unary_arg(&_0, &_1, &_2, &_3);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_4, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Clip the tensor to be upper bounded by a given maximum.
 *
 * @param float max
 * @return self
 */
PHP_METHOD(Tensor_Matrix, clipUpper)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *max_param = NULL, _0, _1, _2, _3, _4, _5;
	double max;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(max_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &max_param);
	max = zephir_get_doubleval(max_param);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "clipUpper");
	ZVAL_DOUBLE(&_3, max);
	tensor_buffer_unary_arg(&_0, &_1, &_2, &_3);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_4, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise sign indication.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, sign)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "sign");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Negate the matrix i.e take the negative of each value elementwise.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, negate)
{
	zval _0, _1, _2, _3, _4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_2);
	ZVAL_STRING(&_2, "negate");
	tensor_buffer_unary(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_3, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Attach matrix b above this matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, augmentAbove)
{
	zval _6$$3, _8$$3, _9$$3;
	zend_bool _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2, _3, _10, _11, _12, _13, _14, _15, _4$$3, _5$$3, _7$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_15);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_7$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_8$$3);
	ZVAL_UNDEF(&_9$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	_1 = ZEPHIR_GT_LONG(&_0, 0);
	if (_1) {
		ZEPHIR_CALL_METHOD(&_2, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
		_1 = !ZEPHIR_IS_IDENTICAL(&_2, &_3);
	}
	if (UNEXPECTED(_1)) {
		ZEPHIR_INIT_VAR(&_4$$3);
		object_init_ex(&_4$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_5$$3);
		zephir_read_property_cached(&_5$$3, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC);
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_CALL_METHOD(&_7$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_8$$3, &_7$$3);
		ZEPHIR_INIT_VAR(&_9$$3);
		ZEPHIR_CONCAT_SVSVS(&_9$$3, "Matrix A requires", &_6$$3, " columns but Matrix B has ", &_8$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_4$$3, "__construct", NULL, 3, &_9$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_4$$3, "tensor/matrix.zep", 1823);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_10);
	zephir_read_property_cached(&_11, b, _zephir_prop_2, 0, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_12, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_append(&_10, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_14, b, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&_15);
	zephir_add_function(&_15, &_13, &_14);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_10, &_15, &_13);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Attach matrix b below this matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, augmentBelow)
{
	zval _6$$3, _8$$3, _9$$3;
	zend_bool _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2, _3, _10, _11, _12, _13, _14, _15, _4$$3, _5$$3, _7$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_15);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_7$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_8$$3);
	ZVAL_UNDEF(&_9$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	_1 = ZEPHIR_GT_LONG(&_0, 0);
	if (_1) {
		ZEPHIR_CALL_METHOD(&_2, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
		_1 = !ZEPHIR_IS_IDENTICAL(&_2, &_3);
	}
	if (UNEXPECTED(_1)) {
		ZEPHIR_INIT_VAR(&_4$$3);
		object_init_ex(&_4$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_5$$3);
		zephir_read_property_cached(&_5$$3, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC);
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_CALL_METHOD(&_7$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_8$$3, &_7$$3);
		ZEPHIR_INIT_VAR(&_9$$3);
		ZEPHIR_CONCAT_SVSVS(&_9$$3, "Matrix A requires", &_6$$3, " columns but Matrix B has ", &_8$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_4$$3, "__construct", NULL, 3, &_9$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_4$$3, "tensor/matrix.zep", 1841);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_10);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_12, b, _zephir_prop_2, 0, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_append(&_10, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_14, b, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&_15);
	zephir_add_function(&_15, &_13, &_14);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_10, &_15, &_13);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Attach matrix b to the left of this matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, augmentLeft)
{
	zval _6$$3, _8$$3, _9$$3;
	zend_bool _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2, _3, _10, _11, _12, _13, _14, _15, _16, _17, _18, _4$$3, _5$$3, _7$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_15);
	ZVAL_UNDEF(&_16);
	ZVAL_UNDEF(&_17);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_7$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_8$$3);
	ZVAL_UNDEF(&_9$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	_1 = ZEPHIR_GT_LONG(&_0, 0);
	if (_1) {
		ZEPHIR_CALL_METHOD(&_2, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_read_property_cached(&_3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
		_1 = !ZEPHIR_IS_IDENTICAL(&_2, &_3);
	}
	if (UNEXPECTED(_1)) {
		ZEPHIR_INIT_VAR(&_4$$3);
		object_init_ex(&_4$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_5$$3);
		zephir_read_property_cached(&_5$$3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_CALL_METHOD(&_7$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_8$$3, &_7$$3);
		ZEPHIR_INIT_VAR(&_9$$3);
		ZEPHIR_CONCAT_SVSVS(&_9$$3, "Matrix A requires", &_6$$3, " rows but Matrix B has ", &_8$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_4$$3, "__construct", NULL, 3, &_9$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_4$$3, "tensor/matrix.zep", 1859);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_10);
	zephir_read_property_cached(&_11, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_12, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_13, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_append_columns(&_10, &_11, &_12, &_13, &_14);
	zephir_read_property_cached(&_15, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_16, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_17, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&_18);
	zephir_add_function(&_18, &_16, &_17);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_10, &_15, &_18);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Attach matrix b to the left of this matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, augmentRight)
{
	zval _6$$3, _8$$3, _9$$3;
	zend_bool _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2, _3, _10, _11, _12, _13, _14, _15, _16, _17, _18, _4$$3, _5$$3, _7$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_15);
	ZVAL_UNDEF(&_16);
	ZVAL_UNDEF(&_17);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_7$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_8$$3);
	ZVAL_UNDEF(&_9$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	_1 = ZEPHIR_GT_LONG(&_0, 0);
	if (_1) {
		ZEPHIR_CALL_METHOD(&_2, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_read_property_cached(&_3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
		_1 = !ZEPHIR_IS_IDENTICAL(&_2, &_3);
	}
	if (UNEXPECTED(_1)) {
		ZEPHIR_INIT_VAR(&_4$$3);
		object_init_ex(&_4$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_5$$3);
		zephir_read_property_cached(&_5$$3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_CALL_METHOD(&_7$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_8$$3, &_7$$3);
		ZEPHIR_INIT_VAR(&_9$$3);
		ZEPHIR_CONCAT_SVSVS(&_9$$3, "Matrix A requires", &_6$$3, " rows but Matrix B has ", &_8$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_4$$3, "__construct", NULL, 3, &_9$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_4$$3, "tensor/matrix.zep", 1877);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_10);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_12, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_14, b, "n", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_append_columns(&_10, &_11, &_12, &_13, &_14);
	zephir_read_property_cached(&_15, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_16, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_17, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&_18);
	zephir_add_function(&_18, &_16, &_17);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_10, &_15, &_18);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Repeat the matrix m times along the vertival axes and n times along the
 * horizontal axes.
 *
 * @param int m
 * @param int n
 * @return self
 */
PHP_METHOD(Tensor_Matrix, repeat)
{
	zend_bool _7$$3;
	zval b, temp;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *m_param = NULL, *n_param = NULL, rowA, _10, _11, _0$$3, *_1$$3, _2$$3, *_3$$3, _6$$3, _4$$4, _5$$4, _8$$6, _9$$6;
	zend_long m, n, ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&rowA);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_4$$4);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_8$$6);
	ZVAL_UNDEF(&_9$$6);
	ZVAL_UNDEF(&b);
	ZVAL_UNDEF(&temp);
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(m)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 2, 0, &m_param, &n_param);
	ZEPHIR_INIT_VAR(&b);
	array_init(&b);
	ZEPHIR_INIT_VAR(&temp);
	array_init(&temp);
	if (n > 0) {
		ZEPHIR_CALL_METHOD(&_0$$3, this_ptr, "asArray", NULL, 0);
		zephir_check_call_status();
		if (Z_TYPE_P(&_0$$3) == IS_STRING) {
			ZEPHIR_INIT_VAR(&_2$$3);
			zephir_string_to_char_array(&_2$$3, &_0$$3);
			_1$$3 = &_2$$3;
		} else {
			_1$$3 = &_0$$3;
		}
		zephir_is_iterable(_1$$3, 0, "tensor/matrix.zep", 1907);
		if (Z_TYPE_P(_1$$3) == IS_ARRAY) {
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_1$$3), _3$$3)
			{
				ZEPHIR_INIT_NVAR(&rowA);
				ZVAL_COPY(&rowA, _3$$3);
				ZEPHIR_INIT_NVAR(&temp);
				array_init(&temp);
				while (1) {
					if (!(zephir_fast_count_int(&temp) <= n)) {
						break;
					}
					zephir_array_append(&temp, &rowA, PH_SEPARATE, "tensor/matrix.zep", 1902);
				}
				ZEPHIR_INIT_NVAR(&_4$$4);
				ZEPHIR_INIT_NVAR(&_5$$4);
				ZVAL_STRING(&_5$$4, "array_merge");
				ZEPHIR_CALL_USER_FUNC_ARRAY(&_4$$4, &_5$$4, &temp);
				zephir_check_call_status();
				zephir_array_append(&b, &_4$$4, PH_SEPARATE, "tensor/matrix.zep", 1905);
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEPHIR_CALL_METHOD(NULL, _1$$3, "rewind", NULL, 0);
			zephir_check_call_status();
			_7$$3 = 1;
			while (1) {
				if (_7$$3) {
					_7$$3 = 0;
				} else {
					ZEPHIR_CALL_METHOD(NULL, _1$$3, "next", NULL, 0);
					zephir_check_call_status();
				}
				ZEPHIR_CALL_METHOD(&_6$$3, _1$$3, "valid", NULL, 0);
				zephir_check_call_status();
				if (!zend_is_true(&_6$$3)) {
					break;
				}
				ZEPHIR_CALL_METHOD(&rowA, _1$$3, "current", NULL, 0);
				zephir_check_call_status();
					ZEPHIR_INIT_NVAR(&temp);
					array_init(&temp);
					while (1) {
						if (!(zephir_fast_count_int(&temp) <= n)) {
							break;
						}
						zephir_array_append(&temp, &rowA, PH_SEPARATE, "tensor/matrix.zep", 1902);
					}
					ZEPHIR_INIT_NVAR(&_8$$6);
					ZEPHIR_INIT_NVAR(&_9$$6);
					ZVAL_STRING(&_9$$6, "array_merge");
					ZEPHIR_CALL_USER_FUNC_ARRAY(&_8$$6, &_9$$6, &temp);
					zephir_check_call_status();
					zephir_array_append(&b, &_8$$6, PH_SEPARATE, "tensor/matrix.zep", 1905);
			}
		}
		ZEPHIR_INIT_NVAR(&rowA);
	}
	ZEPHIR_INIT_NVAR(&temp);
	array_init(&temp);
	while (1) {
		if (!(zephir_fast_count_int(&temp) <= m)) {
			break;
		}
		zephir_array_append(&temp, &b, PH_SEPARATE, "tensor/matrix.zep", 1912);
	}
	ZEPHIR_INIT_VAR(&_10);
	ZEPHIR_INIT_VAR(&_11);
	ZVAL_STRING(&_11, "array_merge");
	ZEPHIR_CALL_USER_FUNC_ARRAY(&_10, &_11, &temp);
	zephir_check_call_status();
	ZEPHIR_RETURN_CALL_SELF("quick", NULL, 0, &_10);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise product between this matrix and another matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, multiplyMatrix)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, _6, _7, _8, _9, _10, _11, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "shape", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, this_ptr, "shape", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		ZEPHIR_CALL_METHOD(&_3$$3, this_ptr, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_CALL_METHOD(&_4$$3, b, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_5$$3);
		ZEPHIR_CONCAT_VSVS(&_5$$3, &_3$$3, " matrix expected but ", &_4$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 1929);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_6);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_9);
	ZVAL_STRING(&_9, "multiply");
	tensor_buffer_binary(&_6, &_7, &_8, &_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_6, &_10, &_11);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the division of two elements, element-wise.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, divideMatrix)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, _6, _7, _8, _9, _10, _11, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "shape", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, this_ptr, "shape", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		ZEPHIR_CALL_METHOD(&_3$$3, this_ptr, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_CALL_METHOD(&_4$$3, b, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_5$$3);
		ZEPHIR_CONCAT_VSVS(&_5$$3, &_3$$3, " matrix expected but ", &_4$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 1946);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_6);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_9);
	ZVAL_STRING(&_9, "divide");
	tensor_buffer_binary(&_6, &_7, &_8, &_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_6, &_10, &_11);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Add this matrix together with another matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, addMatrix)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, _6, _7, _8, _9, _10, _11, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "shape", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, this_ptr, "shape", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		ZEPHIR_CALL_METHOD(&_3$$3, this_ptr, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_CALL_METHOD(&_4$$3, b, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_5$$3);
		ZEPHIR_CONCAT_VSVS(&_5$$3, &_3$$3, " matrix expected but ", &_4$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 1963);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_6);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_9);
	ZVAL_STRING(&_9, "add");
	tensor_buffer_binary(&_6, &_7, &_8, &_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_6, &_10, &_11);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Subtract a matrix from this matrix element-wise.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, subtractMatrix)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, _6, _7, _8, _9, _10, _11, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "shape", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, this_ptr, "shape", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		ZEPHIR_CALL_METHOD(&_3$$3, this_ptr, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_CALL_METHOD(&_4$$3, b, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_5$$3);
		ZEPHIR_CONCAT_VSVS(&_5$$3, &_3$$3, " matrix expected but ", &_4$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 1980);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_6);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_9);
	ZVAL_STRING(&_9, "subtract");
	tensor_buffer_binary(&_6, &_7, &_8, &_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_6, &_10, &_11);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Raise this matrix to the power of the elementwise entry in another
 * matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, powMatrix)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, _6, _7, _8, _9, _10, _11, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "shape", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, this_ptr, "shape", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		ZEPHIR_CALL_METHOD(&_3$$3, this_ptr, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_CALL_METHOD(&_4$$3, b, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_5$$3);
		ZEPHIR_CONCAT_VSVS(&_5$$3, &_3$$3, " matrix expected but ", &_4$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 1998);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_6);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_9);
	ZVAL_STRING(&_9, "pow");
	tensor_buffer_binary(&_6, &_7, &_8, &_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_6, &_10, &_11);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Calculate the modulus i.e remainder of division between this matri and
 * another matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, modMatrix)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, _6, _7, _8, _9, _10, _11, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "shape", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, this_ptr, "shape", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		ZEPHIR_CALL_METHOD(&_3$$3, this_ptr, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_CALL_METHOD(&_4$$3, b, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_5$$3);
		ZEPHIR_CONCAT_VSVS(&_5$$3, &_3$$3, " matrix expected but ", &_4$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2016);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_6);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_9);
	ZVAL_STRING(&_9, "mod");
	tensor_buffer_binary(&_6, &_7, &_8, &_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_6, &_10, &_11);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise equality comparison of this matrix and
 * another matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, equalMatrix)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, _6, _7, _8, _9, _10, _11, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "shape", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, this_ptr, "shape", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		ZEPHIR_CALL_METHOD(&_3$$3, this_ptr, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_CALL_METHOD(&_4$$3, b, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_5$$3);
		ZEPHIR_CONCAT_VSVS(&_5$$3, &_3$$3, " matrix expected but ", &_4$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2034);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_6);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_9);
	ZVAL_STRING(&_9, "equal");
	tensor_buffer_binary(&_6, &_7, &_8, &_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_6, &_10, &_11);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise not equal comparison of this matrix and another matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, notEqualMatrix)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, _6, _7, _8, _9, _10, _11, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "shape", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, this_ptr, "shape", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		ZEPHIR_CALL_METHOD(&_3$$3, this_ptr, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_CALL_METHOD(&_4$$3, b, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_5$$3);
		ZEPHIR_CONCAT_VSVS(&_5$$3, &_3$$3, " matrix expected but ", &_4$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2051);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_6);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_9);
	ZVAL_STRING(&_9, "notEqual");
	tensor_buffer_binary(&_6, &_7, &_8, &_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_6, &_10, &_11);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise greater than comparison of this matrix
 * and another matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, greaterMatrix)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, _6, _7, _8, _9, _10, _11, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "shape", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, this_ptr, "shape", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		ZEPHIR_CALL_METHOD(&_3$$3, this_ptr, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_CALL_METHOD(&_4$$3, b, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_5$$3);
		ZEPHIR_CONCAT_VSVS(&_5$$3, &_3$$3, " matrix expected but ", &_4$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2069);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_6);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_9);
	ZVAL_STRING(&_9, "greater");
	tensor_buffer_binary(&_6, &_7, &_8, &_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_6, &_10, &_11);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise greater than or equal to comparison of
 * this matrix and another matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, greaterEqualMatrix)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, _6, _7, _8, _9, _10, _11, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "shape", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, this_ptr, "shape", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		ZEPHIR_CALL_METHOD(&_3$$3, this_ptr, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_CALL_METHOD(&_4$$3, b, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_5$$3);
		ZEPHIR_CONCAT_VSVS(&_5$$3, &_3$$3, " matrix expected but ", &_4$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2087);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_6);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_9);
	ZVAL_STRING(&_9, "greaterEqual");
	tensor_buffer_binary(&_6, &_7, &_8, &_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_6, &_10, &_11);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise less than comparison of this matrix and another matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, lessMatrix)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, _6, _7, _8, _9, _10, _11, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "shape", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, this_ptr, "shape", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		ZEPHIR_CALL_METHOD(&_3$$3, this_ptr, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_CALL_METHOD(&_4$$3, b, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_5$$3);
		ZEPHIR_CONCAT_VSVS(&_5$$3, &_3$$3, " matrix expected but ", &_4$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2104);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_6);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_9);
	ZVAL_STRING(&_9, "less");
	tensor_buffer_binary(&_6, &_7, &_8, &_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_6, &_10, &_11);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise less than or equal to comparison of this matrix and another matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, lessEqualMatrix)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, _6, _7, _8, _9, _10, _11, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_matrix_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "shape", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, this_ptr, "shape", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		ZEPHIR_CALL_METHOD(&_3$$3, this_ptr, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_CALL_METHOD(&_4$$3, b, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_5$$3);
		ZEPHIR_CONCAT_VSVS(&_5$$3, &_3$$3, " matrix expected but ", &_4$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2121);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&_6);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_9);
	ZVAL_STRING(&_9, "lessEqual");
	tensor_buffer_binary(&_6, &_7, &_8, &_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_6, &_10, &_11);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Multiply this matrix by a vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, multiplyVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_vector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2139);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "multiply");
	tensor_buffer_broadcast_vector(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Divide this matrix by a vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, divideVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_vector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2161);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "divide");
	tensor_buffer_broadcast_vector(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Add this matrix by a vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, addVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_vector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2183);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "add");
	tensor_buffer_broadcast_vector(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Subtract a vector from this matrix.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, subtractVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_vector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2205);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "subtract");
	tensor_buffer_broadcast_vector(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Raise this matrix to the power of a vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, powVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_vector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2227);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "pow");
	tensor_buffer_broadcast_vector(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Calculate the modulus of this matrix with a vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, modVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_vector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2249);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "mod");
	tensor_buffer_broadcast_vector(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise equality comparison of this matrix and a
 * vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, equalVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_vector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2272);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "equal");
	tensor_buffer_broadcast_vector(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise not equal comparison of this matrix and a vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, notEqualVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_vector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2294);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "notEqual");
	tensor_buffer_broadcast_vector(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise greater than comparison of this matrix and a vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, greaterVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_vector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2316);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "greater");
	tensor_buffer_broadcast_vector(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise greater than or equal to comparison of this matrix and a vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, greaterEqualVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_vector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2338);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "greaterEqual");
	tensor_buffer_broadcast_vector(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise less than comparison of this matrix and a vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, lessVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_vector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2360);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "less");
	tensor_buffer_broadcast_vector(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise less than or equal to comparison of
 * this matrix and a vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, lessEqualVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_vector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2383);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "lessEqual");
	tensor_buffer_broadcast_vector(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_2, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_0, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Multiply this matrix with a column vector.
 *
 * @param \Tensor\ColumnVector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, multiplyColumnVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_columnvector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2405);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "multiply");
	tensor_buffer_broadcast_column(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Divide this matrix with a column vector.
 *
 * @param \Tensor\ColumnVector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, divideColumnVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_columnvector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2427);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "divide");
	tensor_buffer_broadcast_column(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Add this matrix to a column vector.
 *
 * @param \Tensor\ColumnVector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, addColumnVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_columnvector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2449);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "add");
	tensor_buffer_broadcast_column(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Subtract a column vector from this matrix.
 *
 * @param \Tensor\ColumnVector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, subtractColumnVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_columnvector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2471);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "subtract");
	tensor_buffer_broadcast_column(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Raise this matrix to the power of a column vector.
 *
 * @param \Tensor\ColumnVector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, powColumnVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_columnvector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2493);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "pow");
	tensor_buffer_broadcast_column(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Mod this matrix with a column vector.
 *
 * @param \Tensor\ColumnVector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, modColumnVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_columnvector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2515);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "mod");
	tensor_buffer_broadcast_column(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise equality comparison of this matrix and a column vector.
 *
 * @param \Tensor\ColumnVector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, equalColumnVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_columnvector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2537);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "equal");
	tensor_buffer_broadcast_column(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise not equal comparison of this matrix and a column vector.
 *
 * @param \Tensor\ColumnVector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, notEqualColumnVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_columnvector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2559);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "notEqual");
	tensor_buffer_broadcast_column(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise greater than comparison of this matrix and a column vector.
 *
 * @param \Tensor\ColumnVector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, greaterColumnVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_columnvector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2581);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "greater");
	tensor_buffer_broadcast_column(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise greater than or equal to comparison of this matrix and a column vector.
 *
 * @param \Tensor\ColumnVector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, greaterEqualColumnVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_columnvector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2603);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "greaterEqual");
	tensor_buffer_broadcast_column(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise less than comparison of this matrix and a column vector.
 *
 * @param \Tensor\ColumnVector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, lessColumnVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_columnvector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2625);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "less");
	tensor_buffer_broadcast_column(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise less than or equal to comparison of this matrix and a column vector.
 *
 * @param \Tensor\ColumnVector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Matrix, lessEqualColumnVector)
{
	zval _4$$3, _6$$3, _7$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, bHat, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, tensor_columnvector_ce)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 3, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2647);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&bHat);
	ZEPHIR_CALL_METHOD(&_8, b, "asArray", NULL, 0);
	zephir_check_call_status();
	tensor_buffer_from_array(&bHat, &_8);
	ZEPHIR_INIT_VAR(&_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_12);
	ZVAL_STRING(&_12, "lessEqual");
	tensor_buffer_broadcast_column(&_9, &_10, &bHat, &_11, &_12);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_9, &_13, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Multiply this matrix by a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Matrix, multiplyScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2, _3, _4, _5;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	ZEPHIR_INIT_VAR(&_3);
	ZVAL_STRING(&_3, "multiply");
	tensor_buffer_binary_scalar(&_0, &_1, &_2, &_3);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_4, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Divide this matrix by a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Matrix, divideScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2, _3, _4, _5;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	ZEPHIR_INIT_VAR(&_3);
	ZVAL_STRING(&_3, "divide");
	tensor_buffer_binary_scalar(&_0, &_1, &_2, &_3);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_4, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Add this matrix by a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Matrix, addScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2, _3, _4, _5;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	ZEPHIR_INIT_VAR(&_3);
	ZVAL_STRING(&_3, "add");
	tensor_buffer_binary_scalar(&_0, &_1, &_2, &_3);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_4, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Subtract a scalar from this matrix.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Matrix, subtractScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2, _3, _4, _5;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	ZEPHIR_INIT_VAR(&_3);
	ZVAL_STRING(&_3, "subtract");
	tensor_buffer_binary_scalar(&_0, &_1, &_2, &_3);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_4, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Raise the matrix to a given scalar power.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Matrix, powScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2, _3, _4, _5;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	ZEPHIR_INIT_VAR(&_3);
	ZVAL_STRING(&_3, "pow");
	tensor_buffer_binary_scalar(&_0, &_1, &_2, &_3);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_4, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Calculate the modulus of this matrix with a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Matrix, modScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2, _3, _4, _5;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	ZEPHIR_INIT_VAR(&_3);
	ZVAL_STRING(&_3, "mod");
	tensor_buffer_binary_scalar(&_0, &_1, &_2, &_3);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_4, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise equality comparison of this matrix and a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Matrix, equalScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2, _3, _4, _5;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	ZEPHIR_INIT_VAR(&_3);
	ZVAL_STRING(&_3, "equal");
	tensor_buffer_binary_scalar(&_0, &_1, &_2, &_3);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_4, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise not equal comparison of this matrix and a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Matrix, notEqualScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2, _3, _4, _5;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	ZEPHIR_INIT_VAR(&_3);
	ZVAL_STRING(&_3, "notEqual");
	tensor_buffer_binary_scalar(&_0, &_1, &_2, &_3);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_4, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise greater than comparison of this matrix and a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Matrix, greaterScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2, _3, _4, _5;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	ZEPHIR_INIT_VAR(&_3);
	ZVAL_STRING(&_3, "greater");
	tensor_buffer_binary_scalar(&_0, &_1, &_2, &_3);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_4, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise greater than or equal to comparison of
 * this matrix and a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Matrix, greaterEqualScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2, _3, _4, _5;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	ZEPHIR_INIT_VAR(&_3);
	ZVAL_STRING(&_3, "greaterEqual");
	tensor_buffer_binary_scalar(&_0, &_1, &_2, &_3);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_4, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise less than comparison of this matrix and a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Matrix, lessScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2, _3, _4, _5;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	ZEPHIR_INIT_VAR(&_3);
	ZVAL_STRING(&_3, "less");
	tensor_buffer_binary_scalar(&_0, &_1, &_2, &_3);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_4, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise less than or equal to comparison of
 * this matrix and a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Matrix, lessEqualScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2, _3, _4, _5;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	ZEPHIR_INIT_VAR(&_3);
	ZVAL_STRING(&_3, "lessEqual");
	tensor_buffer_binary_scalar(&_0, &_1, &_2, &_3);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 17, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_2, 18, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_SELF("wrap", NULL, 0, &_0, &_4, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * @return int
 */
PHP_METHOD(Tensor_Matrix, count)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_RETURN_CALL_METHOD(this_ptr, "size", NULL, 0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * @param mixed index
 * @param array values
 * @throws \Tensor\Exceptions\RuntimeException
 */
PHP_METHOD(Tensor_Matrix, offsetSet)
{
	zval *index, index_sub, *values, values_sub;

	ZVAL_UNDEF(&index_sub);
	ZVAL_UNDEF(&values_sub);
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(index)
		Z_PARAM_ZVAL(values)
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(2, 0, &index, &values);
	ZEPHIR_THROW_EXCEPTION_DEBUG_STRW(tensor_exceptions_runtimeexception_ce, "Matrix cannot be mutated directly.", "tensor/matrix.zep", 2806);
	return;
}

/**
 * Does a given column exist in the matrix.
 *
 * @param mixed index
 * @return bool
 */
PHP_METHOD(Tensor_Matrix, offsetExists)
{
	zend_bool _0, _1;
	zval *index, index_sub, _2;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&index_sub);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(index)
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(1, 0, &index);
	_0 = Z_TYPE_P(index) == IS_LONG;
	if (_0) {
		_0 = ZEPHIR_GE_LONG(index, 0);
	}
	_1 = _0;
	if (_1) {
		zephir_read_property_cached(&_2, this_ptr, _zephir_prop_0, 17, PH_NOISY_CC | PH_READONLY);
		_1 = ZEPHIR_LT(index, &_2);
	}
	RETURN_BOOL(_1);
}

/**
 * @param mixed index
 * @throws \Tensor\Exceptions\RuntimeException
 */
PHP_METHOD(Tensor_Matrix, offsetUnset)
{
	zval *index, index_sub;

	ZVAL_UNDEF(&index_sub);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(index)
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(1, 0, &index);
	ZEPHIR_THROW_EXCEPTION_DEBUG_STRW(tensor_exceptions_runtimeexception_ce, "Matrix cannot be mutated directly.", "tensor/matrix.zep", 2827);
	return;
}

/**
 * Return a row from the matrix at the given index.
 *
 * @param mixed index
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return \Tensor\Vector
 */
PHP_METHOD(Tensor_Matrix, offsetGet)
{
	zval _8, _9;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *index, index_sub, _0, _7, _1$$3, _2$$3, _3$$3, _4$$3, _5$$3, _6$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&index_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(index)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &index);
	ZEPHIR_CALL_METHOD(&_0, this_ptr, "offsetExists", NULL, 0, index);
	zephir_check_call_status();
	if (UNEXPECTED(zephir_is_true(&_0))) {
		object_init_ex(return_value, tensor_vector_ce);
		ZEPHIR_INIT_VAR(&_1$$3);
		zephir_read_property_cached(&_2$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
		ZEPHIR_INIT_VAR(&_4$$3);
		mul_function(&_4$$3, index, &_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_1, 18, PH_NOISY_CC | PH_READONLY);
		ZVAL_LONG(&_5$$3, 1);
		tensor_buffer_stride(&_1$$3, &_2$$3, &_4$$3, &_5$$3, &_3$$3);
		ZVAL_BOOL(&_6$$3, 0);
		ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 1, &_1$$3, &_6$$3);
		zephir_check_call_status();
		RETURN_MM();
	}
	ZEPHIR_INIT_VAR(&_7);
	object_init_ex(&_7, tensor_exceptions_invalidargumentexception_ce);
	zephir_cast_to_string(&_8, index);
	ZEPHIR_INIT_VAR(&_9);
	ZEPHIR_CONCAT_SSVS(&_9, "Element not found at", " offset ", &_8, ".");
	ZEPHIR_CALL_METHOD(NULL, &_7, "__construct", NULL, 3, &_9);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_7, "tensor/matrix.zep", 2844);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * Get an iterator for the rows in the matrix.
 *
 * @return \ArrayIterator
 */
PHP_METHOD(Tensor_Matrix, getIterator)
{
	zval _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, spl_ce_ArrayIterator);
	ZEPHIR_CALL_METHOD(&_0, this_ptr, "asVectors", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 16, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

