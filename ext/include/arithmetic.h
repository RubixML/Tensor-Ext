#ifndef TENSOR_ARITHMETIC_H
#define TENSOR_ARITHMETIC_H

#include <Zend/zend.h>

void tensor_multiply(zval * return_value, zval * a, zval * b);
void tensor_divide(zval * return_value, zval * a, zval * b);
void tensor_add(zval * return_value, zval * a, zval * b);
void tensor_subtract(zval * return_value, zval * a, zval * b);
void tensor_pow(zval * return_value, zval * a, zval * b);
void tensor_mod(zval * return_value, zval * a, zval * b);

void tensor_multiply_scalar(zval * return_value, zval * a, zval * b);
void tensor_divide_scalar(zval * return_value, zval * a, zval * b);
void tensor_add_scalar(zval * return_value, zval * a, zval * b);
void tensor_subtract_scalar(zval * return_value, zval * a, zval * b);
void tensor_pow_scalar(zval * return_value, zval * a, zval * b);
void tensor_mod_scalar(zval * return_value, zval * a, zval * b);

void tensor_multiply_col(zval * return_value, zval * a, zval * b, zval * n);
void tensor_add_col(zval * return_value, zval * a, zval * b, zval * n);
void tensor_divide_col(zval * return_value, zval * a, zval * b, zval * n);
void tensor_divide_col_reverse(zval * return_value, zval * a, zval * b, zval * n);
void tensor_subtract_col(zval * return_value, zval * a, zval * b, zval * n);
void tensor_subtract_col_reverse(zval * return_value, zval * a, zval * b, zval * n);
void tensor_pow_col(zval * return_value, zval * a, zval * b, zval * n);
void tensor_pow_col_reverse(zval * return_value, zval * a, zval * b, zval * n);
void tensor_mod_col(zval * return_value, zval * a, zval * b, zval * n);
void tensor_mod_col_reverse(zval * return_value, zval * a, zval * b, zval * n);

void tensor_multiply_row(zval * return_value, zval * a, zval * b, zval * n);
void tensor_add_row(zval * return_value, zval * a, zval * b, zval * n);
void tensor_divide_row(zval * return_value, zval * a, zval * b, zval * n);
void tensor_subtract_row(zval * return_value, zval * a, zval * b, zval * n);
void tensor_pow_row(zval * return_value, zval * a, zval * b, zval * n);
void tensor_mod_row(zval * return_value, zval * a, zval * b, zval * n);

#endif
