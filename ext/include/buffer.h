#ifndef TENSOR_BUFFER_H
#define TENSOR_BUFFER_H

#include <Zend/zend.h>

/* The Zephir compiler emits references to `tensor_buffer_ce` for the kernel
 * `Tensor\Buffer` class even though the kernel registers and exports it as
 * `zephir_buffer_ce`. Alias the two (see config.json module initializer). */
extern zend_class_entry * tensor_buffer_ce;

/* The `Tensor\TensorBuffer` decorator class. */
extern zend_class_entry * tensor_tensorbuffer_ce;

/* Allocate a new `Tensor\TensorBuffer` wrapping a fresh zero-filled double
 * buffer of `len` elements. The exposed `Buffer` is written to `buffer` so
 * callers can fill it through a raw pointer. Returns FAILURE (and throws) on
 * allocation failure; on SUCCESS the caller owns a reference to `buffer` and
 * must release it with zval_ptr_dtor() when done. */
int tensor_tensorbuffer_create(zval * ret, zend_long len, zval * buffer);

/* Unwrap the double buffer hidden inside a `Tensor\TensorBuffer` object,
 * returning a raw pointer into it. Sets `*success` to 1 on success and 0 on
 * failure (throwing an InvalidArgumentException). An empty buffer yields
 * success with a NULL pointer and `*len` of 0. */
double * tensor_tensorbuffer_doubles(zval * obj, zend_long * len, int * success);

/* Build an object of the kernel `Tensor\Buffer` class from a PHP array of
 * values, casting every element to a double. Passed arrays must be 1-d and
 * positionally indexed; keys are discarded. Returns FAILURE (NULL zval) on
 * invalid input. Used to seed TensorBuffers from arrays in Zephir. */
void tensor_buffer_from_array(zval * ret, zval * arr);

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