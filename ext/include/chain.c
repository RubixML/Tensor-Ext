#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <math.h>
#include <ext/spl/spl_exceptions.h>
#include "kernel/operators.h"
#include "php_ext.h"
#include "kernel/buffer.h"
#include "include/buffer.h"
#include "include/arithmetic.h"
#include "include/linear_algebra.h"
#include "include/unary.h"
#include "include/chain.h"

/*
 * Tensor\Chain planner.
 *
 * The Zephir side (tensor/chain.zep) records the user's chain of primitives
 * in two parallel zval-arrays -- `ops` (int op-codes, TENSOR_CHAIN_OP_*) and
 * `inps` (operands, positionally paired with `ops`) -- plus `extra` (extra
 * per-op integer dimensions, e.g. the weight's column count for MATMUL). It
 * hands all three, together with the source TensorBuffer and its m x n shape,
 * to tensor_chain_plan() in a single call.
 *
 * The planner walks the ops in recording order. At each position it first
 * probes the fuse table (v1: one rule -- SiLU) and, on a match, dispatches a
 * fused C kernel in place of the primitives it would otherwise run one by
 * one. On a miss, it runs the single primitive.
 *
 * Op semantics
 * ------------
 * MATMUL           cur = cur @ w;   w is the weight buffer, wn is its
 *                                            column count carried in `extra`.
 *                                            Shape m x n -> m x wn.
 * ADD (row)        cur <- cur + b elementwise, b must have m x n elements.
 * ADD (scalar)     cur <- cur + s elementwise.
 * NEGATE           cur <- -cur.
 * EXP              cur <- exp(cur).
 * FORK             f := cur (a fresh copy of the buffer; f now holds the
 *                  branch slot; `cur` is untouched and continues to be the
 *                  value the branch operates on).
 * COMBINE_DIVIDE   cur <- cur / f;  f := NULL.
 *                  (Fork-then-transform-then-combine is how a fork/combine
 *                  primitive expresses x / (exp(-x) + 1.0) -- SiLU -- without
 *                  a dedicated SiLU verb.)
 *
 * Ownership
 * ---------
 * zval cur  : strong ref to the running value; owns exactly one.
 * zval f    : strong ref to the forked branch slot, or UNDEF.
 * Every kernel writes a fresh TensorBuffer into a scratch zval and we move
 * that reference into the destination slot, releasing the old one exactly
 * once with zval_ptr_dtor (see chain_release()).
 */

/* Borrowed reader on the `inps` zval-array. Returns NULL when absent or not
 * an object (we do not take a reference). */
static zval * chain_inps_object(const zval * inps, zend_long i)
{
    HashTable * ht = Z_ARRVAL_P(inps);

    if (UNEXPECTED(ht == NULL)) {
        return NULL;
    }

    zval * elem = zend_hash_index_find(ht, (zend_ulong) i);

    if (UNEXPECTED(elem == NULL) || Z_TYPE_P(elem) != IS_OBJECT) {
        return NULL;
    }

    return elem;
}

/* Read the scalar operand at position i. Returns 0 on success; 1 when absent
 * or not a scalar. */
static int chain_inps_scalar(const zval * inps, zend_long i, double * out)
{
    HashTable * ht = Z_ARRVAL_P(inps);

    if (UNEXPECTED(ht == NULL)) {
        return 1;
    }

    zval * elem = zend_hash_index_find(ht, (zend_ulong) i);

    if (UNEXPECTED(elem == NULL) ||
        (Z_TYPE_P(elem) != IS_DOUBLE && Z_TYPE_P(elem) != IS_LONG)) {
        return 1;
    }

    *out = zephir_get_doubleval(elem);

    return 0;
}

/* Borrowed reader on the `extra` zval-array. Returns 0 on success; 1 when absent. */
static int chain_extra_int(const zval * extra, zend_long i, zend_long * out)
{
    HashTable * ht = Z_ARRVAL_P(extra);

    if (UNEXPECTED(ht == NULL)) {
        return 1;
    }

    zval * elem = zend_hash_index_find(ht, (zend_ulong) i);

    if (UNEXPECTED(elem == NULL) ||
        (Z_TYPE_P(elem) != IS_LONG && Z_TYPE_P(elem) != IS_DOUBLE)) {
        return 1;
    }

    *out = zephir_get_intval(elem);

    return 0;
}

/* Release a reference to `*cur` and leave it UNDEF (safe to reassign). */
static void chain_release(zval * cur)
{
    if (UNEXPECTED(Z_TYPE_P(cur) != IS_UNDEF)) {
        zval_ptr_dtor(cur);
        ZVAL_UNDEF(cur);
    }
}

/*
 * SiLU fuse rule.
 *
 * SiLU is  x / (1 + exp(-x)). In the primitive vocabulary of the chain the
 * user writes it as:
 *
 *     fork(); negate(); exp(); add(1.0); combineDivide();
 *
 * i.e. FORK  -> f = x,     (branch slot is x)
 *      NEGATE-> cur = -x,
 *      EXP   -> cur = exp(-x),
 *      ADD(1.0) -> cur = exp(-x) + 1,
 *      COMBINE_DIVIDE -> cur = x / (exp(-x) + 1).
 *
 * Note the direction: `f` keeps holding `x` (the numerator) and `cur`
 * accumulates the denominator. The ADD operand must be the scalar 1.0 --
 * any other value does not fuse, so the planner falls through to the
 * sequential path and produces the same number via the un-optimised
 * FORK/NEGATE/EXP/ADD/COMBINE_DIVIDE walk, keeping the user's expression
 * correct.
 */
#define TENSOR_SILU_FUSE_RULE_LEN 5

static int chain_try_fuse_silu(const zval * ops, zval * inps, zend_long i,
                               zend_long ops_len, zend_long * consumed)
{
    if (UNEXPECTED(i + (zend_long) TENSOR_SILU_FUSE_RULE_LEN > ops_len)) {
        return 0;
    }

    HashTable * hto = Z_ARRVAL_P(ops);

    if (UNEXPECTED(hto == NULL)) {
        return 0;
    }

    zend_long expected[ TENSOR_SILU_FUSE_RULE_LEN ] = {
        TENSOR_CHAIN_OP_FORK,
        TENSOR_CHAIN_OP_NEGATE,
        TENSOR_CHAIN_OP_EXP,
        TENSOR_CHAIN_OP_ADD,
        TENSOR_CHAIN_OP_COMBINE_DIVIDE
    };

    int k;

    for (k = 0; k < TENSOR_SILU_FUSE_RULE_LEN; ++k) {
        zval * op = zend_hash_index_find(hto, (zend_ulong) i + k);

        if (UNEXPECTED(op == NULL) || zephir_get_intval(op) != expected[ k ]) {
            return 0;
        }
    }

    double v;

    if (chain_inps_scalar(inps, i + 3, &v) != 0 || v != 1.0) {
        return 0;
    }

    *consumed = TENSOR_SILU_FUSE_RULE_LEN;

    return 1;
}

static void chain_fail(zval * cur, zval * f, const char * msg)
{
    chain_release(cur);
    chain_release(f);
    zephir_throw_exception_string(spl_ce_InvalidArgumentException, SL(msg));
}

/*
 * tensor_chain_plan -- see chain.h for the wire format.
 *
 * Returns a fresh TensorBuffer in `ret` on success (exactly one owned ref);
 * on any domain failure sets a PHP exception and leaves `ret` untouched.
 */
void tensor_chain_plan(zval * ret,
                       zval * source,
                       zval * init_m,
                       zval * init_n,
                       zval * ops,
                       zval * inps,
                       zval * extra)
{
    zend_long m = zephir_get_intval(init_m);
    zend_long n = zephir_get_intval(init_n);

    HashTable * hto = Z_ARRVAL_P(ops);

    if (UNEXPECTED(hto == NULL || Z_TYPE_P(source) != IS_OBJECT)) {
        zephir_throw_exception_string(spl_ce_InvalidArgumentException,
            SL("Chain source must be a TensorBuffer."));

        return;
    }

    zend_long ops_len = (zend_long) zend_hash_num_elements(hto);

    if (UNEXPECTED(ops_len == 0)) {
        ZVAL_COPY(ret, source);

        return;
    }

    /* `cur` owns exactly one ref; `f` owns one when a FORK has been applied
     * and we have not yet COMBINEd it away. */
    zval cur;
    ZVAL_COPY(&cur, source);

    zval f;
    ZVAL_UNDEF(&f);

    zend_long i = 0;

    while (i < ops_len) {
        zend_long consumed = 0;

        if (chain_try_fuse_silu(ops, inps, i, ops_len, &consumed)) {
            /* FUSED SiLU: one pass over the running value. The `f` branch slot
             * may or may not be set at this point (the fuse rule probes the
             * op sequence before any op runs); either way it holds a strong
             * ref we own, so we release it on the way out of the loop below
             * (via chain_release at the end of the function). */
            zval fused;
            ZVAL_UNDEF(&fused);
            tensor_silu(&fused, &cur);

            if (UNEXPECTED(Z_TYPE_P(&fused) == IS_UNDEF)) {
                chain_fail(&cur, &f, "Silu fused kernel failed.");

                return;
            }

            chain_release(&cur);
            ZVAL_COPY_VALUE(&cur, &fused);

            i += consumed;

            continue;
        }

        zval * op_elem = zend_hash_index_find(hto, (zend_ulong) i);

        if (UNEXPECTED(op_elem == NULL)) {
            chain_fail(&cur, &f, "Chain ops array is shorter than expected.");

            return;
        }

        zend_long opval = zephir_get_intval(op_elem);

        switch (opval) {

            case TENSOR_CHAIN_OP_MATMUL: {
                zval * w = chain_inps_object(inps, i);
                zend_long wn = 0;

                if (UNEXPECTED(w == NULL)) {
                    chain_fail(&cur, &f, "Matmul operand must be a TensorBuffer.");

                    return;
                }

                if (UNEXPECTED(chain_extra_int(extra, i, &wn) != 0 || wn < 1)) {
                    chain_fail(&cur, &f, "Matmul requires the weight's column count in extra[i].");

                    return;
                }

                zval m_z, p_z, n_z;
                ZVAL_LONG(&m_z, m);
                ZVAL_LONG(&p_z, n);
                ZVAL_LONG(&n_z, wn);

                zval out;
                ZVAL_UNDEF(&out);
                tensor_matmul(&out, &cur, w, &m_z, &p_z, &n_z);
                if (UNEXPECTED(Z_TYPE_P(&out) == IS_UNDEF)) {
                    chain_fail(&cur, &f,
                        "Input buffers must match the given dimensions.");

                    return;
                }
                chain_release(&cur);
                ZVAL_COPY_VALUE(&cur, &out);
                n = wn;

                i += 1;

                continue;
            }

            case TENSOR_CHAIN_OP_ADD: {
                zval * w = chain_inps_object(inps, i);
                zval out;

                if (w != NULL) {
                    zval n_z;
                    ZVAL_LONG(&n_z, n);

                    tensor_add_row(&out, &cur, w, &n_z);

                    if (UNEXPECTED(Z_TYPE_P(&out) == IS_UNDEF)) {
                        chain_fail(&cur, &f,
                            "Matrix and vector dimensions must agree.");

                        return;
                    }
                } else {
                    double v;

                    if (UNEXPECTED(chain_inps_scalar(inps, i, &v) != 0)) {
                        chain_fail(&cur, &f, "Add operand is missing or not a scalar.");

                        return;
                    }

                    zval s;
                    ZVAL_DOUBLE(&s, v);

                    tensor_add_scalar(&out, &cur, &s);

                    if (UNEXPECTED(Z_TYPE_P(&out) == IS_UNDEF)) {
                        chain_fail(&cur, &f,
                            "Add scalar operand must be numeric.");

                        return;
                    }
                }

                chain_release(&cur);
                ZVAL_COPY_VALUE(&cur, &out);

                i += 1;

                continue;
            }

            case TENSOR_CHAIN_OP_NEGATE: {
                zval out;
                tensor_negate(&out, &cur);

                if (UNEXPECTED(Z_TYPE_P(&out) == IS_UNDEF)) {
                    chain_fail(&cur, &f, "Negate failed on the running buffer.");

                    return;
                }

                chain_release(&cur);
                ZVAL_COPY_VALUE(&cur, &out);

                i += 1;

                continue;
            }

            case TENSOR_CHAIN_OP_EXP: {
                zval out;
                tensor_exp(&out, &cur);

                if (UNEXPECTED(Z_TYPE_P(&out) == IS_UNDEF)) {
                    chain_fail(&cur, &f, "Exp failed on the running buffer.");

                    return;
                }

                chain_release(&cur);
                ZVAL_COPY_VALUE(&cur, &out);

                i += 1;

                continue;
            }

            case TENSOR_CHAIN_OP_FORK: {
                if (UNEXPECTED(Z_TYPE_P(&f) != IS_UNDEF)) {
                    chain_release(&f);
                }

                ZVAL_COPY(&f, &cur);

                i += 1;

                continue;
            }

            case TENSOR_CHAIN_OP_COMBINE_DIVIDE: {
                if (UNEXPECTED(Z_TYPE_P(&f) == IS_UNDEF)) {
                    chain_fail(&cur, &f, "combineDivide was called before fork.");

                    return;
                }

                zval out;
                tensor_divide(&out, &f, &cur);   /* cur = f / cur */

                if (UNEXPECTED(Z_TYPE_P(&out) == IS_UNDEF)) {
                    chain_fail(&cur, &f, "Combine divide failed.");

                    return;
                }

                chain_release(&f);
                chain_release(&cur);
                ZVAL_COPY_VALUE(&cur, &out);

                i += 1;

                continue;
            }

            default: {
                chain_fail(&cur, &f, "Unknown chain op code.");

                return;
            }
        }
    }

    /* Take a strong ref onto `ret` so it owns its own reference to the
     * buffer (the caller will release it); then drop our local `cur` strong
     * ref, leaving the buffer alive via `ret` alone. */
    ZVAL_COPY(ret, &cur);
    zval_ptr_dtor(&cur);

    chain_release(&f);
}
