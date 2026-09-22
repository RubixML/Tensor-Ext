#ifndef TENSOR_BUFFER_OPS_H
#define TENSOR_BUFFER_OPS_H

#include <Zend/zend.h>

/**
 * Elementwise kernels over <Ns>\Buffer operands (zephir-lang/zephir#2721).
 *
 * The array-backed twins in arithmetic.c and linear_algebra.c spend most of
 * their time converting: a zend_hash_index_find per element on each operand,
 * and an add_next_index_double per element of the result -- or, for the BLAS
 * routines, an emalloc + pack + unpack around the one call that does the work.
 * These take contiguous double* straight off the operands, so nothing is
 * packed, unpacked or allocated beyond the result buffer itself.
 */
/** Converts a PHP array into a double Buffer. The only conversion point. */
void tensor_buffer_from_array(zval * return_value, zval * a);

void tensor_buffer_add(zval * return_value, zval * a, zval * b);
void tensor_buffer_multiply(zval * return_value, zval * a, zval * b);
void tensor_buffer_dot(zval * return_value, zval * a, zval * b);

/**
 * Allocate-then-fill generators.
 *
 * A buffer is fixed size, so the `while count(a) < n { let a[] = ... }` pattern
 * these replace has no equivalent: the extent has to be known up front, which it
 * always was.
 */
void tensor_buffer_fill(zval * return_value, zval * value, zval * n);
void tensor_buffer_linspace(zval * return_value, zval * min, zval * max, zval * n);

#endif
