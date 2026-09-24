<?php

namespace Tensor\Benchmarks\Statistical;

use Tensor\Vector;

/**
 * @Groups({"Statistical"})
 * @BeforeMethods({"setUp"})
 */
class VectorMedianBench
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
    public function median() : void
    {
        $this->a->median();
    }
}
