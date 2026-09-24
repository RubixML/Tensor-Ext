<?php

namespace Tensor\Benchmarks\Functions;

use Tensor\Matrix;

/**
 * @Groups({"Functions"})
 * @BeforeMethods({"setUp"})
 */
class Rad2DegMatrixBench
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
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function rad2deg() : void
    {
        $this->a->rad2deg();
    }
}
