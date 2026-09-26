#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <ext/spl/spl_exceptions.h>
#include "kernel/operators.h"
#include "php_ext.h"
#include "kernel/buffer.h"
#include "include/buffer.h"

/* Values wrapped up by the kernel Buffer used in the unary operations below.
 * Returns a reference to a newly created `Tensor\TensorBuffer` holding the
 * mapped doubles.  Each operation expands into its own dedicated loop so the
 * optimizer can vectorize the elementwise mapping instead of being blocked by
 * an indirect call. */

#define TENSOR_UNARY(name, expr)                                                 \
void tensor_##name(zval * return_value, zval * a)                               \
{                                                                                \
	zend_long n = 0;                                                             \
	int ok = 0;                                                                  \
	                                                                             \
	double * va = tensor_tensorbuffer_doubles(a, &n, &ok);                       \
	                                                                             \
	if (UNEXPECTED(!ok)) {                                                       \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	zval b;                                                                      \
	                                                                             \
	if (UNEXPECTED(tensor_tensorbuffer_create_uninit(return_value, n, &b) == FAILURE)) { \
		return;                                                                  \
	}                                                                            \
	                                                                             \
	double * vb = zephir_buffer_doubles(&b);                                     \
	                                                                             \
	zend_long i;                                                                 \
	                                                                             \
	for (i = 0; i < n; ++i) {                                                    \
		vb[i] = expr;                                                            \
	}                                                                            \
	                                                                             \
	zval_ptr_dtor(&b);                                                           \
}

TENSOR_UNARY(abs, fabs(va[i]))
TENSOR_UNARY(sqrt, sqrt(va[i]))
TENSOR_UNARY(exp, exp(va[i]))
TENSOR_UNARY(expm1, expm1(va[i]))
TENSOR_UNARY(log, log(va[i]))
TENSOR_UNARY(log1p, log1p(va[i]))
TENSOR_UNARY(sin, sin(va[i]))
TENSOR_UNARY(asin, asin(va[i]))
TENSOR_UNARY(cos, cos(va[i]))
TENSOR_UNARY(acos, acos(va[i]))
TENSOR_UNARY(tan, tan(va[i]))
TENSOR_UNARY(atan, atan(va[i]))
TENSOR_UNARY(rad2deg, (va[i] / M_PI) * 180.0)
TENSOR_UNARY(deg2rad, (va[i] / 180.0) * M_PI)
TENSOR_UNARY(floor, floor(va[i]))
TENSOR_UNARY(ceil, ceil(va[i]))
TENSOR_UNARY(negate, -va[i])
TENSOR_UNARY(sign, va[i] > 0.0 ? 1.0 : (va[i] < 0.0 ? -1.0 : 0.0))

#undef TENSOR_UNARY

void tensor_log_base(zval * return_value, zval * a, zval * b)
{
	zend_long i;
	zend_long n = 0;
	int ok = 0;

	double * va = tensor_tensorbuffer_doubles(a, &n, &ok);

	if (UNEXPECTED(!ok)) {
		return;
	}

	double base = zephir_get_doubleval(b);

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create_uninit(return_value, n, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < n; ++i) {
		vc[i] = log(va[i]) / log(base);
	}

	zval_ptr_dtor(&c);
}

/* The following mirror the rounding implementation in ext/standard/math.c so
 * that results match PHP's round() exactly. */
static int tensor_intlog10abs(double value)
{
	value = fabs(value);

	if (value < 1e-8 || value > 1e22) {
		return (int) floor(log10(value));
	} else {
		int result = 15;
		static const double values[] = {
				1e-8, 1e-7, 1e-6, 1e-5, 1e-4, 1e-3, 1e-2, 1e-1, 1e0, 1e1, 1e2,
				1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10, 1e11, 1e12, 1e13,
				1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22};

		if (value < values[result]) {
			result -= 8;
		} else {
			result += 8;
		}

		if (value < values[result]) {
			result -= 4;
		} else {
			result += 4;
		}

		if (value < values[result]) {
			result -= 2;
		} else {
			result += 2;
		}

		if (value < values[result]) {
			result -= 1;
		} else {
			result += 1;
		}

		if (value < values[result]) {
			result -= 1;
		}

		result -= 8;

		return result;
	}
}

static double tensor_intpow10(int power)
{
	if (power < 0 || power > 22) {
		return pow(10.0, (double) power);
	}

	static const double powers[] = {
			1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10, 1e11,
			1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22};

	return powers[power];
}

static double tensor_round_half_up(double value)
{
	return value >= 0.0 ? floor(value + 0.5) : ceil(value - 0.5);
}

static double tensor_math_round(double value, int places)
{
	double f1, f2;
	double tmp_value;
	int precision_places;

	if (!isfinite(value) || value == 0.0) {
		return value;
	}

	places = places < INT_MIN + 1 ? INT_MIN + 1 : places;

	precision_places = 14 - tensor_intlog10abs(value);

	f1 = tensor_intpow10(abs(places));

	if (precision_places > places && precision_places - 15 < places) {
		int64_t use_precision = precision_places < INT_MIN + 1 ? INT_MIN + 1 : precision_places;

		f2 = tensor_intpow10(abs((int) use_precision));

		if (use_precision >= 0) {
			tmp_value = value * f2;
		} else {
			tmp_value = value / f2;
		}

		tmp_value = tensor_round_half_up(tmp_value);

		use_precision = places - precision_places;

		if (use_precision < INT_MIN + 1) {
			use_precision = INT_MIN + 1;
		}

		f2 = tensor_intpow10(abs((int) use_precision));

		tmp_value = tmp_value / f2;
	} else {
		if (places >= 0) {
			tmp_value = value * f1;
		} else {
			tmp_value = value / f1;
		}

		if (fabs(tmp_value) >= 1e15) {
			return value;
		}
	}

	tmp_value = tensor_round_half_up(tmp_value);

	if (abs(places) < 23) {
		if (places > 0) {
			tmp_value = tmp_value / f1;
		} else {
			tmp_value = tmp_value * f1;
		}
	} else {
		char buf[40];

		snprintf(buf, 39, "%15fe%d", tmp_value, -places);

		buf[39] = '\0';

		double parsed = zend_strtod(buf, NULL);

		if (!isfinite(parsed) || isnan(parsed)) {
			parsed = value;
		}

		tmp_value = parsed;
	}

	return tmp_value;
}

void tensor_round(zval * return_value, zval * a, zval * precision)
{
	zend_long i;
	zend_long n = 0;
	int ok = 0;

	double * va = tensor_tensorbuffer_doubles(a, &n, &ok);

	if (UNEXPECTED(!ok)) {
		return;
	}

	zend_long p = zephir_get_intval(precision);

	int places = p > INT_MAX ? INT_MAX : (int) p;

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create_uninit(return_value, n, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < n; ++i) {
		vc[i] = tensor_math_round(va[i], places);
	}

	zval_ptr_dtor(&c);
}

void tensor_clip(zval * return_value, zval * a, zval * min, zval * max)
{
	zend_long i;
	zend_long n = 0;
	int ok = 0;

	double * va = tensor_tensorbuffer_doubles(a, &n, &ok);

	if (UNEXPECTED(!ok)) {
		return;
	}

	double lo = zephir_get_doubleval(min);
	double hi = zephir_get_doubleval(max);

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create_uninit(return_value, n, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < n; ++i) {
		vc[i] = va[i] > hi ? hi : (va[i] < lo ? lo : va[i]);
	}

	zval_ptr_dtor(&c);
}

void tensor_clip_lower(zval * return_value, zval * a, zval * min)
{
	zend_long i;
	zend_long n = 0;
	int ok = 0;

	double * va = tensor_tensorbuffer_doubles(a, &n, &ok);

	if (UNEXPECTED(!ok)) {
		return;
	}

	double lo = zephir_get_doubleval(min);

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create_uninit(return_value, n, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < n; ++i) {
		vc[i] = va[i] < lo ? lo : va[i];
	}

	zval_ptr_dtor(&c);
}

void tensor_clip_upper(zval * return_value, zval * a, zval * max)
{
	zend_long i;
	zend_long n = 0;
	int ok = 0;

	double * va = tensor_tensorbuffer_doubles(a, &n, &ok);

	if (UNEXPECTED(!ok)) {
		return;
	}

	double hi = zephir_get_doubleval(max);

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create_uninit(return_value, n, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < n; ++i) {
		vc[i] = va[i] > hi ? hi : va[i];
	}

	zval_ptr_dtor(&c);
}