#ifndef TENSOR_SHAPE_H
#define TENSOR_SHAPE_H

#include <Zend/zend.h>

void tensor_matrix_transpose(zval * return_value, zval * a, zval * m, zval * n);

/* Unwrap the flat row-major buffer behind a Matrix as `m` contiguous rows of
 * `n` elements, throwing when the row length is not positive or does not
 * divide the buffer. Shared with the row-wise reductions in reductions.c. */
int tensor_matrix_doubles(zval * obj, zval * n_zval, double ** ptr, zend_long * m, zend_long * n);

void tensor_matrix_repeat(zval * return_value, zval * obj, zval * n, zval * times_m, zval * times_n);
void tensor_matrix_to_array(zval * return_value, zval * obj, zval * n);

#endif