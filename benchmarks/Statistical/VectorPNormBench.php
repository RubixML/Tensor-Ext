<?php

namespace Tensor\Benchmarks\Statistical;

use Tensor\Vector;

/**
 * @Groups({"Statistical"})
 * @BeforeMethods({"setUp"})
 */
class VectorPNormBench
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
    public function p2() : void
    {
        $this->a->pNorm(2.0);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function p3() : void
    {
        $this->a->pNorm(3.0);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function p10() : void
    {
        $this->a->pNorm(10.0);
    }
}
