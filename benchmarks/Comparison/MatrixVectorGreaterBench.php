<?php

namespace Tensor\Benchmarks\Comparison;

use Tensor\Matrix;
use Tensor\Vector;

/**
 * @Groups({"Comparison"})
 * @BeforeMethods({"setUp"})
 */
class MatrixVectorGreaterBench
{
    /**
     * @var Matrix
     */
    protected $a;

    /**
     * @var Vector
     */
    protected $b;

    public function setUp() : void
    {
        $this->a = Matrix::uniform(1000, 1000);

        $this->b = Vector::uniform(1000);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function greater() : void
    {
        $this->a->greater($this->b);
    }
}
