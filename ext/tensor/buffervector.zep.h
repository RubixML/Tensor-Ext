
extern zend_class_entry *tensor_buffervector_ce;

ZEPHIR_INIT_CLASS(Tensor_BufferVector);

PHP_METHOD(Tensor_BufferVector, build);
PHP_METHOD(Tensor_BufferVector, quick);
PHP_METHOD(Tensor_BufferVector, __construct);
PHP_METHOD(Tensor_BufferVector, buffer);
PHP_METHOD(Tensor_BufferVector, n);
PHP_METHOD(Tensor_BufferVector, asArray);
PHP_METHOD(Tensor_BufferVector, add);
PHP_METHOD(Tensor_BufferVector, multiply);
PHP_METHOD(Tensor_BufferVector, dot);

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_buffervector_build, 0, 0, Tensor\\BufferVector, 0)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, a, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_buffervector_quick, 0, 1, Tensor\\BufferVector, 0)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_buffervector___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_buffervector_buffer, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tensor_buffervector_n, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tensor_buffervector_asarray, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_buffervector_add, 0, 1, Tensor\\BufferVector, 0)
	ZEND_ARG_OBJ_INFO(0, b, Tensor\\BufferVector, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_buffervector_multiply, 0, 1, Tensor\\BufferVector, 0)
	ZEND_ARG_OBJ_INFO(0, b, Tensor\\BufferVector, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tensor_buffervector_dot, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_OBJ_INFO(0, b, Tensor\\BufferVector, 0)
ZEND_END_ARG_INFO()

ZEPHIR_INIT_FUNCS(tensor_buffervector_method_entry) {
	PHP_ME(Tensor_BufferVector, build, arginfo_tensor_buffervector_build, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(Tensor_BufferVector, quick, arginfo_tensor_buffervector_quick, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(Tensor_BufferVector, __construct, arginfo_tensor_buffervector___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
PHP_ME(Tensor_BufferVector, buffer, arginfo_tensor_buffervector_buffer, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_BufferVector, n, arginfo_tensor_buffervector_n, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_BufferVector, asArray, arginfo_tensor_buffervector_asarray, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_BufferVector, add, arginfo_tensor_buffervector_add, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_BufferVector, multiply, arginfo_tensor_buffervector_multiply, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_BufferVector, dot, arginfo_tensor_buffervector_dot, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
