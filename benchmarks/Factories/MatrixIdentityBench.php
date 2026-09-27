<?php

namespace Tensor\Benchmarks\Factory;

use Tensor\Matrix;

/**
 * @Groups({"Factory"})
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
        Matrix::identity(500);
    }
}
