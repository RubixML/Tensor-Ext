
extern zend_class_entry *tensor_tensorbuffer_ce;

ZEPHIR_INIT_CLASS(Tensor_TensorBuffer);

PHP_METHOD(Tensor_TensorBuffer, __construct);
PHP_METHOD(Tensor_TensorBuffer, asBuffer);
PHP_METHOD(Tensor_TensorBuffer, count);
PHP_METHOD(Tensor_TensorBuffer, type);
PHP_METHOD(Tensor_TensorBuffer, toArray);
PHP_METHOD(Tensor_TensorBuffer, get);
PHP_METHOD(Tensor_TensorBuffer, set);
PHP_METHOD(Tensor_TensorBuffer, sort);
PHP_METHOD(Tensor_TensorBuffer, slice);

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_tensorbuffer___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, buffer, Tensor\\Buffer, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_tensorbuffer_asbuffer, 0, 0, Tensor\\Buffer, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tensor_tensorbuffer_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tensor_tensorbuffer_type, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tensor_tensorbuffer_toarray, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_tensorbuffer_get, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tensor_tensorbuffer_set, 0, 2, IS_VOID, 0)

	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tensor_tensorbuffer_sort, 0, 0, IS_VOID, 0)

	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ascending, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_tensorbuffer_slice, 0, 2, Tensor\\TensorBuffer, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEPHIR_INIT_FUNCS(tensor_tensorbuffer_method_entry) {
	PHP_ME(Tensor_TensorBuffer, __construct, arginfo_tensor_tensorbuffer___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Tensor_TensorBuffer, asBuffer, arginfo_tensor_tensorbuffer_asbuffer, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, count, arginfo_tensor_tensorbuffer_count, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, type, arginfo_tensor_tensorbuffer_type, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, toArray, arginfo_tensor_tensorbuffer_toarray, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, get, arginfo_tensor_tensorbuffer_get, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, set, arginfo_tensor_tensorbuffer_set, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, sort, arginfo_tensor_tensorbuffer_sort, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, slice, arginfo_tensor_tensorbuffer_slice, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
