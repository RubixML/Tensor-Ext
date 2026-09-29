#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <cblas.h>
#include "kernel/operators.h"
#include "include/cpu.h"
#include "include/dispatch.h"

/**
 * Sets the number of threads to use when parallel processesing.
 * 
 * @param return_value
 * @param threads
 */
void tensor_set_num_threads(zval * return_value, zval * threads)
{
    int n = zephir_get_intval(threads);
    
    openblas_set_num_threads(n);

    RETURN_TRUE;
}

/**
 * Return the number of threads to use when parallel processesing.
 *
 * @param return_value
 */
void tensor_get_num_threads(zval * return_value)
{
    long threads = openblas_get_num_threads();

    RETURN_LONG(threads);
}

/**
 * Return the CPU features the extension detected, along with the route the
 * elementwise kernels actually took.
 *
 * @param return_value
 */
void tensor_get_cpu_features(zval * return_value)
{
    array_init_size(return_value, 4);

    add_assoc_bool(return_value, "avx", tensor_cpu_has_avx());
    add_assoc_bool(return_value, "avx2", tensor_cpu_has_avx2());
    add_assoc_bool(return_value, "fma", tensor_cpu_has_fma());
    add_assoc_bool(return_value, "avx512", tensor_cpu_has_avx512());
}

/**
 * Reset the dispatched C kernels to their baseline variants.
 *
 * Idempotent: calling it a second time rewrites the route pointers to their
 * initial file-scope values, so the cost is a handful of pointer stores.
 * The user must call this from a single worker before any tensor operation
 * runs to avoid racing a concurrent kernel call mid-store.
 *
 * The inverse of tensor_enable_optimized_kernels().
 *
 * @param return_value
 */
void tensor_disable_optimized_kernels(zval * return_value)
{
    tensor_arithmetic_dispatch_sse_init();
    tensor_comparison_dispatch_sse_init();
    tensor_unary_dispatch_sse_init();
    tensor_linear_algebra_dispatch_sse_init();
    tensor_signal_processing_dispatch_sse_init();
}

/**
 * Route the dispatched C kernels back through their widest usable variants.
 *
 * The feature bits were sampled once and cached by tensor_cpu_detect(), so
 * this is the same selection the module initializer made, without the CPUID
 * round trip. Off x86, or on a CPU with no usable wide ISA, every feature bit
 * is clear and tensor_cpu_init() leaves each route on its baseline variant,
 * which is the correct answer rather than a silent upgrade to unsupported
 * instructions.
 *
 * Idempotent, and the exact inverse of tensor_disable_optimized_kernels():
 * the two are a pair, and neither carries state of its own -- the route
 * pointers themselves are the state. The same single-worker caveat applies.
 *
 * @param return_value
 */
void tensor_enable_optimized_kernels(zval * return_value)
{
    tensor_cpu_init();
}
