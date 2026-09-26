<?php

namespace Tensor\Benchmarks\Chain;

use Tensor\Chain;
use Tensor\Matrix;

/**
 * Benchmarks the Chain DSL and the planner's SiLU fuse rule.
 *
 * `siluPrimitives` writes the SiLU shape out of the primitives
 * (fork / negate / exp / add(1.0) / combineDivide) and `siluLongForm` writes
 * the same function the "naive" way (negate / exp / add / divide through
 * the public Matrix API) so the before / after comparison for the fusion is
 * apples to apples.
 *
 * @Groups({"Chain"})
 * @BeforeMethods({"setUp"})
 */
class ChainBench
{
    /**
     * @var Matrix
     */
    protected $x;

    public function setUp() : void
    {
        $this->x = Matrix::uniform(500, 500);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function siluPrimitives() : void
    {
        Chain::of($this->x)
            ->fork()
            ->negate()
            ->exp()
            ->add(1.0)
            ->combineDivide()
            ->done();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function siluLongForm() : void
    {
        $den = $this->x->negate()->exp()->add(1.0);
        $this->x->divide($den);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function siluChainAfterMatmul() : void
    {
        $w = Matrix::uniform(500, 500);

        $h = $this->x->matmul($w);

        Chain::of($h)
            ->fork()
            ->negate()
            ->exp()
            ->add(1.0)
            ->combineDivide()
            ->done();
    }

    /**
     * A non-fusing near miss. The planner should fall through to the
     * sequential path, so this benchmark should run at roughly the same speed
     * as `siluLongForm` and meaningfully slower than `siluPrimitives`.
     *
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function siluNonFusingNearMiss() : void
    {
        Chain::of($this->x)
            ->fork()
            ->negate()
            ->exp()
            ->add(0.5)
            ->combineDivide()
            ->done();
    }
}
