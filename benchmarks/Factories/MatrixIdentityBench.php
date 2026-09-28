<?php

namespace Tensor\Benchmarks\Factories;

use Tensor\Matrix;

/**
 * @Groups({"Factories"})
 */
class MatrixIdentityBench
{
    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function identity() : void
    {
        Matrix::identity(1024);
    }
}
