
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
 * Return the softmax of the tensor i.e. each column normalized to sum to 1.
 *
 * @return mixed
 */
ZEPHIR_DOC_METHOD(Tensor_Special, softmax);
