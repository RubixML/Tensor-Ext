#ifndef TENSOR_SETTINGS_H
#define TENSOR_SETTINGS_H

#include <Zend/zend.h>

void tensor_set_num_threads(zval * return_value, zval * threads);
void tensor_get_num_threads(zval * return_value);
void tensor_get_cpu_features(zval * return_value);

/* Reset every dispatched kernel route to its baseline variant. Idempotent. */
void tensor_disable_optimized_kernels(zval * return_value);

/* Re-run the CPU feature selection and route every kernel to its widest
 * usable variant. Idempotent, and the exact inverse of the call above. */
void tensor_enable_optimized_kernels(zval * return_value);

#endif
