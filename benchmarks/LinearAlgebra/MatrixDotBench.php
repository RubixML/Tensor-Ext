<?php

namespace Tensor\Benchmarks\LinearAlgebra;

use Tensor\Matrix;
use Tensor\Vector;

/**
 * @Groups({"LinearAlgebra"})
 * @BeforeMethods({"setUp"})
 */
class MatrixDotBench
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
     * @Revs(100)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function dot() : void
    {
        $this->a->dot($this->b);
    }
}
