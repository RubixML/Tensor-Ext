#ifndef TENSOR_BUFFER_H
#define TENSOR_BUFFER_H

#include <Zend/zend.h>

/* The Zephir compiler emits references to `tensor_buffer_ce` for the kernel
 * `Tensor\Buffer` class even though the kernel registers and exports it as
 * `zephir_buffer_ce`. Alias the two (see config.json module initializer). */
extern zend_class_entry * tensor_buffer_ce;

/* The `Tensor\TensorBuffer` decorator class. */
extern zend_class_entry * tensor_tensorbuffer_ce;

/* Allocate a new `Tensor\TensorBuffer` wrapping a fresh double buffer of `len`
 * elements whose contents are NOT zeroed. The exposed `Buffer` is written to
 * `buffer` so callers can fill it through a raw pointer. Returns FAILURE (and
 * throws) on allocation failure; on SUCCESS the caller owns a reference to
 * `buffer` and must release it with zval_ptr_dtor() when done.
 *
 * The caller MUST write all `len` elements before reading any of them, otherwise
 * uninitialized heap memory is observable in the result. */
int tensor_tensorbuffer_create_uninit(zval * ret, zend_long len, zval * buffer);

/* Allocate a new `Tensor\TensorBuffer` wrapping a fresh zero-filled double
 * buffer of `len` elements. The exposed `Buffer` is written to `buffer` so
 * callers can fill it through a raw pointer. Returns FAILURE (and throws) on
 * allocation failure; on SUCCESS the caller owns a reference to `buffer` and
 * must release it with zval_ptr_dtor() when done.
 *
 * Zero-fills because some callers hand the buffer to BLAS with `beta = 0.0`,
 * which must not read uninitialized bytes. Prefer the _uninit variant
 * everywhere the caller fully overwrites the buffer. */
int tensor_tensorbuffer_create_zeros(zval * ret, zend_long len, zval * buffer);

/* Oversized buffer cache lifecycle, wired to PHP's RINIT and RSHUTDOWN through
 * the `initializers.request` and `destructors.request` hooks in config.json.
 * Both discard the cached pointer without freeing it: the block belongs to a
 * Zend MM heap that is being reclaimed, and Zend MM frees every request
 * allocation itself, so freeing it here would be a double free.
 *
 * Not thread safe: the cache is a plain static, matching the assumption in
 * include/cpu.c. It is only valid for non-thread-safe builds. */
void tensor_pool_activate(void);
void tensor_pool_seal(void);

/* Unwrap the double buffer hidden inside a `Tensor\TensorBuffer` object,
 * returning a raw pointer into it. Sets `*success` to 1 on success and 0 on
 * failure (throwing an InvalidArgumentException). An empty buffer yields
 * success with a NULL pointer and `*len` of 0. */
double * tensor_tensorbuffer_doubles(zval * obj, zend_long * len, int * success);

/* Build a `Tensor\TensorBuffer` decorator from a PHP array of values, casting
 * every element to a double. Passed arrays must be 1-d and positionally
 * indexed; keys are discarded. On failure `ret` is set to NULL. Used to seed
 * tensor classes from arrays in Zephir. */
void tensor_buffer_from_array(zval * ret, zval * arr);

void tensor_buffer_sort(zval * return_value, zval * obj, zval * ascending);
void tensor_buffer_slice(zval * return_value, zval * obj, zval * offset, zval * length);
void tensor_buffer_slice_strided(zval * return_value, zval * obj, zval * offset, zval * length, zval * stride);
void tensor_buffer_concat(zval * return_value, zval * obj, zval * others);
void tensor_buffer_split(zval * return_value, zval * obj, zval * chunk_length);
void tensor_buffer_repeat(zval * return_value, zval * obj, zval * times);

#endif