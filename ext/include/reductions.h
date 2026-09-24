#ifndef TENSOR_REDUCTIONS_H
#define TENSOR_REDUCTIONS_H

#include <Zend/zend.h>

void tensor_buffer_sum(zval * return_value, zval * obj);
void tensor_buffer_product(zval * return_value, zval * obj);
void tensor_buffer_min(zval * return_value, zval * obj);
void tensor_buffer_max(zval * return_value, zval * obj);
void tensor_buffer_argmin(zval * return_value, zval * obj);
void tensor_buffer_argmax(zval * return_value, zval * obj);

#endif