#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <Zend/zend_exceptions.h>
#include <cblas.h>
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
#include "include/buffer_binary.h"

#define TENSOR_BINARY_LOOP(EXPR)                    \
    do {                                            \
        zend_long i;                                \
        for (i = 0; i < n; ++i) {                   \
            const double x = va[i];                 \
            const double y = vb[i];                 \
            vc[i] = (EXPR);                         \
        }                                           \
    } while (0)

#define TENSOR_SCALAR_LOOP(EXPR)                    \
    do {                                            \
        zend_long i;                                \
        for (i = 0; i < n; ++i) {                   \
            const double x = va[i];                 \
            vc[i] = (EXPR);                         \
        }                                           \
    } while (0)

/**
 * PHP's modulo is an integer operation: both operands are converted to integers
 * before the remainder is taken, and a zero divisor raises rather than producing
 * NaN.
 *
 * zend_dval_to_lval() rather than a C cast, because casting an out-of-range
 * double to an integer is undefined behaviour in C while PHP's conversion is
 * defined.
 *
 * One documented difference from the array path: that path went through
 * mod_function(), which emits an "implicit conversion from float ... loses
 * precision" deprecation per element. This does not. On a million-element
 * vector that notice fired a million times, so its absence is not mourned, but
 * it is a behaviour change and is recorded as one.
 */
static inline int tensor_mod_guard(double y)
{
    if (zend_dval_to_lval(y) == 0) {
        zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");

        return FAILURE;
    }

    return SUCCESS;
}

static int tensor_buffer_open_pair(
    zval * return_value,
    zval * a,
    zval * b,
    double ** va,
    double ** vb,
    double ** vc,
    zend_long * n
) {
    if (zephir_buffer_kind(a) != ZEPHIR_BUFFER_DOUBLE
        || zephir_buffer_kind(b) != ZEPHIR_BUFFER_DOUBLE
        || zephir_buffer_len(a) != zephir_buffer_len(b)) {
        zend_throw_error(NULL, "Both operands must be double buffers of the same length.");

        return FAILURE;
    }

    *n = zephir_buffer_len(a);

    zephir_buffer_create(return_value, *n, ZEPHIR_BUFFER_DOUBLE);

    *va = zephir_buffer_doubles(a);
    *vb = zephir_buffer_doubles(b);
    *vc = zephir_buffer_doubles(return_value);

    return SUCCESS;
}

static int tensor_buffer_open_one(zval * return_value, zval * a, double ** va, double ** vc, zend_long * n)
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

void tensor_buffer_binary(zval * return_value, zval * a, zval * b, zval * op)
{
    double * va;
    double * vb;
    double * vc;
    zend_long n, i;
    const char * name;

    if (Z_TYPE_P(op) != IS_STRING) {
        zend_throw_error(NULL, "The operation must be named by a string.");

        RETURN_NULL();
    }

    if (tensor_buffer_open_pair(return_value, a, b, &va, &vb, &vc, &n) == FAILURE) {
        RETURN_NULL();
    }

    if (n == 0) {
        return;
    }

    name = Z_STRVAL_P(op);

    if (strcmp(name, "add") == 0) {
        /* daxpy computes y := alpha*x + y, so the left operand is copied in
         * first and BLAS accumulates the right one onto it. */
        memcpy(vc, va, (size_t) n * sizeof(double));
        cblas_daxpy((int) n, 1.0, vb, 1, vc, 1);
    } else if (strcmp(name, "subtract") == 0) {
        memcpy(vc, va, (size_t) n * sizeof(double));
        cblas_daxpy((int) n, -1.0, vb, 1, vc, 1);
    } else if (strcmp(name, "multiply") == 0) {
        /* BLAS level 1 has no element-wise product; -O3 vectorises this. */
        TENSOR_BINARY_LOOP(x * y);
    } else if (strcmp(name, "divide") == 0) {
        TENSOR_BINARY_LOOP(x / y);
    } else if (strcmp(name, "pow") == 0) {
        TENSOR_BINARY_LOOP(pow(x, y));
    } else if (strcmp(name, "mod") == 0) {
        for (i = 0; i < n; ++i) {
            if (tensor_mod_guard(vb[i]) == FAILURE) {
                zval_ptr_dtor(return_value);

                RETURN_NULL();
            }

            vc[i] = (double) (zend_dval_to_lval(va[i]) % zend_dval_to_lval(vb[i]));
        }
    } else if (strcmp(name, "equal") == 0) {
        TENSOR_BINARY_LOOP(x == y ? 1.0 : 0.0);
    } else if (strcmp(name, "notEqual") == 0) {
        TENSOR_BINARY_LOOP(x != y ? 1.0 : 0.0);
    } else if (strcmp(name, "greater") == 0) {
        TENSOR_BINARY_LOOP(x > y ? 1.0 : 0.0);
    } else if (strcmp(name, "greaterEqual") == 0) {
        TENSOR_BINARY_LOOP(x >= y ? 1.0 : 0.0);
    } else if (strcmp(name, "less") == 0) {
        TENSOR_BINARY_LOOP(x < y ? 1.0 : 0.0);
    } else if (strcmp(name, "lessEqual") == 0) {
        TENSOR_BINARY_LOOP(x <= y ? 1.0 : 0.0);
    } else {
        zval_ptr_dtor(return_value);
        zend_throw_error(NULL, "Unknown binary operation '%s'.", name);

        RETURN_NULL();
    }
}

void tensor_buffer_binary_scalar(zval * return_value, zval * a, zval * b, zval * op)
{
    double * va;
    double * vc;
    zend_long n, i;
    const char * name;
    double y;

    if (Z_TYPE_P(op) != IS_STRING) {
        zend_throw_error(NULL, "The operation must be named by a string.");

        RETURN_NULL();
    }

    if (tensor_buffer_open_one(return_value, a, &va, &vc, &n) == FAILURE) {
        RETURN_NULL();
    }

    name = Z_STRVAL_P(op);
    y = zephir_get_doubleval(b);

    if (n == 0) {
        return;
    }

    if (strcmp(name, "add") == 0) {
        TENSOR_SCALAR_LOOP(x + y);
    } else if (strcmp(name, "subtract") == 0) {
        TENSOR_SCALAR_LOOP(x - y);
    } else if (strcmp(name, "multiply") == 0) {
        memcpy(vc, va, (size_t) n * sizeof(double));
        cblas_dscal((int) n, y, vc, 1);
    } else if (strcmp(name, "divide") == 0) {
        TENSOR_SCALAR_LOOP(x / y);
    } else if (strcmp(name, "pow") == 0) {
        TENSOR_SCALAR_LOOP(pow(x, y));
    } else if (strcmp(name, "mod") == 0) {
        if (tensor_mod_guard(y) == FAILURE) {
            zval_ptr_dtor(return_value);

            RETURN_NULL();
        }

        for (i = 0; i < n; ++i) {
            vc[i] = (double) (zend_dval_to_lval(va[i]) % zend_dval_to_lval(y));
        }
    } else if (strcmp(name, "equal") == 0) {
        TENSOR_SCALAR_LOOP(x == y ? 1.0 : 0.0);
    } else if (strcmp(name, "notEqual") == 0) {
        TENSOR_SCALAR_LOOP(x != y ? 1.0 : 0.0);
    } else if (strcmp(name, "greater") == 0) {
        TENSOR_SCALAR_LOOP(x > y ? 1.0 : 0.0);
    } else if (strcmp(name, "greaterEqual") == 0) {
        TENSOR_SCALAR_LOOP(x >= y ? 1.0 : 0.0);
    } else if (strcmp(name, "less") == 0) {
        TENSOR_SCALAR_LOOP(x < y ? 1.0 : 0.0);
    } else if (strcmp(name, "lessEqual") == 0) {
        TENSOR_SCALAR_LOOP(x <= y ? 1.0 : 0.0);
    } else {
        zval_ptr_dtor(return_value);
        zend_throw_error(NULL, "Unknown binary operation '%s'.", name);

        RETURN_NULL();
    }
}

void tensor_buffer_broadcast_rows(zval * return_value, zval * a, zval * b, zval * op)
{
    double * va;
    zend_long m, i;
    uint32_t j, n;
    const char * name;
    zend_array * rows;
    zval * row;
    zval result, rowResult;

    if (Z_TYPE_P(op) != IS_STRING || Z_TYPE_P(b) != IS_ARRAY) {
        zend_throw_error(NULL, "Expected a buffer, an array of rows and an operation name.");

        RETURN_NULL();
    }

    if (zephir_buffer_kind(a) != ZEPHIR_BUFFER_DOUBLE) {
        zend_throw_error(NULL, "Expected a buffer of doubles.");

        RETURN_NULL();
    }

    rows = Z_ARR_P(b);
    m = zephir_buffer_len(a);
    va = zephir_buffer_doubles(a);
    name = Z_STRVAL_P(op);

    if ((zend_long) zend_array_count(rows) != m) {
        zend_throw_error(NULL, "The buffer must have one element per row.");

        RETURN_NULL();
    }

    array_init_size(&result, (uint32_t) m);

    for (i = 0; i < m; ++i) {
        const double x = va[i];

        row = zend_hash_index_find(rows, (zend_ulong) i);

        if (row == NULL || Z_TYPE_P(row) != IS_ARRAY) {
            zval_ptr_dtor(&result);
            zend_throw_error(NULL, "Row %ld is not an array.", (long) i);

            RETURN_NULL();
        }

        n = zend_array_count(Z_ARR_P(row));

        array_init_size(&rowResult, n);

        for (j = 0; j < n; ++j) {
            const double y = zephir_get_doubleval(zend_hash_index_find(Z_ARR_P(row), j));
            double z;

            if (strcmp(name, "multiply") == 0)          z = x * y;
            else if (strcmp(name, "divide") == 0)       z = x / y;
            else if (strcmp(name, "add") == 0)          z = x + y;
            else if (strcmp(name, "subtract") == 0)     z = x - y;
            else if (strcmp(name, "pow") == 0)          z = pow(x, y);
            else if (strcmp(name, "equal") == 0)        z = x == y ? 1.0 : 0.0;
            else if (strcmp(name, "notEqual") == 0)     z = x != y ? 1.0 : 0.0;
            else if (strcmp(name, "greater") == 0)      z = x > y ? 1.0 : 0.0;
            else if (strcmp(name, "greaterEqual") == 0) z = x >= y ? 1.0 : 0.0;
            else if (strcmp(name, "less") == 0)         z = x < y ? 1.0 : 0.0;
            else if (strcmp(name, "lessEqual") == 0)    z = x <= y ? 1.0 : 0.0;
            else if (strcmp(name, "mod") == 0) {
                if (tensor_mod_guard(y) == FAILURE) {
                    /* Unwinding here is the whole point: the interpreted loop
                     * left its memory frame inconsistent and aborted. */
                    zval_ptr_dtor(&rowResult);
                    zval_ptr_dtor(&result);

                    RETURN_NULL();
                }

                z = (double) (zend_dval_to_lval(x) % zend_dval_to_lval(y));
            } else {
                zval_ptr_dtor(&rowResult);
                zval_ptr_dtor(&result);
                zend_throw_error(NULL, "Unknown broadcast operation '%s'.", name);

                RETURN_NULL();
            }

            add_next_index_double(&rowResult, z);
        }

        add_next_index_zval(&result, &rowResult);
    }

    RETVAL_ARR(Z_ARR(result));
}
