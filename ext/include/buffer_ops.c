#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <Zend/zend_exceptions.h>
#include <cblas.h>
#include <string.h>
#include <math.h>

/* Before kernel/buffer.h: the Buffer declarations are guarded by
 * ZEPHIR_BUFFER_ENABLED, which the compiler emits into php_<project>.h.
 * Without this the prototypes are invisible, zephir_buffer_doubles() is
 * assumed to return int, and the pointer arithmetic silently breaks. It
 * built anyway under `composer compile`, which pre-compiles headers, and
 * failed under the plain phpize/make that CI uses through PIE. */
#include "php_ext.h"
#include "kernel/buffer.h"
#include "kernel/operators.h"
#include "include/buffer_ops.h"

/**
 * Resolves two Buffer operands to raw double pointers of equal length.
 *
 * The kind is checked with zephir_buffer_kind() rather than by testing the
 * pointer for NULL: zephir_buffer_doubles() also returns NULL for an empty
 * buffer, so a NULL test rejects two legitimately empty operands as though they
 * were the wrong type. Callers must therefore guard on `n > 0` before
 * dereferencing, which they do.
 */
static int tensor_buffer_pair(zval * a, zval * b, double ** va, double ** vb, zend_long * n)
{
    if (zephir_buffer_kind(a) != ZEPHIR_BUFFER_DOUBLE
        || zephir_buffer_kind(b) != ZEPHIR_BUFFER_DOUBLE
        || zephir_buffer_len(a) != zephir_buffer_len(b)) {
        zend_throw_error(NULL, "Both operands must be double buffers of the same length.");

        return FAILURE;
    }

    *n = zephir_buffer_len(a);
    *va = zephir_buffer_doubles(a);
    *vb = zephir_buffer_doubles(b);

    return SUCCESS;
}

void tensor_buffer_from_array(zval * return_value, zval * a)
{
    if (Z_TYPE_P(a) != IS_ARRAY) {
        zend_throw_error(NULL, "Expected an array of numbers.");

        RETURN_NULL();
    }

    zephir_buffer_create_from_array(return_value, a, ZEPHIR_BUFFER_DOUBLE);
}

void tensor_buffer_add(zval * return_value, zval * a, zval * b)
{
    double * va;
    double * vb;
    double * vc;
    zend_long n;

    if (tensor_buffer_pair(a, b, &va, &vb, &n) == FAILURE) {
        RETURN_NULL();
    }

    zephir_buffer_create(return_value, n, ZEPHIR_BUFFER_DOUBLE);

    if (n > 0) {
        vc = zephir_buffer_doubles(return_value);

        memcpy(vc, va, (size_t) n * sizeof(double));

        cblas_daxpy((int) n, 1.0, vb, 1, vc, 1);
    }
}

void tensor_buffer_multiply(zval * return_value, zval * a, zval * b)
{
    double * va;
    double * vb;
    double * vc;
    zend_long n, i;

    if (tensor_buffer_pair(a, b, &va, &vb, &n) == FAILURE) {
        RETURN_NULL();
    }

    zephir_buffer_create(return_value, n, ZEPHIR_BUFFER_DOUBLE);

    if (n > 0) {
        vc = zephir_buffer_doubles(return_value);

        /* BLAS level 1 has no elementwise product; -O3 vectorises this. */
        for (i = 0; i < n; ++i) {
            vc[i] = va[i] * vb[i];
        }
    }
}

void tensor_buffer_dot(zval * return_value, zval * a, zval * b)
{
    double * va;
    double * vb;
    zend_long n;

    if (tensor_buffer_pair(a, b, &va, &vb, &n) == FAILURE) {
        RETURN_NULL();
    }

    RETVAL_DOUBLE(n > 0 ? cblas_ddot((int) n, va, 1, vb, 1) : 0.0);
}

void tensor_buffer_fill(zval * return_value, zval * value, zval * n)
{
    double * vc;
    zend_long len = zephir_get_intval(n);
    zend_long i;
    double v = zephir_get_doubleval(value);

    zephir_buffer_create(return_value, len, ZEPHIR_BUFFER_DOUBLE);

    if (len <= 0) {
        return;
    }

    vc = zephir_buffer_doubles(return_value);

    for (i = 0; i < len; ++i) {
        vc[i] = v;
    }
}

void tensor_buffer_linspace(zval * return_value, zval * min, zval * max, zval * n)
{
    double * vc;
    zend_long len = zephir_get_intval(n);
    zend_long k = len - 1;
    zend_long i;
    double lo = zephir_get_doubleval(min);
    double hi = zephir_get_doubleval(max);
    double interval;

    zephir_buffer_create(return_value, len, ZEPHIR_BUFFER_DOUBLE);

    if (len <= 0) {
        return;
    }

    vc = zephir_buffer_doubles(return_value);
    interval = fabs(hi - lo) / (double) k;

    /* Accumulated, not computed as lo + i * interval. The Zephir loop this
     * replaces did `end(a) + interval` each step, and the rounding of a running
     * sum differs from that of a multiplication. */
    vc[0] = lo;

    for (i = 1; i < k; ++i) {
        vc[i] = vc[i - 1] + interval;
    }

    if (k > 0) {
        vc[k] = hi;
    }
}
