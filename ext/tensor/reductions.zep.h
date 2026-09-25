
extern zend_class_entry *tensor_reductions_ce;

ZEPHIR_INIT_CLASS(Tensor_Reductions);

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_reductions_sum, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_reductions_product, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_reductions_min, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_reductions_max, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_reductions_argmin, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_reductions_argmax, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEPHIR_INIT_FUNCS(tensor_reductions_method_entry) {
	PHP_ABSTRACT_ME(Tensor_Reductions, sum, arginfo_tensor_reductions_sum)
	PHP_ABSTRACT_ME(Tensor_Reductions, product, arginfo_tensor_reductions_product)
	PHP_ABSTRACT_ME(Tensor_Reductions, min, arginfo_tensor_reductions_min)
	PHP_ABSTRACT_ME(Tensor_Reductions, max, arginfo_tensor_reductions_max)
	PHP_ABSTRACT_ME(Tensor_Reductions, argmin, arginfo_tensor_reductions_argmin)
	PHP_ABSTRACT_ME(Tensor_Reductions, argmax, arginfo_tensor_reductions_argmax)
	PHP_FE_END
};
