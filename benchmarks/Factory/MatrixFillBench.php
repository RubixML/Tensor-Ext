<?php

namespace Tensor\Benchmarks\Factory;

use Tensor\Matrix;

/**
 * @Groups({"Factory"})
 */
class MatrixFillBench
{
    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function fill() : void
    {
        Matrix::fill(1.0, 500, 500);
    }
}
