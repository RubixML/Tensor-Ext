<?php

namespace Tensor\Benchmarks\Unary;

use Tensor\Matrix;

/**
 * @Groups({"Unary"})
 * @BeforeMethods({"setUp"})
 */
class Deg2RadMatrixBench
{
    /**
     * @var Matrix
     */
    protected $a;

    public function setUp() : void
    {
        $this->a = Matrix::uniform(1024, 1024);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function deg2rad() : void
    {
        $this->a->deg2rad();
    }
}
