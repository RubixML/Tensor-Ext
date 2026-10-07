
#ifdef HAVE_CONFIG_H
#include "../ext_config.h"
#endif

#include <php.h>
#include "../php_ext.h"
#include "../ext.h"

#include <Zend/zend_exceptions.h>

#include "kernel/main.h"


ZEPHIR_INIT_CLASS(Tensor_Unary)
{
	ZEPHIR_REGISTER_INTERFACE(Tensor, Unary, tensor, unary, tensor_unary_method_entry);

	zephir_declare_class_constant_double(tensor_unary_ce, SL("M_E"), 2.7182818284590452354);

	return SUCCESS;
}

/**
 * Take the absolute value of the tensor.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Unary, abs);
/**
 * Square the tensor.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Unary, square);
/**
 * Return the square root of the tensor.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Unary, sqrt);
/**
 * Return the exponential of the tensor.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Unary, exp);
/**
 * Return the exponential of the tensor minus 1.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Unary, expm1);
/**
 * Return the logarithm of the tensor in a specified base.
 *
 * @param float base
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Unary, log);
/**
 * Return the log of 1 plus the tensor i.e. a transform.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Unary, log1p);
/**
 * Round the elements in the tensor to a given decimal place.
 *
 * @param int precision
 * @return self
 */
ZEPHIR_DOC_METHOD(Tensor_Unary, round);
/**
 * Round the tensor down to the nearest integer.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Unary, floor);
/**
 * Round the tensor up to the nearest integer.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Unary, ceil);
/**
 * Return the element-wise sign indication.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Unary, sign);
/**
 * Negate the tensor i.e take the negative.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Unary, negate);
/**
 * Clip the tensor to be between the given minimum and maximum.
 *
 * @param float min
 * @param float max
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Unary, clip);
/**
 * Clip the tensor to be lower bounded by a given minimum.
 *
 * @param float min
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Unary, clipLower);
/**
 * Clip the tensor to be upper bounded by a given maximum.
 *
 * @param float max
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Unary, clipUpper);
