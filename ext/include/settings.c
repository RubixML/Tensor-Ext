#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <cblas.h>
#include "kernel/operators.h"
#include "include/cpu.h"

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
    add_assoc_bool(return_value, "avx512", tensor_cpu_has_avx512());

    /* FMA3 is reported alongside AVX rather than folded into it because it is a
     * separate feature bit, and because the convolution routes are chosen from
     * the two independently: a CPU with 256-bit AVX and no FMA3 still gets the
     * AVX kernels, and one with FMA3 gets the fused ones. */
    add_assoc_bool(return_value, "fma", tensor_cpu_has_fma());
}
