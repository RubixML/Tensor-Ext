
extern zend_class_entry *tensor_special_ce;

ZEPHIR_INIT_CLASS(Tensor_Special);

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_special_softmax, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEPHIR_INIT_FUNCS(tensor_special_method_entry) {
	PHP_ABSTRACT_ME(Tensor_Special, softmax, arginfo_tensor_special_softmax)
	PHP_FE_END
};
