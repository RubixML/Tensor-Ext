#ifndef TENSOR_BUFFER_MATRIX_H
#define TENSOR_BUFFER_MATRIX_H

#include <Zend/zend.h>

/**
 * Shape-aware operations over a matrix held as one flat, row-major buffer.
 *
 * Element-wise matrix maths needs nothing here: a matrix buffer is m*n doubles
 * laid out contiguously, so tensor_buffer_unary() and tensor_buffer_binary()
 * apply to it unchanged. What does need the shape is broadcasting a vector
 * along one axis, reducing along rows, transposing, and taking a row, a column
 * or the diagonal.
 *
 * All of these replace per-row work: the *Vector, *ColumnVector, *Matrix and
 * *Scalar families each made one C call per row, allocating one result array
 * per row, so a 1000-row matrix meant a thousand calls and a thousand
 * allocations for one logical operation.
 */

/** Flattens list<list<float>> into one row-major buffer. The only conversion in. */
void tensor_buffer_from_matrix(zval * return_value, zval * a);

/** Rebuilds list<list<float>> from a buffer, given the row length. The only conversion out. */
void tensor_buffer_to_matrix(zval * return_value, zval * a, zval * n);

/** c[i][j] = op(a[i][j], v[j]) -- a row vector broadcast down the rows. */
void tensor_buffer_broadcast_vector(zval * return_value, zval * a, zval * v, zval * n, zval * op);

/** c[i][j] = op(a[i][j], v[i]) -- a column vector broadcast across the columns. */
void tensor_buffer_broadcast_column(zval * return_value, zval * a, zval * v, zval * n, zval * op);

/** Reduces each row to one element: sum, product, min, max. Yields m elements. */
void tensor_buffer_row_reduce(zval * return_value, zval * a, zval * n, zval * op);

/** Row-major transpose: an n x m buffer from an m x n one. */
void tensor_buffer_transpose(zval * return_value, zval * a, zval * n);

/**
 * A strided copy, which covers every sub-range the class hands out: a row is
 * stride 1, a column is stride n, and the diagonal is stride n + 1.
 */
void tensor_buffer_stride(zval * return_value, zval * a, zval * offset, zval * stride, zval * count);

/** Stacks b beneath a. Both must have the same row length; the buffers concatenate. */
void tensor_buffer_append(zval * return_value, zval * a, zval * b);

/** Places b to the right of a, row by row. Row lengths may differ; row counts may not. */
void tensor_buffer_append_columns(zval * return_value, zval * a, zval * b, zval * an, zval * bn);

#endif
