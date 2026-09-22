
extern zend_class_entry *tensor_tensorbuffer_ce;

ZEPHIR_INIT_CLASS(Tensor_TensorBuffer);

PHP_METHOD(Tensor_TensorBuffer, __construct);
PHP_METHOD(Tensor_TensorBuffer, fromBuffers);
PHP_METHOD(Tensor_TensorBuffer, asBuffer);
PHP_METHOD(Tensor_TensorBuffer, count);
PHP_METHOD(Tensor_TensorBuffer, type);
PHP_METHOD(Tensor_TensorBuffer, toArray);
PHP_METHOD(Tensor_TensorBuffer, get);
PHP_METHOD(Tensor_TensorBuffer, set);
PHP_METHOD(Tensor_TensorBuffer, sort);
PHP_METHOD(Tensor_TensorBuffer, slice);
PHP_METHOD(Tensor_TensorBuffer, map);
PHP_METHOD(Tensor_TensorBuffer, sum);
PHP_METHOD(Tensor_TensorBuffer, product);
PHP_METHOD(Tensor_TensorBuffer, min);
PHP_METHOD(Tensor_TensorBuffer, max);
PHP_METHOD(Tensor_TensorBuffer, argmin);
PHP_METHOD(Tensor_TensorBuffer, argmax);
PHP_METHOD(Tensor_TensorBuffer, sliceStrided);
PHP_METHOD(Tensor_TensorBuffer, concat);
PHP_METHOD(Tensor_TensorBuffer, split);
PHP_METHOD(Tensor_TensorBuffer, repeat);

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_tensorbuffer___construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, buffer, Tensor\\Buffer, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_tensorbuffer_frombuffers, 0, 1, Tensor\\TensorBuffer, 0)
	ZEND_ARG_ARRAY_INFO(0, buffers, 0)
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

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_tensorbuffer_map, 0, 1, Tensor\\TensorBuffer, 0)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tensor_tensorbuffer_sum, 0, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tensor_tensorbuffer_product, 0, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tensor_tensorbuffer_min, 0, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tensor_tensorbuffer_max, 0, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tensor_tensorbuffer_argmin, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tensor_tensorbuffer_argmax, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_tensorbuffer_slicestrided, 0, 3, Tensor\\TensorBuffer, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, stride, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_tensorbuffer_concat, 0, 1, Tensor\\TensorBuffer, 0)
	ZEND_ARG_ARRAY_INFO(0, buffers, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_tensor_tensorbuffer_split, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, chunkLength, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_tensor_tensorbuffer_repeat, 0, 1, Tensor\\TensorBuffer, 0)
	ZEND_ARG_TYPE_INFO(0, times, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEPHIR_INIT_FUNCS(tensor_tensorbuffer_method_entry) {
	PHP_ME(Tensor_TensorBuffer, __construct, arginfo_tensor_tensorbuffer___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Tensor_TensorBuffer, fromBuffers, arginfo_tensor_tensorbuffer_frombuffers, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(Tensor_TensorBuffer, asBuffer, arginfo_tensor_tensorbuffer_asbuffer, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, count, arginfo_tensor_tensorbuffer_count, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, type, arginfo_tensor_tensorbuffer_type, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, toArray, arginfo_tensor_tensorbuffer_toarray, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, get, arginfo_tensor_tensorbuffer_get, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, set, arginfo_tensor_tensorbuffer_set, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, sort, arginfo_tensor_tensorbuffer_sort, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, slice, arginfo_tensor_tensorbuffer_slice, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, map, arginfo_tensor_tensorbuffer_map, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, sum, arginfo_tensor_tensorbuffer_sum, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, product, arginfo_tensor_tensorbuffer_product, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, min, arginfo_tensor_tensorbuffer_min, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, max, arginfo_tensor_tensorbuffer_max, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, argmin, arginfo_tensor_tensorbuffer_argmin, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, argmax, arginfo_tensor_tensorbuffer_argmax, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, sliceStrided, arginfo_tensor_tensorbuffer_slicestrided, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, concat, arginfo_tensor_tensorbuffer_concat, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, split, arginfo_tensor_tensorbuffer_split, ZEND_ACC_PUBLIC)
	PHP_ME(Tensor_TensorBuffer, repeat, arginfo_tensor_tensorbuffer_repeat, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
