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
    array_init_size(return_value, 3);

    add_assoc_bool(return_value, "avx", tensor_cpu_has_avx() ? 1 : 0);
    add_assoc_bool(return_value, "avx2", tensor_cpu_has_avx2() ? 1 : 0);
    add_assoc_string(return_value, "dispatch", tensor_cpu_dispatch_name());
}
