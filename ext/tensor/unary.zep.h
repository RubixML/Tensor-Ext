
extern zend_class_entry *tensor_unary_ce;

ZEPHIR_INIT_CLASS(Tensor_Unary);

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_unary_abs, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_unary_square, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_unary_sqrt, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_unary_exp, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_unary_expm1, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_unary_log, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, base, IS_DOUBLE, 0, "2.7182818284590452354")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_unary_log1p, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_unary_round, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, precision, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_unary_floor, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_unary_ceil, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_unary_sign, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_unary_negate, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_unary_clip, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, min, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_unary_cliplower, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, min, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tensor_unary_clipupper, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, max, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEPHIR_INIT_FUNCS(tensor_unary_method_entry) {
	PHP_ABSTRACT_ME(Tensor_Unary, abs, arginfo_tensor_unary_abs)
	PHP_ABSTRACT_ME(Tensor_Unary, square, arginfo_tensor_unary_square)
	PHP_ABSTRACT_ME(Tensor_Unary, sqrt, arginfo_tensor_unary_sqrt)
	PHP_ABSTRACT_ME(Tensor_Unary, exp, arginfo_tensor_unary_exp)
	PHP_ABSTRACT_ME(Tensor_Unary, expm1, arginfo_tensor_unary_expm1)
	PHP_ABSTRACT_ME(Tensor_Unary, log, arginfo_tensor_unary_log)
	PHP_ABSTRACT_ME(Tensor_Unary, log1p, arginfo_tensor_unary_log1p)
	PHP_ABSTRACT_ME(Tensor_Unary, round, arginfo_tensor_unary_round)
	PHP_ABSTRACT_ME(Tensor_Unary, floor, arginfo_tensor_unary_floor)
	PHP_ABSTRACT_ME(Tensor_Unary, ceil, arginfo_tensor_unary_ceil)
	PHP_ABSTRACT_ME(Tensor_Unary, sign, arginfo_tensor_unary_sign)
	PHP_ABSTRACT_ME(Tensor_Unary, negate, arginfo_tensor_unary_negate)
	PHP_ABSTRACT_ME(Tensor_Unary, clip, arginfo_tensor_unary_clip)
	PHP_ABSTRACT_ME(Tensor_Unary, clipLower, arginfo_tensor_unary_cliplower)
	PHP_ABSTRACT_ME(Tensor_Unary, clipUpper, arginfo_tensor_unary_clipupper)
	PHP_FE_END
};
