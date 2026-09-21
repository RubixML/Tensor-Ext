#ifndef TENSOR_BUFFER_H
#define TENSOR_BUFFER_H

#include <Zend/zend.h>

/* The Zephir compiler emits references to `tensor_buffer_ce` for the kernel
 * `Tensor\Buffer` class even though the kernel registers and exports it as
 * `zephir_buffer_ce`. Alias the two (see config.json module initializer). */
extern zend_class_entry * tensor_buffer_ce;

void tensor_buffer_sort(zval * return_value, zval * obj, zval * ascending);
void tensor_buffer_slice(zval * return_value, zval * obj, zval * offset, zval * length);
void tensor_buffer_sum(zval * return_value, zval * obj);
void tensor_buffer_product(zval * return_value, zval * obj);
void tensor_buffer_min(zval * return_value, zval * obj);
void tensor_buffer_max(zval * return_value, zval * obj);
void tensor_buffer_argmin(zval * return_value, zval * obj);
void tensor_buffer_argmax(zval * return_value, zval * obj);
void tensor_buffer_slice_strided(zval * return_value, zval * obj, zval * offset, zval * length, zval * stride);
void tensor_buffer_concat(zval * return_value, zval * obj, zval * others);
void tensor_buffer_split(zval * return_value, zval * obj, zval * chunk_length);
void tensor_buffer_repeat(zval * return_value, zval * obj, zval * times);

#endif