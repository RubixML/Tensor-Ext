<?php

namespace Tensor\Benchmarks\Factories;

use Tensor\Matrix;

/**
 * @Groups({"Factories"})
 * @BeforeMethods({"setUp"})
 */
class MatrixDiagonalBench
{
    /**
     * @var array
     */
    protected $elements;

    public function setUp() : void
    {
        $elements = [];

        for ($i = 0; $i < 1024; ++$i) {
            $elements[$i] = (float) $i;
        }

        $this->elements = $elements;
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function diagonal() : void
    {
        Matrix::diagonal($this->elements);
    }
}
