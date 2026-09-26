#ifndef TENSOR_CHAIN_H
#define TENSOR_CHAIN_H

#include <Zend/zend.h>

/* Operation codes for the Tensor\Chain planner.
 *
 * Each op in the `ops` zval-array passed to tensor_chain_plan() is one of these
 * values. The integer here and the matching `const OP_*` values declared in
 * tensor/chain.zep form the wire format between Zephir PHP code and the C
 * planner; keep them in lock step. A renumbering is a wire break.
 *
 * The planner matches an op-code window against its fuse table and, on a hit,
 * dispatches a fused C kernel instead of running the recorded primitives one
 * at a time. Adding a new fused combination is a table-row + C-kernel change,
 * not a change to the public API or to per-method Zephir call sites. */
enum {
	TENSOR_CHAIN_OP_MATMUL         = 0,   /* inps: TensorBuffer (weights)      */
	TENSOR_CHAIN_OP_ADD            = 1,   /* inps: TensorBuffer (row) or scalar */
	TENSOR_CHAIN_OP_NEGATE         = 2,   /* inps: null                          */
	TENSOR_CHAIN_OP_EXP            = 3,   /* inps: null                          */
	TENSOR_CHAIN_OP_FORK           = 4,   /* inps: null; snapshot the running value */
	TENSOR_CHAIN_OP_COMBINE_DIVIDE = 5    /* inps: null; running / forked       */
};

/* Wire format (matches the convention of every other kernel in this
 * extension: first argument is the return slot):
 *
 *   source -- zval holding a TensorBuffer of `m x n` elements (row-major),
 *             the chain's running value. Borrowed; the planner holds no
 *             extra reference to it.
 *   init_m -- zval holding the row count of `source`.
 *   init_n -- zval holding the column count of `source`.
 *   ops    -- zval-array of int op-codes (TENSOR_CHAIN_OP_*).
 *   inps   -- zval-array, positionally paired with `ops`. A TensorBuffer
 *             object for MATMUL / ADD(vector), a scalar double for
 *             ADD(scalar), or undef/NULL for the unary ops.
 *   extra  -- zval-array, positionally paired with `ops`, holding an extra
 *             integer dimension where a kernel needs one that the running
 *             shape does not already know (e.g. the column count of a matmul
 *             weight). undef/NULL where it is not used.
 *
 * Fills `return_value` with a fresh TensorBuffer on success (exactly one
 * owned reference). On a domain error, sets a PHP exception and leaves
 * `return_value` unset. */
void tensor_chain_plan(zval * return_value,
                       zval * source,
                       zval * init_m,
                       zval * init_n,
                       zval * ops,
                       zval * inps,
                       zval * extra);

#endif
