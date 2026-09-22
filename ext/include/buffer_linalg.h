#ifndef TENSOR_BUFFER_LINALG_H
#define TENSOR_BUFFER_LINALG_H

#include <Zend/zend.h>

/**
 * BLAS and LAPACK over matrices already held as flat buffers.
 *
 * The array-backed versions in linear_algebra.c emalloc a scratch double*, walk
 * the PHP array into it, call the one routine that does the work, and walk the
 * result back out. With the matrix already contiguous, all of that disappears:
 * the buffer pointer goes straight to BLAS, and the result is another buffer.
 */

/** C = A * B. `k` is A's row length, `n` is B's; A's rows are inferred. */
void tensor_buffer_matmul(zval * return_value, zval * a, zval * b, zval * k, zval * n);

/** The inverse of a square matrix, via LAPACK LU factorisation. */
void tensor_buffer_inverse(zval * return_value, zval * a, zval * n);

#endif
