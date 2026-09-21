#ifndef TENSOR_BUFFER_H
#define TENSOR_BUFFER_H

#include <Zend/zend.h>

/* The Zephir compiler emits references to `tensor_buffer_ce` for the kernel
 * `Tensor\Buffer` class even though the kernel registers and exports it as
 * `zephir_buffer_ce`. Alias the two (see config.json module initializer). */
extern zend_class_entry * tensor_buffer_ce;

void tensor_buffer_sort(zval * return_value, zval * obj, zval * ascending);
void tensor_buffer_slice(zval * return_value, zval * obj, zval * offset, zval * length);

#endif