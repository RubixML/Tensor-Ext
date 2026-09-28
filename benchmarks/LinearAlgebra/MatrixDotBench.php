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
        $this->a = Matrix::uniform(1024, 1024);

        $this->b = Vector::uniform(1024);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function dot() : void
    {
        $this->a->dot($this->b);
    }
}
