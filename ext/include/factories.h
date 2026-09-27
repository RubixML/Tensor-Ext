#ifndef TENSOR_FACTORIES_H
#define TENSOR_FACTORIES_H

#include <Zend/zend.h>

/**
 * Allocate a fresh `Tensor\TensorBuffer` of `n` elements, filled with the
 * given `value`. Returns the buffer in return_value on success; throws
 * otherwise.
 */
void tensor_fill(zval * return_value, zval * value, zval * n);

/**
 * Allocate a fresh `Tensor\TensorBuffer` of `n * n` elements holding a
 * diagonal matrix in row-major order, where `n` is the count of `elements`:
 * each element of the given array on the diagonal, 0.0 elsewhere. Values are
 * taken positionally (keys discarded) and cast with the engine's own
 * double conversion. Returns the buffer in return_value on success; throws
 * otherwise.
 */
void tensor_diagonal(zval * return_value, zval * elements);

/**
 * Allocate a fresh `Tensor\TensorBuffer` of `n` elements, filled with a
 * random uniform in [0, 1) drawn from PHP's MT19937 stream (i.e. the same
 * stream that `mt_rand()`/`rand()` consume). Reproducible with `mt_srand()`.
 * Returns the buffer in return_value on success; throws otherwise.
 */
void tensor_random_uniform_01(zval * return_value, zval * n);

/**
 * Allocate a fresh `Tensor\TensorBuffer` of `n` elements, filled with a random
 * value uniformly distributed over [-1, 1], drawn from PHP's MT19937 stream.
 * Reproducible with `mt_srand()`; same stream as `mt_rand()`/`rand()`.
 */
void tensor_random_uniform_pm1(zval * return_value, zval * n);

/**
 * Allocate a fresh `Tensor\TensorBuffer` of `n` elements, filled with a random
 * value from the standard normal (mean 0, unit variance) via the Box–Muller
 * transform, drawn from PHP's MT19937 stream. Reproducible with `mt_srand()`.
 */
void tensor_random_gaussian(zval * return_value, zval * n);

#endif
