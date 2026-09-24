
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
#include "kernel/operators.h"
#include "kernel/memory.h"
#include "kernel/object.h"
#include "kernel/exception.h"
#include "kernel/concat.h"
#include "kernel/array.h"
#include "math.h"
#include "kernel/math.h"
#include "kernel/string.h"
#include "ext/spl/spl_array.h"
#include "include/buffer.h"
#include "include/linear_algebra.h"
#include "include/signal_processing.h"
#include "include/unary.h"
#include "include/arithmetic.h"
#include "include/comparison.h"


/**
 * Vector
 *
 * A one dimensional (rank 1) tensor with integer and/or floating point elements.
 *
 * @category    Scientific Computing
 * @package     Rubix/Tensor
 * @author      Andrew DalPino
 */
ZEPHIR_INIT_CLASS(Tensor_Vector)
{
	ZEPHIR_REGISTER_CLASS(Tensor, Vector, tensor, vector, tensor_vector_method_entry, 0);

	/**
	 * A 1-d contiguous buffer holding the elements of the vector.
	 *
	 * @var \Tensor\TensorBuffer
	 */
	zend_declare_property_null(tensor_vector_ce, SL("a"), ZEND_ACC_PROTECTED);
	/**
	 * The number of elements in the vector.
	 *
	 * @var int
	 */
	zend_declare_property_null(tensor_vector_ce, SL("n"), ZEND_ACC_PROTECTED);
	zend_class_implements(tensor_vector_ce, 1, tensor_tensor_ce);
	return SUCCESS;
}

/**
 * Build a vector of zeros with n elements.
 *
 * @param int n
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Vector, zeros)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *n_param = NULL, _0, _1;
	zend_long n, ZEPHIR_LAST_CALL_STATUS;

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &n_param);
	ZVAL_DOUBLE(&_0, 0.0);
	ZVAL_LONG(&_1, n);
	ZEPHIR_RETURN_CALL_STATIC("fill", NULL, 0, &_0, &_1);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Build a vector of ones with n elements.
 *
 * @param int n
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Vector, ones)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *n_param = NULL, _0, _1;
	zend_long n, ZEPHIR_LAST_CALL_STATUS;

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &n_param);
	ZVAL_DOUBLE(&_0, 1.0);
	ZVAL_LONG(&_1, n);
	ZEPHIR_RETURN_CALL_STATIC("fill", NULL, 0, &_0, &_1);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Fill a vector with a given value.
 *
 * @param float value
 * @param int n
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Vector, fill)
{
	zend_bool _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long n, ZEPHIR_LAST_CALL_STATUS;
	zval *value_param = NULL, *n_param = NULL, _0, _2, _3, _12, _13, _14, _4$$3, _5$$3, _6$$3, _7$$3, _8$$4, _9$$4, _10$$4, _11$$4;
	double value;

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	ZVAL_UNDEF(&_8$$4);
	ZVAL_UNDEF(&_9$$4);
	ZVAL_UNDEF(&_10$$4);
	ZVAL_UNDEF(&_11$$4);
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(value_param)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 2, 0, &value_param, &n_param);
	value = zephir_get_doubleval(value_param);
	ZVAL_DOUBLE(&_0, value);
	_1 = !(Z_TYPE_P(&_0) == IS_LONG);
	if (_1) {
		ZVAL_DOUBLE(&_2, value);
		ZEPHIR_CALL_FUNCTION(&_3, "is_float", NULL, 1, &_2);
		zephir_check_call_status();
		_1 = !zephir_is_true(&_3);
	}
	if (UNEXPECTED(_1)) {
		ZEPHIR_INIT_VAR(&_4$$3);
		object_init_ex(&_4$$3, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_INIT_VAR(&_5$$3);
		ZVAL_DOUBLE(&_6$$3, value);
		zephir_gettype(&_5$$3, &_6$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SSVS(&_7$$3, "Value must be an", " integer or floating point number, ", &_5$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_4$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_4$$3, "tensor/vector.zep", 70);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(n < 1)) {
		ZEPHIR_INIT_VAR(&_8$$4);
		object_init_ex(&_8$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_9$$4, n);
		ZEPHIR_CALL_FUNCTION(&_10$$4, "strval", NULL, 3, &_9$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_11$$4);
		ZEPHIR_CONCAT_SSVS(&_11$$4, "N must be", " greater than 0, ", &_10$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_8$$4, "__construct", NULL, 2, &_11$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_8$$4, "tensor/vector.zep", 75);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZVAL_LONG(&_2, 0);
	ZVAL_LONG(&_12, n);
	ZVAL_DOUBLE(&_13, value);
	ZEPHIR_CALL_FUNCTION(&_14, "array_fill", NULL, 4, &_2, &_12, &_13);
	zephir_check_call_status();
	ZVAL_BOOL(&_2, 0);
	ZEPHIR_RETURN_CALL_STATIC("fromArray", NULL, 0, &_14, &_2);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return a random uniform vector with values between 0 and 1.
 *
 * @param int n
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Vector, rand)
{
	zval a;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_6 = NULL;
	zval *n_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, _4, _8, _5$$4, _7$$4;
	zend_long n, ZEPHIR_LAST_CALL_STATUS, max;

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&a);
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
		zephir_throw_exception_debug(&_0$$3, "tensor/vector.zep", 92);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&a);
	array_init(&a);
	ZEPHIR_CALL_FUNCTION(&_4, "getrandmax", NULL, 5);
	zephir_check_call_status();
	max = zephir_get_intval(&_4);
	while (1) {
		if (!(zephir_fast_count_int(&a) < n)) {
			break;
		}
		ZEPHIR_CALL_FUNCTION(&_5$$4, "rand", &_6, 6);
		zephir_check_call_status();
		ZEPHIR_INIT_NVAR(&_7$$4);
		ZVAL_DOUBLE(&_7$$4, zephir_safe_div_zval_long(&_5$$4, max));
		zephir_array_append(&a, &_7$$4, PH_SEPARATE, "tensor/vector.zep", 100);
	}
	ZVAL_BOOL(&_8, 0);
	ZEPHIR_RETURN_CALL_STATIC("fromArray", NULL, 0, &a, &_8);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return a standard normally distributed (Gaussian) random vector with mean 0
 * and unit variance.
 *
 * @param int n
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Vector, gaussian)
{
	zval a;
	double r = 0, phi = 0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_6 = NULL, *_9 = NULL;
	zval *n_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, _4, _14, _5$$4, _7$$4, _8$$4, _10$$4, _11$$4, _12$$4, _13$$4;
	zend_long n, ZEPHIR_LAST_CALL_STATUS, max;

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_8$$4);
	ZVAL_UNDEF(&_10$$4);
	ZVAL_UNDEF(&_11$$4);
	ZVAL_UNDEF(&_12$$4);
	ZVAL_UNDEF(&_13$$4);
	ZVAL_UNDEF(&a);
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
		zephir_throw_exception_debug(&_0$$3, "tensor/vector.zep", 118);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&a);
	array_init(&a);
	ZEPHIR_CALL_FUNCTION(&_4, "getrandmax", NULL, 5);
	zephir_check_call_status();
	max = zephir_get_intval(&_4);
	while (1) {
		if (!(zephir_fast_count_int(&a) < n)) {
			break;
		}
		ZEPHIR_CALL_FUNCTION(&_5$$4, "rand", &_6, 6);
		zephir_check_call_status();
		ZVAL_DOUBLE(&_7$$4, zephir_safe_div_zval_long(&_5$$4, max));
		ZEPHIR_CALL_FUNCTION(&_8$$4, "log", &_9, 7, &_7$$4);
		zephir_check_call_status();
		ZVAL_DOUBLE(&_7$$4, (-2.0 * zephir_get_numberval(&_8$$4)));
		r =  (sqrt((-2.0 * zephir_get_numberval(&_8$$4))));
		ZEPHIR_CALL_FUNCTION(&_10$$4, "rand", &_6, 6);
		zephir_check_call_status();
		phi =  ((zephir_safe_div_zval_long(&_10$$4, max) * 6.28318530718));
		ZVAL_DOUBLE(&_11$$4, phi);
		ZEPHIR_INIT_NVAR(&_12$$4);
		ZVAL_DOUBLE(&_12$$4, (r * sin(phi)));
		zephir_array_append(&a, &_12$$4, PH_SEPARATE, "tensor/vector.zep", 132);
		ZVAL_DOUBLE(&_13$$4, phi);
		ZEPHIR_INIT_NVAR(&_12$$4);
		ZVAL_DOUBLE(&_12$$4, (r * cos(phi)));
		zephir_array_append(&a, &_12$$4, PH_SEPARATE, "tensor/vector.zep", 133);
	}
	if (zephir_fast_count_int(&a) > n) {
		ZEPHIR_MAKE_REF(&a);
		ZEPHIR_CALL_FUNCTION(NULL, "array_pop", NULL, 8, &a);
		ZEPHIR_UNREF(&a);
		zephir_check_call_status();
	}
	ZVAL_BOOL(&_14, 0);
	ZEPHIR_RETURN_CALL_STATIC("fromArray", NULL, 0, &a, &_14);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Generate a vector with n elements from a Poisson distribution.
 *
 * @param int n
 * @param float lambda
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Vector, poisson)
{
	zval a;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_3 = NULL, *_15 = NULL;
	double lambda, p = 0, k = 0, l;
	zval *n_param = NULL, *lambda_param = NULL, _0$$3, _1$$3, _2$$3, _4$$3, _5$$4, _6$$4, _7$$4, _8$$4, _9$$5, _10$$5, _11, _12, _13, _14$$7, _16$$6;
	zend_long n, ZEPHIR_LAST_CALL_STATUS, max;

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
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14$$7);
	ZVAL_UNDEF(&_16$$6);
	ZVAL_UNDEF(&a);
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(n)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(lambda_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 1, &n_param, &lambda_param);
	if (!lambda_param) {
		lambda = 1.0;
	} else {
		lambda = zephir_get_doubleval(lambda_param);
	}
	if (UNEXPECTED(n < 1)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		object_init_ex(&_0$$3, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_1$$3, n);
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", &_3, 3, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_4$$3);
		ZEPHIR_CONCAT_SSVS(&_4$$3, "N must be", " greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_4$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/vector.zep", 155);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(lambda < 0.0)) {
		ZEPHIR_INIT_VAR(&_5$$4);
		object_init_ex(&_5$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_DOUBLE(&_6$$4, lambda);
		ZEPHIR_CALL_FUNCTION(&_7$$4, "strval", &_3, 3, &_6$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_8$$4);
		ZEPHIR_CONCAT_SSVS(&_8$$4, "Lambda must be", " greater than or equal to 0, ", &_7$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_5$$4, "__construct", NULL, 2, &_8$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_5$$4, "tensor/vector.zep", 160);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (lambda == 0.0) {
		ZVAL_DOUBLE(&_9$$5, 0.0);
		ZVAL_LONG(&_10$$5, n);
		ZEPHIR_RETURN_CALL_STATIC("fill", NULL, 0, &_9$$5, &_10$$5);
		zephir_check_call_status();
		RETURN_MM();
	}
	ZEPHIR_INIT_VAR(&a);
	array_init(&a);
	ZVAL_DOUBLE(&_11, -lambda);
	ZEPHIR_CALL_FUNCTION(&_12, "exp", NULL, 9, &_11);
	zephir_check_call_status();
	l =  (zephir_get_doubleval(&_12));
	ZEPHIR_CALL_FUNCTION(&_13, "getrandmax", NULL, 5);
	zephir_check_call_status();
	max = zephir_get_intval(&_13);
	while (1) {
		if (!(zephir_fast_count_int(&a) < n)) {
			break;
		}
		k =  (0.0);
		p =  (1.0);
		while (1) {
			if (!(p > l)) {
				break;
			}
			k++;
			ZEPHIR_CALL_FUNCTION(&_14$$7, "rand", &_15, 6);
			zephir_check_call_status();
			p *=  (zephir_safe_div_zval_long(&_14$$7, max));
		}
		ZEPHIR_INIT_NVAR(&_16$$6);
		ZVAL_DOUBLE(&_16$$6, (k - 1.0));
		zephir_array_append(&a, &_16$$6, PH_SEPARATE, "tensor/vector.zep", 185);
	}
	ZVAL_BOOL(&_11, 0);
	ZEPHIR_RETURN_CALL_STATIC("fromArray", NULL, 0, &a, &_11);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return a random uniformly distributed vector with values between -1 and 1.
 *
 * @param int n
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Vector, uniform)
{
	zval a;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_8 = NULL;
	zval *n_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, _4, _10, _5$$4, _6$$4, _7$$4, _9$$4;
	zend_long n, ZEPHIR_LAST_CALL_STATUS, max;

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_6$$4);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_9$$4);
	ZVAL_UNDEF(&a);
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
		zephir_throw_exception_debug(&_0$$3, "tensor/vector.zep", 202);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&a);
	array_init(&a);
	ZEPHIR_CALL_FUNCTION(&_4, "getrandmax", NULL, 5);
	zephir_check_call_status();
	max = zephir_get_intval(&_4);
	while (1) {
		if (!(zephir_fast_count_int(&a) < n)) {
			break;
		}
		ZVAL_LONG(&_5$$4, -max);
		ZVAL_LONG(&_6$$4, max);
		ZEPHIR_CALL_FUNCTION(&_7$$4, "rand", &_8, 6, &_5$$4, &_6$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_NVAR(&_9$$4);
		ZVAL_DOUBLE(&_9$$4, zephir_safe_div_zval_long(&_7$$4, max));
		zephir_array_append(&a, &_9$$4, PH_SEPARATE, "tensor/vector.zep", 210);
	}
	ZVAL_BOOL(&_10, 0);
	ZEPHIR_RETURN_CALL_STATIC("fromArray", NULL, 0, &a, &_10);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return evenly spaced values within a given interval.
 *
 * @param float start
 * @param float end
 * @param float interval
 * @return self
 */
PHP_METHOD(Tensor_Vector, range)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *start_param = NULL, *end_param = NULL, *interval_param = NULL, _0, _1, _2, _3;
	double start, end, interval;

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ZVAL(start_param)
		Z_PARAM_ZVAL(end_param)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(interval_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 2, 1, &start_param, &end_param, &interval_param);
	start = zephir_get_doubleval(start_param);
	end = zephir_get_doubleval(end_param);
	if (!interval_param) {
		interval = 1.0;
	} else {
		interval = zephir_get_doubleval(interval_param);
	}
	ZVAL_DOUBLE(&_0, start);
	ZVAL_DOUBLE(&_1, end);
	ZVAL_DOUBLE(&_2, interval);
	ZEPHIR_CALL_FUNCTION(&_3, "range", NULL, 10, &_0, &_1, &_2);
	zephir_check_call_status();
	ZVAL_BOOL(&_0, 0);
	ZEPHIR_RETURN_CALL_STATIC("fromArray", NULL, 0, &_3, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return a vector of n evenly spaced numbers between minimum and maximum.
 *
 * @param float min
 * @param float max
 * @param int n
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Vector, linspace)
{
	zval a;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_8 = NULL;
	zend_long n, ZEPHIR_LAST_CALL_STATUS, k;
	zval *min_param = NULL, *max_param = NULL, *n_param = NULL, _0$$4, _1$$4, _2$$4, _3$$4, _4, _5, _6, _7$$5, _9$$5;
	double min, max, interval;

	ZVAL_UNDEF(&_0$$4);
	ZVAL_UNDEF(&_1$$4);
	ZVAL_UNDEF(&_2$$4);
	ZVAL_UNDEF(&_3$$4);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7$$5);
	ZVAL_UNDEF(&_9$$5);
	ZVAL_UNDEF(&a);
	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_ZVAL(min_param)
		Z_PARAM_ZVAL(max_param)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 3, 0, &min_param, &max_param, &n_param);
	min = zephir_get_doubleval(min_param);
	max = zephir_get_doubleval(max_param);
	if (UNEXPECTED(min > max)) {
		ZEPHIR_THROW_EXCEPTION_DEBUG_STR(tensor_exceptions_invalidargumentexception_ce, "Minimum must be less than maximum.", "tensor/vector.zep", 241);
		return;
	}
	if (UNEXPECTED(n < 2)) {
		ZEPHIR_INIT_VAR(&_0$$4);
		object_init_ex(&_0$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_1$$4, n);
		ZEPHIR_CALL_FUNCTION(&_2$$4, "strval", NULL, 3, &_1$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_3$$4);
		ZEPHIR_CONCAT_SSVS(&_3$$4, "Number of elements", " must be greater than 1, ", &_2$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$4, "__construct", NULL, 2, &_3$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$4, "tensor/vector.zep", 246);
		ZEPHIR_MM_RESTORE();
		return;
	}
	k = (n - 1);
	ZVAL_DOUBLE(&_4, (max - min));
	ZEPHIR_CALL_FUNCTION(&_5, "abs", NULL, 11, &_4);
	zephir_check_call_status();
	interval =  (zephir_safe_div_zval_long(&_5, k));
	ZEPHIR_INIT_VAR(&a);
	zephir_create_array(&a, 1, 0);
	ZEPHIR_INIT_VAR(&_6);
	ZVAL_DOUBLE(&_6, min);
	zephir_array_fast_append(&a, &_6);
	while (1) {
		if (!(zephir_fast_count_int(&a) < k)) {
			break;
		}
		ZEPHIR_MAKE_REF(&a);
		ZEPHIR_CALL_FUNCTION(&_7$$5, "end", &_8, 12, &a);
		ZEPHIR_UNREF(&a);
		zephir_check_call_status();
		ZEPHIR_INIT_NVAR(&_9$$5);
		ZVAL_DOUBLE(&_9$$5, (zephir_get_numberval(&_7$$5) + interval));
		zephir_array_append(&a, &_9$$5, PH_SEPARATE, "tensor/vector.zep", 256);
	}
	ZEPHIR_INIT_NVAR(&_6);
	ZVAL_DOUBLE(&_6, max);
	zephir_array_append(&a, &_6, PH_SEPARATE, "tensor/vector.zep", 259);
	ZVAL_BOOL(&_4, 0);
	ZEPHIR_RETURN_CALL_SELF("fromArray", NULL, 0, &a, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Build a new vector from a flat PHP array of numeric elements.
 *
 * @param array a
 * @param bool validate
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Vector, fromArray)
{
	zval _5$$4, _11$$6;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_6 = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zend_bool validate, _8, _3$$3, _9$$5;
	zval *a_param = NULL, *validate_param = NULL, valueA, _0, *_2, _7, buffer, _4$$4, _10$$6;
	zval a, flat, _1;

	ZVAL_UNDEF(&a);
	ZVAL_UNDEF(&flat);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&valueA);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&buffer);
	ZVAL_UNDEF(&_4$$4);
	ZVAL_UNDEF(&_10$$6);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_11$$6);
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
	ZEPHIR_INIT_VAR(&flat);
	array_init(&flat);
	ZEPHIR_CPY_WRT(&_0, &a);
	zephir_get_arrval(&_1, &_0);
	zephir_is_iterable(&_1, 0, "tensor/vector.zep", 287);
	if (Z_TYPE_P(&_1) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(&_1), _2)
		{
			ZEPHIR_INIT_NVAR(&valueA);
			ZVAL_COPY(&valueA, _2);
			_3$$3 = validate;
			if (_3$$3) {
				_3$$3 = Z_TYPE_P(&valueA) == IS_ARRAY;
			}
			if (UNEXPECTED(_3$$3)) {
				ZEPHIR_INIT_NVAR(&_4$$4);
				object_init_ex(&_4$$4, tensor_exceptions_invalidargumentexception_ce);
				ZEPHIR_INIT_NVAR(&_5$$4);
				ZEPHIR_CONCAT_SS(&_5$$4, "Vector requires a", " flat array of numeric elements.");
				ZEPHIR_CALL_METHOD(NULL, &_4$$4, "__construct", &_6, 2, &_5$$4);
				zephir_check_call_status();
				zephir_throw_exception_debug(&_4$$4, "tensor/vector.zep", 281);
				ZEPHIR_MM_RESTORE();
				return;
			}
			zephir_array_append(&flat, &valueA, PH_SEPARATE, "tensor/vector.zep", 284);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, &_1, "rewind", NULL, 0);
		zephir_check_call_status();
		_8 = 1;
		while (1) {
			if (_8) {
				_8 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, &_1, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_7, &_1, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_7)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&valueA, &_1, "current", NULL, 0);
			zephir_check_call_status();
				_9$$5 = validate;
				if (_9$$5) {
					_9$$5 = Z_TYPE_P(&valueA) == IS_ARRAY;
				}
				if (UNEXPECTED(_9$$5)) {
					ZEPHIR_INIT_NVAR(&_10$$6);
					object_init_ex(&_10$$6, tensor_exceptions_invalidargumentexception_ce);
					ZEPHIR_INIT_NVAR(&_11$$6);
					ZEPHIR_CONCAT_SS(&_11$$6, "Vector requires a", " flat array of numeric elements.");
					ZEPHIR_CALL_METHOD(NULL, &_10$$6, "__construct", &_6, 2, &_11$$6);
					zephir_check_call_status();
					zephir_throw_exception_debug(&_10$$6, "tensor/vector.zep", 281);
					ZEPHIR_MM_RESTORE();
					return;
				}
				zephir_array_append(&flat, &valueA, PH_SEPARATE, "tensor/vector.zep", 284);
		}
	}
	ZEPHIR_INIT_NVAR(&valueA);
	ZEPHIR_INIT_VAR(&buffer);
	tensor_buffer_from_array(&buffer, &flat);
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &buffer);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Construct a new vector from a TensorBuffer holding its elements.
 *
 * @param \Tensor\TensorBuffer a
 * @throws \Tensor\Exceptions\InvalidArgumentException
 */
PHP_METHOD(Tensor_Vector, __construct)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *a, a_sub, _0, _1;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&a_sub);
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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(a, zephir_get_internal_ce(SL("tensor\\tensorbuffer")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &a);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_0, 1, a);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, &_0, "count", NULL, 0);
	zephir_check_call_status();
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_1, 2, &_1);
	ZEPHIR_MM_RESTORE();
}

/**
 * Return a tuple with the dimensionality of the tensor.
 *
 * @return int[]
 */
PHP_METHOD(Tensor_Vector, shape)
{
	zval _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	zephir_create_array(return_value, 1, 0);
	zephir_memory_observe(&_0);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
	zephir_array_fast_append(return_value, &_0);
	RETURN_MM();
}

/**
 * Return the shape of the tensor as a string.
 *
 * @return string
 */
PHP_METHOD(Tensor_Vector, shapeString)
{
	zval _1;
	zval _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	zephir_memory_observe(&_0);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
	zephir_cast_to_string(&_1, &_0);
	RETURN_CTOR(&_1);
}

/**
 * Return the number of elements in the vector.
 *
 * @return int
 */
PHP_METHOD(Tensor_Vector, size)
{

	RETURN_MEMBER_TYPED(getThis(), "n", IS_LONG);
}

/**
 * Return the number of rows in the vector.
 *
 * @return int
 */
PHP_METHOD(Tensor_Vector, m)
{

	RETURN_LONG(1);
}

/**
 * Return the number of columns in the vector.
 *
 * @return int
 */
PHP_METHOD(Tensor_Vector, n)
{

	RETURN_MEMBER_TYPED(getThis(), "n", IS_LONG);
}

/**
 * Return the vector as an array.
 *
 * @return list<float>
 */
PHP_METHOD(Tensor_Vector, asArray)
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

	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_METHOD(&_0, "toArray", NULL, 0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the underlying TensorBuffer of the vector.
 *
 * @return \Tensor\TensorBuffer
 */
PHP_METHOD(Tensor_Vector, asTensorBuffer)
{

	RETURN_MEMBER(getThis(), "a");
}

/**
 * Return this vector as a row matrix.
 *
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, asRowMatrix)
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

	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_1, 2, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_2, 1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &_0, &_2, &_1);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return this vector as a column matrix.
 *
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, asColumnMatrix)
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

	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_1, 2, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_2, 1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &_0, &_1, &_2);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return a matrix in the shape specified.
 *
 * @param int m
 * @param int n
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, reshape)
{
	zval _2$$3;
	zend_bool _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *m_param = NULL, *n_param = NULL, _3, _8, _9, _10, _1$$3, _4$$4, _5$$4, _6$$4, _7$$4;
	zend_long m, n, ZEPHIR_LAST_CALL_STATUS, nHat;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_4$$4);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_6$$4);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_2$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(m)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 2, 0, &m_param, &n_param);
	_0 = m < 0;
	if (!(_0)) {
		_0 = n < 0;
	}
	if (UNEXPECTED(_0)) {
		ZEPHIR_INIT_VAR(&_1$$3);
		object_init_ex(&_1$$3, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_INIT_VAR(&_2$$3);
		ZEPHIR_CONCAT_SS(&_2$$3, "The number of rows", " and/or columns cannot be less than 0.");
		ZEPHIR_CALL_METHOD(NULL, &_1$$3, "__construct", NULL, 2, &_2$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_1$$3, "tensor/vector.zep", 406);
		ZEPHIR_MM_RESTORE();
		return;
	}
	nHat = (m * n);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_LONG_IDENTICAL(&_3, nHat))) {
		ZEPHIR_INIT_VAR(&_4$$4);
		object_init_ex(&_4$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_5$$4, nHat);
		ZEPHIR_CALL_FUNCTION(&_6$$4, "strval", NULL, 3, &_5$$4);
		zephir_check_call_status();
		zephir_read_property_cached(&_5$$4, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
		ZEPHIR_INIT_VAR(&_7$$4);
		ZEPHIR_CONCAT_VSSVS(&_7$$4, &_6$$4, " elements", " are needed but vector only has ", &_5$$4, ".");
		ZEPHIR_CALL_METHOD(NULL, &_4$$4, "__construct", NULL, 2, &_7$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_4$$4, "tensor/vector.zep", 413);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_9, m);
	ZVAL_LONG(&_10, n);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &_8, &_9, &_10);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Transpose the vector i.e. rotate it.
 *
 * @return mixed
 */
PHP_METHOD(Tensor_Vector, transpose)
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

	object_init_ex(return_value, tensor_columnvector_ce);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Map a function over the elements in the vector and return a new vector.
 *
 * @internal
 *
 * @param callable callback
 * @return self
 */
PHP_METHOD(Tensor_Vector, map)
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
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(callback)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &callback);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, &_0, "toArray", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_FUNCTION(&_2, "array_map", NULL, 15, callback, &_1);
	zephir_check_call_status();
	ZVAL_BOOL(&_3, 0);
	ZEPHIR_RETURN_CALL_STATIC("fromArray", NULL, 0, &_2, &_3);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Reduce the vector down to a scalar.
 *
 * @internal
 *
 * @param callable callback function (float carry, float value): float
 * @param float initial
 * @return float
 */
PHP_METHOD(Tensor_Vector, reduce)
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
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, &_0, "toArray", NULL, 0);
	zephir_check_call_status();
	ZVAL_DOUBLE(&_2, initial);
	ZEPHIR_RETURN_CALL_FUNCTION("array_reduce", NULL, 16, &_1, callback, &_2);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Compute the dot product of this vector and another vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return float
 */
PHP_METHOD(Tensor_Vector, dot)
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "size", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "size", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A expects ", &_4$$3, " elements but vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 468);
		ZEPHIR_MM_RESTORE();
		return;
	}
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	tensor_dot(return_value, &_8, &_9);
	RETURN_MM();
}

/**
 * Return the 1D convolution of this vector and a kernel vector with given stride.
 *
 * @param \Tensor\Vector b
 * @param int stride
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Vector, convolve)
{
	zval _3$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long stride, ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, *stride_param = NULL, _0, _1, _8, _9, _10, _11, _2$$3, _4$$4, _5$$4, _6$$4, _7$$4;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_4$$4);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_6$$4);
	ZVAL_UNDEF(&_7$$4);
	ZVAL_UNDEF(&_3$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
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
	ZEPHIR_CALL_METHOD(&_0, b, "size", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(ZEPHIR_GT(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SS(&_3$$3, "Vector B cannot be", " larger than Vector A.");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 486);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (UNEXPECTED(stride < 1)) {
		ZEPHIR_INIT_VAR(&_4$$4);
		object_init_ex(&_4$$4, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_5$$4, stride);
		ZEPHIR_CALL_FUNCTION(&_6$$4, "strval", NULL, 3, &_5$$4);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_7$$4);
		ZEPHIR_CONCAT_SSVS(&_7$$4, "Stride cannot be", " less than 1, ", &_6$$4, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_4$$4, "__construct", NULL, 2, &_7$$4);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_4$$4, "tensor/vector.zep", 491);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_11, stride);
	tensor_convolve_1d(&_8, &_9, &_10, &_11);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_8);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Compute the vector-matrix product of this vector and matrix b.
 *
 * @param \Tensor\Matrix b
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, matmul)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_CALL_METHOD(&_0, this_ptr, "asRowMatrix", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_RETURN_CALL_METHOD(&_0, "matmul", NULL, 0, b);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the inner product of two vectors.
 *
 * @param \Tensor\Vector b
 * @return float
 */
PHP_METHOD(Tensor_Vector, inner)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_RETURN_CALL_METHOD(this_ptr, "dot", NULL, 0, b);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Calculate the outer product of this and another vector.
 *
 * @param \Tensor\Vector b
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, outer)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, result, _0, _1, _2, _3, _4, _5;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_2, this_ptr, _zephir_prop_1, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_3, b, "n", NULL, 0);
	zephir_check_call_status();
	tensor_outer(&result, &_0, &_1, &_2, &_3);
	object_init_ex(return_value, tensor_matrix_ce);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_5, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &result, &_4, &_5);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Calculate the L1 or Manhattan norm of the vector.
 *
 * @return float
 */
PHP_METHOD(Tensor_Vector, l1Norm)
{
	zval _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_CALL_METHOD(&_0, this_ptr, "abs", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_RETURN_CALL_METHOD(&_0, "sum", NULL, 0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Calculate the L2 or Euclidean norm of the vector.
 *
 * @return float
 */
PHP_METHOD(Tensor_Vector, l2Norm)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_CALL_METHOD(&_0, this_ptr, "square", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_1, &_0, "sum", NULL, 0);
	zephir_check_call_status();
	RETURN_MM_DOUBLE(zephir_sqrt(&_1));
}

/**
 * Calculate the p-norm of the vector.
 *
 * @param float p
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return float
 */
PHP_METHOD(Tensor_Vector, pNorm)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *p_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, _4, _5, _6, _7;
	double p;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(p_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 0, 1, &p_param);
	if (!p_param) {
		p = 3.0;
	} else {
		p = zephir_get_doubleval(p_param);
	}
	if (UNEXPECTED(p <= 0.0)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		object_init_ex(&_0$$3, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_DOUBLE(&_1$$3, p);
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", NULL, 3, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SSVS(&_3$$3, "P must be greater", " than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/vector.zep", 563);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_CALL_METHOD(&_4, this_ptr, "abs", NULL, 0);
	zephir_check_call_status();
	ZVAL_DOUBLE(&_6, p);
	ZEPHIR_CALL_METHOD(&_5, &_4, "powScalar", NULL, 0, &_6);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_7, &_5, "sum", NULL, 0);
	zephir_check_call_status();
	ZVAL_DOUBLE(&_6, zephir_safe_div_double_double(1.0, p));
	ZEPHIR_RETURN_CALL_FUNCTION("pow", NULL, 17, &_7, &_6);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Calculate the max norm of the vector.
 *
 * @return float
 */
PHP_METHOD(Tensor_Vector, maxNorm)
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
	RETURN_MM_DOUBLE(zephir_get_doubleval(&_1));
}

/**
 * A universal function to multiply this vector with another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Vector, multiply)
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
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_1; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "multiplyMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
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
	ZEPHIR_CONCAT_SS(&_3, "Cannot multiply", " vector by the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/vector.zep", 606);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to divide this vector by another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Vector, divide)
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
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_1; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "divideMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
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
	ZEPHIR_CONCAT_SS(&_3, "Cannot divide", " vector by the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/vector.zep", 636);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to add this vector with another tensor
 * element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Vector, add)
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
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_1; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "addMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
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
	ZEPHIR_CONCAT_SS(&_3, "Cannot add", " vector by the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/vector.zep", 667);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to subtract this vector from another tensor
 * element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Vector, subtract)
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
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_1; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "subtractMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
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
	ZEPHIR_CONCAT_SS(&_3, "Cannot subtract", " vector from the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/vector.zep", 698);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to raise this vector to the power of another
 * tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Vector, pow)
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
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_1; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "powMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
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
	ZEPHIR_CONCAT_SS(&_3, "Cannot raise", " vector to the power of the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/vector.zep", 729);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to compute the modulus of this vector and
 * another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Vector, mod)
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
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_1; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "modMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
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
	ZEPHIR_CONCAT_SS(&_3, "Cannot mod", " vector with the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/vector.zep", 760);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to compute the equality comparison of
 * this vector and another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Vector, equal)
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
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_1; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "equalMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
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
	ZEPHIR_CONCAT_SS(&_3, "Cannot compare", " vector to the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/vector.zep", 791);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to compute the not equal comparison of
 * this vector and another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Vector, notEqual)
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
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_1; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "notEqualMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
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
	ZEPHIR_CONCAT_SS(&_3, "Cannot compare", " vector to the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/vector.zep", 822);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to compute the greater than comparison of
 * this vector and another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Vector, greater)
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
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_1; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "greaterMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
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
	ZEPHIR_CONCAT_SS(&_3, "Cannot compare", " vector to the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/vector.zep", 853);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to compute the greater than or equal to
 * comparison of this vector and another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Vector, greaterEqual)
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
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_1; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "greaterEqualMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
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
	ZEPHIR_CONCAT_SS(&_3, "Cannot compare", " vector to the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/vector.zep", 884);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to compute the less than comparison of
 * this vector and another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Vector, less)
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
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_1; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "lessMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
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
	ZEPHIR_CONCAT_SS(&_3, "Cannot compare", " vector to the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/vector.zep", 915);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * A universal function to compute the less than or equal to
 * comparison of this vector and another tensor element-wise.
 *
 * @param mixed b
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return mixed
 */
PHP_METHOD(Tensor_Vector, lessEqual)
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
		if (_1$$3 == zephir_instance_of_ev(b, tensor_vector_ce)) { goto zephir_switch_1_clause_1; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			ZEPHIR_RETURN_CALL_METHOD(this_ptr, "lessEqualMatrix", NULL, 0, b);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_clause_1: ;
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
	ZEPHIR_CONCAT_SS(&_3, "Cannot compare", " vector to the given input.");
	ZEPHIR_CALL_METHOD(NULL, &_2, "__construct", NULL, 2, &_3);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_2, "tensor/vector.zep", 946);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * Return the reciprocal of the vector element-wise.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, reciprocal)
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

	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_STATIC(&_0, "ones", NULL, 0, &_1);
	zephir_check_call_status();
	ZEPHIR_RETURN_CALL_METHOD(&_0, "divideVector", NULL, 0, this_ptr);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the absolute value of the vector.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, abs)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_abs(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Square the vector.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, square)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_RETURN_CALL_METHOD(this_ptr, "multiplyVector", NULL, 0, this_ptr);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Take the square root of the vector.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, sqrt)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_sqrt(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Exponentiate each element in the vector.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, exp)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_exp(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the exponential of the vector minus 1.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, expm1)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_expm1(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Take the log to the given base of each element in the vector.
 *
 * @param float base
 * @return self
 */
PHP_METHOD(Tensor_Vector, log)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *base_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, _4$$4, _5$$4, _6, _7, _8;
	double base;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$4);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
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
	if (UNEXPECTED(base <= 0.0)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		object_init_ex(&_0$$3, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_DOUBLE(&_1$$3, base);
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", NULL, 3, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SSVS(&_3$$3, "Log base must be greater", " than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/vector.zep", 1020);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (base == 2.7182818284590452354) {
		object_init_ex(return_value, zend_get_called_scope(execute_data));
		ZEPHIR_INIT_VAR(&_4$$4);
		zephir_read_property_cached(&_5$$4, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
		tensor_log(&_4$$4, &_5$$4);
		ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_4$$4);
		zephir_check_call_status();
		RETURN_MM();
	}
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_6);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_8, base);
	tensor_log_base(&_6, &_7, &_8);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_6);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the log of 1 plus the tensor i.e. a transform.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, log1p)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_log1p(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the sine of this vector.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, sin)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_sin(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Compute the arc sine of the vector.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, asin)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_asin(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the cosine of this vector.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, cos)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_cos(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Compute the arc cosine of the vector.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, acos)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_acos(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the tangent of this vector.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, tan)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_tan(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Compute the arc tangent of the vector.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, atan)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_atan(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Convert angles from radians to degrees.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, rad2deg)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_rad2deg(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Convert angles from degrees to radians.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, deg2rad)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_deg2rad(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * The sum of the vector.
 *
 * @return float
 */
PHP_METHOD(Tensor_Vector, sum)
{
	zval _0;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_sum(return_value, &_0);
	return;
}

/**
 * Return the product of the vector.
 *
 * @return float
 */
PHP_METHOD(Tensor_Vector, product)
{
	zval _0;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_product(return_value, &_0);
	return;
}

/**
 * Return the minimum element in the vector.
 *
 * @return float
 */
PHP_METHOD(Tensor_Vector, min)
{
	zval _0;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_min(return_value, &_0);
	return;
}

/**
 * Return the maximum element in the vector.
 *
 * @return float
 */
PHP_METHOD(Tensor_Vector, max)
{
	zval _0;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_max(return_value, &_0);
	return;
}

/**
 * Return the index of the minimum element in the vector.
 *
 * @return int
 */
PHP_METHOD(Tensor_Vector, argmin)
{
	zval _0;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_argmin(return_value, &_0);
	return;
}

/**
 * Return the index of the maximum element in the vector.
 *
 * @return int
 */
PHP_METHOD(Tensor_Vector, argmax)
{
	zval _0;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_buffer_argmax(return_value, &_0);
	return;
}

/**
 * Return the mean of the vector.
 *
 * @return float
 */
PHP_METHOD(Tensor_Vector, mean)
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
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	div_function(return_value, &_0, &_1);
	RETURN_MM();
}

/**
 * Return the median of the vector.
 *
 * @return float
 */
PHP_METHOD(Tensor_Vector, median)
{
	zval median, _0, _1, _2, a, _3, _4$$4, _5$$4, _6$$4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS, mid;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&median);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&a);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4$$4);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_6$$4);
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

	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_1, 2);
	ZEPHIR_CALL_FUNCTION(&_2, "intdiv", NULL, 18, &_0, &_1);
	zephir_check_call_status();
	mid = zephir_get_intval(&_2);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&a, &_1, "toArray", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_MAKE_REF(&a);
	ZEPHIR_CALL_FUNCTION(NULL, "sort", NULL, 19, &a);
	ZEPHIR_UNREF(&a);
	zephir_check_call_status();
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	if (zephir_safe_mod_zval_long(&_3, 2) == 1) {
		zephir_memory_observe(&median);
		zephir_array_fetch_long(&median, &a, mid, PH_NOISY, "tensor/vector.zep", 1206);
	} else {
		zephir_memory_observe(&_4$$4);
		zephir_array_fetch_long(&_4$$4, &a, (mid - 1), PH_NOISY, "tensor/vector.zep", 1208);
		zephir_memory_observe(&_5$$4);
		zephir_array_fetch_long(&_5$$4, &a, mid, PH_NOISY, "tensor/vector.zep", 1208);
		ZEPHIR_INIT_VAR(&_6$$4);
		zephir_add_function(&_6$$4, &_4$$4, &_5$$4);
		ZEPHIR_INIT_NVAR(&median);
		ZVAL_DOUBLE(&median, zephir_safe_div_zval_double(&_6$$4, 2.0));
	}
	RETURN_CCTOR(&median);
}

/**
 * Return the q'th quantile of the vector.
 *
 * @param float q
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return float
 */
PHP_METHOD(Tensor_Vector, quantile)
{
	zend_bool _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS, xHat;
	zval *q_param = NULL, a, _5, _6, _7, _10, _11, _1$$3, _2$$3, _3$$3, _4$$3, _8$$4, _9$$4;
	double q, x, remainder, t;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&a);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_8$$4);
	ZVAL_UNDEF(&_9$$4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
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
		ZEPHIR_CONCAT_SSVS(&_4$$3, "Q must be", " between 0 and 1, ", &_3$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_1$$3, "__construct", NULL, 2, &_4$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_1$$3, "tensor/vector.zep", 1225);
		ZEPHIR_MM_RESTORE();
		return;
	}
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&a, &_5, "toArray", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_MAKE_REF(&a);
	ZEPHIR_CALL_FUNCTION(NULL, "sort", NULL, 19, &a);
	ZEPHIR_UNREF(&a);
	zephir_check_call_status();
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_1, 2, PH_NOISY_CC | PH_READONLY);
	x =  (((q * (double) ((zephir_get_numberval(&_6) - 1))) + (double) (1)));
	xHat = (int) x;
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_1, 2, PH_NOISY_CC | PH_READONLY);
	if (ZEPHIR_LE_LONG(&_7, xHat)) {
		zephir_memory_observe(&_8$$4);
		zephir_read_property_cached(&_9$$4, this_ptr, _zephir_prop_1, 2, PH_NOISY_CC | PH_READONLY);
		zephir_array_fetch_long(&_8$$4, &a, (zephir_get_numberval(&_9$$4) - 1), PH_NOISY, "tensor/vector.zep", 1237);
		RETURN_MM_DOUBLE(zephir_get_doubleval(&_8$$4));
	}
	remainder =  ((x -  (double) xHat));
	zephir_memory_observe(&_10);
	zephir_array_fetch_long(&_10, &a, (xHat - 1), PH_NOISY, "tensor/vector.zep", 1242);
	t =  (zephir_get_doubleval(&_10));
	zephir_memory_observe(&_11);
	zephir_array_fetch_long(&_11, &a, xHat, PH_NOISY, "tensor/vector.zep", 1244);
	RETURN_MM_DOUBLE((t + (remainder * (zephir_get_numberval(&_11) - t))));
}

/**
 * Return the variance of the vector.
 *
 * @param mixed mean
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return float
 */
PHP_METHOD(Tensor_Vector, variance)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *mean = NULL, mean_sub, __$null, _0$$3, _1$$4, _2$$4, _3$$4, ssd, _4, _5, _6;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&mean_sub);
	ZVAL_NULL(&__$null);
	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$4);
	ZVAL_UNDEF(&_2$$4);
	ZVAL_UNDEF(&_3$$4);
	ZVAL_UNDEF(&ssd);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
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
		ZEPHIR_CALL_FUNCTION(&_0$$3, "is_float", NULL, 1, mean);
		zephir_check_call_status();
		if (UNEXPECTED(!zephir_is_true(&_0$$3))) {
			ZEPHIR_INIT_VAR(&_1$$4);
			object_init_ex(&_1$$4, tensor_exceptions_invalidargumentexception_ce);
			ZEPHIR_INIT_VAR(&_2$$4);
			zephir_gettype(&_2$$4, mean);
			ZEPHIR_INIT_VAR(&_3$$4);
			ZEPHIR_CONCAT_SSVS(&_3$$4, "Mean scalar must be", " a floating point number ", &_2$$4, " given.");
			ZEPHIR_CALL_METHOD(NULL, &_1$$4, "__construct", NULL, 2, &_3$$4);
			zephir_check_call_status();
			zephir_throw_exception_debug(&_1$$4, "tensor/vector.zep", 1259);
			ZEPHIR_MM_RESTORE();
			return;
		}
	} else {
		ZEPHIR_CALL_METHOD(mean, this_ptr, "mean", NULL, 0);
		zephir_check_call_status();
	}
	ZEPHIR_CALL_METHOD(&_4, this_ptr, "subtractScalar", NULL, 0, mean);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_5, &_4, "square", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&ssd, &_5, "sum", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	div_function(return_value, &ssd, &_6);
	RETURN_MM();
}

/**
 * Round the elements in the matrix to a given decimal place.
 *
 * @param int precision
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Vector, round)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *precision_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, _4, _5, _6;
	zend_long precision, ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
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
	if (UNEXPECTED(precision < 0)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		object_init_ex(&_0$$3, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_1$$3, precision);
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", NULL, 3, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SSVS(&_3$$3, "Decimal precision cannot", " be less than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/vector.zep", 1283);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_4);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_LONG(&_6, precision);
	tensor_round(&_4, &_5, &_6);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_4);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Round the elements in the vector down to the nearest integer.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, floor)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_floor(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Round the elements in the vector up to the nearest integer.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, ceil)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_ceil(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Clip the elements in the vector to be between given minimum and maximum
 * and return a new vector.
 *
 * @param float min
 * @param float max
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return self
 */
PHP_METHOD(Tensor_Vector, clip)
{
	zval _1$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *min_param = NULL, *max_param = NULL, _0$$3, _2, _3, _4, _5;
	double min, max;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_3);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_1$$3);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
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
		zephir_throw_exception_debug(&_0$$3, "tensor/vector.zep", 1322);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_2);
	zephir_read_property_cached(&_3, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_4, min);
	ZVAL_DOUBLE(&_5, max);
	tensor_clip(&_2, &_3, &_4, &_5);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_2);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Clip the tensor to be lower bounded by a given minimum.
 *
 * @param float min
 * @return self
 */
PHP_METHOD(Tensor_Vector, clipLower)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *min_param = NULL, _0, _1, _2;
	double min;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(min_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &min_param);
	min = zephir_get_doubleval(min_param);
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, min);
	tensor_clip_lower(&_0, &_1, &_2);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Clip the tensor to be upper bounded by a given maximum.
 *
 * @param float max
 * @return self
 */
PHP_METHOD(Tensor_Vector, clipUpper)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *max_param = NULL, _0, _1, _2;
	double max;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(max_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &max_param);
	max = zephir_get_doubleval(max_param);
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, max);
	tensor_clip_upper(&_0, &_1, &_2);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise sign indication.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, sign)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_sign(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Negate the vector i.e take the negative of each value elementwise.
 *
 * @return self
 */
PHP_METHOD(Tensor_Vector, negate)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	tensor_negate(&_0, &_1);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Multiply this vector with a matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, multiplyMatrix)
{
	zval _4$$3, _6$$3, _7$$3;
	zend_bool _15;
	zval c;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, rowB, _8, *_9, _10, *_11, _14, _18, _19, _20, _2$$3, _3$$3, _5$$3, _12$$4, _13$$4, _16$$5, _17$$5;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&rowB);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19);
	ZVAL_UNDEF(&_20);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_12$$4);
	ZVAL_UNDEF(&_13$$4);
	ZVAL_UNDEF(&_16$$5);
	ZVAL_UNDEF(&_17$$5);
	ZVAL_UNDEF(&c);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "n", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A expects ", &_4$$3, " columns but Matrix B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1382);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&c);
	array_init(&c);
	ZEPHIR_CALL_METHOD(&_8, b, "asRowBuffers", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_8) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_10);
		zephir_string_to_char_array(&_10, &_8);
		_9 = &_10;
	} else {
		_9 = &_8;
	}
	zephir_is_iterable(_9, 0, "tensor/vector.zep", 1393);
	if (Z_TYPE_P(_9) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_9), _11)
		{
			ZEPHIR_INIT_NVAR(&rowB);
			ZVAL_COPY(&rowB, _11);
			ZEPHIR_INIT_NVAR(&_12$$4);
			zephir_read_property_cached(&_13$$4, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
			tensor_multiply(&_12$$4, &_13$$4, &rowB);
			zephir_array_append(&c, &_12$$4, PH_SEPARATE, "tensor/vector.zep", 1390);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _9, "rewind", NULL, 0);
		zephir_check_call_status();
		_15 = 1;
		while (1) {
			if (_15) {
				_15 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _9, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_14, _9, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_14)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowB, _9, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_16$$5);
				zephir_read_property_cached(&_17$$5, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
				tensor_multiply(&_16$$5, &_17$$5, &rowB);
				zephir_array_append(&c, &_16$$5, PH_SEPARATE, "tensor/vector.zep", 1390);
		}
	}
	ZEPHIR_INIT_NVAR(&rowB);
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_CALL_CE_STATIC(&_18, tensor_tensorbuffer_ce, "fromBuffers", NULL, 0, &c);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_19, b, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_20, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &_18, &_19, &_20);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Divide this vector with a matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, divideMatrix)
{
	zval _4$$3, _6$$3, _7$$3;
	zend_bool _15;
	zval c;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, rowB, _8, *_9, _10, *_11, _14, _18, _19, _20, _2$$3, _3$$3, _5$$3, _12$$4, _13$$4, _16$$5, _17$$5;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&rowB);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19);
	ZVAL_UNDEF(&_20);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_12$$4);
	ZVAL_UNDEF(&_13$$4);
	ZVAL_UNDEF(&_16$$5);
	ZVAL_UNDEF(&_17$$5);
	ZVAL_UNDEF(&c);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "n", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A expects ", &_4$$3, " columns but Matrix B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1408);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&c);
	array_init(&c);
	ZEPHIR_CALL_METHOD(&_8, b, "asRowBuffers", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_8) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_10);
		zephir_string_to_char_array(&_10, &_8);
		_9 = &_10;
	} else {
		_9 = &_8;
	}
	zephir_is_iterable(_9, 0, "tensor/vector.zep", 1419);
	if (Z_TYPE_P(_9) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_9), _11)
		{
			ZEPHIR_INIT_NVAR(&rowB);
			ZVAL_COPY(&rowB, _11);
			ZEPHIR_INIT_NVAR(&_12$$4);
			zephir_read_property_cached(&_13$$4, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
			tensor_divide(&_12$$4, &_13$$4, &rowB);
			zephir_array_append(&c, &_12$$4, PH_SEPARATE, "tensor/vector.zep", 1416);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _9, "rewind", NULL, 0);
		zephir_check_call_status();
		_15 = 1;
		while (1) {
			if (_15) {
				_15 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _9, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_14, _9, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_14)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowB, _9, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_16$$5);
				zephir_read_property_cached(&_17$$5, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
				tensor_divide(&_16$$5, &_17$$5, &rowB);
				zephir_array_append(&c, &_16$$5, PH_SEPARATE, "tensor/vector.zep", 1416);
		}
	}
	ZEPHIR_INIT_NVAR(&rowB);
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_CALL_CE_STATIC(&_18, tensor_tensorbuffer_ce, "fromBuffers", NULL, 0, &c);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_19, b, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_20, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &_18, &_19, &_20);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Add this vector to a matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, addMatrix)
{
	zval _4$$3, _6$$3, _7$$3;
	zend_bool _15;
	zval c;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, rowB, _8, *_9, _10, *_11, _14, _18, _19, _20, _2$$3, _3$$3, _5$$3, _12$$4, _13$$4, _16$$5, _17$$5;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&rowB);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19);
	ZVAL_UNDEF(&_20);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_12$$4);
	ZVAL_UNDEF(&_13$$4);
	ZVAL_UNDEF(&_16$$5);
	ZVAL_UNDEF(&_17$$5);
	ZVAL_UNDEF(&c);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "n", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A expects ", &_4$$3, " columns but Matrix B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1434);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&c);
	array_init(&c);
	ZEPHIR_CALL_METHOD(&_8, b, "asRowBuffers", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_8) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_10);
		zephir_string_to_char_array(&_10, &_8);
		_9 = &_10;
	} else {
		_9 = &_8;
	}
	zephir_is_iterable(_9, 0, "tensor/vector.zep", 1445);
	if (Z_TYPE_P(_9) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_9), _11)
		{
			ZEPHIR_INIT_NVAR(&rowB);
			ZVAL_COPY(&rowB, _11);
			ZEPHIR_INIT_NVAR(&_12$$4);
			zephir_read_property_cached(&_13$$4, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
			tensor_add(&_12$$4, &_13$$4, &rowB);
			zephir_array_append(&c, &_12$$4, PH_SEPARATE, "tensor/vector.zep", 1442);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _9, "rewind", NULL, 0);
		zephir_check_call_status();
		_15 = 1;
		while (1) {
			if (_15) {
				_15 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _9, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_14, _9, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_14)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowB, _9, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_16$$5);
				zephir_read_property_cached(&_17$$5, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
				tensor_add(&_16$$5, &_17$$5, &rowB);
				zephir_array_append(&c, &_16$$5, PH_SEPARATE, "tensor/vector.zep", 1442);
		}
	}
	ZEPHIR_INIT_NVAR(&rowB);
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_CALL_CE_STATIC(&_18, tensor_tensorbuffer_ce, "fromBuffers", NULL, 0, &c);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_19, b, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_20, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &_18, &_19, &_20);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Subtract a matrix from this vector.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, subtractMatrix)
{
	zval _4$$3, _6$$3, _7$$3;
	zend_bool _15;
	zval c;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, rowB, _8, *_9, _10, *_11, _14, _18, _19, _20, _2$$3, _3$$3, _5$$3, _12$$4, _13$$4, _16$$5, _17$$5;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&rowB);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19);
	ZVAL_UNDEF(&_20);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_12$$4);
	ZVAL_UNDEF(&_13$$4);
	ZVAL_UNDEF(&_16$$5);
	ZVAL_UNDEF(&_17$$5);
	ZVAL_UNDEF(&c);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "n", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A expects ", &_4$$3, " columns but Matrix B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1460);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&c);
	array_init(&c);
	ZEPHIR_CALL_METHOD(&_8, b, "asRowBuffers", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_8) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_10);
		zephir_string_to_char_array(&_10, &_8);
		_9 = &_10;
	} else {
		_9 = &_8;
	}
	zephir_is_iterable(_9, 0, "tensor/vector.zep", 1471);
	if (Z_TYPE_P(_9) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_9), _11)
		{
			ZEPHIR_INIT_NVAR(&rowB);
			ZVAL_COPY(&rowB, _11);
			ZEPHIR_INIT_NVAR(&_12$$4);
			zephir_read_property_cached(&_13$$4, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
			tensor_subtract(&_12$$4, &_13$$4, &rowB);
			zephir_array_append(&c, &_12$$4, PH_SEPARATE, "tensor/vector.zep", 1468);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _9, "rewind", NULL, 0);
		zephir_check_call_status();
		_15 = 1;
		while (1) {
			if (_15) {
				_15 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _9, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_14, _9, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_14)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowB, _9, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_16$$5);
				zephir_read_property_cached(&_17$$5, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
				tensor_subtract(&_16$$5, &_17$$5, &rowB);
				zephir_array_append(&c, &_16$$5, PH_SEPARATE, "tensor/vector.zep", 1468);
		}
	}
	ZEPHIR_INIT_NVAR(&rowB);
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_CALL_CE_STATIC(&_18, tensor_tensorbuffer_ce, "fromBuffers", NULL, 0, &c);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_19, b, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_20, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &_18, &_19, &_20);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Raise this vector to the power of a matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, powMatrix)
{
	zval _4$$3, _6$$3, _7$$3;
	zend_bool _15;
	zval c;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, rowB, _8, *_9, _10, *_11, _14, _18, _19, _20, _2$$3, _3$$3, _5$$3, _12$$4, _13$$4, _16$$5, _17$$5;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&rowB);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19);
	ZVAL_UNDEF(&_20);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_12$$4);
	ZVAL_UNDEF(&_13$$4);
	ZVAL_UNDEF(&_16$$5);
	ZVAL_UNDEF(&_17$$5);
	ZVAL_UNDEF(&c);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "n", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A expects ", &_4$$3, " columns but Matrix B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1486);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&c);
	array_init(&c);
	ZEPHIR_CALL_METHOD(&_8, b, "asRowBuffers", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_8) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_10);
		zephir_string_to_char_array(&_10, &_8);
		_9 = &_10;
	} else {
		_9 = &_8;
	}
	zephir_is_iterable(_9, 0, "tensor/vector.zep", 1497);
	if (Z_TYPE_P(_9) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_9), _11)
		{
			ZEPHIR_INIT_NVAR(&rowB);
			ZVAL_COPY(&rowB, _11);
			ZEPHIR_INIT_NVAR(&_12$$4);
			zephir_read_property_cached(&_13$$4, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
			tensor_pow(&_12$$4, &_13$$4, &rowB);
			zephir_array_append(&c, &_12$$4, PH_SEPARATE, "tensor/vector.zep", 1494);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _9, "rewind", NULL, 0);
		zephir_check_call_status();
		_15 = 1;
		while (1) {
			if (_15) {
				_15 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _9, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_14, _9, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_14)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowB, _9, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_16$$5);
				zephir_read_property_cached(&_17$$5, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
				tensor_pow(&_16$$5, &_17$$5, &rowB);
				zephir_array_append(&c, &_16$$5, PH_SEPARATE, "tensor/vector.zep", 1494);
		}
	}
	ZEPHIR_INIT_NVAR(&rowB);
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_CALL_CE_STATIC(&_18, tensor_tensorbuffer_ce, "fromBuffers", NULL, 0, &c);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_19, b, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_20, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &_18, &_19, &_20);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Mod this vector with a matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, modMatrix)
{
	zval _4$$3, _6$$3, _7$$3;
	zend_bool _15;
	zval c;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, rowB, _8, *_9, _10, *_11, _14, _18, _19, _20, _2$$3, _3$$3, _5$$3, _12$$4, _13$$4, _16$$5, _17$$5;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&rowB);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19);
	ZVAL_UNDEF(&_20);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_12$$4);
	ZVAL_UNDEF(&_13$$4);
	ZVAL_UNDEF(&_16$$5);
	ZVAL_UNDEF(&_17$$5);
	ZVAL_UNDEF(&c);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "n", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A expects ", &_4$$3, " columns but Matrix B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1512);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&c);
	array_init(&c);
	ZEPHIR_CALL_METHOD(&_8, b, "asRowBuffers", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_8) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_10);
		zephir_string_to_char_array(&_10, &_8);
		_9 = &_10;
	} else {
		_9 = &_8;
	}
	zephir_is_iterable(_9, 0, "tensor/vector.zep", 1523);
	if (Z_TYPE_P(_9) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_9), _11)
		{
			ZEPHIR_INIT_NVAR(&rowB);
			ZVAL_COPY(&rowB, _11);
			ZEPHIR_INIT_NVAR(&_12$$4);
			zephir_read_property_cached(&_13$$4, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
			tensor_mod(&_12$$4, &_13$$4, &rowB);
			zephir_array_append(&c, &_12$$4, PH_SEPARATE, "tensor/vector.zep", 1520);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _9, "rewind", NULL, 0);
		zephir_check_call_status();
		_15 = 1;
		while (1) {
			if (_15) {
				_15 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _9, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_14, _9, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_14)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowB, _9, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_16$$5);
				zephir_read_property_cached(&_17$$5, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
				tensor_mod(&_16$$5, &_17$$5, &rowB);
				zephir_array_append(&c, &_16$$5, PH_SEPARATE, "tensor/vector.zep", 1520);
		}
	}
	ZEPHIR_INIT_NVAR(&rowB);
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_CALL_CE_STATIC(&_18, tensor_tensorbuffer_ce, "fromBuffers", NULL, 0, &c);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_19, b, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_20, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &_18, &_19, &_20);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise equality comparison of this vector and a matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, equalMatrix)
{
	zval _4$$3, _6$$3, _7$$3;
	zend_bool _15;
	zval c;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, rowB, _8, *_9, _10, *_11, _14, _18, _19, _20, _2$$3, _3$$3, _5$$3, _12$$4, _13$$4, _16$$5, _17$$5;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&rowB);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19);
	ZVAL_UNDEF(&_20);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_12$$4);
	ZVAL_UNDEF(&_13$$4);
	ZVAL_UNDEF(&_16$$5);
	ZVAL_UNDEF(&_17$$5);
	ZVAL_UNDEF(&c);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "n", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A expects ", &_4$$3, " columns but Matrix B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1538);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&c);
	array_init(&c);
	ZEPHIR_CALL_METHOD(&_8, b, "asRowBuffers", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_8) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_10);
		zephir_string_to_char_array(&_10, &_8);
		_9 = &_10;
	} else {
		_9 = &_8;
	}
	zephir_is_iterable(_9, 0, "tensor/vector.zep", 1549);
	if (Z_TYPE_P(_9) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_9), _11)
		{
			ZEPHIR_INIT_NVAR(&rowB);
			ZVAL_COPY(&rowB, _11);
			ZEPHIR_INIT_NVAR(&_12$$4);
			zephir_read_property_cached(&_13$$4, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
			tensor_equal(&_12$$4, &_13$$4, &rowB);
			zephir_array_append(&c, &_12$$4, PH_SEPARATE, "tensor/vector.zep", 1546);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _9, "rewind", NULL, 0);
		zephir_check_call_status();
		_15 = 1;
		while (1) {
			if (_15) {
				_15 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _9, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_14, _9, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_14)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowB, _9, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_16$$5);
				zephir_read_property_cached(&_17$$5, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
				tensor_equal(&_16$$5, &_17$$5, &rowB);
				zephir_array_append(&c, &_16$$5, PH_SEPARATE, "tensor/vector.zep", 1546);
		}
	}
	ZEPHIR_INIT_NVAR(&rowB);
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_CALL_CE_STATIC(&_18, tensor_tensorbuffer_ce, "fromBuffers", NULL, 0, &c);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_19, b, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_20, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &_18, &_19, &_20);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise not equal comparison of this vector and a matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, notEqualMatrix)
{
	zval _4$$3, _6$$3, _7$$3;
	zend_bool _15;
	zval c;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, rowB, _8, *_9, _10, *_11, _14, _18, _19, _20, _2$$3, _3$$3, _5$$3, _12$$4, _13$$4, _16$$5, _17$$5;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&rowB);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19);
	ZVAL_UNDEF(&_20);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_12$$4);
	ZVAL_UNDEF(&_13$$4);
	ZVAL_UNDEF(&_16$$5);
	ZVAL_UNDEF(&_17$$5);
	ZVAL_UNDEF(&c);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "n", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A expects ", &_4$$3, " columns but Matrix B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1564);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&c);
	array_init(&c);
	ZEPHIR_CALL_METHOD(&_8, b, "asRowBuffers", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_8) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_10);
		zephir_string_to_char_array(&_10, &_8);
		_9 = &_10;
	} else {
		_9 = &_8;
	}
	zephir_is_iterable(_9, 0, "tensor/vector.zep", 1575);
	if (Z_TYPE_P(_9) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_9), _11)
		{
			ZEPHIR_INIT_NVAR(&rowB);
			ZVAL_COPY(&rowB, _11);
			ZEPHIR_INIT_NVAR(&_12$$4);
			zephir_read_property_cached(&_13$$4, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
			tensor_not_equal(&_12$$4, &_13$$4, &rowB);
			zephir_array_append(&c, &_12$$4, PH_SEPARATE, "tensor/vector.zep", 1572);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _9, "rewind", NULL, 0);
		zephir_check_call_status();
		_15 = 1;
		while (1) {
			if (_15) {
				_15 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _9, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_14, _9, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_14)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowB, _9, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_16$$5);
				zephir_read_property_cached(&_17$$5, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
				tensor_not_equal(&_16$$5, &_17$$5, &rowB);
				zephir_array_append(&c, &_16$$5, PH_SEPARATE, "tensor/vector.zep", 1572);
		}
	}
	ZEPHIR_INIT_NVAR(&rowB);
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_CALL_CE_STATIC(&_18, tensor_tensorbuffer_ce, "fromBuffers", NULL, 0, &c);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_19, b, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_20, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &_18, &_19, &_20);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise greater than comparison of this vector and a matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, greaterMatrix)
{
	zval _4$$3, _6$$3, _7$$3;
	zend_bool _15;
	zval c;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, rowB, _8, *_9, _10, *_11, _14, _18, _19, _20, _2$$3, _3$$3, _5$$3, _12$$4, _13$$4, _16$$5, _17$$5;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&rowB);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19);
	ZVAL_UNDEF(&_20);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_12$$4);
	ZVAL_UNDEF(&_13$$4);
	ZVAL_UNDEF(&_16$$5);
	ZVAL_UNDEF(&_17$$5);
	ZVAL_UNDEF(&c);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "n", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A expects ", &_4$$3, " columns but Matrix B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1590);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&c);
	array_init(&c);
	ZEPHIR_CALL_METHOD(&_8, b, "asRowBuffers", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_8) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_10);
		zephir_string_to_char_array(&_10, &_8);
		_9 = &_10;
	} else {
		_9 = &_8;
	}
	zephir_is_iterable(_9, 0, "tensor/vector.zep", 1601);
	if (Z_TYPE_P(_9) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_9), _11)
		{
			ZEPHIR_INIT_NVAR(&rowB);
			ZVAL_COPY(&rowB, _11);
			ZEPHIR_INIT_NVAR(&_12$$4);
			zephir_read_property_cached(&_13$$4, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
			tensor_greater(&_12$$4, &_13$$4, &rowB);
			zephir_array_append(&c, &_12$$4, PH_SEPARATE, "tensor/vector.zep", 1598);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _9, "rewind", NULL, 0);
		zephir_check_call_status();
		_15 = 1;
		while (1) {
			if (_15) {
				_15 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _9, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_14, _9, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_14)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowB, _9, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_16$$5);
				zephir_read_property_cached(&_17$$5, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
				tensor_greater(&_16$$5, &_17$$5, &rowB);
				zephir_array_append(&c, &_16$$5, PH_SEPARATE, "tensor/vector.zep", 1598);
		}
	}
	ZEPHIR_INIT_NVAR(&rowB);
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_CALL_CE_STATIC(&_18, tensor_tensorbuffer_ce, "fromBuffers", NULL, 0, &c);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_19, b, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_20, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &_18, &_19, &_20);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise greater than or equal to comparison of this vector and a matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, greaterEqualMatrix)
{
	zval _4$$3, _6$$3, _7$$3;
	zend_bool _15;
	zval c;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, rowB, _8, *_9, _10, *_11, _14, _18, _19, _20, _2$$3, _3$$3, _5$$3, _12$$4, _13$$4, _16$$5, _17$$5;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&rowB);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19);
	ZVAL_UNDEF(&_20);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_12$$4);
	ZVAL_UNDEF(&_13$$4);
	ZVAL_UNDEF(&_16$$5);
	ZVAL_UNDEF(&_17$$5);
	ZVAL_UNDEF(&c);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "n", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A expects ", &_4$$3, " columns but Matrix B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1616);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&c);
	array_init(&c);
	ZEPHIR_CALL_METHOD(&_8, b, "asRowBuffers", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_8) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_10);
		zephir_string_to_char_array(&_10, &_8);
		_9 = &_10;
	} else {
		_9 = &_8;
	}
	zephir_is_iterable(_9, 0, "tensor/vector.zep", 1627);
	if (Z_TYPE_P(_9) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_9), _11)
		{
			ZEPHIR_INIT_NVAR(&rowB);
			ZVAL_COPY(&rowB, _11);
			ZEPHIR_INIT_NVAR(&_12$$4);
			zephir_read_property_cached(&_13$$4, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
			tensor_greater_equal(&_12$$4, &_13$$4, &rowB);
			zephir_array_append(&c, &_12$$4, PH_SEPARATE, "tensor/vector.zep", 1624);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _9, "rewind", NULL, 0);
		zephir_check_call_status();
		_15 = 1;
		while (1) {
			if (_15) {
				_15 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _9, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_14, _9, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_14)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowB, _9, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_16$$5);
				zephir_read_property_cached(&_17$$5, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
				tensor_greater_equal(&_16$$5, &_17$$5, &rowB);
				zephir_array_append(&c, &_16$$5, PH_SEPARATE, "tensor/vector.zep", 1624);
		}
	}
	ZEPHIR_INIT_NVAR(&rowB);
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_CALL_CE_STATIC(&_18, tensor_tensorbuffer_ce, "fromBuffers", NULL, 0, &c);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_19, b, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_20, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &_18, &_19, &_20);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise less than comparison of this vector and a matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, lessMatrix)
{
	zval _4$$3, _6$$3, _7$$3;
	zend_bool _15;
	zval c;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, rowB, _8, *_9, _10, *_11, _14, _18, _19, _20, _2$$3, _3$$3, _5$$3, _12$$4, _13$$4, _16$$5, _17$$5;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&rowB);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19);
	ZVAL_UNDEF(&_20);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_12$$4);
	ZVAL_UNDEF(&_13$$4);
	ZVAL_UNDEF(&_16$$5);
	ZVAL_UNDEF(&_17$$5);
	ZVAL_UNDEF(&c);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "n", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A expects ", &_4$$3, " columns but Matrix B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1642);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&c);
	array_init(&c);
	ZEPHIR_CALL_METHOD(&_8, b, "asRowBuffers", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_8) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_10);
		zephir_string_to_char_array(&_10, &_8);
		_9 = &_10;
	} else {
		_9 = &_8;
	}
	zephir_is_iterable(_9, 0, "tensor/vector.zep", 1653);
	if (Z_TYPE_P(_9) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_9), _11)
		{
			ZEPHIR_INIT_NVAR(&rowB);
			ZVAL_COPY(&rowB, _11);
			ZEPHIR_INIT_NVAR(&_12$$4);
			zephir_read_property_cached(&_13$$4, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
			tensor_less(&_12$$4, &_13$$4, &rowB);
			zephir_array_append(&c, &_12$$4, PH_SEPARATE, "tensor/vector.zep", 1650);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _9, "rewind", NULL, 0);
		zephir_check_call_status();
		_15 = 1;
		while (1) {
			if (_15) {
				_15 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _9, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_14, _9, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_14)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowB, _9, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_16$$5);
				zephir_read_property_cached(&_17$$5, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
				tensor_less(&_16$$5, &_17$$5, &rowB);
				zephir_array_append(&c, &_16$$5, PH_SEPARATE, "tensor/vector.zep", 1650);
		}
	}
	ZEPHIR_INIT_NVAR(&rowB);
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_CALL_CE_STATIC(&_18, tensor_tensorbuffer_ce, "fromBuffers", NULL, 0, &c);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_19, b, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_20, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &_18, &_19, &_20);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise less than or equal to comparison of this vector and a matrix.
 *
 * @param \Tensor\Matrix b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Vector, lessEqualMatrix)
{
	zval _4$$3, _6$$3, _7$$3;
	zend_bool _15;
	zval c;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b, b_sub, _0, _1, rowB, _8, *_9, _10, *_11, _14, _18, _19, _20, _2$$3, _3$$3, _5$$3, _12$$4, _13$$4, _16$$5, _17$$5;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&b_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&rowB);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_18);
	ZVAL_UNDEF(&_19);
	ZVAL_UNDEF(&_20);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_12$$4);
	ZVAL_UNDEF(&_13$$4);
	ZVAL_UNDEF(&_16$$5);
	ZVAL_UNDEF(&_17$$5);
	ZVAL_UNDEF(&c);
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "n", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "n", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A expects ", &_4$$3, " columns but Matrix B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1668);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&c);
	array_init(&c);
	ZEPHIR_CALL_METHOD(&_8, b, "asRowBuffers", NULL, 0);
	zephir_check_call_status();
	if (Z_TYPE_P(&_8) == IS_STRING) {
		ZEPHIR_INIT_VAR(&_10);
		zephir_string_to_char_array(&_10, &_8);
		_9 = &_10;
	} else {
		_9 = &_8;
	}
	zephir_is_iterable(_9, 0, "tensor/vector.zep", 1679);
	if (Z_TYPE_P(_9) == IS_ARRAY) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(_9), _11)
		{
			ZEPHIR_INIT_NVAR(&rowB);
			ZVAL_COPY(&rowB, _11);
			ZEPHIR_INIT_NVAR(&_12$$4);
			zephir_read_property_cached(&_13$$4, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
			tensor_less_equal(&_12$$4, &_13$$4, &rowB);
			zephir_array_append(&c, &_12$$4, PH_SEPARATE, "tensor/vector.zep", 1676);
		} ZEND_HASH_FOREACH_END();
	} else {
		ZEPHIR_CALL_METHOD(NULL, _9, "rewind", NULL, 0);
		zephir_check_call_status();
		_15 = 1;
		while (1) {
			if (_15) {
				_15 = 0;
			} else {
				ZEPHIR_CALL_METHOD(NULL, _9, "next", NULL, 0);
				zephir_check_call_status();
			}
			ZEPHIR_CALL_METHOD(&_14, _9, "valid", NULL, 0);
			zephir_check_call_status();
			if (!zend_is_true(&_14)) {
				break;
			}
			ZEPHIR_CALL_METHOD(&rowB, _9, "current", NULL, 0);
			zephir_check_call_status();
				ZEPHIR_INIT_NVAR(&_16$$5);
				zephir_read_property_cached(&_17$$5, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
				tensor_less_equal(&_16$$5, &_17$$5, &rowB);
				zephir_array_append(&c, &_16$$5, PH_SEPARATE, "tensor/vector.zep", 1676);
		}
	}
	ZEPHIR_INIT_NVAR(&rowB);
	object_init_ex(return_value, tensor_matrix_ce);
	ZEPHIR_CALL_CE_STATIC(&_18, tensor_tensorbuffer_ce, "fromBuffers", NULL, 0, &c);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_19, b, "m", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_20, b, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 14, &_18, &_19, &_20);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Multiply this vector with another vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Vector, multiplyVector)
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "size", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "size", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1694);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	tensor_multiply(&_8, &_9, &_10);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_8);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Divide this vector by another vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Vector, divideVector)
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "size", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "size", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1712);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	tensor_divide(&_8, &_9, &_10);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_8);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Add this vector to another vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Vector, addVector)
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "size", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "size", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1730);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	tensor_add(&_8, &_9, &_10);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_8);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Subtract a vector from this vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Vector, subtractVector)
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "size", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "size", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1748);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	tensor_subtract(&_8, &_9, &_10);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_8);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Raise this vector to a power of another vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Vector, powVector)
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "size", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "size", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1766);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	tensor_pow(&_8, &_9, &_10);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_8);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Calculate the modulus of this vector with another vector elementwise.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Vector, modVector)
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "size", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "size", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1784);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	tensor_mod(&_8, &_9, &_10);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_8);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise equality comparison of this vector and a
 * another vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Vector, equalVector)
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "size", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "size", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1803);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	tensor_equal(&_8, &_9, &_10);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_8);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise not equal comparison of this vector and a another vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Vector, notEqualVector)
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "size", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "size", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1821);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	tensor_not_equal(&_8, &_9, &_10);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_8);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise greater than comparison of this vector and a another vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Vector, greaterVector)
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "size", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "size", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1839);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	tensor_greater(&_8, &_9, &_10);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_8);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise greater than or equal to comparison of this vector and a another vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Vector, greaterEqualVector)
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "size", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "size", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1857);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	tensor_greater_equal(&_8, &_9, &_10);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_8);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise less than comparison of this vector and a another vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Vector, lessVector)
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "size", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "size", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1875);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	tensor_less(&_8, &_9, &_10);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_8);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise less than or equal to comparison of this vector and a another vector.
 *
 * @param \Tensor\Vector b
 * @throws \Tensor\Exceptions\DimensionalityMismatch
 * @return self
 */
PHP_METHOD(Tensor_Vector, lessEqualVector)
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
		Z_PARAM_OBJECT_OF_CLASS(b, zephir_get_internal_ce(SL("tensor\\vector")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, b, "size", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_memory_observe(&_3$$3);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
		zephir_cast_to_string(&_4$$3, &_3$$3);
		ZEPHIR_CALL_METHOD(&_5$$3, b, "size", NULL, 0);
		zephir_check_call_status();
		zephir_cast_to_string(&_6$$3, &_5$$3);
		ZEPHIR_INIT_VAR(&_7$$3);
		ZEPHIR_CONCAT_SVSVS(&_7$$3, "Vector A requires ", &_4$$3, " elements but Vector B has ", &_6$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_7$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/vector.zep", 1893);
		ZEPHIR_MM_RESTORE();
		return;
	}
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_8);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_10, b, _zephir_prop_1, 0, PH_NOISY_CC | PH_READONLY);
	tensor_less_equal(&_8, &_9, &_10);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_8);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Multiply this vector by a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Vector, multiplyScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	tensor_multiply_scalar(&_0, &_1, &_2);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Divide this vector by a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Vector, divideScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	tensor_divide_scalar(&_0, &_1, &_2);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Add a scalar to this vector.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Vector, addScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	tensor_add_scalar(&_0, &_1, &_2);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Subtract a scalar from this vector.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Vector, subtractScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	tensor_subtract_scalar(&_0, &_1, &_2);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Raise the vector to a the power of a scalar value.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Vector, powScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	tensor_pow_scalar(&_0, &_1, &_2);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Calculate the modulus of this vector with a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Vector, modScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	tensor_mod_scalar(&_0, &_1, &_2);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise equality comparison of this vector and a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Vector, equalScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	tensor_equal_scalar(&_0, &_1, &_2);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise not equal comparison of this vector and a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Vector, notEqualScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	tensor_not_equal_scalar(&_0, &_1, &_2);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise greater than comparison of this vector and a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Vector, greaterScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	tensor_greater_scalar(&_0, &_1, &_2);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise greater than or equal to comparison of
 * this vector and a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Vector, greaterEqualScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	tensor_greater_equal_scalar(&_0, &_1, &_2);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise less than comparison of this vector and a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Vector, lessScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	tensor_less_scalar(&_0, &_1, &_2);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the element-wise less than or equal to comparison of
 * this vector and a scalar.
 *
 * @param float b
 * @return self
 */
PHP_METHOD(Tensor_Vector, lessEqualScalar)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *b_param = NULL, _0, _1, _2;
	double b;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_2);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(b_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &b_param);
	b = zephir_get_doubleval(b_param);
	object_init_ex(return_value, zend_get_called_scope(execute_data));
	ZEPHIR_INIT_VAR(&_0);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZVAL_DOUBLE(&_2, b);
	tensor_less_equal_scalar(&_0, &_1, &_2);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &_0);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Count method to implement countable interface.
 *
 * @return int
 */
PHP_METHOD(Tensor_Vector, count)
{

	RETURN_MEMBER_TYPED(getThis(), "n", IS_LONG);
}

/**
 * @param mixed index
 * @param array values
 * @throws \Tensor\Exceptions\RuntimeException
 */
PHP_METHOD(Tensor_Vector, offsetSet)
{
	zval *index, index_sub, *values, values_sub;

	ZVAL_UNDEF(&index_sub);
	ZVAL_UNDEF(&values_sub);
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(index)
		Z_PARAM_ZVAL(values)
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(2, 0, &index, &values);
	ZEPHIR_THROW_EXCEPTION_DEBUG_STRW(tensor_exceptions_runtimeexception_ce, "Vector cannot be mutated directly.", "tensor/vector.zep", 2050);
	return;
}

/**
 * Does a given element exist in the vector.
 *
 * @param mixed index
 * @return bool
 */
PHP_METHOD(Tensor_Vector, offsetExists)
{
	zend_bool _0;
	zval *index, index_sub, _1;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&index_sub);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(index)
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(1, 0, &index);
	if (UNEXPECTED(!(Z_TYPE_P(index) == IS_LONG))) {
		RETURN_BOOL(0);
	}
	_0 = ZEPHIR_GE_LONG(index, 0);
	if (_0) {
		zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
		_0 = ZEPHIR_LT(index, &_1);
	}
	RETURN_BOOL(_0);
}

/**
 * @param mixed index
 * @throws \Tensor\Exceptions\RuntimeException
 */
PHP_METHOD(Tensor_Vector, offsetUnset)
{
	zval *index, index_sub;

	ZVAL_UNDEF(&index_sub);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(index)
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(1, 0, &index);
	ZEPHIR_THROW_EXCEPTION_DEBUG_STRW(tensor_exceptions_runtimeexception_ce, "Vector cannot be mutated directly.", "tensor/vector.zep", 2074);
	return;
}

/**
 * Return an element from the vector at the given index.
 *
 * @param mixed index
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @return int|float
 */
PHP_METHOD(Tensor_Vector, offsetGet)
{
	zval _4$$3, _5$$3;
	zend_bool _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *index, index_sub, _2, _6, _3$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&index_sub);
	ZVAL_UNDEF(&_2);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_5$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("a", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(index)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &index);
	_0 = !(Z_TYPE_P(index) == IS_LONG);
	if (!(_0)) {
		_1 = ZEPHIR_LT_LONG(index, 0);
		if (!(_1)) {
			zephir_read_property_cached(&_2, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC | PH_READONLY);
			_1 = ZEPHIR_GE(index, &_2);
		}
		_0 = _1;
	}
	if (UNEXPECTED(_0)) {
		ZEPHIR_INIT_VAR(&_3$$3);
		object_init_ex(&_3$$3, tensor_exceptions_invalidargumentexception_ce);
		zephir_cast_to_string(&_4$$3, index);
		ZEPHIR_INIT_VAR(&_5$$3);
		ZEPHIR_CONCAT_SSVS(&_5$$3, "Element not found at", " offset ", &_4$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_3$$3, "__construct", NULL, 2, &_5$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_3$$3, "tensor/vector.zep", 2088);
		ZEPHIR_MM_RESTORE();
		return;
	}
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_1, 1, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_METHOD(&_6, "get", NULL, 0, index);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Get an iterator for the items in the vector.
 *
 * @return \ArrayIterator
 */
PHP_METHOD(Tensor_Vector, getIterator)
{
	zval _0, _1;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	static zend_string *_zephir_prop_0 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	object_init_ex(return_value, spl_ce_ArrayIterator);
	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 1, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_CALL_METHOD(&_1, &_0, "toArray", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 20, &_1);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * Return the elements of the vector as a plain PHP array so that only the
 * values, and not the object structure, appear in the serialized form.
 *
 * @return list<float>
 */
PHP_METHOD(Tensor_Vector, __serialize)
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

	zephir_create_array(return_value, 2, 0);
	ZEPHIR_CALL_METHOD(&_0, this_ptr, "asArray", NULL, 0);
	zephir_check_call_status();
	zephir_array_update_string(return_value, SL("data"), &_0, PH_COPY | PH_SEPARATE);
	zephir_memory_observe(&_1);
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 2, PH_NOISY_CC);
	zephir_array_update_string(return_value, SL("n"), &_1, PH_COPY | PH_SEPARATE);
	RETURN_MM();
}

/**
 * Restore the vector from the plain array of elements produced by
 * __serialize() by rebuilding its TensorBuffer.
 *
 * @param float[] data
 * @throws \Tensor\Exceptions\InvalidArgumentException
 */
PHP_METHOD(Tensor_Vector, __unserialize)
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
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("a", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("n", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		ZEPHIR_Z_PARAM_ARRAY(data, data_param)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &data_param);
	zephir_get_arrval(&data, data_param);
	zephir_array_fetch_string(&_0, &data, SL("data"), PH_NOISY | PH_READONLY, "tensor/vector.zep", 2129);
	ZVAL_BOOL(&_1, 0);
	ZEPHIR_CALL_SELF(&rebuilt, "fromArray", NULL, 0, &_0, &_1);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, &rebuilt, _zephir_prop_0, 0, PH_NOISY_CC | PH_READONLY);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_0, 1, &_1);
	zephir_array_fetch_string(&_2, &data, SL("n"), PH_NOISY | PH_READONLY, "tensor/vector.zep", 2132);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_1, 2, &_2);
	ZEPHIR_MM_RESTORE();
}

