#ifndef TENSOR_BUFFER_BINARY_H
#define TENSOR_BUFFER_BINARY_H

#include <Zend/zend.h>

/**
 * Element-wise binary maths over <Ns>\Buffer operands.
 *
 * These replace the twelve arithmetic and comparison handlers in arithmetic.c
 * and comparison.c, which contain no BLAS call at all: each is a hash walk
 * doing one zend_hash_index_find per element per operand and one
 * add_next_index_double per element of the result. The arithmetic was about one
 * percent of the work.
 *
 * As with the unary handlers, the operation is named by a string, resolved once
 * per call, and each operation then runs its own loop.
 */
void tensor_buffer_binary(zval * return_value, zval * a, zval * b, zval * op);

/** The same twelve operations against a scalar right-hand side. */
void tensor_buffer_binary_scalar(zval * return_value, zval * a, zval * b, zval * op);

/**
 * Broadcasts one buffer element down each row of a matrix: c[i][j] = op(a[i], b[i][j]).
 *
 * This is ColumnVector's *Matrix family, which was the only element-wise family
 * left running as interpreted Zephir. It was also the only one that could take
 * the process down: when an arithmetic exception was raised part-way through
 * one of those loops the memory frame was not unwound, and the next
 * ZEPHIR_OBS_VAR on the same slot tripped a kernel assertion and called
 * abort(). Moving the loop into C removes the cause.
 *
 * The matrix operand and the result are PHP arrays because Matrix is still
 * array-backed; that changes when Matrix moves onto a flat buffer.
 */
void tensor_buffer_broadcast_rows(zval * return_value, zval * a, zval * b, zval * op);

#endif
