#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <Zend/zend_exceptions.h>
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
#include "include/buffer_matrix.h"

/**
 * Resolves the operation once, outside the loops, into a small enum. A strcmp
 * per element would cost more than the arithmetic several times over.
 */
enum tensor_op {
    TENSOR_OP_MULTIPLY, TENSOR_OP_DIVIDE, TENSOR_OP_ADD, TENSOR_OP_SUBTRACT,
    TENSOR_OP_POW, TENSOR_OP_MOD, TENSOR_OP_EQUAL, TENSOR_OP_NOT_EQUAL,
    TENSOR_OP_GREATER, TENSOR_OP_GREATER_EQUAL, TENSOR_OP_LESS, TENSOR_OP_LESS_EQUAL,
    TENSOR_OP_UNKNOWN
};

static enum tensor_op tensor_resolve_op(const char * name)
{
    if (strcmp(name, "multiply") == 0)     return TENSOR_OP_MULTIPLY;
    if (strcmp(name, "divide") == 0)       return TENSOR_OP_DIVIDE;
    if (strcmp(name, "add") == 0)          return TENSOR_OP_ADD;
    if (strcmp(name, "subtract") == 0)     return TENSOR_OP_SUBTRACT;
    if (strcmp(name, "pow") == 0)          return TENSOR_OP_POW;
    if (strcmp(name, "mod") == 0)          return TENSOR_OP_MOD;
    if (strcmp(name, "equal") == 0)        return TENSOR_OP_EQUAL;
    if (strcmp(name, "notEqual") == 0)     return TENSOR_OP_NOT_EQUAL;
    if (strcmp(name, "greater") == 0)      return TENSOR_OP_GREATER;
    if (strcmp(name, "greaterEqual") == 0) return TENSOR_OP_GREATER_EQUAL;
    if (strcmp(name, "less") == 0)         return TENSOR_OP_LESS;
    if (strcmp(name, "lessEqual") == 0)    return TENSOR_OP_LESS_EQUAL;

    return TENSOR_OP_UNKNOWN;
}

/** Zero divisor guard shared by the modulo paths, matching PHP's behaviour. */
static int tensor_matrix_mod_guard(double y)
{
    if (zend_dval_to_lval(y) == 0) {
        zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");

        return FAILURE;
    }

    return SUCCESS;
}

static inline double tensor_apply(enum tensor_op op, double x, double y)
{
    switch (op) {
        case TENSOR_OP_MULTIPLY:      return x * y;
        case TENSOR_OP_DIVIDE:        return x / y;
        case TENSOR_OP_ADD:           return x + y;
        case TENSOR_OP_SUBTRACT:      return x - y;
        case TENSOR_OP_POW:           return pow(x, y);
        case TENSOR_OP_MOD:           return (double) (zend_dval_to_lval(x) % zend_dval_to_lval(y));
        case TENSOR_OP_EQUAL:         return x == y ? 1.0 : 0.0;
        case TENSOR_OP_NOT_EQUAL:     return x != y ? 1.0 : 0.0;
        case TENSOR_OP_GREATER:       return x > y ? 1.0 : 0.0;
        case TENSOR_OP_GREATER_EQUAL: return x >= y ? 1.0 : 0.0;
        case TENSOR_OP_LESS:          return x < y ? 1.0 : 0.0;
        case TENSOR_OP_LESS_EQUAL:    return x <= y ? 1.0 : 0.0;
        default:                      return 0.0;
    }
}

void tensor_buffer_from_matrix(zval * return_value, zval * a)
{
    zend_array * rows;
    zval * row;
    zval * value;
    double * vc;
    zend_long m, n, i, k = 0;

    if (Z_TYPE_P(a) != IS_ARRAY) {
        zend_throw_error(NULL, "Expected an array of rows.");

        RETURN_NULL();
    }

    rows = Z_ARR_P(a);
    m = (zend_long) zend_array_count(rows);

    if (m == 0) {
        zephir_buffer_create(return_value, 0, ZEPHIR_BUFFER_DOUBLE);

        return;
    }

    row = zend_hash_index_find(rows, 0);
    n = (row != NULL && Z_TYPE_P(row) == IS_ARRAY) ? (zend_long) zend_array_count(Z_ARR_P(row)) : 0;

    zephir_buffer_create(return_value, m * n, ZEPHIR_BUFFER_DOUBLE);

    if (m * n == 0) {
        return;
    }

    vc = zephir_buffer_doubles(return_value);

    for (i = 0; i < m; ++i) {
        row = zend_hash_index_find(rows, (zend_ulong) i);

        if (row == NULL || Z_TYPE_P(row) != IS_ARRAY) {
            zval_ptr_dtor(return_value);
            zend_throw_error(NULL, "Row %ld is not an array.", (long) i);

            RETURN_NULL();
        }

        ZEND_HASH_FOREACH_VAL(Z_ARR_P(row), value) {
            if (k >= m * n) {
                break;
            }

            vc[k++] = zephir_get_doubleval(value);
        } ZEND_HASH_FOREACH_END();
    }
}

void tensor_buffer_to_matrix(zval * return_value, zval * a, zval * n)
{
    double * va;
    zend_long width = zephir_get_intval(n);
    zend_long len, m, i, j, k = 0;
    zval rows, row;

    if (zephir_buffer_kind(a) != ZEPHIR_BUFFER_DOUBLE) {
        zend_throw_error(NULL, "Expected a buffer of doubles.");

        RETURN_NULL();
    }

    len = zephir_buffer_len(a);
    va = zephir_buffer_doubles(a);
    m = width > 0 ? len / width : 0;

    array_init_size(&rows, (uint32_t) (m > 0 ? m : 0));

    for (i = 0; i < m; ++i) {
        array_init_size(&row, (uint32_t) width);

        for (j = 0; j < width; ++j) {
            add_next_index_double(&row, va[k++]);
        }

        add_next_index_zval(&rows, &row);
    }

    RETVAL_ARR(Z_ARR(rows));
}

/**
 * Shared opening for the two broadcasts: validates kinds, derives m from the
 * buffer length and the row width, and allocates the result.
 */
static int tensor_matrix_open(
    zval * return_value,
    zval * a,
    zval * v,
    zend_long width,
    zend_long expected,
    double ** va,
    double ** vv,
    double ** vc,
    zend_long * m,
    zend_long * n
) {
    zend_long len;

    if (zephir_buffer_kind(a) != ZEPHIR_BUFFER_DOUBLE || zephir_buffer_kind(v) != ZEPHIR_BUFFER_DOUBLE) {
        zend_throw_error(NULL, "Expected buffers of doubles.");

        return FAILURE;
    }

    len = zephir_buffer_len(a);
    *n = width;
    *m = width > 0 ? len / width : 0;

    if (zephir_buffer_len(v) != (expected == 0 ? *n : *m)) {
        zend_throw_error(NULL, "The broadcast operand has the wrong length.");

        return FAILURE;
    }

    zephir_buffer_create(return_value, len, ZEPHIR_BUFFER_DOUBLE);

    *va = zephir_buffer_doubles(a);
    *vv = zephir_buffer_doubles(v);
    *vc = zephir_buffer_doubles(return_value);

    return SUCCESS;
}

void tensor_buffer_broadcast_vector(zval * return_value, zval * a, zval * v, zval * n, zval * op)
{
    double * va;
    double * vv;
    double * vc;
    zend_long m, cols, i, j, k = 0;
    enum tensor_op code;

    if (Z_TYPE_P(op) != IS_STRING) {
        zend_throw_error(NULL, "The operation must be named by a string.");

        RETURN_NULL();
    }

    code = tensor_resolve_op(Z_STRVAL_P(op));

    if (code == TENSOR_OP_UNKNOWN) {
        zend_throw_error(NULL, "Unknown broadcast operation '%s'.", Z_STRVAL_P(op));

        RETURN_NULL();
    }

    if (tensor_matrix_open(return_value, a, v, zephir_get_intval(n), 0, &va, &vv, &vc, &m, &cols) == FAILURE) {
        RETURN_NULL();
    }

    if (m * cols == 0) {
        return;
    }

    if (code == TENSOR_OP_MOD) {
        for (j = 0; j < cols; ++j) {
            if (tensor_matrix_mod_guard(vv[j]) == FAILURE) {
                zval_ptr_dtor(return_value);

                RETURN_NULL();
            }
        }
    }

    for (i = 0; i < m; ++i) {
        for (j = 0; j < cols; ++j, ++k) {
            vc[k] = tensor_apply(code, va[k], vv[j]);
        }
    }
}

void tensor_buffer_broadcast_column(zval * return_value, zval * a, zval * v, zval * n, zval * op)
{
    double * va;
    double * vv;
    double * vc;
    zend_long m, cols, i, j, k = 0;
    enum tensor_op code;

    if (Z_TYPE_P(op) != IS_STRING) {
        zend_throw_error(NULL, "The operation must be named by a string.");

        RETURN_NULL();
    }

    code = tensor_resolve_op(Z_STRVAL_P(op));

    if (code == TENSOR_OP_UNKNOWN) {
        zend_throw_error(NULL, "Unknown broadcast operation '%s'.", Z_STRVAL_P(op));

        RETURN_NULL();
    }

    if (tensor_matrix_open(return_value, a, v, zephir_get_intval(n), 1, &va, &vv, &vc, &m, &cols) == FAILURE) {
        RETURN_NULL();
    }

    if (m * cols == 0) {
        return;
    }

    for (i = 0; i < m; ++i) {
        const double y = vv[i];

        if (code == TENSOR_OP_MOD && tensor_matrix_mod_guard(y) == FAILURE) {
            zval_ptr_dtor(return_value);

            RETURN_NULL();
        }

        for (j = 0; j < cols; ++j, ++k) {
            vc[k] = tensor_apply(code, va[k], y);
        }
    }
}

void tensor_buffer_row_reduce(zval * return_value, zval * a, zval * n, zval * op)
{
    double * va;
    double * vc;
    zend_long width = zephir_get_intval(n);
    zend_long len, m, i, j;
    const char * name;

    if (Z_TYPE_P(op) != IS_STRING || zephir_buffer_kind(a) != ZEPHIR_BUFFER_DOUBLE) {
        zend_throw_error(NULL, "Expected a buffer of doubles and an operation name.");

        RETURN_NULL();
    }

    len = zephir_buffer_len(a);
    m = width > 0 ? len / width : 0;
    name = Z_STRVAL_P(op);

    if ((strcmp(name, "min") == 0 || strcmp(name, "max") == 0) && width == 0) {
        zend_throw_exception_ex(
            zend_ce_value_error, 0,
            "%s(): Argument #1 ($value) must contain at least one element", name
        );

        RETURN_NULL();
    }

    zephir_buffer_create(return_value, m, ZEPHIR_BUFFER_DOUBLE);

    if (m == 0) {
        return;
    }

    va = zephir_buffer_doubles(a);
    vc = zephir_buffer_doubles(return_value);

    for (i = 0; i < m; ++i) {
        const double * row = va + i * width;
        double acc;

        if (strcmp(name, "sum") == 0) {
            /* Sequential, matching array_sum() addition for addition. */
            acc = 0.0;

            for (j = 0; j < width; ++j) {
                acc += row[j];
            }
        } else if (strcmp(name, "product") == 0) {
            acc = 1.0;

            for (j = 0; j < width; ++j) {
                acc *= row[j];
            }
        } else if (strcmp(name, "min") == 0) {
            acc = row[0];

            for (j = 1; j < width; ++j) {
                if (row[j] < acc) {
                    acc = row[j];
                }
            }
        } else if (strcmp(name, "max") == 0) {
            acc = row[0];

            for (j = 1; j < width; ++j) {
                if (row[j] > acc) {
                    acc = row[j];
                }
            }
        } else {
            zval_ptr_dtor(return_value);
            zend_throw_error(NULL, "Unknown row reduction '%s'.", name);

            RETURN_NULL();
        }

        vc[i] = acc;
    }
}

void tensor_buffer_transpose(zval * return_value, zval * a, zval * n)
{
    double * va;
    double * vc;
    zend_long width = zephir_get_intval(n);
    zend_long len, m, i, j;

    if (zephir_buffer_kind(a) != ZEPHIR_BUFFER_DOUBLE) {
        zend_throw_error(NULL, "Expected a buffer of doubles.");

        RETURN_NULL();
    }

    len = zephir_buffer_len(a);
    m = width > 0 ? len / width : 0;

    zephir_buffer_create(return_value, len, ZEPHIR_BUFFER_DOUBLE);

    if (len == 0) {
        return;
    }

    va = zephir_buffer_doubles(a);
    vc = zephir_buffer_doubles(return_value);

    /* Reading down a column and writing along a row: the write is sequential
     * and the read strided, which is the friendlier way round for the
     * prefetcher at these sizes. */
    for (j = 0; j < width; ++j) {
        for (i = 0; i < m; ++i) {
            vc[j * m + i] = va[i * width + j];
        }
    }
}

void tensor_buffer_stride(zval * return_value, zval * a, zval * offset, zval * stride, zval * count)
{
    double * va;
    double * vc;
    zend_long start = zephir_get_intval(offset);
    zend_long step = zephir_get_intval(stride);
    zend_long len = zephir_get_intval(count);
    zend_long total, i;

    if (zephir_buffer_kind(a) != ZEPHIR_BUFFER_DOUBLE) {
        zend_throw_error(NULL, "Expected a buffer of doubles.");

        RETURN_NULL();
    }

    total = zephir_buffer_len(a);

    if (len < 0 || start < 0 || (len > 0 && start + (len - 1) * step >= total)) {
        zend_throw_error(NULL, "The requested range lies outside the buffer.");

        RETURN_NULL();
    }

    zephir_buffer_create(return_value, len, ZEPHIR_BUFFER_DOUBLE);

    if (len == 0) {
        return;
    }

    va = zephir_buffer_doubles(a);
    vc = zephir_buffer_doubles(return_value);

    if (step == 1) {
        memcpy(vc, va + start, (size_t) len * sizeof(double));

        return;
    }

    for (i = 0; i < len; ++i) {
        vc[i] = va[start + i * step];
    }
}

void tensor_buffer_append(zval * return_value, zval * a, zval * b)
{
    double * vc;
    zend_long la, lb;

    if (zephir_buffer_kind(a) != ZEPHIR_BUFFER_DOUBLE || zephir_buffer_kind(b) != ZEPHIR_BUFFER_DOUBLE) {
        zend_throw_error(NULL, "Expected buffers of doubles.");

        RETURN_NULL();
    }

    la = zephir_buffer_len(a);
    lb = zephir_buffer_len(b);

    zephir_buffer_create(return_value, la + lb, ZEPHIR_BUFFER_DOUBLE);

    if (la + lb == 0) {
        return;
    }

    vc = zephir_buffer_doubles(return_value);

    /* Row-major and the same width, so stacking vertically is one memcpy each. */
    if (la > 0) {
        memcpy(vc, zephir_buffer_doubles(a), (size_t) la * sizeof(double));
    }

    if (lb > 0) {
        memcpy(vc + la, zephir_buffer_doubles(b), (size_t) lb * sizeof(double));
    }
}

void tensor_buffer_append_columns(zval * return_value, zval * a, zval * b, zval * an, zval * bn)
{
    double * va;
    double * vb;
    double * vc;
    zend_long wa = zephir_get_intval(an);
    zend_long wb = zephir_get_intval(bn);
    zend_long m, i;

    if (zephir_buffer_kind(a) != ZEPHIR_BUFFER_DOUBLE || zephir_buffer_kind(b) != ZEPHIR_BUFFER_DOUBLE) {
        zend_throw_error(NULL, "Expected buffers of doubles.");

        RETURN_NULL();
    }

    m = wa > 0 ? zephir_buffer_len(a) / wa : (wb > 0 ? zephir_buffer_len(b) / wb : 0);

    if ((wa > 0 && zephir_buffer_len(a) != m * wa) || (wb > 0 && zephir_buffer_len(b) != m * wb)) {
        zend_throw_error(NULL, "The operands do not have the same number of rows.");

        RETURN_NULL();
    }

    zephir_buffer_create(return_value, m * (wa + wb), ZEPHIR_BUFFER_DOUBLE);

    if (m * (wa + wb) == 0) {
        return;
    }

    va = zephir_buffer_doubles(a);
    vb = zephir_buffer_doubles(b);
    vc = zephir_buffer_doubles(return_value);

    /* Rows interleave, so this is two memcpys per row rather than one. */
    for (i = 0; i < m; ++i) {
        if (wa > 0) {
            memcpy(vc + i * (wa + wb), va + i * wa, (size_t) wa * sizeof(double));
        }

        if (wb > 0) {
            memcpy(vc + i * (wa + wb) + wa, vb + i * wb, (size_t) wb * sizeof(double));
        }
    }
}
