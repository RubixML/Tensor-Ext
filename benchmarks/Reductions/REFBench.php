<?php

namespace Tensor\Benchmarks\Reductions;

use Tensor\Matrix;

/**
 * @Groups({"Reductions"})
 * @BeforeMethods({"setUp"})
 */
class REFBench
{
    /**
     * A full rank matrix, which takes the LAPACK `dgetrf` route.
     *
     * @var Matrix
     */
    protected $a;

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

        $b = Matrix::gaussian(500, 250);

        $this->deficient = $b->matmul($b->transpose());
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function ref() : void
    {
        $this->a->ref();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function refRankDeficient() : void
    {
        $this->deficient->ref();
    }
}
