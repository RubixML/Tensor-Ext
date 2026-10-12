#ifndef TENSOR_SHAPE_H
#define TENSOR_SHAPE_H

#include <Zend/zend.h>
#include "include/view.h"

void tensor_matrix_transpose(zval * return_value, zval * a, zval * m, zval * n);

/* Resolve a Matrix (or a wrapped Buffer) into a view over `m` rows of `n`
 * elements, throwing when the row length is not positive. Shared with the
 * row-wise reductions in reductions.c. The view is consumed natively: callers
 * walk `m` rows via tensor_view_copy_row() rather than assuming contiguous
 * storage, so a transposed or otherwise strided matrix is read in place. */
int tensor_matrix_view(zval * obj, zend_long n_hat, tensor_double_view * v, zend_long * m, zend_long * n);

void tensor_matrix_repeat(zval * return_value, zval * obj, zval * n, zval * times_m, zval * times_n);
void tensor_matrix_to_array(zval * return_value, zval * obj, zval * n);

#endif