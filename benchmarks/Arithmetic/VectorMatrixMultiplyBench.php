<?php

namespace Tensor\Benchmarks\Arithmetic;

use Tensor\Matrix;
use Tensor\Vector;

/**
 * @Groups({"Arithmetic"})
 * @BeforeMethods({"setUp"})
 */
class VectorMatrixMultiplyBench
{
    /**
     * @var Vector
     */
    protected $a;

    /**
     * @var Matrix
     */
    protected $b;

    public function setUp() : void
    {
        $this->a = Vector::uniform(1024);

        $this->b = Matrix::uniform(1024, 1024);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function multiply() : void
    {
        $this->a->multiplyMatrix($this->b);
    }
}
