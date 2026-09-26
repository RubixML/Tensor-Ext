
#ifdef HAVE_CONFIG_H
#include "../ext_config.h"
#endif

#include <php.h>
#include "../php_ext.h"
#include "../ext.h"

#include <Zend/zend_operators.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>

#include "kernel/main.h"
#include "kernel/exception.h"
#include "kernel/memory.h"
#include "kernel/fcall.h"
#include "kernel/concat.h"
#include "kernel/object.h"
#include "kernel/operators.h"
#include "include/chain.h"


/**
 * Chain
 *
 * A composable pipeline of tensor primitives, planned into fused kernels
 * where a shape is recognized.
 *
 * The user composes a chain out of the primitive verbs -- matmul, add, negate,
 * exp, fork, and combineDivide -- and the C planner
 * (ext/include/chain.c) walks the recorded op sequence in order. On a shape
 * it recognizes (v1: the SiLU composition `fork(); negate(); exp();
 * add(1.0); combineDivide()`) it dispatches a dedicated fused kernel;
 * otherwise it re-runs the primitives one at a time. The user never sees the
 * fused kernel: they only ever compose with the primitives.
 *
 * Wire format. The three positional zval-arrays -- `ops`, `inps`, `extra` --
 * are handed to `tensor_chain_plan()` in `done()`. The op codes are literal
 * integers here matching the `TENSOR_CHAIN_OP_*` values in
 * `ext/include/chain.h`; keep the two in lock step:
 *
 *   MATMUL  = 0      inps[i] = weight TensorBuffer,  extra[i] = weight n
 *   ADD     = 1      inps[i] = row TensorBuffer or a scalar double
 *   NEGATE  = 2
 *   EXP     = 3
 *   FORK    = 4
 *   COMBINE_DIVIDE = 5
 *
 * The chain's shape (m, n) is tracked incrementally:
 *
 *   matmul(w)        : n := len(w) / m (weight shape must be m x n')
 *   add(row/scalar)  : m, n unchanged
 *   negate/exp      : m, n unchanged
 *   fork/combineDivide : no shape change (fork snapshots the value)
 *
 * @category    Scientific Computing
 * @package     Rubix/Tensor
 * @author      Andrew DalPino
 */
ZEPHIR_INIT_CLASS(Tensor_Chain)
{
	ZEPHIR_REGISTER_CLASS(Tensor, Chain, tensor, chain, tensor_chain_method_entry, 0);

	/**
	 * The source buffer being transformed.
	 *
	 * @var \Tensor\TensorBuffer
	 */
	zend_declare_property_null(tensor_chain_ce, SL("buffer"), ZEND_ACC_PROTECTED);
	/**
	 * The number of rows in the source matrix (never changes through the
	 * chain).
	 *
	 * @var int
	 */
	zend_declare_property_null(tensor_chain_ce, SL("initM"), ZEND_ACC_PROTECTED);
	/**
	 * The number of columns in the source matrix (never changes through the
	 * chain).
	 *
	 * @var int
	 */
	zend_declare_property_null(tensor_chain_ce, SL("initN"), ZEND_ACC_PROTECTED);
	/**
	 * The current number of rows in the running value.
	 *
	 * @var int
	 */
	zend_declare_property_null(tensor_chain_ce, SL("m"), ZEND_ACC_PROTECTED);
	/**
	 * The current number of columns in the running value.
	 *
	 * @var int
	 */
	zend_declare_property_null(tensor_chain_ce, SL("n"), ZEND_ACC_PROTECTED);
	/**
	 * Recorded op codes in call order.
	 *
	 * @var int[]
	 */
	zend_declare_property_null(tensor_chain_ce, SL("ops"), ZEND_ACC_PROTECTED);
	/**
	 * Recorded operands, positionally paired with `ops`. For matmul the weight
	 * TensorBuffer; for add, a TensorBuffer (row) or a scalar double; for the
	 * unary operations, null so the length matches `ops`.
	 *
	 * @var array
	 */
	zend_declare_property_null(tensor_chain_ce, SL("inps"), ZEND_ACC_PROTECTED);
	/**
	 * Extra per-op integer dimensions, positionally paired with `ops`. Only
	 * matmul uses it (the weight's column count); every other slot is null.
	 *
	 * @var array
	 */
	zend_declare_property_null(tensor_chain_ce, SL("extra"), ZEND_ACC_PROTECTED);
	return SUCCESS;
}

/**
 * Create a chain from a source matrix at the given shape.
 *
 * @param \Tensor\TensorBuffer buffer
 * @param int m
 * @param int n
 * @throws \Tensor\Exceptions\InvalidArgumentException
 */
PHP_METHOD(Tensor_Chain, __construct)
{
	zend_bool _0;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_4 = NULL;
	zend_long m, n, ZEPHIR_LAST_CALL_STATUS;
	zval *buffer, buffer_sub, *m_param = NULL, *n_param = NULL, _7, _8, _9, _10, _1$$3, _2$$3, _3$$3, _5$$3, _6$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&buffer_sub);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_6$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	static zend_string *_zephir_prop_3 = NULL;
	static zend_string *_zephir_prop_4 = NULL;
	static zend_string *_zephir_prop_5 = NULL;
	static zend_string *_zephir_prop_6 = NULL;
	static zend_string *_zephir_prop_7 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("buffer", 6, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("initM", 5, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("initN", 5, 1);
	}
	if (UNEXPECTED(!_zephir_prop_3)) {
		_zephir_prop_3 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_4)) {
		_zephir_prop_4 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_5)) {
		_zephir_prop_5 = zend_string_init("ops", 3, 1);
	}
	if (UNEXPECTED(!_zephir_prop_6)) {
		_zephir_prop_6 = zend_string_init("inps", 4, 1);
	}
	if (UNEXPECTED(!_zephir_prop_7)) {
		_zephir_prop_7 = zend_string_init("extra", 5, 1);
	}

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_OBJECT_OF_CLASS(buffer, zephir_get_internal_ce(SL("tensor\\tensorbuffer")))
		Z_PARAM_LONG(m)
		Z_PARAM_LONG(n)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 3, 0, &buffer, &m_param, &n_param);
	_0 = m < 1;
	if (!(_0)) {
		_0 = n < 1;
	}
	if (UNEXPECTED(_0)) {
		ZEPHIR_INIT_VAR(&_1$$3);
		object_init_ex(&_1$$3, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_2$$3, m);
		ZEPHIR_CALL_FUNCTION(&_3$$3, "strval", &_4, 3, &_2$$3);
		zephir_check_call_status();
		ZVAL_LONG(&_2$$3, n);
		ZEPHIR_CALL_FUNCTION(&_5$$3, "strval", &_4, 3, &_2$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_6$$3);
		ZEPHIR_CONCAT_SSVSVS(&_6$$3, "Chain dimensions must be", " positive, ", &_3$$3, "x", &_5$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_1$$3, "__construct", NULL, 2, &_6$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_1$$3, "tensor/chain.zep", 118);
		ZEPHIR_MM_RESTORE();
		return;
	}
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_0, 3, buffer);
	ZVAL_UNDEF(&_7);
	ZVAL_LONG(&_7, m);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_1, 4, &_7);
	ZVAL_UNDEF(&_7);
	ZVAL_LONG(&_7, n);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_2, 5, &_7);
	ZVAL_UNDEF(&_7);
	ZVAL_LONG(&_7, m);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_3, 6, &_7);
	ZVAL_UNDEF(&_7);
	ZVAL_LONG(&_7, n);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_4, 7, &_7);
	ZEPHIR_INIT_VAR(&_8);
	array_init(&_8);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_5, 8, &_8);
	ZEPHIR_INIT_VAR(&_9);
	array_init(&_9);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_6, 9, &_9);
	ZEPHIR_INIT_VAR(&_10);
	array_init(&_10);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_7, 10, &_10);
	ZEPHIR_MM_RESTORE();
}

/**
 * Start a chain from a 2D tensor.
 *
 * @param \Tensor\Matrix|Vector source
 * @return self
 */
PHP_METHOD(Tensor_Chain, of)
{
	zend_bool _1$$3;
	zval _6;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *source, source_sub, _0, _5, _2$$4, _3$$4, _4$$4;

	ZVAL_UNDEF(&source_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_2$$4);
	ZVAL_UNDEF(&_3$$4);
	ZVAL_UNDEF(&_4$$4);
	ZVAL_UNDEF(&_6);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(source)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &source);
	ZEPHIR_INIT_VAR(&_0);
	zephir_gettype(&_0, source);
	if (ZEPHIR_IS_STRING(&_0, "object")) { goto zephir_switch_0_clause_0; }
	goto zephir_switch_0_end;
	zephir_switch_0_clause_0: ;
		_1$$3 = 1;
		if (_1$$3 == zephir_instance_of_ev(source, tensor_matrix_ce)) { goto zephir_switch_1_clause_0; }
		goto zephir_switch_1_end;
		zephir_switch_1_clause_0: ;
			object_init_ex(return_value, tensor_chain_ce);
			ZEPHIR_CALL_METHOD(&_2$$4, source, "asTensorBuffer", NULL, 0);
			zephir_check_call_status();
			ZEPHIR_CALL_METHOD(&_3$$4, source, "m", NULL, 0);
			zephir_check_call_status();
			ZEPHIR_CALL_METHOD(&_4$$4, source, "n", NULL, 0);
			zephir_check_call_status();
			ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 18, &_2$$4, &_3$$4, &_4$$4);
			zephir_check_call_status();
			RETURN_MM();
		zephir_switch_1_end: ;

		goto zephir_switch_0_end;
	zephir_switch_0_end: ;

	ZEPHIR_INIT_VAR(&_5);
	object_init_ex(&_5, tensor_exceptions_invalidargumentexception_ce);
	ZEPHIR_INIT_VAR(&_6);
	ZEPHIR_CONCAT_SS(&_6, "Chain source must", " be a Matrix at present.");
	ZEPHIR_CALL_METHOD(NULL, &_5, "__construct", NULL, 2, &_6);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_5, "tensor/chain.zep", 150);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * Record a matrix multiplication: cur = cur @ w.
 *
 * @param \Tensor\Matrix w
 * @return self
 */
PHP_METHOD(Tensor_Chain, matmul)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_5 = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *w, w_sub, _0, _1, _13, _14, _2$$3, _3$$3, _4$$3, _6$$3, _7$$3, _8$$3, _9$$3, _10$$3, _11$$3, _12$$3;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&w_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_1);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_14);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_7$$3);
	ZVAL_UNDEF(&_8$$3);
	ZVAL_UNDEF(&_9$$3);
	ZVAL_UNDEF(&_10$$3);
	ZVAL_UNDEF(&_11$$3);
	ZVAL_UNDEF(&_12$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(w, zephir_get_internal_ce(SL("tensor\\matrix")))
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &w);
	ZEPHIR_CALL_METHOD(&_0, w, "m", NULL, 0);
	zephir_check_call_status();
	zephir_read_property_cached(&_1, this_ptr, _zephir_prop_0, 7, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(!ZEPHIR_IS_IDENTICAL(&_0, &_1))) {
		ZEPHIR_INIT_VAR(&_2$$3);
		object_init_ex(&_2$$3, tensor_exceptions_dimensionalitymismatch_ce);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_1, 6, PH_NOISY_CC | PH_READONLY);
		ZEPHIR_CALL_FUNCTION(&_4$$3, "strval", &_5, 3, &_3$$3);
		zephir_check_call_status();
		zephir_read_property_cached(&_6$$3, this_ptr, _zephir_prop_0, 7, PH_NOISY_CC | PH_READONLY);
		ZEPHIR_CALL_FUNCTION(&_7$$3, "strval", &_5, 3, &_6$$3);
		zephir_check_call_status();
		zephir_read_property_cached(&_8$$3, this_ptr, _zephir_prop_0, 7, PH_NOISY_CC | PH_READONLY);
		ZEPHIR_CALL_FUNCTION(&_9$$3, "strval", &_5, 3, &_8$$3);
		zephir_check_call_status();
		ZEPHIR_CALL_METHOD(&_10$$3, w, "m", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_CALL_FUNCTION(&_11$$3, "strval", &_5, 3, &_10$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_12$$3);
		ZEPHIR_CONCAT_SVSVSSVSSVS(&_12$$3, "Chain at ", &_4$$3, "x", &_7$$3, " expects a weight with ", " ", &_9$$3, " rows but Matrix B has ", " ", &_11$$3, ".");
		ZEPHIR_CALL_METHOD(NULL, &_2$$3, "__construct", NULL, 2, &_12$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_2$$3, "tensor/chain.zep", 165);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZVAL_UNDEF(&_13);
	ZVAL_LONG(&_13, 0);
	zephir_update_property_array_append(this_ptr, SL("ops"), &_13);
	ZEPHIR_CALL_METHOD(&_14, w, "asTensorBuffer", NULL, 0);
	zephir_check_call_status();
	zephir_update_property_array_append(this_ptr, SL("inps"), &_14);
	ZEPHIR_CALL_METHOD(&_14, w, "n", NULL, 0);
	zephir_check_call_status();
	zephir_update_property_array_append(this_ptr, SL("extra"), &_14);
	ZEPHIR_CALL_METHOD(&_14, w, "n", NULL, 0);
	zephir_check_call_status();
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_0, 7, &_14);
	RETURN_THIS();
}

/**
 * Record either a scalar add or a row-vector add: cur = cur + value.
 *
 * When `value` is an object it is interpreted as a Matrix row (shape
 * 1 x n) and the underlying TensorBuffer is stored. When it is numeric
 * it is stored as a scalar double.
 *
 * @param mixed value
 * @return self
 */
PHP_METHOD(Tensor_Chain, add)
{
	zend_bool _4$$3;
	zval _24, _2$$4;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zephir_fcall_cache_entry *_10 = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *value, value_sub, __$null, _0, _23, _1$$4, _3$$3, _5$$3, _6$$3, _20$$3, _21$$3, _7$$5, _8$$5, _9$$5, _11$$5, _12$$5, _13$$5, _14$$5, _15$$5, _16$$5, _17$$5, _18$$5, _19$$5, _22$$6;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&value_sub);
	ZVAL_NULL(&__$null);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_23);
	ZVAL_UNDEF(&_1$$4);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_5$$3);
	ZVAL_UNDEF(&_6$$3);
	ZVAL_UNDEF(&_20$$3);
	ZVAL_UNDEF(&_21$$3);
	ZVAL_UNDEF(&_7$$5);
	ZVAL_UNDEF(&_8$$5);
	ZVAL_UNDEF(&_9$$5);
	ZVAL_UNDEF(&_11$$5);
	ZVAL_UNDEF(&_12$$5);
	ZVAL_UNDEF(&_13$$5);
	ZVAL_UNDEF(&_14$$5);
	ZVAL_UNDEF(&_15$$5);
	ZVAL_UNDEF(&_16$$5);
	ZVAL_UNDEF(&_17$$5);
	ZVAL_UNDEF(&_18$$5);
	ZVAL_UNDEF(&_19$$5);
	ZVAL_UNDEF(&_22$$6);
	ZVAL_UNDEF(&_24);
	ZVAL_UNDEF(&_2$$4);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("n", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("m", 1, 1);
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &value);
	ZEPHIR_INIT_VAR(&_0);
	zephir_gettype(&_0, value);
	if (ZEPHIR_IS_STRING(&_0, "object")) { goto zephir_switch_0_clause_0; }
	if (ZEPHIR_IS_STRING(&_0, "double")) { goto zephir_switch_0_clause_1; }
	if (ZEPHIR_IS_STRING(&_0, "integer")) { goto zephir_switch_0_clause_2; }
	goto zephir_switch_0_end;
	zephir_switch_0_clause_0: ;
		if (UNEXPECTED(!((zephir_instance_of_ev(value, tensor_matrix_ce))))) {
			ZEPHIR_INIT_VAR(&_1$$4);
			object_init_ex(&_1$$4, tensor_exceptions_invalidargumentexception_ce);
			ZEPHIR_INIT_VAR(&_2$$4);
			ZEPHIR_CONCAT_SS(&_2$$4, "Chain add with", " an object expects a Matrix.");
			ZEPHIR_CALL_METHOD(NULL, &_1$$4, "__construct", NULL, 2, &_2$$4);
			zephir_check_call_status();
			zephir_throw_exception_debug(&_1$$4, "tensor/chain.zep", 193);
			ZEPHIR_MM_RESTORE();
			return;
		}
		ZEPHIR_CALL_METHOD(&_3$$3, value, "m", NULL, 0);
		zephir_check_call_status();
		_4$$3 = !ZEPHIR_IS_LONG_IDENTICAL(&_3$$3, 1);
		if (!(_4$$3)) {
			ZEPHIR_CALL_METHOD(&_5$$3, value, "n", NULL, 0);
			zephir_check_call_status();
			zephir_read_property_cached(&_6$$3, this_ptr, _zephir_prop_0, 7, PH_NOISY_CC | PH_READONLY);
			_4$$3 = !ZEPHIR_IS_IDENTICAL(&_5$$3, &_6$$3);
		}
		if (UNEXPECTED(_4$$3)) {
			ZEPHIR_INIT_VAR(&_7$$5);
			object_init_ex(&_7$$5, tensor_exceptions_dimensionalitymismatch_ce);
			zephir_read_property_cached(&_8$$5, this_ptr, _zephir_prop_1, 6, PH_NOISY_CC | PH_READONLY);
			ZEPHIR_CALL_FUNCTION(&_9$$5, "strval", &_10, 3, &_8$$5);
			zephir_check_call_status();
			zephir_read_property_cached(&_11$$5, this_ptr, _zephir_prop_0, 7, PH_NOISY_CC | PH_READONLY);
			ZEPHIR_CALL_FUNCTION(&_12$$5, "strval", &_10, 3, &_11$$5);
			zephir_check_call_status();
			zephir_read_property_cached(&_13$$5, this_ptr, _zephir_prop_0, 7, PH_NOISY_CC | PH_READONLY);
			ZEPHIR_CALL_FUNCTION(&_14$$5, "strval", &_10, 3, &_13$$5);
			zephir_check_call_status();
			ZEPHIR_CALL_METHOD(&_15$$5, value, "m", NULL, 0);
			zephir_check_call_status();
			ZEPHIR_CALL_FUNCTION(&_16$$5, "strval", &_10, 3, &_15$$5);
			zephir_check_call_status();
			ZEPHIR_CALL_METHOD(&_17$$5, value, "n", NULL, 0);
			zephir_check_call_status();
			ZEPHIR_CALL_FUNCTION(&_18$$5, "strval", &_10, 3, &_17$$5);
			zephir_check_call_status();
			ZEPHIR_INIT_VAR(&_19$$5);
			ZEPHIR_CONCAT_SVSVSVSVSVS(&_19$$5, "Chain at ", &_9$$5, "x", &_12$$5, " expects a row vector of ", &_14$$5, " cols but ", &_16$$5, "x", &_18$$5, " given.");
			ZEPHIR_CALL_METHOD(NULL, &_7$$5, "__construct", NULL, 2, &_19$$5);
			zephir_check_call_status();
			zephir_throw_exception_debug(&_7$$5, "tensor/chain.zep", 201);
			ZEPHIR_MM_RESTORE();
			return;
		}
		ZVAL_UNDEF(&_20$$3);
		ZVAL_LONG(&_20$$3, 1);
		zephir_update_property_array_append(this_ptr, SL("ops"), &_20$$3);
		ZEPHIR_CALL_METHOD(&_21$$3, value, "asTensorBuffer", NULL, 0);
		zephir_check_call_status();
		zephir_update_property_array_append(this_ptr, SL("inps"), &_21$$3);
		zephir_update_property_array_append(this_ptr, SL("extra"), &__$null);
		RETURN_THIS();
	zephir_switch_0_clause_1: ;
	zephir_switch_0_clause_2: ;
		ZVAL_UNDEF(&_22$$6);
		ZVAL_LONG(&_22$$6, 1);
		zephir_update_property_array_append(this_ptr, SL("ops"), &_22$$6);
		ZVAL_UNDEF(&_22$$6);
		ZVAL_DOUBLE(&_22$$6, zephir_get_doubleval(value));
		zephir_update_property_array_append(this_ptr, SL("inps"), &_22$$6);
		zephir_update_property_array_append(this_ptr, SL("extra"), &__$null);
		RETURN_THIS();
	zephir_switch_0_end: ;

	ZEPHIR_INIT_VAR(&_23);
	object_init_ex(&_23, tensor_exceptions_invalidargumentexception_ce);
	ZEPHIR_INIT_VAR(&_24);
	ZEPHIR_CONCAT_SS(&_24, "Chain add with", " a non-numeric or non-Matrix operand is not supported.");
	ZEPHIR_CALL_METHOD(NULL, &_23, "__construct", NULL, 2, &_24);
	zephir_check_call_status();
	zephir_throw_exception_debug(&_23, "tensor/chain.zep", 220);
	ZEPHIR_MM_RESTORE();
	return;
}

/**
 * Record a sign flip: cur = -cur.
 *
 * @return self
 */
PHP_METHOD(Tensor_Chain, negate)
{
	zval __$null, _0;
	zval *this_ptr = getThis();

	ZVAL_NULL(&__$null);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_0);
	ZVAL_LONG(&_0, 2);
	zephir_update_property_array_append(this_ptr, SL("ops"), &_0);
	zephir_update_property_array_append(this_ptr, SL("inps"), &__$null);
	zephir_update_property_array_append(this_ptr, SL("extra"), &__$null);
	RETURN_THISW();
}

/**
 * Record the natural exponential: cur = exp(cur).
 *
 * @return self
 */
PHP_METHOD(Tensor_Chain, exp)
{
	zval __$null, _0;
	zval *this_ptr = getThis();

	ZVAL_NULL(&__$null);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_0);
	ZVAL_LONG(&_0, 3);
	zephir_update_property_array_append(this_ptr, SL("ops"), &_0);
	zephir_update_property_array_append(this_ptr, SL("inps"), &__$null);
	zephir_update_property_array_append(this_ptr, SL("extra"), &__$null);
	RETURN_THISW();
}

/**
 * Snapshot the current value into the branch slot. Forked value continues
 * to be transformed; combineDivide later divides branch by current.
 *
 * @return self
 */
PHP_METHOD(Tensor_Chain, fork)
{
	zval __$null, _0;
	zval *this_ptr = getThis();

	ZVAL_NULL(&__$null);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_0);
	ZVAL_LONG(&_0, 4);
	zephir_update_property_array_append(this_ptr, SL("ops"), &_0);
	zephir_update_property_array_append(this_ptr, SL("inps"), &__$null);
	zephir_update_property_array_append(this_ptr, SL("extra"), &__$null);
	RETURN_THISW();
}

/**
 * Combine the branch with the running value: cur = branch / cur, and
 * clear the branch slot.
 *
 * @return self
 */
PHP_METHOD(Tensor_Chain, combineDivide)
{
	zval __$null, _0;
	zval *this_ptr = getThis();

	ZVAL_NULL(&__$null);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&_0);
	ZVAL_LONG(&_0, 5);
	zephir_update_property_array_append(this_ptr, SL("ops"), &_0);
	zephir_update_property_array_append(this_ptr, SL("inps"), &__$null);
	zephir_update_property_array_append(this_ptr, SL("extra"), &__$null);
	RETURN_THISW();
}

/**
 * Run the recorded chain against the source and return the result Matrix.
 *
 * On the SiLU shape (`fork/negate/exp/add(1.0)/combineDivide`) the planner
 * dispatches a single fused kernel. Otherwise it re-runs the primitives in
 * the order recorded.
 *
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Chain, done)
{
	zval _0, result, _4, _5, _6, _7, _8, _9, _10, _11, _1$$3, _2$$3, _3$$3;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&_4);
	ZVAL_UNDEF(&_5);
	ZVAL_UNDEF(&_6);
	ZVAL_UNDEF(&_7);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	static zend_string *_zephir_prop_3 = NULL;
	static zend_string *_zephir_prop_4 = NULL;
	static zend_string *_zephir_prop_5 = NULL;
	static zend_string *_zephir_prop_6 = NULL;
	static zend_string *_zephir_prop_7 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("ops", 3, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("buffer", 6, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("initM", 5, 1);
	}
	if (UNEXPECTED(!_zephir_prop_3)) {
		_zephir_prop_3 = zend_string_init("initN", 5, 1);
	}
	if (UNEXPECTED(!_zephir_prop_4)) {
		_zephir_prop_4 = zend_string_init("inps", 4, 1);
	}
	if (UNEXPECTED(!_zephir_prop_5)) {
		_zephir_prop_5 = zend_string_init("extra", 5, 1);
	}
	if (UNEXPECTED(!_zephir_prop_6)) {
		_zephir_prop_6 = zend_string_init("m", 1, 1);
	}
	if (UNEXPECTED(!_zephir_prop_7)) {
		_zephir_prop_7 = zend_string_init("n", 1, 1);
	}
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	zephir_read_property_cached(&_0, this_ptr, _zephir_prop_0, 8, PH_NOISY_CC | PH_READONLY);
	if (UNEXPECTED(zephir_fast_count_int(&_0) == 0)) {
		zephir_read_property_cached(&_1$$3, this_ptr, _zephir_prop_1, 3, PH_NOISY_CC | PH_READONLY);
		zephir_read_property_cached(&_2$$3, this_ptr, _zephir_prop_2, 4, PH_NOISY_CC | PH_READONLY);
		zephir_read_property_cached(&_3$$3, this_ptr, _zephir_prop_3, 5, PH_NOISY_CC | PH_READONLY);
		ZEPHIR_RETURN_CALL_CE_STATIC(tensor_matrix_ce, "fromBuffer", NULL, 0, &_1$$3, &_2$$3, &_3$$3);
		zephir_check_call_status();
		RETURN_MM();
	}
	ZEPHIR_INIT_VAR(&result);
	zephir_read_property_cached(&_4, this_ptr, _zephir_prop_1, 3, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_5, this_ptr, _zephir_prop_2, 4, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_6, this_ptr, _zephir_prop_3, 5, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_7, this_ptr, _zephir_prop_0, 8, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_8, this_ptr, _zephir_prop_4, 9, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_9, this_ptr, _zephir_prop_5, 10, PH_NOISY_CC | PH_READONLY);
	tensor_chain_plan(&result, &_4, &_5, &_6, &_7, &_8, &_9);
	zephir_read_property_cached(&_10, this_ptr, _zephir_prop_6, 6, PH_NOISY_CC | PH_READONLY);
	zephir_read_property_cached(&_11, this_ptr, _zephir_prop_7, 7, PH_NOISY_CC | PH_READONLY);
	ZEPHIR_RETURN_CALL_CE_STATIC(tensor_matrix_ce, "fromBuffer", NULL, 0, &result, &_10, &_11);
	zephir_check_call_status();
	RETURN_MM();
}

