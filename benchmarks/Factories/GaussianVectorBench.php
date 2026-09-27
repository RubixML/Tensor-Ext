<?php

namespace Tensor\Benchmarks\Random;

use Tensor\Vector;

/**
 * @Groups({"Random"})
 */
class GaussianVectorBench
{
    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function gaussian() : void
    {
        Vector::gaussian(250000);
    }
}
