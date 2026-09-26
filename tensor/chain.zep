namespace Tensor;

use Tensor\Exceptions\InvalidArgumentException;
use Tensor\Exceptions\DimensionalityMismatch;

/**
 * Chain
 *
 * A composable pipeline of tensor primitives, planned into fused kernels
 * where a shape is recognized.
 *
 * The user composes a chain out of the primitive verbs -- matmul, add, negate,
 * exp, fork, and combineDivide -- and the C planner
 * (ext/include/chain.c) walks the recorded op sequence in order. On a shape
 * it recognizes (v1: the SiLU composition `fork(); negate(); exp();
 * add(1.0); combineDivide()`) it dispatches a dedicated fused kernel;
 * otherwise it re-runs the primitives one at a time. The user never sees the
 * fused kernel: they only ever compose with the primitives.
 *
 * Wire format. The three positional zval-arrays -- `ops`, `inps`, `extra` --
 * are handed to `tensor_chain_plan()` in `done()`. The op codes are literal
 * integers here matching the `TENSOR_CHAIN_OP_*` values in
 * `ext/include/chain.h`; keep the two in lock step:
 *
 *   MATMUL  = 0      inps[i] = weight TensorBuffer,  extra[i] = weight n
 *   ADD     = 1      inps[i] = row TensorBuffer or a scalar double
 *   NEGATE  = 2
 *   EXP     = 3
 *   FORK    = 4
 *   COMBINE_DIVIDE = 5
 *
 * The chain's shape (m, n) is tracked incrementally:
 *
 *   matmul(w)        : n := len(w) / m (weight shape must be m x n')
 *   add(row/scalar)  : m, n unchanged
 *   negate/exp      : m, n unchanged
 *   fork/combineDivide : no shape change (fork snapshots the value)
 *
 * @category    Scientific Computing
 * @package     Rubix/Tensor
 * @author      Andrew DalPino
 */
class Chain
{
    /**
     * The source buffer being transformed.
     *
     * @var \Tensor\TensorBuffer
     */
    protected buffer;

    /**
     * The number of rows in the source matrix (never changes through the
     * chain).
     *
     * @var int
     */
    protected initM;

    /**
     * The number of columns in the source matrix (never changes through the
     * chain).
     *
     * @var int
     */
    protected initN;

    /**
     * The current number of rows in the running value.
     *
     * @var int
     */
    protected m;

    /**
     * The current number of columns in the running value.
     *
     * @var int
     */
    protected n;

    /**
     * Recorded op codes in call order.
     *
     * @var int[]
     */
    protected ops;

    /**
     * Recorded operands, positionally paired with `ops`. For matmul the weight
     * TensorBuffer; for add, a TensorBuffer (row) or a scalar double; for the
     * unary operations, null so the length matches `ops`.
     *
     * @var array
     */
    protected inps;

    /**
     * Extra per-op integer dimensions, positionally paired with `ops`. Only
     * matmul uses it (the weight's column count); every other slot is null.
     *
     * @var array
     */
    protected extra;

    /**
     * Create a chain from a source matrix at the given shape.
     *
     * @param \Tensor\TensorBuffer buffer
     * @param int m
     * @param int n
     * @throws \Tensor\Exceptions\InvalidArgumentException
     */
    protected function __construct(<TensorBuffer> buffer, const int m, const int n)
    {
        if unlikely m < 1 || n < 1 {
            throw new InvalidArgumentException("Chain dimensions must be"
                . " positive, " . strval(m) . "x" . strval(n) . " given.");
        }

        let this->buffer = buffer;
        let this->initM = m;
        let this->initN = n;
        let this->m = m;
        let this->n = n;
        let this->ops = [];
        let this->inps = [];
        let this->extra = [];
    }

    /**
     * Start a chain from a 2D tensor.
     *
     * @param \Tensor\Matrix|Vector source
     * @return self
     */
    public static function of(var source) -> <Chain>
    {
        switch (gettype(source)) {
            case "object":
                switch true {
                    case source instanceof Matrix:
                        return new self(source->asTensorBuffer(), source->m(), source->n());
                }

                break;
        }

        throw new InvalidArgumentException("Chain source must"
            . " be a Matrix at present.");
    }

    /**
     * Record a matrix multiplication: cur = cur @ w.
     *
     * @param \Tensor\Matrix w
     * @return self
     */
    public function matmul(const <Matrix> w) -> <Chain>
    {
        if unlikely w->m() !== this->n {
            throw new DimensionalityMismatch("Chain at " . strval(this->m)
                . "x" . strval(this->n) . " expects a weight with "
                . " " . strval(this->n) . " rows but Matrix B has "
                . " " . strval(w->m()) . ".");
        }

        let this->ops[] = 0;
        let this->inps[] = w->asTensorBuffer();
        let this->extra[] = w->n();

        let this->n = w->n();

        return this;
    }

    /**
     * Record either a scalar add or a row-vector add: cur = cur + value.
     *
     * When `value` is an object it is interpreted as a Matrix row (shape
     * 1 x n) and the underlying TensorBuffer is stored. When it is numeric
     * it is stored as a scalar double.
     *
     * @param mixed value
     * @return self
     */
    public function add(const var value) -> <Chain>
    {
        switch (gettype(value)) {
            case "object":
                if unlikely !(value instanceof Matrix) {
                    throw new InvalidArgumentException("Chain add with"
                        . " an object expects a Matrix.");
                }

                if unlikely value->m() !== 1 || value->n() !== this->n {
                    throw new DimensionalityMismatch("Chain at "
                        . strval(this->m) . "x" . strval(this->n)
                        . " expects a row vector of " . strval(this->n)
                        . " cols but " . strval(value->m()) . "x"
                        . strval(value->n()) . " given.");
                }

                let this->ops[] = 1;
                let this->inps[] = value->asTensorBuffer();
                let this->extra[] = null;

                return this;

            case "double":
            case "integer":
                let this->ops[] = 1;
                let this->inps[] = (double) value;
                let this->extra[] = null;

                return this;
        }

        throw new InvalidArgumentException("Chain add with"
            . " a non-numeric or non-Matrix operand is not supported.");
    }

    /**
     * Record a sign flip: cur = -cur.
     *
     * @return self
     */
    public function negate() -> <Chain>
    {
        let this->ops[] = 2;
        let this->inps[] = null;
        let this->extra[] = null;

        return this;
    }

    /**
     * Record the natural exponential: cur = exp(cur).
     *
     * @return self
     */
    public function exp() -> <Chain>
    {
        let this->ops[] = 3;
        let this->inps[] = null;
        let this->extra[] = null;

        return this;
    }

    /**
     * Snapshot the current value into the branch slot. Forked value continues
     * to be transformed; combineDivide later divides branch by current.
     *
     * @return self
     */
    public function fork() -> <Chain>
    {
        let this->ops[] = 4;
        let this->inps[] = null;
        let this->extra[] = null;

        return this;
    }

    /**
     * Combine the branch with the running value: cur = branch / cur, and
     * clear the branch slot.
     *
     * @return self
     */
    public function combineDivide() -> <Chain>
    {
        let this->ops[] = 5;
        let this->inps[] = null;
        let this->extra[] = null;

        return this;
    }

    /**
     * Run the recorded chain against the source and return the result Matrix.
     *
     * On the SiLU shape (`fork/negate/exp/add(1.0)/combineDivide`) the planner
     * dispatches a single fused kernel. Otherwise it re-runs the primitives in
     * the order recorded.
     *
     * @return \Tensor\Matrix
     */
    public function done() -> <Matrix>
    {
        if unlikely count(this->ops) == 0 {
            return Matrix::fromBuffer(this->buffer, this->initM, this->initN);
        }

        var result = tensor_chain_plan(
            this->buffer,
            this->initM,
            this->initN,
            this->ops,
            this->inps,
            this->extra
        );

        return Matrix::fromBuffer(result, this->m, this->n);
    }
}
