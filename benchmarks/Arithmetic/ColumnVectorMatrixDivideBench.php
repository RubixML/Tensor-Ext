<?php

namespace Tensor\Benchmarks\Arithmetic;

use Tensor\Matrix;
use Tensor\ColumnVector;

/**
 * @Groups({"Arithmetic"})
 * @BeforeMethods({"setUp"})
 */
class ColumnVectorMatrixDivideBench
{
    /**
     * @var \Tensor\Vector
     */
    protected $a;

    /**
     * @var Matrix
     */
    protected $b;

    public function setUp() : void
    {
        $this->a = ColumnVector::uniform(1000);

        $this->b = Matrix::uniform(1000, 1000);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function divide() : void
    {
        $this->a->divide($this->b);
    }
}
