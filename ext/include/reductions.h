#ifndef TENSOR_REDUCTIONS_H
#define TENSOR_REDUCTIONS_H

#include <Zend/zend.h>

void tensor_reduce_sum(zval * return_value, zval * obj, zval * groups, zval * length);
void tensor_reduce_product(zval * return_value, zval * obj, zval * groups, zval * length);
void tensor_reduce_min(zval * return_value, zval * obj, zval * groups, zval * length);
void tensor_reduce_max(zval * return_value, zval * obj, zval * groups, zval * length);
void tensor_reduce_argmin(zval * return_value, zval * obj, zval * groups, zval * length);
void tensor_reduce_argmax(zval * return_value, zval * obj, zval * groups, zval * length);

void tensor_median(zval * return_value, zval * obj, zval * n);
void tensor_quantile(zval * return_value, zval * obj, zval * n, zval * q);

double tensor_sum_doubles(const double * data, zend_long len);
double tensor_product_doubles(const double * data, zend_long len);

#endif