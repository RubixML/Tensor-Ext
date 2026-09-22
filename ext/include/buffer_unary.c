#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <Zend/zend_exceptions.h>
#include <ext/standard/php_math.h>
#include <math.h>
#include <string.h>

/* Before kernel/buffer.h: the Buffer declarations are guarded by
 * ZEPHIR_BUFFER_ENABLED, which the compiler emits into php_<project>.h.
 * Without this the prototypes are invisible, zephir_buffer_doubles() is
 * assumed to return int, and the pointer arithmetic silently breaks. It
 * built anyway under `composer compile`, which pre-compiles headers, and
 * failed under the plain phpize/make that CI uses through PIE. */
#include "php_ext.h"
#include "kernel/buffer.h"
#include "kernel/operators.h"
#include "include/buffer_unary.h"

/**
 * One tight loop per operation.
 *
 * The switch is outside the loop on purpose: a switch per element would defeat
 * vectorisation and cost more than several of these operations do.
 */
#define TENSOR_UNARY_LOOP(EXPR)                     \
    do {                                            \
        zend_long i;                                \
        for (i = 0; i < n; ++i) {                   \
            const double x = va[i];                 \
            vc[i] = (EXPR);                         \
        }                                           \
    } while (0)

/**
 * Resolves the operand and allocates a result buffer of the same length.
 *
 * The kind is checked with zephir_buffer_kind() rather than by testing the
 * pointer, because zephir_buffer_doubles() also returns NULL for an empty
 * buffer and an empty vector is legal.
 */
static int tensor_buffer_open(zval * return_value, zval * a, double ** va, double ** vc, zend_long * n)
{
    if (zephir_buffer_kind(a) != ZEPHIR_BUFFER_DOUBLE) {
        zend_throw_error(NULL, "Expected a buffer of doubles.");

        return FAILURE;
    }

    *n = zephir_buffer_len(a);

    zephir_buffer_create(return_value, *n, ZEPHIR_BUFFER_DOUBLE);

    *va = zephir_buffer_doubles(a);
    *vc = zephir_buffer_doubles(return_value);

    return SUCCESS;
}

/**
 * PHP's sign convention, which is not copysign(): zero and NaN both fall to the
 * else branch and produce 0.0.
 */
static inline double tensor_sign(double x)
{
    if (x > 0.0) {
        return 1.0;
    }

    return x < 0.0 ? -1.0 : 0.0;
}

void tensor_buffer_unary(zval * return_value, zval * a, zval * op)
{
    double * va;
    double * vc;
    zend_long n;
    const char * name;

    if (Z_TYPE_P(op) != IS_STRING) {
        zend_throw_error(NULL, "The operation must be named by a string.");

        RETURN_NULL();
    }

    if (tensor_buffer_open(return_value, a, &va, &vc, &n) == FAILURE) {
        RETURN_NULL();
    }

    if (n == 0) {
        return;
    }

    name = Z_STRVAL_P(op);

    /* Mirrors PHP's own implementations, including their order of operations:
     * deg2rad is (x / 180) * M_PI and rad2deg is (x / M_PI) * 180, so that the
     * results are bit-identical to what array_map produced. */
    if (strcmp(name, "abs") == 0)          TENSOR_UNARY_LOOP(fabs(x));
    else if (strcmp(name, "sqrt") == 0)    TENSOR_UNARY_LOOP(sqrt(x));
    else if (strcmp(name, "exp") == 0)     TENSOR_UNARY_LOOP(exp(x));
    else if (strcmp(name, "expm1") == 0)   TENSOR_UNARY_LOOP(expm1(x));
    else if (strcmp(name, "log") == 0)     TENSOR_UNARY_LOOP(log(x));
    else if (strcmp(name, "log1p") == 0)   TENSOR_UNARY_LOOP(log1p(x));
    else if (strcmp(name, "sin") == 0)     TENSOR_UNARY_LOOP(sin(x));
    else if (strcmp(name, "asin") == 0)    TENSOR_UNARY_LOOP(asin(x));
    else if (strcmp(name, "cos") == 0)     TENSOR_UNARY_LOOP(cos(x));
    else if (strcmp(name, "acos") == 0)    TENSOR_UNARY_LOOP(acos(x));
    else if (strcmp(name, "tan") == 0)     TENSOR_UNARY_LOOP(tan(x));
    else if (strcmp(name, "atan") == 0)    TENSOR_UNARY_LOOP(atan(x));
    else if (strcmp(name, "rad2deg") == 0) TENSOR_UNARY_LOOP((x / M_PI) * 180.0);
    else if (strcmp(name, "deg2rad") == 0) TENSOR_UNARY_LOOP((x / 180.0) * M_PI);
    else if (strcmp(name, "floor") == 0)   TENSOR_UNARY_LOOP(floor(x));
    else if (strcmp(name, "ceil") == 0)    TENSOR_UNARY_LOOP(ceil(x));
    else if (strcmp(name, "round") == 0)   TENSOR_UNARY_LOOP(_php_math_round(x, 0, PHP_ROUND_HALF_UP));
    else if (strcmp(name, "negate") == 0)  TENSOR_UNARY_LOOP(-x);
    else if (strcmp(name, "square") == 0)  TENSOR_UNARY_LOOP(x * x);
    else if (strcmp(name, "sign") == 0)    TENSOR_UNARY_LOOP(tensor_sign(x));
    else if (strcmp(name, "reciprocal") == 0) TENSOR_UNARY_LOOP(1.0 / x);
    else {
        zval_ptr_dtor(return_value);
        zend_throw_error(NULL, "Unknown unary operation '%s'.", name);

        RETURN_NULL();
    }
}

void tensor_buffer_unary_arg(zval * return_value, zval * a, zval * op, zval * arg)
{
    double * va;
    double * vc;
    zend_long n;
    const char * name;
    double p;

    if (Z_TYPE_P(op) != IS_STRING) {
        zend_throw_error(NULL, "The operation must be named by a string.");

        RETURN_NULL();
    }

    if (tensor_buffer_open(return_value, a, &va, &vc, &n) == FAILURE) {
        RETURN_NULL();
    }

    name = Z_STRVAL_P(op);
    p = zephir_get_doubleval(arg);

    if (n == 0) {
        return;
    }

    if (strcmp(name, "clipLower") == 0) {
        /* NaN compares false against everything, so it passes through
         * unchanged, exactly as the Zephir loop left it. */
        TENSOR_UNARY_LOOP(x < p ? p : x);
    } else if (strcmp(name, "clipUpper") == 0) {
        TENSOR_UNARY_LOOP(x > p ? p : x);
    } else if (strcmp(name, "round") == 0) {
        TENSOR_UNARY_LOOP(_php_math_round(x, (int) p, PHP_ROUND_HALF_UP));
    } else if (strcmp(name, "log") == 0) {
        /* PHP special-cases these two bases rather than dividing logarithms,
         * and the results differ in the last ulp if we do not. */
        if (p == 10.0) {
            TENSOR_UNARY_LOOP(log10(x));
        } else if (p == 2.0) {
            TENSOR_UNARY_LOOP(log2(x));
        } else {
            const double denominator = log(p);

            TENSOR_UNARY_LOOP(log(x) / denominator);
        }
    } else {
        zval_ptr_dtor(return_value);
        zend_throw_error(NULL, "Unknown unary operation '%s'.", name);

        RETURN_NULL();
    }
}

void tensor_buffer_clip(zval * return_value, zval * a, zval * min, zval * max)
{
    double * va;
    double * vc;
    zend_long n;
    double lo, hi;

    if (tensor_buffer_open(return_value, a, &va, &vc, &n) == FAILURE) {
        RETURN_NULL();
    }

    if (n == 0) {
        return;
    }

    lo = zephir_get_doubleval(min);
    hi = zephir_get_doubleval(max);

    /* Ordered as the Zephir loop was -- upper bound first -- so that a NaN,
     * which fails both comparisons, still passes through untouched. */
    TENSOR_UNARY_LOOP(x > hi ? hi : (x < lo ? lo : x));
}

void tensor_buffer_reduce(zval * return_value, zval * a, zval * op)
{
    double * va;
    zend_long n, i;
    const char * name;
    double acc;

    if (Z_TYPE_P(op) != IS_STRING) {
        zend_throw_error(NULL, "The operation must be named by a string.");

        RETURN_NULL();
    }

    if (zephir_buffer_kind(a) != ZEPHIR_BUFFER_DOUBLE) {
        zend_throw_error(NULL, "Expected a buffer of doubles.");

        RETURN_NULL();
    }

    n = zephir_buffer_len(a);
    va = zephir_buffer_doubles(a);
    name = Z_STRVAL_P(op);

    if (strcmp(name, "sum") == 0) {
        /* Sequential, matching array_sum() addition for addition. */
        acc = 0.0;

        for (i = 0; i < n; ++i) {
            acc += va[i];
        }

        RETURN_DOUBLE(acc);
    }

    if (strcmp(name, "product") == 0) {
        /* array_product() of an empty array is the integer 1. */
        acc = 1.0;

        for (i = 0; i < n; ++i) {
            acc *= va[i];
        }

        RETURN_DOUBLE(acc);
    }

    if (strcmp(name, "min") == 0 || strcmp(name, "max") == 0) {
        const int wantMax = name[1] == 'a';

        if (n == 0) {
            /* PHP's min()/max() reject an empty argument, and so must this. */
            zend_throw_exception_ex(
                zend_ce_value_error, 0,
                "%s(): Argument #1 ($value) must contain at least one element", name
            );

            RETURN_NULL();
        }

        acc = va[0];

        for (i = 1; i < n; ++i) {
            if (wantMax ? va[i] > acc : va[i] < acc) {
                acc = va[i];
            }
        }

        RETURN_DOUBLE(acc);
    }

    zend_throw_error(NULL, "Unknown reduction '%s'.", name);

    RETURN_NULL();
}
