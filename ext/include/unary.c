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
#include "include/dispatch.h"

/* Values wrapped up by the kernel Buffer used in the unary operations below.
 * Returns a reference to a newly created `Tensor\TensorBuffer` holding the
 * mapped doubles.  Each operation expands into its own dedicated loop so the
 * optimizer can vectorize the elementwise mapping instead of being blocked by
 * an indirect call.
 *
 * The mapping operations are dispatched between the two ISAs on the same terms
 * as the arithmetic kernels in include/arithmetic.c: one body compiled twice,
 * once at the extension's baseline ISA and once under TENSOR_TARGET_AVX so the
 * loop widens from two doubles per vector to four, with a route pointer
 * deciding which one runs.  The body is a macro rather than a shared helper so
 * that the baseline and the AVX copy are the same text -- there is no second
 * definition that could drift.
 *
 * Both buffers are marked restrict: the input is a fixed-size Buffer that is
 * never reallocated and the output was allocated microseconds ago, so they
 * cannot overlap, and telling the compiler so spares it a runtime alias check
 * on every call.
 */

#define TENSOR_UNARY_BODY(expr)                                                      \
	zend_long i;                                                                 \
	zend_long n = 0;                                                             \
	int ok = 0;                                                                  \
                                                                                     \
	double * restrict va = tensor_tensorbuffer_doubles(a, &n, &ok);              \
                                                                                     \
	if (UNEXPECTED(!ok)) {                                                       \
		return;                                                              \
	}                                                                            \
                                                                                     \
	zval b;                                                                      \
                                                                                     \
	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, n, &b) == FAILURE)) {\
		return;                                                              \
	}                                                                            \
                                                                                     \
	double * restrict vb = zephir_buffer_doubles(&b);                            \
                                                                                     \
	for (i = 0; i < n; ++i) {                                                    \
		vb[i] = expr;                                                        \
	}                                                                            \
                                                                                     \
	zval_ptr_dtor(&b);

#define TENSOR_UNARY_DISPATCH(name, expr)                                       \
	static void tensor_##name##_baseline(zval * return_value, zval * a)     \
	{                                                                       \
		TENSOR_UNARY_BODY(expr)                                         \
	}                                                                       \
                                                                                \
	TENSOR_TARGET_AVX                                                       \
	static void tensor_##name##_avx(zval * return_value, zval * a)          \
	{                                                                       \
		TENSOR_UNARY_BODY(expr)                                         \
	}                                                                       \
                                                                                \
	static tensor_unary_fn tensor_##name##_route = tensor_##name##_baseline;\
                                                                                \
	void tensor_##name(zval * return_value, zval * a)                       \
	{                                                                       \
		tensor_##name##_route(return_value, a);                         \
	}

TENSOR_UNARY_DISPATCH(abs, fabs(va[i]))
TENSOR_UNARY_DISPATCH(sqrt, sqrt(va[i]))
TENSOR_UNARY_DISPATCH(negate, -va[i])
TENSOR_UNARY_DISPATCH(sign, va[i] > 0.0 ? 1.0 : (va[i] < 0.0 ? -1.0 : 0.0))
TENSOR_UNARY_DISPATCH(rad2deg, (va[i] / M_PI) * 180.0)
TENSOR_UNARY_DISPATCH(deg2rad, (va[i] / 180.0) * M_PI)

#undef TENSOR_UNARY_DISPATCH
#undef TENSOR_UNARY_BODY

/* floor and ceil, plus every operation whose cost sits inside a libm call, keep
 * the single body the whole file used before there was any dispatching.  The
 * reasons are spelled out in include/dispatch.h: widening the register does not
 * help a scalar library call, and for these two the compiler actually gives up
 * on vectorizing once AVX is in scope, so dispatching them would be slower. */

#define TENSOR_UNARY(name, expr)                                                             \
	void tensor_##name(zval * return_value, zval * a)                                    \
	{                                                                                    \
		zend_long n = 0;                                                             \
		int ok = 0;                                                                  \
                                                                                             \
		double * va = tensor_tensorbuffer_doubles(a, &n, &ok);                       \
                                                                                             \
		if (UNEXPECTED(!ok)) {                                                       \
			return;                                                              \
		}                                                                            \
                                                                                             \
		zval b;                                                                      \
                                                                                             \
		if (UNEXPECTED(tensor_tensorbuffer_create(return_value, n, &b) == FAILURE)) {\
			return;                                                              \
		}                                                                            \
                                                                                             \
		double * vb = zephir_buffer_doubles(&b);                                     \
                                                                                             \
		zend_long i;                                                                 \
                                                                                             \
		for (i = 0; i < n; ++i) {                                                    \
			vb[i] = expr;                                                        \
		}                                                                            \
                                                                                             \
		zval_ptr_dtor(&b);                                                           \
	}

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
TENSOR_UNARY(floor, floor(va[i]))
TENSOR_UNARY(ceil, ceil(va[i]))

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

	double log_base = log(zephir_get_doubleval(b));

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, n, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < n; ++i) {
		vc[i] = log(va[i]) / log_base;
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

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, n, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < n; ++i) {
		vc[i] = tensor_math_round(va[i], places);
	}

	zval_ptr_dtor(&c);
}

/* Clamping. The three kernels take a different number of bounds, so each gets
 * its own route signature and its own body, in the same spirit as the separate
 * column and row macros in include/arithmetic.c. The bounds are loop invariant
 * and get broadcast into a register, leaving a per-lane compare and select. */

/* Note on the parameter names below: the body macros are parameterised on
 * TENSOR_CLIP_EXPR rather than on something like `hi`, because a macro argument
 * is substituted for every occurrence of the parameter in the invoking macro's
 * body -- naming it `hi` would rewrite the `const double hi` that reads the
 * bound out of the zval. */

#define TENSOR_CLIP_BODY(TENSOR_CLIP_EXPR)                                           \
	zend_long i;                                                                 \
	zend_long n = 0;                                                             \
	int ok = 0;                                                                  \
                                                                                     \
	double * restrict va = tensor_tensorbuffer_doubles(a, &n, &ok);              \
                                                                                     \
	if (UNEXPECTED(!ok)) {                                                       \
		return;                                                              \
	}                                                                            \
                                                                                     \
	zval c;                                                                      \
                                                                                     \
	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, n, &c) == FAILURE)) {\
		return;                                                              \
	}                                                                            \
                                                                                     \
	double * restrict vc = zephir_buffer_doubles(&c);                            \
                                                                                     \
	for (i = 0; i < n; ++i) {                                                    \
		vc[i] = TENSOR_CLIP_EXPR;                                            \
	}                                                                            \
                                                                                     \
	zval_ptr_dtor(&c);

#define TENSOR_CLIP_DISPATCH(name, TENSOR_CLIP_EXPR)                                 \
	static void tensor_##name##_baseline(                                        \
		zval * return_value, zval * a, zval * lo_zval, zval * hi_zval)       \
	{                                                                            \
		const double lo = zephir_get_doubleval(lo_zval);                     \
		const double hi = zephir_get_doubleval(hi_zval);                     \
		TENSOR_CLIP_BODY(TENSOR_CLIP_EXPR)                                   \
	}                                                                            \
                                                                                     \
	TENSOR_TARGET_AVX                                                            \
	static void tensor_##name##_avx(                                             \
		zval * return_value, zval * a, zval * lo_zval, zval * hi_zval)       \
	{                                                                            \
		const double lo = zephir_get_doubleval(lo_zval);                     \
		const double hi = zephir_get_doubleval(hi_zval);                     \
		TENSOR_CLIP_BODY(TENSOR_CLIP_EXPR)                                   \
	}                                                                            \
                                                                                     \
	static tensor_unary_clip_fn tensor_##name##_route = tensor_##name##_baseline;\
                                                                                     \
	void tensor_##name(                                                          \
		zval * return_value, zval * a, zval * lo_zval, zval * hi_zval)       \
	{                                                                            \
		tensor_##name##_route(return_value, a, lo_zval, hi_zval);            \
	}

TENSOR_CLIP_DISPATCH(clip, va[i] > hi ? hi : (va[i] < lo ? lo : va[i]))

#undef TENSOR_CLIP_DISPATCH

#define TENSOR_CLIP_BOUND_DISPATCH(name, TENSOR_CLIP_EXPR)                            \
	static void tensor_##name##_baseline(zval * return_value, zval * a, zval * b) \
	{                                                                             \
		const double bound = zephir_get_doubleval(b);                         \
		TENSOR_CLIP_BODY(TENSOR_CLIP_EXPR)                                    \
	}                                                                             \
                                                                                      \
	TENSOR_TARGET_AVX                                                             \
	static void tensor_##name##_avx(zval * return_value, zval * a, zval * b)      \
	{                                                                             \
		const double bound = zephir_get_doubleval(b);                         \
		TENSOR_CLIP_BODY(TENSOR_CLIP_EXPR)                                    \
	}                                                                             \
                                                                                      \
	static tensor_unary_bound_fn tensor_##name##_route = tensor_##name##_baseline;\
                                                                                      \
	void tensor_##name(zval * return_value, zval * a, zval * b)                   \
	{                                                                             \
		tensor_##name##_route(return_value, a, b);                            \
	}

TENSOR_CLIP_BOUND_DISPATCH(clip_lower, va[i] < bound ? bound : va[i])
TENSOR_CLIP_BOUND_DISPATCH(clip_upper, va[i] > bound ? bound : va[i])

#undef TENSOR_CLIP_BOUND_DISPATCH
#undef TENSOR_CLIP_BODY

/**
 * Point every dispatched kernel in this file at its AVX variant.
 *
 * Called once from tensor_cpu_init() in include/cpu.c, which gates it on the
 * CPU actually supporting AVX. The routes are all already pointing at the
 * baseline variants before this runs, so the effect is strictly an upgrade.
 *
 * floor and ceil are absent on purpose: see include/dispatch.h for why letting
 * the compiler see AVX makes them slower rather than faster.
 */
void tensor_unary_dispatch_avx_init(void)
{
	tensor_abs_route = tensor_abs_avx;
	tensor_sqrt_route = tensor_sqrt_avx;
	tensor_negate_route = tensor_negate_avx;
	tensor_sign_route = tensor_sign_avx;
	tensor_rad2deg_route = tensor_rad2deg_avx;
	tensor_deg2rad_route = tensor_deg2rad_avx;

	tensor_clip_route = tensor_clip_avx;
	tensor_clip_lower_route = tensor_clip_lower_avx;
	tensor_clip_upper_route = tensor_clip_upper_avx;
}