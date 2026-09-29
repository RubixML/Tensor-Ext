#ifndef TENSOR_SOFTMAX_H
#define TENSOR_SOFTMAX_H

#include <Zend/zend.h>

/* Normalize every column of the flat row-major m x n buffer behind a Matrix in
 * place, i.e. element (i, j) of the result is exp(a(i, j) - max_j) divided by
 * the sum of that column, so each of the n output columns sums to 1. The column
 * count arrives as `n`; a Vector passes 1, which presents its flat buffer as a
 * single column and turns the same kernel into a whole-vector softmax. */
void tensor_softmax(zval * return_value, zval * a, zval * n);

#endif
