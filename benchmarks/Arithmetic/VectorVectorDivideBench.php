<?php

namespace Tensor\Benchmarks\Arithmetic;

use Tensor\Vector;

/**
 * @Groups({"Arithmetic"})
 * @BeforeMethods({"setUp"})
 */
class VectorVectorDivideBench
{
    /**
     * @var Vector
     */
    protected $a;

    /**
     * @var Vector
     */
    protected $b;

    public function setUp() : void
    {
        $this->a = Vector::uniform(1024 * 1024);

        $this->b = Vector::uniform(1024 * 1024);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function divide() : void
    {
        $this->a->divide($this->b);
    }
}
