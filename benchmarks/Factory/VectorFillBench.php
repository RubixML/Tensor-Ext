<?php

namespace Tensor\Benchmarks\Factory;

use Tensor\Vector;

/**
 * @Groups({"Factory"})
 */
class VectorFillBench
{
    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function fill() : void
    {
        Vector::fill(1.0, 100000);
    }
}
