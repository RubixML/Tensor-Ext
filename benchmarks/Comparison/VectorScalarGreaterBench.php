<?php

namespace Tensor\Benchmarks\Comparison;

use Tensor\Vector;

/**
 * @Groups({"Comparison", "Scalar"})
 * @BeforeMethods({"setUp"})
 */
class VectorScalarGreaterBench
{
    /**
     * @var Vector
     */
    protected $a;

    public function setUp() : void
    {
        $this->a = Vector::uniform(1024 * 1024);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function greaterScalar() : void
    {
        $this->a->greaterScalar(0.5);
    }
}
