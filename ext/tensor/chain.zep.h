
extern zend_class_entry *tensor_chain_ce;

ZEPHIR_INIT_CLASS(Tensor_Chain);

PHP_METHOD(Tensor_Chain, __construct);
PHP_METHOD(Tensor_Chain, of);
PHP_METHOD(Tensor_Chain, matmul);
PHP_METHOD(Tensor_Chain, add);
PHP_METHOD(Tensor_Chain, negate);
PHP_METHOD(Tensor_Chain, exp);
PHP_METHOD(Tensor_Chain, fork);
PHP_METHOD(Tensor_Chain, combineDivide);
PHP_METHOD(Tensor_Chain, done);

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_chain___construct, 0, 0, 3)
	ZEND_ARG_OBJ_INFO(0, buffer, Tensor\\TensorBuffer, 0)
	ZEND_ARG_TYPE_INFO(0, m, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, n, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_chain_of, 0, 1, Tensor\\Chain, 0)
	ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_chain_matmul, 0, 1, Tensor\\Chain, 0)
	ZEND_ARG_OBJ_INFO(0, w, Tensor\\Matrix, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_chain_add, 0, 1, Tensor\\Chain, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_chain_negate, 0, 0, Tensor\\Chain, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_chain_exp, 0, 0, Tensor\\Chain, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_chain_fork, 0, 0, Tensor\\Chain, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_chain_combinedivide, 0, 0, Tensor\\Chain, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_chain_done, 0, 0, Tensor\\Matrix, 0)
ZEND_END_ARG_INFO()

ZEPHIR_INIT_FUNCS(tensor_chain_method_entry) {
	PHP_ME(Tensor_Chain, __construct, arginfo_tensor_chain___construct, ZEND_ACC_PROTECTED|ZEND_ACC_CTOR)
	PHP_ME(Tensor_Chain, of, arginfo_tensor_chain_of, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(Tensor_Chain, matmul, arginfo_tensor_chain_matmul, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_Chain, add, arginfo_tensor_chain_add, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_Chain, negate, arginfo_tensor_chain_negate, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_Chain, exp, arginfo_tensor_chain_exp, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_Chain, fork, arginfo_tensor_chain_fork, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_Chain, combineDivide, arginfo_tensor_chain_combinedivide, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_Chain, done, arginfo_tensor_chain_done, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
