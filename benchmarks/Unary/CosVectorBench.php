<?php

namespace Tensor\Benchmarks\Unary;

use Tensor\Vector;

/**
 * @Groups({"Unary"})
 * @BeforeMethods({"setUp"})
 */
class CosVectorBench
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
    public function cosine() : void
    {
        $this->a->cos();
    }
}
