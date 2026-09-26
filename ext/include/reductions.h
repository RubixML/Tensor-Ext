#ifndef TENSOR_REDUCTIONS_H
#define TENSOR_REDUCTIONS_H

#include <Zend/zend.h>

void tensor_reduce_sum(zval * return_value, zval * obj, zval * groups, zval * length);
void tensor_reduce_product(zval * return_value, zval * obj, zval * groups, zval * length);
void tensor_reduce_min(zval * return_value, zval * obj, zval * groups, zval * length);
void tensor_reduce_max(zval * return_value, zval * obj, zval * groups, zval * length);
void tensor_reduce_argmin(zval * return_value, zval * obj, zval * groups, zval * length);
void tensor_reduce_argmax(zval * return_value, zval * obj, zval * groups, zval * length);

/* The eight-way pairwise sum shared by every reduction over a run of doubles.
 * Callers that fuse a reduction into a wider pass use this so the accumulation
 * stays identical to tensor_reduce_sum's. */
double tensor_sum_doubles(const double * data, zend_long len);

/* The eight-way pairwise product over a run of doubles, with its partials
 * merged in a small tree. Exported alongside the sum so the whole group of
 * reduction helpers is visible from one place. */
double tensor_product_doubles(const double * data, zend_long len);

void tensor_median(zval * return_value, zval * obj, zval * n);
void tensor_quantile(zval * return_value, zval * obj, zval * n, zval * q);
void tensor_matrix_repeat(zval * return_value, zval * obj, zval * n, zval * times_m, zval * times_n);
void tensor_matrix_to_array(zval * return_value, zval * obj, zval * n);

#endif