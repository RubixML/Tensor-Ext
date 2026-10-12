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
int tensor_tensorbuffer_create(zval * ret, zend_long len, zval * buffer);

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

/* Structural views
 *
 * A view is a `Tensor\TensorBuffer` that keeps the flat `Buffer` of its source
 * and adds an `offset`, an integer `shape`, and a matching integer `strides`
 * array. A buffer created without those three properties is a plain dense 1-D
 * buffer and is treated as `offset = 0`, `shape = [len]`, `strides = [1]`.
 *
 * Views are read-only: set() and sort() reject them, and asBuffer() returns a
 * freshly materialised dense Buffer. Kernels that still require contiguous
 * storage reach the data through tensor_tensorbuffer_doubles(), which gathers a
 * non-contiguous view into a cached dense Buffer held on the view for the
 * lifetime of the object.
 */

/* Build a `Tensor\TensorBuffer` view over `buffer` with the given element
 * `offset`, `ndim` (at most 4) and parallel shape/strides lists. Returns FAILURE
 * (throwing) when the properties cannot be written. */
int tensor_buffer_make_view(zval * ret, zval * buffer, zend_long offset, int ndim, const zend_long * shape, const zend_long * strides);

/* Write the logical element count of a TensorBuffer, taken from its `shape`
 * property, to `ret`. A NULL/non-array shape falls back to the length of the
 * wrapped Buffer. */
void tensor_tensorbuffer_count(zval * ret, zval * buffer, zval * shape);

/* Write the view as a PHP array of doubles to `ret`. */
void tensor_tensorbuffer_to_array(zval * ret, zval * buffer, zval * offset, zval * shape, zval * strides);

/* Write a dense `Tensor\Buffer` holding the view's elements in row-major order
 * to `ret`. */
void tensor_buffer_view_materialize(zval * ret, zval * buffer, zval * offset, zval * shape, zval * strides);

/* Write the view's element at `index` to `ret`, throwing when out of bounds. */
void tensor_tensorbuffer_get(zval * ret, zval * buffer, zval * offset, zval * shape, zval * strides, zval * index);

/* Set the element at `index`, throwing when the buffer is a read-only view.
 * Bumps the in-place write epoch so cached gathered views are refreshed. */
void tensor_tensorbuffer_set(zval * ret, zval * buffer, zval * offset, zval * shape, zval * strides, zval * index, zval * value);

/* 1-D view producers; they compose with an existing 1-D view. */
void tensor_buffer_slice_view(zval * ret, zval * buffer, zval * offset, zval * shape, zval * strides, zval * slice_offset, zval * slice_length);
void tensor_buffer_slice_strided_view(zval * ret, zval * buffer, zval * offset, zval * shape, zval * strides, zval * slice_offset, zval * slice_length, zval * slice_stride);
void tensor_buffer_split_view(zval * ret, zval * buffer, zval * offset, zval * shape, zval * strides, zval * chunk_length);

/* Build a 2-D transpose view of `a`, interpreted as an `m` x `n` matrix. */
void tensor_buffer_transpose_view(zval * ret, zval * a, zend_long m, zend_long n);

#endif