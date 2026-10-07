
#ifdef HAVE_CONFIG_H
#include "../ext_config.h"
#endif

#include <php.h>
#include "../php_ext.h"
#include "../ext.h"

#include <Zend/zend_exceptions.h>

#include "kernel/main.h"


ZEPHIR_INIT_CLASS(Tensor_Special)
{
	ZEPHIR_REGISTER_INTERFACE(Tensor, Special, tensor, special, tensor_special_method_entry);

	return SUCCESS;
}

/**
 * Return the element-wise logistic function i.e. 1 / (1 + exp(-x)).
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Special, sigmoid);
/**
 * Return the element-wise softplus i.e. log(1 + exp(x)).
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Special, softplus);
/**
 * Return the softmax of the tensor i.e. each row normalized to sum to 1.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Special, softmax);
/**
 * Return the element-wise Gaussian error function.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Special, erf);
