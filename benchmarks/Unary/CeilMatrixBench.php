<?php

namespace Tensor\Benchmarks\Unary;

use Tensor\Matrix;

/**
 * @Groups({"Unary"})
 * @BeforeMethods({"setUp"})
 */
class CeilMatrixBench
{
    /**
     * @var Matrix
     */
    protected $a;

    public function setUp() : void
    {
        $this->a = Matrix::uniform(1000, 1000);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function ceil() : void
    {
        $this->a->ceil();
    }
}
