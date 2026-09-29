
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
#include "kernel/operators.h"
#include "kernel/object.h"
#include "include/settings.h"


/**
 * Settings
 *
 * @category    Scientific Computing
 * @package     Rubix/Tensor
 * @author      Andrew DalPino
 */
ZEPHIR_INIT_CLASS(Tensor_Settings)
{
	ZEPHIR_REGISTER_CLASS(Tensor, Settings, tensor, settings, tensor_settings_method_entry, 0);

	return SUCCESS;
}

/**
 * Set the number of CPU threads to use when multiprocessing.
 *
 * @param int threads
 * @return void
 */
PHP_METHOD(Tensor_Settings, setNumThreads)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zval *threads_param = NULL, _0$$3, _1$$3, _2$$3, _3$$3, status, _4;
	zend_long threads, ZEPHIR_LAST_CALL_STATUS;

	ZVAL_UNDEF(&_0$$3);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&status);
	ZVAL_UNDEF(&_4);
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(threads)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 0, &threads_param);
	if (UNEXPECTED(threads < 1)) {
		ZEPHIR_INIT_VAR(&_0$$3);
		object_init_ex(&_0$$3, tensor_exceptions_invalidargumentexception_ce);
		ZVAL_LONG(&_1$$3, threads);
		ZEPHIR_CALL_FUNCTION(&_2$$3, "strval", NULL, 5, &_1$$3);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SSVS(&_3$$3, "The number of threads", " must be greater than 0, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_0$$3, "__construct", NULL, 2, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_0$$3, "tensor/settings.zep", 24);
		ZEPHIR_MM_RESTORE();
		return;
	}
	ZEPHIR_INIT_VAR(&status);
	ZVAL_LONG(&_4, threads);
	tensor_set_num_threads(&status, &_4);
	ZEPHIR_MM_RESTORE();
}

/**
 * Return the number of CPU threads used for multiprocessing.
 *
 * @return int
 */
PHP_METHOD(Tensor_Settings, numThreads)
{

	tensor_get_num_threads(return_value);
	return;
}

/**
 * Return the CPU features the extension detected at startup.
 *
 * The returned array has four keys: "avx", "avx2", "avx512" and "fma",
 * each a flag reporting whether that instruction set is available on the
 * CPU. "fma" reports FMA3, which is a separate feature from "avx" rather
 * than a consequence of it: a CPU can have 256-bit AVX and no fused
 * multiply-add, and the two decide different kernel routes.
 *
 * @return array
 */
PHP_METHOD(Tensor_Settings, cpuFeatures)
{

	tensor_get_cpu_features(return_value);
	return;
}

/**
 * Reset the dispatched C kernels to their scalar baseline variants.
 *
 * The elementwise arithmetic, comparison, unary, linear algebra, and
 * convolution kernels each have a baseline (scalar) variant and a wide
 * SIMD variant, selected once at load time by the CPU feature bits; this
 * method walks every route back to its baseline variant for the rest of
 * the process. The OpenBLAS/LAPACKE-backed routines and the per-elem libm
 * transcendental ops are unaffected: they are scalar either way, and the
 * thread pool is controlled by `setNumThreads`.
 *
 * This is one half of a pair: `enableOptimizedKernels` puts the widest
 * route the CPU supports back in place. It is idempotent, and safe to call
 * multiple times.
 *
 * Call this from a single worker before any tensor op runs so the route
 * pointer resets do not race a concurrent kernel call.
 *
 * @return void
 */
PHP_METHOD(Tensor_Settings, disableOptimizedKernels)
{
	zval status;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;

	ZVAL_UNDEF(&status);
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&status);
	tensor_disable_optimized_kernels(&status);
	ZEPHIR_MM_RESTORE();
}

/**
 * Route the dispatched C kernels back through their widest usable variants.
 *
 * The inverse of `disableOptimizedKernels`: the CPU feature bits were
 * sampled once and cached when the extension loaded, so this replays the
 * same selection the module initializer made, restoring the AVX, FMA, or
 * AVX-512 route as appropriate. On a CPU with no usable wide instruction
 * set the routes stay on their baseline variants, which is the correct
 * outcome rather than a failed upgrade.
 *
 * Idempotent, and safe to call multiple times. Call this from a single
 * worker before any tensor op runs, for the same reason as the disable.
 *
 * @return void
 */
PHP_METHOD(Tensor_Settings, enableOptimizedKernels)
{
	zval status;
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;

	ZVAL_UNDEF(&status);
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);

	ZEPHIR_INIT_VAR(&status);
	tensor_enable_optimized_kernels(&status);
	ZEPHIR_MM_RESTORE();
}

