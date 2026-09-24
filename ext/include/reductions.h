#ifndef TENSOR_REDUCTIONS_H
#define TENSOR_REDUCTIONS_H

#include <Zend/zend.h>

void tensor_buffer_sum(zval * return_value, zval * obj);
void tensor_buffer_product(zval * return_value, zval * obj);
void tensor_buffer_min(zval * return_value, zval * obj);
void tensor_buffer_max(zval * return_value, zval * obj);
void tensor_buffer_argmin(zval * return_value, zval * obj);
void tensor_buffer_argmax(zval * return_value, zval * obj);

void tensor_matrix_sum(zval * return_value, zval * obj, zval * n);
void tensor_matrix_product(zval * return_value, zval * obj, zval * n);
void tensor_matrix_min(zval * return_value, zval * obj, zval * n);
void tensor_matrix_max(zval * return_value, zval * obj, zval * n);
void tensor_matrix_argmin(zval * return_value, zval * obj, zval * n);
void tensor_matrix_argmax(zval * return_value, zval * obj, zval * n);
void tensor_matrix_median(zval * return_value, zval * obj, zval * n);
void tensor_matrix_quantile(zval * return_value, zval * obj, zval * n, zval * q);
void tensor_matrix_repeat(zval * return_value, zval * obj, zval * n, zval * times_m, zval * times_n);
void tensor_matrix_to_array(zval * return_value, zval * obj, zval * n);

#endif