#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <Zend/zend_exceptions.h>
#include <cblas.h>
#include <lapacke.h>
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
#include "include/buffer_linalg.h"

void tensor_buffer_matmul(zval * return_value, zval * a, zval * b, zval * k, zval * n)
{
    double * va;
    double * vb;
    double * vc;
    zend_long inner = zephir_get_intval(k);
    zend_long cols = zephir_get_intval(n);
    zend_long rows;

    if (zephir_buffer_kind(a) != ZEPHIR_BUFFER_DOUBLE || zephir_buffer_kind(b) != ZEPHIR_BUFFER_DOUBLE) {
        zend_throw_error(NULL, "Expected buffers of doubles.");

        RETURN_NULL();
    }

    rows = inner > 0 ? zephir_buffer_len(a) / inner : 0;

    if (cols <= 0 || zephir_buffer_len(b) != inner * cols) {
        zend_throw_error(NULL, "The operands do not conform.");

        RETURN_NULL();
    }

    zephir_buffer_create(return_value, rows * cols, ZEPHIR_BUFFER_DOUBLE);

    if (rows * cols == 0) {
        return;
    }

    va = zephir_buffer_doubles(a);
    vb = zephir_buffer_doubles(b);
    vc = zephir_buffer_doubles(return_value);

    /* No packing either side: both operands are already the contiguous,
     * row-major double* that dgemm wants. */
    cblas_dgemm(
        CblasRowMajor, CblasNoTrans, CblasNoTrans,
        (int) rows, (int) cols, (int) inner,
        1.0, va, (int) inner, vb, (int) cols,
        0.0, vc, (int) cols
    );
}

void tensor_buffer_inverse(zval * return_value, zval * a, zval * n)
{
    double * vc;
    zend_long order = zephir_get_intval(n);
    lapack_int * pivots;
    lapack_int status;

    if (zephir_buffer_kind(a) != ZEPHIR_BUFFER_DOUBLE) {
        zend_throw_error(NULL, "Expected a buffer of doubles.");

        RETURN_NULL();
    }

    if (order <= 0 || zephir_buffer_len(a) != order * order) {
        zend_throw_error(NULL, "The matrix must be square.");

        RETURN_NULL();
    }

    zephir_buffer_create(return_value, order * order, ZEPHIR_BUFFER_DOUBLE);

    vc = zephir_buffer_doubles(return_value);

    /* dgetrf and dgetri both work in place, so the result buffer starts as a
     * copy of the operand and is factorised and inverted where it lies. */
    memcpy(vc, zephir_buffer_doubles(a), (size_t) (order * order) * sizeof(double));

    pivots = (lapack_int *) emalloc((size_t) order * sizeof(lapack_int));

    status = LAPACKE_dgetrf(LAPACK_ROW_MAJOR, (lapack_int) order, (lapack_int) order,
                            vc, (lapack_int) order, pivots);

    if (status != 0) {
        efree(pivots);
        zval_ptr_dtor(return_value);

        RETURN_NULL();
    }

    status = LAPACKE_dgetri(LAPACK_ROW_MAJOR, (lapack_int) order, vc, (lapack_int) order, pivots);

    efree(pivots);

    if (status != 0) {
        zval_ptr_dtor(return_value);

        RETURN_NULL();
    }
}
