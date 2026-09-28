<?php

namespace Tensor\Benchmarks\Reductions;

use Tensor\Matrix;

/**
 * @Groups({"Reductions"})
 * @BeforeMethods({"setUp"})
 */
class RREFBench
{
    /**
     * A full rank matrix. More rows than columns, so the reduced form is the
     * identity over a zero block and is written directly.
     *
     * @var Matrix
     */
    protected $a;

    /**
     * A full row rank matrix with fewer rows than columns, whose reduced form
     * is `[I | X]` and has to be computed.
     *
     * @var Matrix
     */
    protected $wide;

    /**
     * A rank deficient matrix of half the rank, which takes the singular
     * fallback: a Gram matrix `B * B'` has rank equal to the number of columns
     * of `B` no matter what `B` holds, so this is rank 250 for any input.
     *
     * @var Matrix
     */
    protected $deficient;

    public function setUp() : void
    {
        $this->a = Matrix::uniform(500, 500);

        $this->wide = Matrix::uniform(300, 500);

        $b = Matrix::gaussian(500, 250);

        $this->deficient = $b->matmul($b->transpose());
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function rref() : void
    {
        $this->a->rref();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function rrefWide() : void
    {
        $this->wide->rref();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function rrefRankDeficient() : void
    {
        $this->deficient->rref();
    }
}
