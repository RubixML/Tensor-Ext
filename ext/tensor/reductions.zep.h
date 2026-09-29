
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_reductions_mean, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_reductions_variance, 0, 0, 0)
	ZEND_ARG_INFO(0, mean)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_reductions_median, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_reductions_quantile, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, q, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEPHIR_INIT_FUNCS(tensor_reductions_method_entry) {
	PHP_ABSTRACT_ME(Tensor_Reductions, sum, arginfo_tensor_reductions_sum)
	PHP_ABSTRACT_ME(Tensor_Reductions, product, arginfo_tensor_reductions_product)
	PHP_ABSTRACT_ME(Tensor_Reductions, min, arginfo_tensor_reductions_min)
	PHP_ABSTRACT_ME(Tensor_Reductions, max, arginfo_tensor_reductions_max)
	PHP_ABSTRACT_ME(Tensor_Reductions, argmin, arginfo_tensor_reductions_argmin)
	PHP_ABSTRACT_ME(Tensor_Reductions, argmax, arginfo_tensor_reductions_argmax)
	PHP_ABSTRACT_ME(Tensor_Reductions, mean, arginfo_tensor_reductions_mean)
	PHP_ABSTRACT_ME(Tensor_Reductions, variance, arginfo_tensor_reductions_variance)
	PHP_ABSTRACT_ME(Tensor_Reductions, median, arginfo_tensor_reductions_median)
	PHP_ABSTRACT_ME(Tensor_Reductions, quantile, arginfo_tensor_reductions_quantile)
	PHP_FE_END
};
