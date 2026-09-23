
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
#include "kernel/fcall.h"
#include "kernel/exception.h"
#include "kernel/concat.h"
#include "kernel/operators.h"
#include "kernel/string.h"
#include "kernel/array.h"
#include "kernel/object.h"
#include "math.h"
#include "kernel/math.h"
#include "ext/spl/spl_array.h"
#include "include/buffer.h"
#include "include/linear_algebra.h"
#include "include/signal_processing.h"
#include "include/unary.h"
#include "include/arithmetic.h"
#include "include/comparison.h"


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
	 * A contiguous row-major buffer holding the elements of the matrix.
	 *
	 * @var \Tensor\TensorBuffer
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
 * Build a new matrix from a PHP array of rows, each row being a PHP array
 * of numeric elements.
 *
 * @param array[] a
 * @param bool validate
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Matrix, fromArray)
{
	zval _6$$4, _10$$6, _28$$11;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_2 = NULL, *_14 = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS, rows;
	zend_bool validate, _25, _8$$5, _11$$5, _22$$5, _26$$10, _29$$10, _39$$10;
	zval *a_param = NULL, *validate_param = NULL, rowA, valueA, firstRow, n, *_7, _24, buffer, _41, _42, zero$$3, _0$$3, _1$$3, _3$$3, _4$$3, _5$$4, *_17$$5, _18$$5, *_19$$5, _21$$5, _9$$6, _12$$7, _13$$7, _15$$7, _16$$7, _20$$8, _23$$9, *_34$$10, _35$$10, *_36$$10, _38$$10, _27$$11, _30$$12, _31$$12, _32$$12, _33$$12, _37$$13, _40$$14;
	zval a, flat;

	ZVAL_UNDEF(&a);
	ZVAL_UNDEF(&flat);
	ZVAL_UNDEF(&rowA);
	ZVAL_UNDEF(&valueA);
	ZVAL_UNDEF(&firstRow);
	ZVAL_UNDEF(&n);
	ZVAL_UNDEF(&_24);
	ZVAL_UNDEF(&buffer);
	ZVAL_UNDEF(&_41);
	ZVAL_UNDEF(&_42);
	ZVAL_UNDEF(&zero$$3);
	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_18$$5);
	ZVAL_UNDEF(&_21$$5);
	ZVAL_UNDEF(&_9$$6);
	ZVAL_UNDEF(&_12$$7);
	ZVAL_UNDEF(&_13$$7);
	ZVAL_UNDEF(&_15$$7);
	ZVAL_UNDEF(&_16$$7);
	ZVAL_UNDEF(&_20$$8);
	ZVAL_UNDEF(&_23$$9);
	ZVAL_UNDEF(&_35$$10);
	ZVAL_UNDEF(&_38$$10);
	ZVAL_UNDEF(&_27$$11);
	ZVAL_UNDEF(&_30$$12);
	ZVAL_UNDEF(&_31$$12);
	ZVAL_UNDEF(&_32$$12);
	ZVAL_UNDEF(&_33$$12);
	ZVAL_UNDEF(&_37$$13);
	ZVAL_UNDEF(&_40$$14);
	ZVAL_UNDEF(&_6$$4);
	ZVAL_UNDEF(&_10$$6);
	ZVAL_UNDEF(&_28$$11);
	ZEND_PARSE_PARAMETERS_START(1, 2)
		ZEPHIR_Z_PARAM_ARRAY(a, a_param)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(validate)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 1, &a_param, &validate_param);
	zephir_get_arrval(&a, a_param);
	if (!validate_param) {
		validate = 1;
	} else {
		}
	rows = zephir_fast_count_int(&a);
	if (UNEXPECTED(rows < 1)) {
		ZEPHIR_INIT_VAR(&zero$$3);
		ZEPHIR_INIT_VAR(&_0$$3);
		array_init(&_0$$3);
		tensor_buffer_from_array(&zero$$3, &_0$$3);
		object_init_ex(return_value, tensor_matrix_ce);
		ZEPHIR_INIT_VAR(&_1$$3);
		object_init_ex(&_1$$3, tensor_tensorbuffer_ce);
		ZEPHIR_CALL_METHOD(NULL, &_1$$3, "__construct", &_2, 14, &zero$$3);
		zephir_check_call_status();
		ZVAL_LONG(&_3$$3, 0);
		ZVAL_LONG(&_4$$3, 0);
		ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_1$$3, &_3$$3, &_4$$3);
		zephir_check_call_status();
		RETURN_MM();
	}
	ZEPHIR_CALL_FUNCTION(&firstRow, "current", NULL, 27, &a);
	zephir_check_call_status();
	if (UNEXPECTED(!(Z_TYPE_P(&firstRow) == IS_ARRAY))) {
		ZEPHIR_INIT_VAR(&_5$$4);
		object_init_ex(&_5$$4, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_INIT_VAR(&_6$$4);
		ZEPHIR_CONCAT_SS(&_6$$4, "Matrix requires an", " array of arrays.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 2, &_6$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/matrix.zep", 72);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&n);
	ZVAL_LONG(&n, zephir_fast_count_int(&firstRow));
	ZEPHIR_INIT_VAR(&flat);
	array_init(&flat);
	zephir_is_iterable(&a, 0, "tensor/matrix.zep", 96);
	if (Z_TYPE_P(&a) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(&a), _7)
		{
			ZEPHIR_INIT_NVAR(&rowA);
			ZVAL_COPY(&rowA, _7);
			_8$$5 = validate;
			if (_8$$5) {
				_8$$5 = !(Z_TYPE_P(&rowA) == IS_ARRAY);
			}
			if (UNEXPECTED(_8$$5)) {
				ZEPHIR_INIT_NVAR(&_9$$6);
				object_init_ex(&_9$$6, tensor_exceptions_invalidargumentexception_ce);
				ZEPHIR_INIT_NVAR(&_10$$6);
				ZEPHIR_CONCAT_SS(&_10$$6, "Matrix requires an", " array of arrays.");
				ZEPHIR_CALL_METHOD(NULL, &_9$$6, "__construct", NULL, 2, &_10$$6);
				zephir_check_call_status();
				zephir_throw_exception_debug(&_9$$6, "tensor/matrix.zep", 82);
				ZEPHIR_MM_RESTORE();
				return;
			}
			_11$$5 = validate;
			if (_11$$5) {
				_11$$5 = !ZEPHIR_IS_LONG_IDENTICAL(&n, zephir_fast_count_int(&rowA));
			}
			if (UNEXPECTED(_11$$5)) {
				ZEPHIR_INIT_NVAR(&_12$$7);
				object_init_ex(&_12$$7, tensor_exceptions_invalidargumentexception_ce);
				ZEPHIR_CALL_FUNCTION(&_13$$7, "strval", &_14, 3, &n);
				zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_15$$7);
				ZVAL_LONG(&_15$$7, zephir_fast_count_int(&rowA));
				ZEPHIR_INIT_NVAR(&_16$$7);
				ZEPHIR_CONCAT_SSVSVS(&_16$$7, "The number of", " columns must be equal for all rows, ", &_13$$7, " needed but ", &_15$$7, " given.");
				ZEPHIR_CALL_METHOD(NULL, &_12$$7, "__construct", NULL, 2, &_16$$7);
				zephir_check_call_status();
				zephir_throw_exception_debug(&_12$$7, "tensor/matrix.zep", 88);
				ZEPHIR_MM_RESTORE();
				return;
			}
			if (Z_TYPE_P(&rowA) == IS_STRING) {
				ZEPHIR_INIT_NVAR(&_18$$5);
				zephir_string_to_char_array(&_18$$5, &rowA);
				_17$$5 = &_18$$5;
			} else {
				_17$$5 = &rowA;
			}
			zephir_is_iterable(_17$$5, 0, "tensor/matrix.zep", 94);
			if (Z_TYPE_P(_17$$5) == IS_ARRAY) {
				ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_17$$5), _19$$5)
				{
					ZEPHIR_INIT_NVAR(&valueA);
					ZVAL_COPY(&valueA, _19$$5);
					ZEPHIR_INIT_NVAR(&_20$$8);
					ZVAL_DOUBLE(&_20$$8, zephir_get_doubleval(&valueA));
					zephir_array_append(&flat, &_20$$8, PH_SEPARATE, "tensor/matrix.zep", 92);
				} ZEND_HASH_FOREACH_END();
			} else {
				ZEPHIR_CALL_METHOD(NULL, _17$$5, "rewind", NULL, 0);
				zephir_check_call_status();
				_22$$5 = 1;
				while (1) {
					if (_22$$5) {
						_22$$5 = 0;
					} else {
						ZEPHIR_CALL_METHOD(NULL, _17$$5, "next", NULL, 0);
						zephir_check_call_status();
					}
					ZEPHIR_CALL_METHOD(&_21$$5, _17$$5, "valid", NULL, 0);
					zephir_check_call_status();
					if (!zend_is_true(&_21$$5)) {
						break;
					}
					ZEPHIR_CALL_METHOD(&valueA, _17$$5, "current", NULL, 0);
					zephir_check_call_status();
						ZEPHIR_INIT_NVAR(&_23$$9);
						ZVAL_DOUBLE(&_23$$9, zephir_get_doubleval(&valueA));
						zephir_array_append(&flat, &_23$$9, PH_SEPARATE, "tensor/matrix.zep", 92);
				}
			}
			ZEPHIR_INIT_NVAR(&valueA);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, &a, "rewind", NULL, 0);
		zephir_check_call_status();
		_25 = 1;
		while (1) {
			if (_25) {
				_25 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, &a, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_24, &a, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_24)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowA, &a, "current", NULL, 0);
			zephir_check_call_status();
				_26$$10 = validate;
				if (_26$$10) {
					_26$$10 = !(Z_TYPE_P(&rowA) == IS_ARRAY);
				}
				if (UNEXPECTED(_26$$10)) {
					ZEPHIR_INIT_NVAR(&_27$$11);
					object_init_ex(&_27$$11, tensor_exceptions_invalidargumentexception_ce);
					ZEPHIR_INIT_NVAR(&_28$$11);
					ZEPHIR_CONCAT_SS(&_28$$11, "Matrix requires an", " array of arrays.");
					ZEPHIR_CALL_METHOD(NULL, &_27$$11, "__construct", NULL, 2, &_28$$11);
					zephir_check_call_status();
					zephir_throw_exception_debug(&_27$$11, "tensor/matrix.zep", 82);
					ZEPHIR_MM_RESTORE();
					return;
				}
				_29$$10 = validate;
				if (_29$$10) {
					_29$$10 = !ZEPHIR_IS_LONG_IDENTICAL(&n, zephir_fast_count_int(&rowA));
				}
				if (UNEXPECTED(_29$$10)) {
					ZEPHIR_INIT_NVAR(&_30$$12);
					object_init_ex(&_30$$12, tensor_exceptions_invalidargumentexception_ce);
					ZEPHIR_CALL_FUNCTION(&_31$$12, "strval", &_14, 3, &n);
					zephir_check_call_status();
					ZEPHIR_INIT_NVAR(&_32$$12);
					ZVAL_LONG(&_32$$12, zephir_fast_count_int(&rowA));
					ZEPHIR_INIT_NVAR(&_33$$12);
					ZEPHIR_CONCAT_SSVSVS(&_33$$12, "The number of", " columns must be equal for all rows, ", &_31$$12, " needed but ", &_32$$12, " given.");
					ZEPHIR_CALL_METHOD(NULL, &_30$$12, "__construct", NULL, 2, &_33$$12);
					zephir_check_call_status();
					zephir_throw_exception_debug(&_30$$12, "tensor/matrix.zep", 88);
					ZEPHIR_MM_RESTORE();
					return;
				}
				if (Z_TYPE_P(&rowA) == IS_STRING) {
					ZEPHIR_INIT_NVAR(&_35$$10);
					zephir_string_to_char_array(&_35$$10, &rowA);
					_34$$10 = &_35$$10;
				} else {
					_34$$10 = &rowA;
				}
				zephir_is_iterable(_34$$10, 0, "tensor/matrix.zep", 94);
				if (Z_TYPE_P(_34$$10) == IS_ARRAY) {
					ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_34$$10), _36$$10)
					{
						ZEPHIR_INIT_NVAR(&valueA);
						ZVAL_COPY(&valueA, _36$$10);
						ZEPHIR_INIT_NVAR(&_37$$13);
						ZVAL_DOUBLE(&_37$$13, zephir_get_doubleval(&valueA));
						zephir_array_append(&flat, &_37$$13, PH_SEPARATE, "tensor/matrix.zep", 92);
					} ZEND_HASH_FOREACH_END();
				} else {
					ZEPHIR_CALL_METHOD(NULL, _34$$10, "rewind", NULL, 0);
					zephir_check_call_status();
					_39$$10 = 1;
					while (1) {
						if (_39$$10) {
							_39$$10 = 0;
						} else {
							ZEPHIR_CALL_METHOD(NULL, _34$$10, "next", NULL, 0);
							zephir_check_call_status();
						}
						ZEPHIR_CALL_METHOD(&_38$$10, _34$$10, "valid", NULL, 0);
						zephir_check_call_status();
						if (!zend_is_true(&_38$$10)) {
							break;
						}
						ZEPHIR_CALL_METHOD(&valueA, _34$$10, "current", NULL, 0);
						zephir_check_call_status();
							ZEPHIR_INIT_NVAR(&_40$$14);
							ZVAL_DOUBLE(&_40$$14, zephir_get_doubleval(&valueA));
							zephir_array_append(&flat, &_40$$14, PH_SEPARATE, "tensor/matrix.zep", 92);
					}
				}
				ZEPHIR_INIT_NVAR(&valueA);
		}
	}
	ZEPHIR_INIT_NVAR(&rowA);
	ZEPHIR_INIT_VAR(&buffer);
	tensor_buffer_from_array(&buffer, &flat);
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_41);
	object_init_ex(&_41, tensor_tensorbuffer_ce);
	ZEPHIR_CALL_METHOD(NULL, &_41, "__construct", &_2, 14, &buffer);
	zephir_check_call_status();
	ZVAL_LONG(&_42, rows);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_41, &_42, &n);
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
	zval *n_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, _11, _10$$5;
	zend_long n, ZEPHIR_LAST_CALL_STATUS, i = 0, j = 0, _5, _6, _8$$4, _9$$4;

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_11);
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
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", NULL, 3, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SSVS(&_3$$3, "N must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/matrix.zep", 112);
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
					zephir_array_append(&rowA, &_10$$5, PH_SEPARATE, "tensor/matrix.zep", 124);
				}
			}
			zephir_array_append(&a, &rowA, PH_SEPARATE, "tensor/matrix.zep", 127);
		}
	}
	ZVAL_BOOL(&_11, 0);
	ZEPHIR_RETURN_CALL_SELF("fromArray", NULL, 0, &a, &_11);
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
	zval *elements_param = NULL, _0, _8, _7$$4;
	zval elements, a, rowA;

	ZVAL_UNDEF(&elements);
	ZVAL_UNDEF(&a);
	ZVAL_UNDEF(&rowA);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_7$$4);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		ZEPHIR_Z_PARAM_ARRAY(elements, elements_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &elements_param);
	zephir_get_arrval(&elements, elements_param);
	n = zephir_fast_count_int(&elements);
	ZEPHIR_CALL_FUNCTION(&_0, "array_values", NULL, 28, &elements);
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
						zephir_array_fetch_long(&_7$$4, &elements, i, PH_NOISY, "tensor/matrix.zep", 181);
					} else {
						ZEPHIR_INIT_NVAR(&_7$$4);
						ZVAL_DOUBLE(&_7$$4, 0.0);
					}
					zephir_array_append(&rowA, &_7$$4, PH_SEPARATE, "tensor/matrix.zep", 181);
				}
			}
			zephir_array_append(&a, &rowA, PH_SEPARATE, "tensor/matrix.zep", 184);
		}
	}
	ZVAL_BOOL(&_8, 0);
	ZEPHIR_RETURN_CALL_SELF("fromArray", NULL, 0, &a, &_8);
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
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", &_3, 3, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_4$$3);
		ZEPHIR_CONCAT_SSVS(&_4$$3, "M must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_4$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/matrix.zep", 203);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(n < 1)) {
		ZEPHIR_INIT_VAR(&_5$$4);
		object_init_ex(&_5$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_6$$4, n);
		ZEPHIR_CALL_FUNCTION(&_7$$4, "strval", &_3, 3, &_6$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_8$$4);
		ZEPHIR_CONCAT_SSVS(&_8$$4, "N must be", " greater than 0, ", &_7$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 2, &_8$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/matrix.zep", 208);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZVAL_LONG(&_9, 0);
	ZVAL_LONG(&_10, n);
	ZVAL_DOUBLE(&_11, value);
	ZEPHIR_CALL_FUNCTION(&_12, "array_fill", &_13, 4, &_9, &_10, &_11);
	zephir_check_call_status();
	ZVAL_LONG(&_9, 0);
	ZVAL_LONG(&_10, m);
	ZEPHIR_CALL_FUNCTION(&_14, "array_fill", &_13, 4, &_9, &_10, &_12);
	zephir_check_call_status();
	ZVAL_BOOL(&_9, 0);
	ZEPHIR_RETURN_CALL_SELF("fromArray", NULL, 0, &_14, &_9);
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
	zval *m_param = NULL, *n_param = NULL, _0$$3, _1$$3, _2$$3, _4$$3, _5$$4, _6$$4, _7$$4, _8$$4, _9, _13, _10$$6, _12$$6;
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
	ZVAL_UNDEF(&_13);
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
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", &_3, 3, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_4$$3);
		ZEPHIR_CONCAT_SSVS(&_4$$3, "M must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_4$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/matrix.zep", 226);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(n < 1)) {
		ZEPHIR_INIT_VAR(&_5$$4);
		object_init_ex(&_5$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_6$$4, n);
		ZEPHIR_CALL_FUNCTION(&_7$$4, "strval", &_3, 3, &_6$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_8$$4);
		ZEPHIR_CONCAT_SSVS(&_8$$4, "N must be", " greater than 0, ", &_7$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 2, &_8$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/matrix.zep", 231);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&a);
	array_init(&a);
	ZEPHIR_INIT_VAR(&rowA);
	array_init(&rowA);
	ZEPHIR_CALL_FUNCTION(&_9, "getrandmax", NULL, 6);
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
			ZEPHIR_CALL_FUNCTION(&_10$$6, "rand", &_11, 7);
			zephir_check_call_status();
			ZEPHIR_INIT_NVAR(&_12$$6);
			ZVAL_DOUBLE(&_12$$6, zephir_safe_div_zval_long(&_10$$6, max));
			zephir_array_append(&rowA, &_12$$6, PH_SEPARATE, "tensor/matrix.zep", 243);
		}
		zephir_array_append(&a, &rowA, PH_SEPARATE, "tensor/matrix.zep", 246);
	}
	ZVAL_BOOL(&_13, 0);
	ZEPHIR_RETURN_CALL_SELF("fromArray", NULL, 0, &a, &_13);
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
	zval *m_param = NULL, *n_param = NULL, _0$$3, _1$$3, _2$$3, _4$$3, _5$$4, _6$$4, _7$$4, _8$$4, _9, _22, _10$$6, _12$$7, _14$$7, _15$$7, _17$$7, _18$$7, _19$$7, _20$$7, _21$$8;
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
	ZVAL_UNDEF(&_22);
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
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", &_3, 3, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_4$$3);
		ZEPHIR_CONCAT_SSVS(&_4$$3, "M must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_4$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/matrix.zep", 264);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(n < 1)) {
		ZEPHIR_INIT_VAR(&_5$$4);
		object_init_ex(&_5$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_6$$4, n);
		ZEPHIR_CALL_FUNCTION(&_7$$4, "strval", &_3, 3, &_6$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_8$$4);
		ZEPHIR_CONCAT_SSVS(&_8$$4, "N must be", " greater than 0, ", &_7$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 2, &_8$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/matrix.zep", 269);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&a);
	array_init(&a);
	ZEPHIR_INIT_VAR(&rowA);
	array_init(&rowA);
	ZEPHIR_INIT_VAR(&extras);
	array_init(&extras);
	ZEPHIR_CALL_FUNCTION(&_9, "getrandmax", NULL, 6);
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
			ZEPHIR_CALL_FUNCTION(&_10$$6, "array_pop", &_11, 9, &extras);
			ZEPHIR_UNREF(&extras);
			zephir_check_call_status();
			zephir_array_append(&rowA, &_10$$6, PH_SEPARATE, "tensor/matrix.zep", 284);
		}
		while (1) {
			if (!(zephir_fast_count_int(&rowA) < n)) {
				break;
			}
			ZEPHIR_CALL_FUNCTION(&_12$$7, "rand", &_13, 7);
			zephir_check_call_status();
			ZVAL_DOUBLE(&_14$$7, zephir_safe_div_zval_long(&_12$$7, max));
			ZEPHIR_CALL_FUNCTION(&_15$$7, "log", &_16, 8, &_14$$7);
			zephir_check_call_status();
			ZVAL_DOUBLE(&_14$$7, (-2.0 * zephir_get_numberval(&_15$$7)));
			r =  (sqrt((-2.0 * zephir_get_numberval(&_15$$7))));
			ZEPHIR_CALL_FUNCTION(&_17$$7, "rand", &_13, 7);
			zephir_check_call_status();
			phi =  ((zephir_safe_div_zval_long(&_17$$7, max) * 6.28318530718));
			ZVAL_DOUBLE(&_18$$7, phi);
			ZEPHIR_INIT_NVAR(&_19$$7);
			ZVAL_DOUBLE(&_19$$7, (r * sin(phi)));
			zephir_array_append(&rowA, &_19$$7, PH_SEPARATE, "tensor/matrix.zep", 292);
			ZVAL_DOUBLE(&_20$$7, phi);
			ZEPHIR_INIT_NVAR(&_19$$7);
			ZVAL_DOUBLE(&_19$$7, (r * cos(phi)));
			zephir_array_append(&rowA, &_19$$7, PH_SEPARATE, "tensor/matrix.zep", 293);
		}
		if (zephir_fast_count_int(&rowA) > n) {
			ZEPHIR_MAKE_REF(&rowA);
			ZEPHIR_CALL_FUNCTION(&_21$$8, "array_pop", &_11, 9, &rowA);
			ZEPHIR_UNREF(&rowA);
			zephir_check_call_status();
			zephir_array_append(&extras, &_21$$8, PH_SEPARATE, "tensor/matrix.zep", 297);
		}
		zephir_array_append(&a, &rowA, PH_SEPARATE, "tensor/matrix.zep", 300);
	}
	ZVAL_BOOL(&_22, 0);
	ZEPHIR_RETURN_CALL_SELF("fromArray", NULL, 0, &a, &_22);
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
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", &_3, 3, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_4$$3);
		ZEPHIR_CONCAT_SSVS(&_4$$3, "M must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_4$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/matrix.zep", 319);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(n < 1)) {
		ZEPHIR_INIT_VAR(&_5$$4);
		object_init_ex(&_5$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_6$$4, n);
		ZEPHIR_CALL_FUNCTION(&_7$$4, "strval", &_3, 3, &_6$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_8$$4);
		ZEPHIR_CONCAT_SSVS(&_8$$4, "N must be", " greater than 0, ", &_7$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 2, &_8$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/matrix.zep", 324);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(lambda < 0.0)) {
		ZEPHIR_INIT_VAR(&_9$$5);
		object_init_ex(&_9$$5, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_DOUBLE(&_10$$5, lambda);
		ZEPHIR_CALL_FUNCTION(&_11$$5, "strval", &_3, 3, &_10$$5);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_12$$5);
		ZEPHIR_CONCAT_SSVS(&_12$$5, "Lambda must be", " greater than or equal to 0, ", &_11$$5, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_9$$5, "__construct", NULL, 2, &_12$$5);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_9$$5, "tensor/matrix.zep", 329);
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
	ZEPHIR_CALL_FUNCTION(&_17, "exp", NULL, 10, &_16);
	zephir_check_call_status();
	l =  (zephir_get_doubleval(&_17));
	ZEPHIR_CALL_FUNCTION(&_18, "getrandmax", NULL, 6);
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
				ZEPHIR_CALL_FUNCTION(&_19$$9, "rand", &_20, 7);
				zephir_check_call_status();
				p *=  (zephir_safe_div_zval_long(&_19$$9, max));
			}
			ZEPHIR_INIT_NVAR(&_21$$8);
			ZVAL_DOUBLE(&_21$$8, (k - 1.0));
			zephir_array_append(&rowA, &_21$$8, PH_SEPARATE, "tensor/matrix.zep", 358);
		}
		zephir_array_append(&a, &rowA, PH_SEPARATE, "tensor/matrix.zep", 361);
	}
	ZVAL_BOOL(&_16, 0);
	ZEPHIR_RETURN_CALL_SELF("fromArray", NULL, 0, &a, &_16);
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
	zval *m_param = NULL, *n_param = NULL, _0$$3, _1$$3, _2$$3, _4$$3, _5$$4, _6$$4, _7$$4, _8$$4, _9, _15, _10$$6, _11$$6, _12$$6, _14$$6;
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
	ZVAL_UNDEF(&_15);
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
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", &_3, 3, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_4$$3);
		ZEPHIR_CONCAT_SSVS(&_4$$3, "M must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_4$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/matrix.zep", 379);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(n < 1)) {
		ZEPHIR_INIT_VAR(&_5$$4);
		object_init_ex(&_5$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_6$$4, n);
		ZEPHIR_CALL_FUNCTION(&_7$$4, "strval", &_3, 3, &_6$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_8$$4);
		ZEPHIR_CONCAT_SSVS(&_8$$4, "N must be", " greater than 0, ", &_7$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 2, &_8$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/matrix.zep", 384);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&a);
	array_init(&a);
	ZEPHIR_INIT_VAR(&rowA);
	array_init(&rowA);
	ZEPHIR_CALL_FUNCTION(&_9, "getrandmax", NULL, 6);
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
			ZEPHIR_CALL_FUNCTION(&_12$$6, "rand", &_13, 7, &_10$$6, &_11$$6);
			zephir_check_call_status();
			ZEPHIR_INIT_NVAR(&_14$$6);
			ZVAL_DOUBLE(&_14$$6, zephir_safe_div_zval_long(&_12$$6, max));
			zephir_array_append(&rowA, &_14$$6, PH_SEPARATE, "tensor/matrix.zep", 396);
		}
		zephir_array_append(&a, &rowA, PH_SEPARATE, "tensor/matrix.zep", 399);
	}
	ZVAL_BOOL(&_15, 0);
	ZEPHIR_RETURN_CALL_SELF("fromArray", NULL, 0, &a, &_15);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Construct a matrix from a single TensorBuffer holding the elements in
 * row-major order together with the target dimensionality.
 *
 * @param \Tensor\TensorBuffer a
 * @param int m
 * @param int n
 */
PHP_METHOD(Tensor_Matrix, __construct)
{
	zend_long m, n;
	zval *a, a_sub, *m_param = NULL, *n_param = NULL, _0;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&a_sub);
	ZVAL_UNDEF(&_0);
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

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_OBJECT_OF_CLASS(a, zephir_get_internal_ce(SL("tensor\\tensorbuffer")))
		Z_PARAM_LONG(m)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(3, 0, &a, &m_param, &n_param);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_0, 14, a);
	ZVAL_UNDEF(&_0);
	ZVAL_LONG(&_0, m);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_1, 15, &_0);
	ZVAL_UNDEF(&_0);
	ZVAL_LONG(&_0, n);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_2, 16, &_0);
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
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
	zephir_array_fast_append(return_value, &_0);
	ZEPHIR_OBS_NVAR(&_0);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC);
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
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
	zephir_cast_to_string(&_1, &_0);
	zephir_memory_observe(&_2);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC);
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
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
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
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
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
 * @throws \InvalidArgumentException
 */
PHP_METHOD(Tensor_Matrix, rowAsVector)
{
	zval _3$$3;
	zend_bool _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *index_param = NULL, _1, _4, _5, _6, _7, _8, _2$$3;
	zend_long index, ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
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
		Z_PARAM_LONG(index)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &index_param);
	_0 = index < 0;
	if (!(_0)) {
		zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
		_0 = ZEPHIR_LE_LONG(&_1, index);
	}
	if (UNEXPECTED(_0)) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SS(&_3$$3, "Row offset out of", " bounds.");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 491);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, tensor_vector_ce);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_8, (index * (zend_long) zephir_get_numberval(&_6)));
	ZEPHIR_CALL_METHOD(&_5, &_4, "slice", NULL, 0, &_8, &_7);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 5, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return a column as a vector from the matrix.
 *
 * @param int index
 * @return \Tensor\ColumnVector
 * @throws \InvalidArgumentException
 */
PHP_METHOD(Tensor_Matrix, columnAsVector)
{
	zval _3$$3;
	zend_bool _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *index_param = NULL, _1, _4, _5, _6, _7, _8, _2$$3;
	zend_long index, ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
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
		Z_PARAM_LONG(index)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &index_param);
	_0 = index < 0;
	if (!(_0)) {
		zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
		_0 = ZEPHIR_LE_LONG(&_1, index);
	}
	if (UNEXPECTED(_0)) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SS(&_3$$3, "Column offset out of", " bounds.");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 508);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, tensor_columnvector_ce);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_8, index);
	ZEPHIR_CALL_METHOD(&_5, &_4, "sliceStrided", NULL, 0, &_8, &_6, &_7);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 5, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the diagonal elements of a square matrix as a vector.
 *
 * @return \Tensor\Vector
 * @throws \Tensor\Exceptions\InvalidArgumentException
 */
PHP_METHOD(Tensor_Matrix, diagonalAsVector)
{
	zval _0, _4, _5, _6, _7, _8, _9, _1$$3, _2$$3, _3$$3;
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
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("n", 1, 1);
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
		ZEPHIR_CALL_METHOD(NULL, &_1$$3, "__construct", NULL, 2, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_1$$3, "tensor/matrix.zep", 524);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, tensor_vector_ce);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_8, 0);
	ZVAL_LONG(&_9, (zephir_get_numberval(&_7) + 1));
	ZEPHIR_CALL_METHOD(&_5, &_4, "sliceStrided", NULL, 0, &_8, &_6, &_9);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 5, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the elements of the matrix as a vector taken in row-major order.
 *
 * @return \Tensor\Vector
 */
PHP_METHOD(Tensor_Matrix, asVector)
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

	object_init_ex(return_value, tensor_vector_ce);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 5, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the underlying TensorBuffer of the matrix.
 *
 * @return \Tensor\TensorBuffer
 */
PHP_METHOD(Tensor_Matrix, asTensorBuffer)
{

	RETURN_MEMBER(getThis(), "a");
}

/**
 * Return the matrix as an array of arrays.
 *
 * @return array[]
 */
PHP_METHOD(Tensor_Matrix, asArray)
{
	zend_bool _9;
	zval b;
	zval rowBuffer, _0, _1, _2, _3, *_4, _5, *_6, _8, _7$$4, _10$$5;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&rowBuffer);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_10$$5);
	ZVAL_UNDEF(&b);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&b);
	array_init(&b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(ZEPHIR_LT_LONG(&_0, 1))) {
		array_init(return_value);
		RETURN_MM();
	}
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_2, &_1, "split", NULL, 0, &_3);
	zephir_check_call_status();
	if (Z_TYPE_P(&_2) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_5);
		zephir_string_to_char_array(&_5, &_2);
		_4 = &_5;
	} else {
		_4 = &_2;
	}
	zephir_is_iterable(_4, 0, "tensor/matrix.zep", 569);
	if (Z_TYPE_P(_4) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_4), _6)
		{
			ZEPHIR_INIT_NVAR(&rowBuffer);
			ZVAL_COPY(&rowBuffer, _6);
			ZEPHIR_CALL_METHOD(&_7$$4, &rowBuffer, "toArray", NULL, 0);
			zephir_check_call_status();
			zephir_array_append(&b, &_7$$4, PH_SEPARATE, "tensor/matrix.zep", 566);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _4, "rewind", NULL, 0);
		zephir_check_call_status();
		_9 = 1;
		while (1) {
			if (_9) {
				_9 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _4, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_8, _4, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_8)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowBuffer, _4, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_CALL_METHOD(&_10$$5, &rowBuffer, "toArray", NULL, 0);
				zephir_check_call_status();
				zephir_array_append(&b, &_10$$5, PH_SEPARATE, "tensor/matrix.zep", 566);
		}
	}
	ZEPHIR_INIT_NVAR(&rowBuffer);
	RETURN_CTOR(&b);
}

/**
 * Return each row of the matrix as a TensorBuffer.
 *
 * @return \Tensor\TensorBuffer[]
 */
PHP_METHOD(Tensor_Matrix, asRowBuffers)
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
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(ZEPHIR_LT_LONG(&_0, 1))) {
		array_init(return_value);
		RETURN_MM();
	}
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_METHOD(&_1, "split", NULL, 0, &_2);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the rows of the matrix as an array of Vector objects.
 *
 * @return \Tensor\Vector[]
 */
PHP_METHOD(Tensor_Matrix, asRowVectors)
{
	zend_bool _10;
	zval b;
	zval rowBuffer, _0, _1, _2, _3, *_4, _5, *_6, _9, _7$$4, _11$$5;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_8 = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&rowBuffer);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_11$$5);
	ZVAL_UNDEF(&b);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&b);
	array_init(&b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(ZEPHIR_LT_LONG(&_0, 1))) {
		array_init(return_value);
		RETURN_MM();
	}
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_2, &_1, "split", NULL, 0, &_3);
	zephir_check_call_status();
	if (Z_TYPE_P(&_2) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_5);
		zephir_string_to_char_array(&_5, &_2);
		_4 = &_5;
	} else {
		_4 = &_2;
	}
	zephir_is_iterable(_4, 0, "tensor/matrix.zep", 605);
	if (Z_TYPE_P(_4) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_4), _6)
		{
			ZEPHIR_INIT_NVAR(&rowBuffer);
			ZVAL_COPY(&rowBuffer, _6);
			ZEPHIR_INIT_NVAR(&_7$$4);
			object_init_ex(&_7$$4, tensor_vector_ce);
			ZEPHIR_CALL_METHOD(NULL, &_7$$4, "__construct", &_8, 5, &rowBuffer);
			zephir_check_call_status();
			zephir_array_append(&b, &_7$$4, PH_SEPARATE, "tensor/matrix.zep", 602);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _4, "rewind", NULL, 0);
		zephir_check_call_status();
		_10 = 1;
		while (1) {
			if (_10) {
				_10 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _4, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_9, _4, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_9)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowBuffer, _4, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_11$$5);
				object_init_ex(&_11$$5, tensor_vector_ce);
				ZEPHIR_CALL_METHOD(NULL, &_11$$5, "__construct", &_8, 5, &rowBuffer);
				zephir_check_call_status();
				zephir_array_append(&b, &_11$$5, PH_SEPARATE, "tensor/matrix.zep", 602);
		}
	}
	ZEPHIR_INIT_NVAR(&rowBuffer);
	RETURN_CTOR(&b);
}

/**
 * Return each column of the matrix as a TensorBuffer.
 *
 * @return \Tensor\TensorBuffer[]
 */
PHP_METHOD(Tensor_Matrix, asColumnBuffers)
{
	zend_bool _2;
	zval b;
	zval i, _0, _1, _5$$4, _6$$4, _7$$4, _8$$4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS, _3, _4;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&i);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_6$$4);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_8$$4);
	ZVAL_UNDEF(&b);
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
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&b);
	array_init(&b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(ZEPHIR_LT_LONG(&_0, 1))) {
		array_init(return_value);
		RETURN_MM();
	}
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	_4 = (zephir_get_numberval(&_1) - 1);
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
			ZEPHIR_INIT_NVAR(&i);
			ZVAL_LONG(&i, _3);
			zephir_read_property_cached(&_5$$4, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
			zephir_read_property_cached(&_7$$4, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
			zephir_read_property_cached(&_8$$4, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
			ZEPHIR_CALL_METHOD(&_6$$4, &_5$$4, "sliceStrided", NULL, 0, &i, &_7$$4, &_8$$4);
			zephir_check_call_status();
			zephir_array_append(&b, &_6$$4, PH_SEPARATE, "tensor/matrix.zep", 624);
		}
	}
	RETURN_CTOR(&b);
}

/**
 * Return the columns of the matrix as an array of ColumnVector objects.
 *
 * @return \Tensor\ColumnVector[]
 */
PHP_METHOD(Tensor_Matrix, asColumnVectors)
{
	zend_bool _8;
	zval b;
	zval columnBuffer, _0, _1, *_2, _3, *_4, _7, _5$$4, _9$$5;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_6 = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&columnBuffer);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_9$$5);
	ZVAL_UNDEF(&b);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&b);
	array_init(&b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(ZEPHIR_LT_LONG(&_0, 1))) {
		array_init(return_value);
		RETURN_MM();
	}
	ZEPHIR_CALL_METHOD(&_1, this_ptr, "asColumnBuffers", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_1) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_3);
		zephir_string_to_char_array(&_3, &_1);
		_2 = &_3;
	} else {
		_2 = &_1;
	}
	zephir_is_iterable(_2, 0, "tensor/matrix.zep", 649);
	if (Z_TYPE_P(_2) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_2), _4)
		{
			ZEPHIR_INIT_NVAR(&columnBuffer);
			ZVAL_COPY(&columnBuffer, _4);
			ZEPHIR_INIT_NVAR(&_5$$4);
			object_init_ex(&_5$$4, tensor_columnvector_ce);
			ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", &_6, 5, &columnBuffer);
			zephir_check_call_status();
			zephir_array_append(&b, &_5$$4, PH_SEPARATE, "tensor/matrix.zep", 646);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _2, "rewind", NULL, 0);
		zephir_check_call_status();
		_8 = 1;
		while (1) {
			if (_8) {
				_8 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _2, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_7, _2, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_7)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&columnBuffer, _2, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_9$$5);
				object_init_ex(&_9$$5, tensor_columnvector_ce);
				ZEPHIR_CALL_METHOD(NULL, &_9$$5, "__construct", &_6, 5, &columnBuffer);
				zephir_check_call_status();
				zephir_array_append(&b, &_9$$5, PH_SEPARATE, "tensor/matrix.zep", 646);
		}
	}
	ZEPHIR_INIT_NVAR(&columnBuffer);
	RETURN_CTOR(&b);
}

/**
 * Return the rows of the matrix as an array of Vector objects.
 *
 * @return \Tensor\Vector[]
 */
PHP_METHOD(Tensor_Matrix, asVectors)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_RETURN_CALL_METHOD(this_ptr, "asRowVectors", NULL, 0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the elements of the matrix as a vector taken in row-major order.
 *
 * @return \Tensor\Vector
 */
PHP_METHOD(Tensor_Matrix, flatten)
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

	object_init_ex(return_value, tensor_vector_ce);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 5, &_0);
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
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *callback, callback_sub, _0, _1, _2, _3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&callback_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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
		Z_PARAM_ZVAL(callback)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &callback);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, &_0, "map", NULL, 0, callback);
	zephir_check_call_status();
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_1, &_2, &_3);
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
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	double initial;
	zval *callback, callback_sub, *initial_param = NULL, _0, _1, _2;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&callback_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

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
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, &_0, "toArray", NULL, 0);
	zephir_check_call_status();
	ZVAL_DOUBLE(&_2, initial);
	ZEPHIR_RETURN_CALL_FUNCTION("array_reduce", NULL, 17, &_1, callback, &_2);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Transpose the matrix i.e row become columns and columns become rows.
 *
 * @return self
 */
PHP_METHOD(Tensor_Matrix, transpose)
{
	zval col, cols, _2, _3, _4, _5, _0$$3, _1$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&col);
	ZVAL_UNDEF(&cols);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_CALL_METHOD(&cols, this_ptr, "asColumnBuffers", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(zephir_fast_count_int(&cols) < 1)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		array_init(&_0$$3);
		ZVAL_BOOL(&_1$$3, 0);
		ZEPHIR_RETURN_CALL_SELF("fromArray", NULL, 0, &_0$$3, &_1$$3);
		zephir_check_call_status();
		RETURN_MM();
	}
	zephir_memory_observe(&col);
	zephir_array_fetch_long(&col, &cols, 0, PH_NOISY, "tensor/matrix.zep", 714);
	ZVAL_LONG(&_3, 1);
	ZEPHIR_CALL_FUNCTION(&_4, "array_slice", NULL, 29, &cols, &_3);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_2, &col, "concat", NULL, 0, &_4);
	zephir_check_call_status();
	ZEPHIR_CPY_WRT(&col, &_2);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &col, &_3, &_5);
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
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
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
		ZEPHIR_CALL_METHOD(NULL, &_1$$3, "__construct", NULL, 2, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_1$$3, "tensor/matrix.zep", 731);
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
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 30, &_6$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/matrix.zep", 736);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	tensor_inverse(&result, &_7, &_8);
	if (Z_TYPE_P(&result) == IS_NULL) {
		ZEPHIR_INIT_VAR(&_9$$5);
		object_init_ex(&_9$$5, tensor_exceptions_runtimeexception_ce);
		ZEPHIR_INIT_VAR(&_10$$5);
		ZEPHIR_CONCAT_SS(&_10$$5, "Failed to compute the inverse", " of a singular matrix.");
		ZEPHIR_CALL_METHOD(NULL, &_9$$5, "__construct", NULL, 30, &_10$$5);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_9$$5, "tensor/matrix.zep", 743);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_12, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_11, &_12);
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
	zval _4$$3;
	zval result, _0, _1, _2, _5, _6, _3$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
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

	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	tensor_pseudoinverse(&result, &_0, &_1, &_2);
	if (Z_TYPE_P(&result) == IS_NULL) {
		ZEPHIR_INIT_VAR(&_3$$3);
		object_init_ex(&_3$$3, tensor_exceptions_runtimeexception_ce);
		ZEPHIR_INIT_VAR(&_4$$3);
		ZEPHIR_CONCAT_SS(&_4$$3, "Failed to compute the pseudoinverse", " of the matrix.");
		ZEPHIR_CALL_METHOD(NULL, &_3$$3, "__construct", NULL, 30, &_4$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_3$$3, "tensor/matrix.zep", 760);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_5, &_6);
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
		ZEPHIR_CALL_METHOD(NULL, &_1$$3, "__construct", NULL, 2, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_1$$3, "tensor/matrix.zep", 776);
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
	ZEPHIR_CALL_FUNCTION(&_8, "pow", NULL, 18, &_7, &_6);
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
	zval rref, _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&rref);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_CALL_METHOD(&_0, this_ptr, "rref", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&rref, &_0, "a", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, &rref, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_2, &rref, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_3, &rref, "n", NULL, 0);
	zephir_check_call_status();
	tensor_rank(return_value, &_1, &_2, &_3);
	RETURN_MM();
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
	ZEPHIR_CALL_FUNCTION(&_2, "min", NULL, 31, &_1);
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
		_zephir_prop_0 = zend_string_init("a", 1, 1);
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
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	tensor_is_symmetric(return_value, &_1, &_2);
	RETURN_MM();
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
	zval *b, b_sub, _0, _1, result, _8, _9, _10, _11, _12, _13, _14, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&result);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "m", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A requires ", &_4$$3, " rows but Matrix B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 844);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_12, b, "n", NULL, 0);
	zephir_check_call_status();
	tensor_matmul(&result, &_8, &_9, &_10, &_11, &_12);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_14, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_13, &_14);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "size", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "size", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 864);
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
	zend_long stride, ZEPHIR_LAST_CALL_STATUS, outM, outN;
	zval *b, b_sub, *stride_param = NULL, _0, _1, _3, _4, result, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _5$$3, _7$$4, _8$$4, _9$$4, _10$$4;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_15);
	ZVAL_UNDEF(&_16);
	ZVAL_UNDEF(&_17);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19);
	ZVAL_UNDEF(&_20);
	ZVAL_UNDEF(&_21);
	ZVAL_UNDEF(&_22);
	ZVAL_UNDEF(&_23);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_8$$4);
	ZVAL_UNDEF(&_9$$4);
	ZVAL_UNDEF(&_10$$4);
	ZVAL_UNDEF(&_6$$3);
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

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
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
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	_2 = ZEPHIR_GT(&_0, &_1);
	if (!(_2)) {
		ZEPHIR_CALL_METHOD(&_3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
		_2 = ZEPHIR_GT(&_3, &_4);
	}
	if (UNEXPECTED(_2)) {
		ZEPHIR_INIT_VAR(&_5$$3);
		object_init_ex(&_5$$3, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_INIT_VAR(&_6$$3);
		ZEPHIR_CONCAT_SS(&_6$$3, "Matrix B cannot be", " larger than Matrix A.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$3, "__construct", NULL, 2, &_6$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$3, "tensor/matrix.zep", 882);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(stride < 1)) {
		ZEPHIR_INIT_VAR(&_7$$4);
		object_init_ex(&_7$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_8$$4, stride);
		ZEPHIR_CALL_FUNCTION(&_9$$4, "strval", NULL, 3, &_8$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_10$$4);
		ZEPHIR_CONCAT_SSVS(&_10$$4, "Stride cannot be", " less than 1, ", &_9$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_7$$4, "__construct", NULL, 2, &_10$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_7$$4, "tensor/matrix.zep", 887);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_12, b, _zephir_prop_2, 0, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_15, b, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_16, b, "n", NULL, 0);
	zephir_check_call_status();
	ZVAL_LONG(&_17, stride);
	tensor_convolve_2d(&result, &_11, &_12, &_17, &_13, &_14, &_15, &_16);
	zephir_read_property_cached(&_18, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_19, ((zephir_get_numberval(&_18) + stride) - 1));
	ZVAL_LONG(&_20, stride);
	ZEPHIR_CALL_FUNCTION(&_21, "intdiv", NULL, 19, &_19, &_20);
	zephir_check_call_status();
	outM = zephir_get_intval(&_21);
	zephir_read_property_cached(&_19, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_20, ((zephir_get_numberval(&_19) + stride) - 1));
	ZVAL_LONG(&_22, stride);
	ZEPHIR_CALL_FUNCTION(&_23, "intdiv", NULL, 19, &_20, &_22);
	zephir_check_call_status();
	outN = zephir_get_intval(&_23);
	object_init_ex(return_value, tensor_matrix_ce);
	ZVAL_LONG(&_20, outM);
	ZVAL_LONG(&_22, outN);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_20, &_22);
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
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1031);
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
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1064);
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
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1098);
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
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1132);
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
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1166);
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
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1200);
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
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1234);
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
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1268);
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
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1302);
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
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1336);
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
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1370);
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
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/matrix.zep", 1404);
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

	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_abs(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_sqrt(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_exp(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_expm1(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
	zval *base_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, _4, _5, _6, _7, _8;
	double base;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
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
		object_init_ex(return_value, tensor_matrix_ce);
		ZEPHIR_INIT_VAR(&_0$$3);
		zephir_read_property_cached(&_1$$3, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
		tensor_log(&_0$$3, &_1$$3);
		zephir_read_property_cached(&_2$$3, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
		ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0$$3, &_2$$3, &_3$$3);
		zephir_check_call_status();
		RETURN_MM();
	}
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_4);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_6, base);
	tensor_log_base(&_4, &_5, &_6);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_4, &_7, &_8);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_log1p(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_sin(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_asin(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_cos(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_acos(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_tan(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_atan(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_rad2deg(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_deg2rad(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
	zend_bool _8;
	zval b;
	zval rowBuffer, _0, _1, _2, *_3, _4, *_5, _7, _6$$3, _9$$4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&rowBuffer);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_9$$4);
	ZVAL_UNDEF(&b);
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

	ZEPHIR_INIT_VAR(&b);
	array_init(&b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, &_0, "split", NULL, 0, &_2);
	zephir_check_call_status();
	if (Z_TYPE_P(&_1) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_4);
		zephir_string_to_char_array(&_4, &_1);
		_3 = &_4;
	} else {
		_3 = &_1;
	}
	zephir_is_iterable(_3, 0, "tensor/matrix.zep", 1590);
	if (Z_TYPE_P(_3) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_3), _5)
		{
			ZEPHIR_INIT_NVAR(&rowBuffer);
			ZVAL_COPY(&rowBuffer, _5);
			ZEPHIR_CALL_METHOD(&_6$$3, &rowBuffer, "sum", NULL, 0);
			zephir_check_call_status();
			zephir_array_append(&b, &_6$$3, PH_SEPARATE, "tensor/matrix.zep", 1587);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _3, "rewind", NULL, 0);
		zephir_check_call_status();
		_8 = 1;
		while (1) {
			if (_8) {
				_8 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _3, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_7, _3, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_7)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowBuffer, _3, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_CALL_METHOD(&_9$$4, &rowBuffer, "sum", NULL, 0);
				zephir_check_call_status();
				zephir_array_append(&b, &_9$$4, PH_SEPARATE, "tensor/matrix.zep", 1587);
		}
	}
	ZEPHIR_INIT_NVAR(&rowBuffer);
	object_init_ex(return_value, tensor_columnvector_ce);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 5, &b);
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
	zend_bool _8;
	zval b;
	zval rowBuffer, _0, _1, _2, *_3, _4, *_5, _7, _6$$3, _9$$4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&rowBuffer);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_9$$4);
	ZVAL_UNDEF(&b);
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

	ZEPHIR_INIT_VAR(&b);
	array_init(&b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, &_0, "split", NULL, 0, &_2);
	zephir_check_call_status();
	if (Z_TYPE_P(&_1) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_4);
		zephir_string_to_char_array(&_4, &_1);
		_3 = &_4;
	} else {
		_3 = &_1;
	}
	zephir_is_iterable(_3, 0, "tensor/matrix.zep", 1608);
	if (Z_TYPE_P(_3) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_3), _5)
		{
			ZEPHIR_INIT_NVAR(&rowBuffer);
			ZVAL_COPY(&rowBuffer, _5);
			ZEPHIR_CALL_METHOD(&_6$$3, &rowBuffer, "product", NULL, 0);
			zephir_check_call_status();
			zephir_array_append(&b, &_6$$3, PH_SEPARATE, "tensor/matrix.zep", 1605);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _3, "rewind", NULL, 0);
		zephir_check_call_status();
		_8 = 1;
		while (1) {
			if (_8) {
				_8 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _3, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_7, _3, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_7)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowBuffer, _3, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_CALL_METHOD(&_9$$4, &rowBuffer, "product", NULL, 0);
				zephir_check_call_status();
				zephir_array_append(&b, &_9$$4, PH_SEPARATE, "tensor/matrix.zep", 1605);
		}
	}
	ZEPHIR_INIT_NVAR(&rowBuffer);
	object_init_ex(return_value, tensor_columnvector_ce);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 5, &b);
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
	zend_bool _8;
	zval b;
	zval rowBuffer, _0, _1, _2, *_3, _4, *_5, _7, _6$$3, _9$$4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&rowBuffer);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_9$$4);
	ZVAL_UNDEF(&b);
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

	ZEPHIR_INIT_VAR(&b);
	array_init(&b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, &_0, "split", NULL, 0, &_2);
	zephir_check_call_status();
	if (Z_TYPE_P(&_1) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_4);
		zephir_string_to_char_array(&_4, &_1);
		_3 = &_4;
	} else {
		_3 = &_1;
	}
	zephir_is_iterable(_3, 0, "tensor/matrix.zep", 1626);
	if (Z_TYPE_P(_3) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_3), _5)
		{
			ZEPHIR_INIT_NVAR(&rowBuffer);
			ZVAL_COPY(&rowBuffer, _5);
			ZEPHIR_CALL_METHOD(&_6$$3, &rowBuffer, "min", NULL, 0);
			zephir_check_call_status();
			zephir_array_append(&b, &_6$$3, PH_SEPARATE, "tensor/matrix.zep", 1623);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _3, "rewind", NULL, 0);
		zephir_check_call_status();
		_8 = 1;
		while (1) {
			if (_8) {
				_8 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _3, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_7, _3, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_7)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowBuffer, _3, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_CALL_METHOD(&_9$$4, &rowBuffer, "min", NULL, 0);
				zephir_check_call_status();
				zephir_array_append(&b, &_9$$4, PH_SEPARATE, "tensor/matrix.zep", 1623);
		}
	}
	ZEPHIR_INIT_NVAR(&rowBuffer);
	object_init_ex(return_value, tensor_columnvector_ce);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 5, &b);
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
	zend_bool _8;
	zval b;
	zval rowBuffer, _0, _1, _2, *_3, _4, *_5, _7, _6$$3, _9$$4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&rowBuffer);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_9$$4);
	ZVAL_UNDEF(&b);
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

	ZEPHIR_INIT_VAR(&b);
	array_init(&b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, &_0, "split", NULL, 0, &_2);
	zephir_check_call_status();
	if (Z_TYPE_P(&_1) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_4);
		zephir_string_to_char_array(&_4, &_1);
		_3 = &_4;
	} else {
		_3 = &_1;
	}
	zephir_is_iterable(_3, 0, "tensor/matrix.zep", 1644);
	if (Z_TYPE_P(_3) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_3), _5)
		{
			ZEPHIR_INIT_NVAR(&rowBuffer);
			ZVAL_COPY(&rowBuffer, _5);
			ZEPHIR_CALL_METHOD(&_6$$3, &rowBuffer, "max", NULL, 0);
			zephir_check_call_status();
			zephir_array_append(&b, &_6$$3, PH_SEPARATE, "tensor/matrix.zep", 1641);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _3, "rewind", NULL, 0);
		zephir_check_call_status();
		_8 = 1;
		while (1) {
			if (_8) {
				_8 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _3, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_7, _3, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_7)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowBuffer, _3, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_CALL_METHOD(&_9$$4, &rowBuffer, "max", NULL, 0);
				zephir_check_call_status();
				zephir_array_append(&b, &_9$$4, PH_SEPARATE, "tensor/matrix.zep", 1641);
		}
	}
	ZEPHIR_INIT_NVAR(&rowBuffer);
	object_init_ex(return_value, tensor_columnvector_ce);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 5, &b);
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
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
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
	zend_bool odd, _15;
	zval b;
	zval rowBuffer, median, _0, _1, _2, _3, _4, _5, *_6, _7, *_8, _14, _9$$4, _10$$5, _11$$5, _12$$5, _13$$5, _16$$7, _17$$8, _18$$8, _19$$8, _20$$8;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS, mid;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&rowBuffer);
	ZVAL_UNDEF(&median);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_9$$4);
	ZVAL_UNDEF(&_10$$5);
	ZVAL_UNDEF(&_11$$5);
	ZVAL_UNDEF(&_12$$5);
	ZVAL_UNDEF(&_13$$5);
	ZVAL_UNDEF(&_16$$7);
	ZVAL_UNDEF(&_17$$8);
	ZVAL_UNDEF(&_18$$8);
	ZVAL_UNDEF(&_19$$8);
	ZVAL_UNDEF(&_20$$8);
	ZVAL_UNDEF(&b);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&b);
	array_init(&b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_1, 2);
	ZEPHIR_CALL_FUNCTION(&_2, "intdiv", NULL, 19, &_0, &_1);
	zephir_check_call_status();
	mid = zephir_get_intval(&_2);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	odd = zephir_safe_mod_zval_long(&_1, 2) == 1;
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_4, &_3, "split", NULL, 0, &_5);
	zephir_check_call_status();
	if (Z_TYPE_P(&_4) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_7);
		zephir_string_to_char_array(&_7, &_4);
		_6 = &_7;
	} else {
		_6 = &_4;
	}
	zephir_is_iterable(_6, 0, "tensor/matrix.zep", 1683);
	if (Z_TYPE_P(_6) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_6), _8)
		{
			ZEPHIR_INIT_NVAR(&rowBuffer);
			ZVAL_COPY(&rowBuffer, _8);
			ZEPHIR_CALL_METHOD(NULL, &rowBuffer, "sort", NULL, 0);
			zephir_check_call_status();
			if (odd) {
				ZVAL_LONG(&_9$$4, mid);
				ZEPHIR_CALL_METHOD(&median, &rowBuffer, "get", NULL, 0, &_9$$4);
				zephir_check_call_status();
			} else {
				ZVAL_LONG(&_11$$5, (mid - 1));
				ZEPHIR_CALL_METHOD(&_10$$5, &rowBuffer, "get", NULL, 0, &_11$$5);
				zephir_check_call_status();
				ZVAL_LONG(&_11$$5, mid);
				ZEPHIR_CALL_METHOD(&_12$$5, &rowBuffer, "get", NULL, 0, &_11$$5);
				zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_13$$5);
				zephir_add_function(&_13$$5, &_10$$5, &_12$$5);
				ZEPHIR_INIT_NVAR(&median);
				ZVAL_DOUBLE(&median, zephir_safe_div_zval_double(&_13$$5, 2.0));
			}
			zephir_array_append(&b, &median, PH_SEPARATE, "tensor/matrix.zep", 1680);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _6, "rewind", NULL, 0);
		zephir_check_call_status();
		_15 = 1;
		while (1) {
			if (_15) {
				_15 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _6, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_14, _6, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_14)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowBuffer, _6, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_CALL_METHOD(NULL, &rowBuffer, "sort", NULL, 0);
				zephir_check_call_status();
				if (odd) {
					ZVAL_LONG(&_16$$7, mid);
					ZEPHIR_CALL_METHOD(&median, &rowBuffer, "get", NULL, 0, &_16$$7);
					zephir_check_call_status();
				} else {
					ZVAL_LONG(&_18$$8, (mid - 1));
					ZEPHIR_CALL_METHOD(&_17$$8, &rowBuffer, "get", NULL, 0, &_18$$8);
					zephir_check_call_status();
					ZVAL_LONG(&_18$$8, mid);
					ZEPHIR_CALL_METHOD(&_19$$8, &rowBuffer, "get", NULL, 0, &_18$$8);
					zephir_check_call_status();
					ZEPHIR_INIT_NVAR(&_20$$8);
					zephir_add_function(&_20$$8, &_17$$8, &_19$$8);
					ZEPHIR_INIT_NVAR(&median);
					ZVAL_DOUBLE(&median, zephir_safe_div_zval_double(&_20$$8, 2.0));
				}
				zephir_array_append(&b, &median, PH_SEPARATE, "tensor/matrix.zep", 1680);
		}
	}
	ZEPHIR_INIT_NVAR(&rowBuffer);
	object_init_ex(return_value, tensor_columnvector_ce);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 5, &b);
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
	zend_bool _0, _22;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS, xHat;
	zval *q_param = NULL, rowBuffer, _5, _6, _7, _8, *_9, _10, *_11, _21, _1$$3, _2$$3, _3$$3, _4$$3, _12$$4, _17$$4, _18$$4, _19$$4, _20$$4, _13$$5, _14$$5, _15$$5, _16$$5, _23$$6, _28$$6, _29$$6, _30$$6, _31$$6, _24$$7, _25$$7, _26$$7, _27$$7;
	double q, t = 0, x, remainder;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&rowBuffer);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_21);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_12$$4);
	ZVAL_UNDEF(&_17$$4);
	ZVAL_UNDEF(&_18$$4);
	ZVAL_UNDEF(&_19$$4);
	ZVAL_UNDEF(&_20$$4);
	ZVAL_UNDEF(&_13$$5);
	ZVAL_UNDEF(&_14$$5);
	ZVAL_UNDEF(&_15$$5);
	ZVAL_UNDEF(&_16$$5);
	ZVAL_UNDEF(&_23$$6);
	ZVAL_UNDEF(&_28$$6);
	ZVAL_UNDEF(&_29$$6);
	ZVAL_UNDEF(&_30$$6);
	ZVAL_UNDEF(&_31$$6);
	ZVAL_UNDEF(&_24$$7);
	ZVAL_UNDEF(&_25$$7);
	ZVAL_UNDEF(&_26$$7);
	ZVAL_UNDEF(&_27$$7);
	ZVAL_UNDEF(&b);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
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
		ZEPHIR_CALL_FUNCTION(&_3$$3, "strval", NULL, 3, &_2$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_4$$3);
		ZEPHIR_CONCAT_SSVS(&_4$$3, "Q must be between", " 0 and 1, ", &_3$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_1$$3, "__construct", NULL, 2, &_4$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_1$$3, "tensor/matrix.zep", 1697);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&b);
	array_init(&b);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	x =  (((q * (double) ((zephir_get_numberval(&_5) - 1))) + (double) (1)));
	xHat = (int) x;
	remainder =  ((x -  (double) xHat));
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_7, &_6, "split", NULL, 0, &_8);
	zephir_check_call_status();
	if (Z_TYPE_P(&_7) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_10);
		zephir_string_to_char_array(&_10, &_7);
		_9 = &_10;
	} else {
		_9 = &_7;
	}
	zephir_is_iterable(_9, 0, "tensor/matrix.zep", 1725);
	if (Z_TYPE_P(_9) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_9), _11)
		{
			ZEPHIR_INIT_NVAR(&rowBuffer);
			ZVAL_COPY(&rowBuffer, _11);
			ZEPHIR_CALL_METHOD(NULL, &rowBuffer, "sort", NULL, 0);
			zephir_check_call_status();
			zephir_read_property_cached(&_12$$4, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
			if (ZEPHIR_LE_LONG(&_12$$4, xHat)) {
				zephir_read_property_cached(&_14$$5, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
				ZVAL_LONG(&_15$$5, (zephir_get_numberval(&_14$$5) - 1));
				ZEPHIR_CALL_METHOD(&_13$$5, &rowBuffer, "get", NULL, 0, &_15$$5);
				zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_16$$5);
				ZVAL_DOUBLE(&_16$$5, zephir_get_doubleval(&_13$$5));
				zephir_array_append(&b, &_16$$5, PH_SEPARATE, "tensor/matrix.zep", 1715);
				continue;
			}
			ZVAL_LONG(&_18$$4, (xHat - 1));
			ZEPHIR_CALL_METHOD(&_17$$4, &rowBuffer, "get", NULL, 0, &_18$$4);
			zephir_check_call_status();
			t =  (zephir_get_doubleval(&_17$$4));
			ZVAL_LONG(&_18$$4, xHat);
			ZEPHIR_CALL_METHOD(&_19$$4, &rowBuffer, "get", NULL, 0, &_18$$4);
			zephir_check_call_status();
			ZEPHIR_INIT_NVAR(&_20$$4);
			ZVAL_DOUBLE(&_20$$4, (t + (remainder * (zephir_get_numberval(&_19$$4) - t))));
			zephir_array_append(&b, &_20$$4, PH_SEPARATE, "tensor/matrix.zep", 1722);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _9, "rewind", NULL, 0);
		zephir_check_call_status();
		_22 = 1;
		while (1) {
			if (_22) {
				_22 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _9, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_21, _9, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_21)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowBuffer, _9, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_CALL_METHOD(NULL, &rowBuffer, "sort", NULL, 0);
				zephir_check_call_status();
				zephir_read_property_cached(&_23$$6, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
				if (ZEPHIR_LE_LONG(&_23$$6, xHat)) {
					zephir_read_property_cached(&_25$$7, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
					ZVAL_LONG(&_26$$7, (zephir_get_numberval(&_25$$7) - 1));
					ZEPHIR_CALL_METHOD(&_24$$7, &rowBuffer, "get", NULL, 0, &_26$$7);
					zephir_check_call_status();
					ZEPHIR_INIT_NVAR(&_27$$7);
					ZVAL_DOUBLE(&_27$$7, zephir_get_doubleval(&_24$$7));
					zephir_array_append(&b, &_27$$7, PH_SEPARATE, "tensor/matrix.zep", 1715);
					continue;
				}
				ZVAL_LONG(&_29$$6, (xHat - 1));
				ZEPHIR_CALL_METHOD(&_28$$6, &rowBuffer, "get", NULL, 0, &_29$$6);
				zephir_check_call_status();
				t =  (zephir_get_doubleval(&_28$$6));
				ZVAL_LONG(&_29$$6, xHat);
				ZEPHIR_CALL_METHOD(&_30$$6, &rowBuffer, "get", NULL, 0, &_29$$6);
				zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_31$$6);
				ZVAL_DOUBLE(&_31$$6, (t + (remainder * (zephir_get_numberval(&_30$$6) - t))));
				zephir_array_append(&b, &_31$$6, PH_SEPARATE, "tensor/matrix.zep", 1722);
		}
	}
	ZEPHIR_INIT_NVAR(&rowBuffer);
	object_init_ex(return_value, tensor_columnvector_ce);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 5, &b);
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
			ZEPHIR_CALL_METHOD(NULL, &_0$$4, "__construct", NULL, 2, &_2$$4);
			zephir_check_call_status();
			zephir_throw_exception_debug(&_0$$4, "tensor/matrix.zep", 1741);
			ZEPHIR_MM_RESTORE();
			return;
		}
		ZEPHIR_CALL_METHOD(&_3$$3, mean, "m", NULL, 0);
		zephir_check_call_status();
		zephir_read_property_cached(&_4$$3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
		if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_3$$3, &_4$$3))) {
			ZEPHIR_INIT_VAR(&_5$$5);
			object_init_ex(&_5$$5, tensor_exceptions_dimensionalitymismatch_ce);
			zephir_memory_observe(&_6$$5);
			zephir_read_property_cached(&_6$$5, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
			zephir_cast_to_string(&_7$$5, &_6$$5);
			ZEPHIR_CALL_METHOD(&_8$$5, mean, "m", NULL, 0);
			zephir_check_call_status();
			zephir_cast_to_string(&_9$$5, &_8$$5);
			ZEPHIR_INIT_VAR(&_10$$5);
			ZEPHIR_CONCAT_SSVSVS(&_10$$5, "Mean vector must", " have ", &_7$$5, " rows, ", &_9$$5, " given.");
			ZEPHIR_CALL_METHOD(NULL, &_5$$5, "__construct", NULL, 2, &_10$$5);
			zephir_check_call_status();
			zephir_throw_exception_debug(&_5$$5, "tensor/matrix.zep", 1747);
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
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
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
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(mean, zephir_get_internal_ce(SL("tensor\\columnvector")))
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
		zephir_read_property_cached(&_1$$3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
		if (!ZEPHIR_IS_IDENTICAL(&_0$$3, &_1$$3)) {
			ZEPHIR_INIT_VAR(&_2$$4);
			object_init_ex(&_2$$4, tensor_exceptions_dimensionalitymismatch_ce);
			zephir_memory_observe(&_3$$4);
			zephir_read_property_cached(&_3$$4, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
			zephir_cast_to_string(&_4$$4, &_3$$4);
			ZEPHIR_CALL_METHOD(&_5$$4, mean, "m", NULL, 0);
			zephir_check_call_status();
			zephir_cast_to_string(&_6$$4, &_5$$4);
			ZEPHIR_INIT_VAR(&_7$$4);
			ZEPHIR_CONCAT_SSVSVS(&_7$$4, "Mean vector must", " have ", &_4$$4, " rows, ", &_6$$4, " given.");
			ZEPHIR_CALL_METHOD(NULL, &_2$$4, "__construct", NULL, 2, &_7$$4);
			zephir_check_call_status();
			zephir_throw_exception_debug(&_2$$4, "tensor/matrix.zep", 1772);
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
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
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
	zend_bool _22;
	zval b;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_17 = NULL, *_20 = NULL, *_25 = NULL, *_28 = NULL;
	zval *precision_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, _4$$3, _5$$4, _6$$4, _7$$4, _8$$4, rowA, valueA, _9, _10, _11, *_12, _13, *_14, _21, _29, _30, _31, _15$$5, _16$$6, _18$$6, _19$$6, _23$$7, _24$$8, _26$$8, _27$$8;
	zend_long precision, ZEPHIR_LAST_CALL_STATUS, i = 0;
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
	ZVAL_UNDEF(&rowA);
	ZVAL_UNDEF(&valueA);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_21);
	ZVAL_UNDEF(&_29);
	ZVAL_UNDEF(&_30);
	ZVAL_UNDEF(&_31);
	ZVAL_UNDEF(&_15$$5);
	ZVAL_UNDEF(&_16$$6);
	ZVAL_UNDEF(&_18$$6);
	ZVAL_UNDEF(&_19$$6);
	ZVAL_UNDEF(&_23$$7);
	ZVAL_UNDEF(&_24$$8);
	ZVAL_UNDEF(&_26$$8);
	ZVAL_UNDEF(&_27$$8);
	ZVAL_UNDEF(&b);
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
		object_init_ex(return_value, tensor_matrix_ce);
		ZEPHIR_INIT_VAR(&_0$$3);
		zephir_read_property_cached(&_1$$3, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
		ZVAL_LONG(&_2$$3, 0);
		tensor_round(&_0$$3, &_1$$3, &_2$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
		zephir_read_property_cached(&_4$$3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
		ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0$$3, &_3$$3, &_4$$3);
		zephir_check_call_status();
		RETURN_MM();
	}
	if (UNEXPECTED(precision < 0)) {
		ZEPHIR_INIT_VAR(&_5$$4);
		object_init_ex(&_5$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_6$$4, precision);
		ZEPHIR_CALL_FUNCTION(&_7$$4, "strval", NULL, 3, &_6$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_8$$4);
		ZEPHIR_CONCAT_SSVS(&_8$$4, "Decimal precision cannot", " be less than 0, ", &_7$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 2, &_8$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/matrix.zep", 1800);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&b);
	array_init(&b);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_10, &_9, "split", NULL, 0, &_11);
	zephir_check_call_status();
	if (Z_TYPE_P(&_10) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_13);
		zephir_string_to_char_array(&_13, &_10);
		_12 = &_13;
	} else {
		_12 = &_10;
	}
	zephir_is_iterable(_12, 0, "tensor/matrix.zep", 1823);
	if (Z_TYPE_P(_12) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_12), _14)
		{
			ZEPHIR_INIT_NVAR(&rowA);
			ZVAL_COPY(&rowA, _14);
			i = 0;
			while (1) {
				zephir_read_property_cached(&_15$$5, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
				if (!(ZEPHIR_GT_LONG(&_15$$5, i))) {
					break;
				}
				ZVAL_LONG(&_16$$6, i);
				ZEPHIR_CALL_METHOD(&valueA, &rowA, "get", &_17, 0, &_16$$6);
				zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_18$$6);
				ZVAL_LONG(&_16$$6, precision);
				zephir_round(&_18$$6, &valueA, &_16$$6, NULL);
				ZVAL_LONG(&_19$$6, i);
				ZEPHIR_CALL_METHOD(NULL, &rowA, "set", &_20, 0, &_19$$6, &_18$$6);
				zephir_check_call_status();
				i++;
			}
			zephir_array_append(&b, &rowA, PH_SEPARATE, "tensor/matrix.zep", 1820);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _12, "rewind", NULL, 0);
		zephir_check_call_status();
		_22 = 1;
		while (1) {
			if (_22) {
				_22 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _12, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_21, _12, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_21)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowA, _12, "current", NULL, 0);
			zephir_check_call_status();
				i = 0;
				while (1) {
					zephir_read_property_cached(&_23$$7, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
					if (!(ZEPHIR_GT_LONG(&_23$$7, i))) {
						break;
					}
					ZVAL_LONG(&_24$$8, i);
					ZEPHIR_CALL_METHOD(&valueA, &rowA, "get", &_25, 0, &_24$$8);
					zephir_check_call_status();
					ZEPHIR_INIT_NVAR(&_26$$8);
					ZVAL_LONG(&_24$$8, precision);
					zephir_round(&_26$$8, &valueA, &_24$$8, NULL);
					ZVAL_LONG(&_27$$8, i);
					ZEPHIR_CALL_METHOD(NULL, &rowA, "set", &_28, 0, &_27$$8, &_26$$8);
					zephir_check_call_status();
					i++;
				}
				zephir_array_append(&b, &rowA, PH_SEPARATE, "tensor/matrix.zep", 1820);
		}
	}
	ZEPHIR_INIT_NVAR(&rowA);
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_CALL_CE_STATIC(&_29, tensor_tensorbuffer_ce, "fromBuffers", NULL, 0, &b);
	zephir_check_call_status();
	zephir_read_property_cached(&_30, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_31, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_29, &_30, &_31);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_floor(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_ceil(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_1$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/matrix.zep", 1859);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_4, min);
	ZVAL_DOUBLE(&_5, max);
	tensor_clip(&_2, &_3, &_4, &_5);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_2, &_6, &_7);
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
	zval *min_param = NULL, _0, _1, _2, _3, _4;
	double min;
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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(min_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &min_param);
	min = zephir_get_doubleval(min_param);
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, min);
	tensor_clip_lower(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_3, &_4);
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
	zval *max_param = NULL, _0, _1, _2, _3, _4;
	double max;
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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(max_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &max_param);
	max = zephir_get_doubleval(max_param);
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, max);
	tensor_clip_upper(&_0, &_1, &_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_3, &_4);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_sign(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
	zval _0, _1, _2, _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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

	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	tensor_negate(&_0, &_1);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_0, &_2, &_3);
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
	zval _11;
	zend_bool _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2, _3, buffer, _10, _12, _13, _14, _15, _4$$3, _5$$3, _7$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&buffer);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_15);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_7$$3);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	_1 = ZEPHIR_GT_LONG(&_0, 0);
	if (_1) {
		ZEPHIR_CALL_METHOD(&_2, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
		_1 = !ZEPHIR_IS_IDENTICAL(&_2, &_3);
	}
	if (UNEXPECTED(_1)) {
		ZEPHIR_INIT_VAR(&_4$$3);
		object_init_ex(&_4$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_5$$3);
		zephir_read_property_cached(&_5$$3, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC);
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_CALL_METHOD(&_7$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_8$$3, &_7$$3);
		ZEPHIR_INIT_VAR(&_9$$3);
		ZEPHIR_CONCAT_SVSVS(&_9$$3, "Matrix A requires", &_6$$3, " columns but Matrix B has ", &_8$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_4$$3, "__construct", NULL, 2, &_9$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_4$$3, "tensor/matrix.zep", 1925);
		ZEPHIR_MM_RESTORE();
		return;
	}
	zephir_read_property_cached(&_10, b, _zephir_prop_2, 0, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_11);
	zephir_create_array(&_11, 1, 0);
	zephir_memory_observe(&_12);
	zephir_read_property_cached(&_12, this_ptr, _zephir_prop_2, 14, PH_NOISY_CC);
	zephir_array_fast_append(&_11, &_12);
	ZEPHIR_CALL_METHOD(&buffer, &_10, "concat", NULL, 0, &_11);
	zephir_check_call_status();
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_CALL_METHOD(&_13, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_15);
	zephir_add_function(&_15, &_13, &_14);
	zephir_read_property_cached(&_14, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &buffer, &_15, &_14);
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
	zval _11;
	zend_bool _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _2, _3, buffer, _10, _12, _13, _14, _15, _4$$3, _5$$3, _7$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&buffer);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_15);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_7$$3);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	_1 = ZEPHIR_GT_LONG(&_0, 0);
	if (_1) {
		ZEPHIR_CALL_METHOD(&_2, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_read_property_cached(&_3, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
		_1 = !ZEPHIR_IS_IDENTICAL(&_2, &_3);
	}
	if (UNEXPECTED(_1)) {
		ZEPHIR_INIT_VAR(&_4$$3);
		object_init_ex(&_4$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_5$$3);
		zephir_read_property_cached(&_5$$3, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC);
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_CALL_METHOD(&_7$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_8$$3, &_7$$3);
		ZEPHIR_INIT_VAR(&_9$$3);
		ZEPHIR_CONCAT_SVSVS(&_9$$3, "Matrix A requires", &_6$$3, " columns but Matrix B has ", &_8$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_4$$3, "__construct", NULL, 2, &_9$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_4$$3, "tensor/matrix.zep", 1945);
		ZEPHIR_MM_RESTORE();
		return;
	}
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 14, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_11);
	zephir_create_array(&_11, 1, 0);
	zephir_memory_observe(&_12);
	zephir_read_property_cached(&_12, b, _zephir_prop_2, 0, PH_NOISY_CC);
	zephir_array_fast_append(&_11, &_12);
	ZEPHIR_CALL_METHOD(&buffer, &_10, "concat", NULL, 0, &_11);
	zephir_check_call_status();
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_14, b, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&_15);
	zephir_add_function(&_15, &_13, &_14);
	zephir_read_property_cached(&_13, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &buffer, &_15, &_13);
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
	zval c, _22$$4;
	zend_bool _1, _11;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_16 = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS, _12, _13;
	zval *b, b_sub, _0, _2, _3, i, bufferB, bufferA, _10, _23, _24, _25, _26, _27, _4$$3, _5$$3, _7$$3, _14$$4, _15$$4, _17$$4, _18$$4, _19$$4, _20$$4, _21$$4;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&i);
	ZVAL_UNDEF(&bufferB);
	ZVAL_UNDEF(&bufferA);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_23);
	ZVAL_UNDEF(&_24);
	ZVAL_UNDEF(&_25);
	ZVAL_UNDEF(&_26);
	ZVAL_UNDEF(&_27);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_7$$3);
	ZVAL_UNDEF(&_14$$4);
	ZVAL_UNDEF(&_15$$4);
	ZVAL_UNDEF(&_17$$4);
	ZVAL_UNDEF(&_18$$4);
	ZVAL_UNDEF(&_19$$4);
	ZVAL_UNDEF(&_20$$4);
	ZVAL_UNDEF(&_21$$4);
	ZVAL_UNDEF(&c);
	ZVAL_UNDEF(&_22$$4);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	_1 = ZEPHIR_GT_LONG(&_0, 0);
	if (_1) {
		ZEPHIR_CALL_METHOD(&_2, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_read_property_cached(&_3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
		_1 = !ZEPHIR_IS_IDENTICAL(&_2, &_3);
	}
	if (UNEXPECTED(_1)) {
		ZEPHIR_INIT_VAR(&_4$$3);
		object_init_ex(&_4$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_5$$3);
		zephir_read_property_cached(&_5$$3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_CALL_METHOD(&_7$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_8$$3, &_7$$3);
		ZEPHIR_INIT_VAR(&_9$$3);
		ZEPHIR_CONCAT_SVSVS(&_9$$3, "Matrix A requires", &_6$$3, " rows but Matrix B has ", &_8$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_4$$3, "__construct", NULL, 2, &_9$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_4$$3, "tensor/matrix.zep", 1965);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&c);
	array_init(&c);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	_13 = (zephir_get_numberval(&_10) - 1);
	_12 = 0;
	_11 = 0;
	if (_12 <= _13) {
		while (1) {
			if (_11) {
				_12++;
				if (!(_12 <= _13)) {
					break;
				}
			} else {
				_11 = 1;
			}
			ZEPHIR_INIT_NVAR(&i);
			ZVAL_LONG(&i, _12);
			zephir_read_property_cached(&_14$$4, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
			ZEPHIR_CALL_METHOD(&_15$$4, b, "n", &_16, 0);
			zephir_check_call_status();
			ZEPHIR_INIT_NVAR(&_17$$4);
			mul_function(&_17$$4, &i, &_15$$4);
			ZEPHIR_CALL_METHOD(&_15$$4, b, "n", &_16, 0);
			zephir_check_call_status();
			ZEPHIR_CALL_METHOD(&bufferB, &_14$$4, "slice", NULL, 0, &_17$$4, &_15$$4);
			zephir_check_call_status();
			zephir_read_property_cached(&_18$$4, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
			zephir_read_property_cached(&_19$$4, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
			ZEPHIR_INIT_NVAR(&_20$$4);
			mul_function(&_20$$4, &i, &_19$$4);
			zephir_read_property_cached(&_19$$4, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
			ZEPHIR_CALL_METHOD(&bufferA, &_18$$4, "slice", NULL, 0, &_20$$4, &_19$$4);
			zephir_check_call_status();
			ZEPHIR_INIT_NVAR(&_22$$4);
			zephir_create_array(&_22$$4, 1, 0);
			zephir_array_fast_append(&_22$$4, &bufferA);
			ZEPHIR_CALL_METHOD(&_21$$4, &bufferB, "concat", NULL, 0, &_22$$4);
			zephir_check_call_status();
			zephir_array_append(&c, &_21$$4, PH_SEPARATE, "tensor/matrix.zep", 1979);
		}
	}
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_CALL_CE_STATIC(&_23, tensor_tensorbuffer_ce, "fromBuffers", NULL, 0, &c);
	zephir_check_call_status();
	zephir_read_property_cached(&_24, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_25, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_26, b, "n", &_16, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&_27);
	zephir_add_function(&_27, &_25, &_26);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_23, &_24, &_27);
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
	zval c, _22$$4;
	zend_bool _1, _11;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_19 = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS, _12, _13;
	zval *b, b_sub, _0, _2, _3, i, bufferA, bufferB, _10, _23, _24, _25, _26, _27, _4$$3, _5$$3, _7$$3, _14$$4, _15$$4, _16$$4, _17$$4, _18$$4, _20$$4, _21$$4;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&i);
	ZVAL_UNDEF(&bufferA);
	ZVAL_UNDEF(&bufferB);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_23);
	ZVAL_UNDEF(&_24);
	ZVAL_UNDEF(&_25);
	ZVAL_UNDEF(&_26);
	ZVAL_UNDEF(&_27);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_7$$3);
	ZVAL_UNDEF(&_14$$4);
	ZVAL_UNDEF(&_15$$4);
	ZVAL_UNDEF(&_16$$4);
	ZVAL_UNDEF(&_17$$4);
	ZVAL_UNDEF(&_18$$4);
	ZVAL_UNDEF(&_20$$4);
	ZVAL_UNDEF(&_21$$4);
	ZVAL_UNDEF(&c);
	ZVAL_UNDEF(&_22$$4);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	_1 = ZEPHIR_GT_LONG(&_0, 0);
	if (_1) {
		ZEPHIR_CALL_METHOD(&_2, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_read_property_cached(&_3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
		_1 = !ZEPHIR_IS_IDENTICAL(&_2, &_3);
	}
	if (UNEXPECTED(_1)) {
		ZEPHIR_INIT_VAR(&_4$$3);
		object_init_ex(&_4$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_5$$3);
		zephir_read_property_cached(&_5$$3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_CALL_METHOD(&_7$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_8$$3, &_7$$3);
		ZEPHIR_INIT_VAR(&_9$$3);
		ZEPHIR_CONCAT_SVSVS(&_9$$3, "Matrix A requires", &_6$$3, " rows but Matrix B has ", &_8$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_4$$3, "__construct", NULL, 2, &_9$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_4$$3, "tensor/matrix.zep", 1997);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&c);
	array_init(&c);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	_13 = (zephir_get_numberval(&_10) - 1);
	_12 = 0;
	_11 = 0;
	if (_12 <= _13) {
		while (1) {
			if (_11) {
				_12++;
				if (!(_12 <= _13)) {
					break;
				}
			} else {
				_11 = 1;
			}
			ZEPHIR_INIT_NVAR(&i);
			ZVAL_LONG(&i, _12);
			zephir_read_property_cached(&_14$$4, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
			zephir_read_property_cached(&_15$$4, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
			ZEPHIR_INIT_NVAR(&_16$$4);
			mul_function(&_16$$4, &i, &_15$$4);
			zephir_read_property_cached(&_15$$4, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
			ZEPHIR_CALL_METHOD(&bufferA, &_14$$4, "slice", NULL, 0, &_16$$4, &_15$$4);
			zephir_check_call_status();
			zephir_read_property_cached(&_17$$4, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
			ZEPHIR_CALL_METHOD(&_18$$4, b, "n", &_19, 0);
			zephir_check_call_status();
			ZEPHIR_INIT_NVAR(&_20$$4);
			mul_function(&_20$$4, &i, &_18$$4);
			ZEPHIR_CALL_METHOD(&_18$$4, b, "n", &_19, 0);
			zephir_check_call_status();
			ZEPHIR_CALL_METHOD(&bufferB, &_17$$4, "slice", NULL, 0, &_20$$4, &_18$$4);
			zephir_check_call_status();
			ZEPHIR_INIT_NVAR(&_22$$4);
			zephir_create_array(&_22$$4, 1, 0);
			zephir_array_fast_append(&_22$$4, &bufferB);
			ZEPHIR_CALL_METHOD(&_21$$4, &bufferA, "concat", NULL, 0, &_22$$4);
			zephir_check_call_status();
			zephir_array_append(&c, &_21$$4, PH_SEPARATE, "tensor/matrix.zep", 2011);
		}
	}
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_CALL_CE_STATIC(&_23, tensor_tensorbuffer_ce, "fromBuffers", NULL, 0, &c);
	zephir_check_call_status();
	zephir_read_property_cached(&_24, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_25, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_26, b, "n", &_19, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&_27);
	zephir_add_function(&_27, &_25, &_26);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &_23, &_24, &_27);
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
	zend_bool _9$$3;
	zval bigRows;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *m_param = NULL, *n_param = NULL, rowA, _18, _19, _0$$3, _1$$3, _2$$3, *_3$$3, _4$$3, *_5$$3, _8$$3, _6$$4, _7$$4, _10$$5, _11$$5, flat$$6, _12$$6, _13$$6, _14$$6, _15$$6, _16$$6, _17$$6;
	zend_long m, n, ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&rowA);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19);
	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_8$$3);
	ZVAL_UNDEF(&_6$$4);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_10$$5);
	ZVAL_UNDEF(&_11$$5);
	ZVAL_UNDEF(&flat$$6);
	ZVAL_UNDEF(&_12$$6);
	ZVAL_UNDEF(&_13$$6);
	ZVAL_UNDEF(&_14$$6);
	ZVAL_UNDEF(&_15$$6);
	ZVAL_UNDEF(&_16$$6);
	ZVAL_UNDEF(&_17$$6);
	ZVAL_UNDEF(&bigRows);
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

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(m)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 2, 0, &m_param, &n_param);
	ZEPHIR_INIT_VAR(&bigRows);
	array_init(&bigRows);
	if (n > 0) {
		zephir_read_property_cached(&_0$$3, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
		zephir_read_property_cached(&_2$$3, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
		ZEPHIR_CALL_METHOD(&_1$$3, &_0$$3, "split", NULL, 0, &_2$$3);
		zephir_check_call_status();
		if (Z_TYPE_P(&_1$$3) == IS_STRING) {
			ZEPHIR_INIT_VAR(&_4$$3);
			zephir_string_to_char_array(&_4$$3, &_1$$3);
			_3$$3 = &_4$$3;
		} else {
			_3$$3 = &_1$$3;
		}
		zephir_is_iterable(_3$$3, 0, "tensor/matrix.zep", 2035);
		if (Z_TYPE_P(_3$$3) == IS_ARRAY) {
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_3$$3), _5$$3)
			{
				ZEPHIR_INIT_NVAR(&rowA);
				ZVAL_COPY(&rowA, _5$$3);
				ZVAL_LONG(&_7$$4, (n + 1));
				ZEPHIR_CALL_METHOD(&_6$$4, &rowA, "repeat", NULL, 0, &_7$$4);
				zephir_check_call_status();
				zephir_array_append(&bigRows, &_6$$4, PH_SEPARATE, "tensor/matrix.zep", 2033);
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEPHIR_CALL_METHOD(NULL, _3$$3, "rewind", NULL, 0);
			zephir_check_call_status();
			_9$$3 = 1;
			while (1) {
				if (_9$$3) {
					_9$$3 = 0;
				} else {
					ZEPHIR_CALL_METHOD(NULL, _3$$3, "next", NULL, 0);
					zephir_check_call_status();
				}
				ZEPHIR_CALL_METHOD(&_8$$3, _3$$3, "valid", NULL, 0);
				zephir_check_call_status();
				if (!zend_is_true(&_8$$3)) {
					break;
				}
				ZEPHIR_CALL_METHOD(&rowA, _3$$3, "current", NULL, 0);
				zephir_check_call_status();
					ZVAL_LONG(&_11$$5, (n + 1));
					ZEPHIR_CALL_METHOD(&_10$$5, &rowA, "repeat", NULL, 0, &_11$$5);
					zephir_check_call_status();
					zephir_array_append(&bigRows, &_10$$5, PH_SEPARATE, "tensor/matrix.zep", 2033);
			}
		}
		ZEPHIR_INIT_NVAR(&rowA);
	}
	if (zephir_fast_count_int(&bigRows) > 0) {
		zephir_memory_observe(&flat$$6);
		zephir_array_fetch_long(&flat$$6, &bigRows, 0, PH_NOISY, "tensor/matrix.zep", 2038);
		ZVAL_LONG(&_13$$6, 1);
		ZEPHIR_CALL_FUNCTION(&_14$$6, "array_slice", NULL, 29, &bigRows, &_13$$6);
		zephir_check_call_status();
		ZEPHIR_CALL_METHOD(&_12$$6, &flat$$6, "concat", NULL, 0, &_14$$6);
		zephir_check_call_status();
		ZEPHIR_CPY_WRT(&flat$$6, &_12$$6);
		ZVAL_LONG(&_13$$6, (m + 1));
		ZEPHIR_CALL_METHOD(&_12$$6, &flat$$6, "repeat", NULL, 0, &_13$$6);
		zephir_check_call_status();
		ZEPHIR_CPY_WRT(&flat$$6, &_12$$6);
		object_init_ex(return_value, tensor_matrix_ce);
		zephir_read_property_cached(&_13$$6, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
		zephir_read_property_cached(&_15$$6, this_ptr, _zephir_prop_1, 16, PH_NOISY_CC | PH_READONLY);
		ZVAL_LONG(&_16$$6, (zephir_get_numberval(&_13$$6) * ((m + 1))));
		ZVAL_LONG(&_17$$6, (zephir_get_numberval(&_15$$6) * ((n + 1))));
		ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &flat$$6, &_16$$6, &_17$$6);
		zephir_check_call_status();
		RETURN_MM();
	}
	ZEPHIR_INIT_VAR(&_18);
	array_init(&_18);
	ZVAL_BOOL(&_19, 0);
	ZEPHIR_RETURN_CALL_SELF("fromArray", NULL, 0, &_18, &_19);
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
	zval *b, b_sub, _0, _1, result, _6, _7, _8, _9, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
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
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2061);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	tensor_multiply(&result, &_6, &_7);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_8, &_9);
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
	zval *b, b_sub, _0, _1, result, _6, _7, _8, _9, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
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
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2080);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	tensor_divide(&result, &_6, &_7);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_8, &_9);
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
	zval *b, b_sub, _0, _1, result, _6, _7, _8, _9, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
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
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2099);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	tensor_add(&result, &_6, &_7);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_8, &_9);
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
	zval *b, b_sub, _0, _1, result, _6, _7, _8, _9, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
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
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2118);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	tensor_subtract(&result, &_6, &_7);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_8, &_9);
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
	zval *b, b_sub, _0, _1, result, _6, _7, _8, _9, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
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
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2138);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	tensor_pow(&result, &_6, &_7);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_8, &_9);
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
	zval *b, b_sub, _0, _1, result, _6, _7, _8, _9, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
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
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2158);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	tensor_mod(&result, &_6, &_7);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_8, &_9);
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
	zval *b, b_sub, _0, _1, result, _6, _7, _8, _9, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
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
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2178);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	tensor_equal(&result, &_6, &_7);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_8, &_9);
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
	zval *b, b_sub, _0, _1, result, _6, _7, _8, _9, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
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
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2197);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	tensor_not_equal(&result, &_6, &_7);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_8, &_9);
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
	zval *b, b_sub, _0, _1, result, _6, _7, _8, _9, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
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
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2217);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	tensor_greater(&result, &_6, &_7);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_8, &_9);
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
	zval *b, b_sub, _0, _1, result, _6, _7, _8, _9, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
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
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2237);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	tensor_greater_equal(&result, &_6, &_7);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_8, &_9);
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
	zval *b, b_sub, _0, _1, result, _6, _7, _8, _9, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
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
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2256);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	tensor_less(&result, &_6, &_7);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_8, &_9);
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
	zval *b, b_sub, _0, _1, result, _6, _7, _8, _9, _2$$3, _3$$3, _4$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
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
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2275);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, b, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	tensor_less_equal(&result, &_6, &_7);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_8, &_9);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2295);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	tensor_multiply_row(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2319);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	tensor_divide_row(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2343);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	tensor_add_row(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2367);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	tensor_subtract_row(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2391);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	tensor_pow_row(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2415);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	tensor_mod_row(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2440);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	tensor_equal_row(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2464);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	tensor_not_equal_row(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2488);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	tensor_greater_row(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2512);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	tensor_greater_equal_row(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2536);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	tensor_less_row(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "n", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " columns but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2561);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	tensor_less_equal_row(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_0, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\columnvector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2585);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	tensor_multiply_col(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\columnvector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2609);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	tensor_divide_col(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\columnvector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2633);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	tensor_add_col(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\columnvector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2657);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	tensor_subtract_col(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\columnvector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2681);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	tensor_pow_col(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\columnvector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2705);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	tensor_mod_col(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\columnvector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2729);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	tensor_equal_col(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\columnvector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2753);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	tensor_not_equal_col(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\columnvector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2777);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	tensor_greater_col(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\columnvector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2801);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	tensor_greater_equal_col(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\columnvector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2825);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	tensor_less_col(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b, b_sub, _0, _1, bHat, result, _8, _9, _10, _11, _2$$3, _3$$3, _5$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&bHat);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\columnvector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, b, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "m", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Matrix A expects ", &_4$$3, " rows but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/matrix.zep", 2849);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&bHat, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	tensor_less_equal_col(&result, &_8, &bHat, &_9);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_10, &_11);
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
	zval *b_param = NULL, result, _0, _1, _2, _3;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_1, b);
	tensor_multiply_scalar(&result, &_0, &_1);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_2, &_3);
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
	zval *b_param = NULL, result, _0, _1, _2, _3;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_1, b);
	tensor_divide_scalar(&result, &_0, &_1);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_2, &_3);
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
	zval *b_param = NULL, result, _0, _1, _2, _3;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_1, b);
	tensor_add_scalar(&result, &_0, &_1);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_2, &_3);
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
	zval *b_param = NULL, result, _0, _1, _2, _3;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_1, b);
	tensor_subtract_scalar(&result, &_0, &_1);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_2, &_3);
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
	zval *b_param = NULL, result, _0, _1, _2, _3;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_1, b);
	tensor_pow_scalar(&result, &_0, &_1);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_2, &_3);
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
	zval *b_param = NULL, result, _0, _1, _2, _3;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_1, b);
	tensor_mod_scalar(&result, &_0, &_1);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_2, &_3);
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
	zval *b_param = NULL, result, _0, _1, _2, _3;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_1, b);
	tensor_equal_scalar(&result, &_0, &_1);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_2, &_3);
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
	zval *b_param = NULL, result, _0, _1, _2, _3;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_1, b);
	tensor_not_equal_scalar(&result, &_0, &_1);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_2, &_3);
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
	zval *b_param = NULL, result, _0, _1, _2, _3;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_1, b);
	tensor_greater_scalar(&result, &_0, &_1);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_2, &_3);
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
	zval *b_param = NULL, result, _0, _1, _2, _3;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_1, b);
	tensor_greater_equal_scalar(&result, &_0, &_1);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_2, &_3);
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
	zval *b_param = NULL, result, _0, _1, _2, _3;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_1, b);
	tensor_less_scalar(&result, &_0, &_1);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_2, &_3);
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
	zval *b_param = NULL, result, _0, _1, _2, _3;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
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
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 14, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_1, b);
	tensor_less_equal_scalar(&result, &_0, &_1);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 15, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 15, &result, &_2, &_3);
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
	ZEPHIR_THROW_EXCEPTION_DEBUG_STRW(tensor_exceptions_runtimeexception_ce, "Matrix cannot be mutated directly.", "tensor/matrix.zep", 3034);
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
	zend_bool _0;
	zval *index, index_sub, _1;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&index_sub);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(index)
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(1, 0, &index);
	if (Z_TYPE_P(index) != IS_LONG) {
		RETURN_BOOL(0);
	}
	_0 = ZEPHIR_GE_LONG(index, 0);
	if (_0) {
		zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
		_0 = ZEPHIR_LT(index, &_1);
	}
	RETURN_BOOL(_0);
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
	ZEPHIR_THROW_EXCEPTION_DEBUG_STRW(tensor_exceptions_runtimeexception_ce, "Matrix cannot be mutated directly.", "tensor/matrix.zep", 3058);
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
	zval _6$$4, _7$$4;
	zend_bool _3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *index, index_sub, _0$$3, _1$$3, _2$$3, _4, _8, _9, _10, _11, _5$$4;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&index_sub);
	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_6$$4);
	ZVAL_UNDEF(&_7$$4);
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
		Z_PARAM_ZVAL(index)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &index);
	if (UNEXPECTED(Z_TYPE_P(index) != IS_LONG)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		object_init_ex(&_0$$3, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_INIT_VAR(&_1$$3);
		zephir_gettype(&_1$$3, index);
		ZEPHIR_INIT_VAR(&_2$$3);
		ZEPHIR_CONCAT_SSVS(&_2$$3, "Row offset must be", " an integer, ", &_1$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_2$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/matrix.zep", 3072);
		ZEPHIR_MM_RESTORE();
		return;
	}
	_3 = ZEPHIR_LT_LONG(index, 0);
	if (!(_3)) {
		zephir_read_property_cached(&_4, this_ptr, _zephir_prop_0, 15, PH_NOISY_CC | PH_READONLY);
		_3 = ZEPHIR_GE(index, &_4);
	}
	if (UNEXPECTED(_3)) {
		ZEPHIR_INIT_VAR(&_5$$4);
		object_init_ex(&_5$$4, tensor_exceptions_invalidargumentexception_ce);
		zephir_cast_to_string(&_6$$4, index);
		ZEPHIR_INIT_VAR(&_7$$4);
		ZEPHIR_CONCAT_SSVS(&_7$$4, "Row offset out of", " bounds, ", &_6$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 2, &_7$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/matrix.zep", 3077);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, tensor_vector_ce);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 14, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_INIT_VAR(&_11);
	mul_function(&_11, index, &_10);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_2, 16, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_9, &_8, "slice", NULL, 0, &_11, &_10);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 5, &_9);
	zephir_check_call_status();
	RETURN_MM();
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
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 21, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the elements of the matrix as a plain PHP array of rows so that
 * only the values, and not the object structure, appear in the
 * serialized form.
 *
 * @return array[]
 */
PHP_METHOD(Tensor_Matrix, __serialize)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_RETURN_CALL_METHOD(this_ptr, "asArray", NULL, 0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Restore the matrix from the plain array of rows produced by
 * __serialize() by rebuilding its TensorBuffer and shape.
 *
 * @param float[][] data
 * @throws \Tensor\Exceptions\InvalidArgumentException
 */
PHP_METHOD(Tensor_Matrix, __unserialize)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *data_param = NULL, rebuilt, _0, _1, _2;
	zval data;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&data);
	ZVAL_UNDEF(&rebuilt);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
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
		ZEPHIR_Z_PARAM_ARRAY(data, data_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &data_param);
	zephir_get_arrval(&data, data_param);
	ZEPHIR_CALL_SELF(&rebuilt, "fromArray", NULL, 0, &data);
	zephir_check_call_status();
	zephir_read_property_cached(&_0, &rebuilt, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_0, 14, &_0);
	zephir_read_property_cached(&_1, &rebuilt, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_1, 15, &_1);
	zephir_read_property_cached(&_2, &rebuilt, _zephir_prop_2, 0, PH_NOISY_CC | PH_READONLY);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_2, 16, &_2);
	ZEPHIR_MM_RESTORE();
}

