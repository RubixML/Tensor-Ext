
#ifdef HAVE_CONFIG_H
#include "../ext_config.h"
#endif

#include <php.h>
#include "../php_ext.h"
#include "../ext.h"

#include <Zend/zend_exceptions.h>

#include "kernel/main.h"


ZEPHIR_INIT_CLASS(Tensor_Reductions)
{
	ZEPHIR_REGISTER_INTERFACE(Tensor, Reductions, tensor, reductions, tensor_reductions_method_entry);

	return SUCCESS;
}

/**
 * Sum the tensor.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Reductions, sum);
/**
 * Calculate the product of the tensor.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Reductions, product);
/**
 * Return the minimum of the tensor.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Reductions, min);
/**
 * Return the maximum of the tensor.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Reductions, max);
/**
 * Return the index (or per-row indices) of the minimum of the tensor.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Reductions, argmin);
/**
 * Return the index (or per-row indices) of the maximum of the tensor.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Reductions, argmax);
