<?php

namespace Tensor\Benchmarks\Statistical;

use Tensor\Vector;

/**
 * @Groups({"Statistical"})
 * @BeforeMethods({"setUp"})
 */
class VectorQuantileBench
{
    /**
     * @var Vector
     */
    protected $a;

    public function setUp() : void
    {
        $this->a = Vector::uniform(100000);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function quantile() : void
    {
        $this->a->quantile(0.5);
    }
}
