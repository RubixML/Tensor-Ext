#ifndef TENSOR_ELEMENTWISE_H
#define TENSOR_ELEMENTWISE_H

#include <Zend/zend.h>

void tensor_abs(zval * return_value, zval * a);
void tensor_sqrt(zval * return_value, zval * a);
void tensor_exp(zval * return_value, zval * a);
void tensor_expm1(zval * return_value, zval * a);
void tensor_log(zval * return_value, zval * a);
void tensor_log_base(zval * return_value, zval * a, zval * b);
void tensor_log1p(zval * return_value, zval * a);
void tensor_sin(zval * return_value, zval * a);
void tensor_asin(zval * return_value, zval * a);
void tensor_cos(zval * return_value, zval * a);
void tensor_acos(zval * return_value, zval * a);
void tensor_tan(zval * return_value, zval * a);
void tensor_atan(zval * return_value, zval * a);
void tensor_rad2deg(zval * return_value, zval * a);
void tensor_deg2rad(zval * return_value, zval * a);
void tensor_floor(zval * return_value, zval * a);
void tensor_ceil(zval * return_value, zval * a);
void tensor_round(zval * return_value, zval * a, zval * precision);
void tensor_negate(zval * return_value, zval * a);
void tensor_sign(zval * return_value, zval * a);
void tensor_clip(zval * return_value, zval * a, zval * min, zval * max);
void tensor_clip_lower(zval * return_value, zval * a, zval * min);
void tensor_clip_upper(zval * return_value, zval * a, zval * max);

#endif