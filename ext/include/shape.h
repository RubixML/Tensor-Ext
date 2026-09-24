#ifndef TENSOR_SHAPE_H
#define TENSOR_SHAPE_H

#include <Zend/zend.h>

void tensor_matrix_transpose(zval * return_value, zval * a, zval * m, zval * n);

#endif