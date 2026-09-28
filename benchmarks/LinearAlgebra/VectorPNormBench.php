<?php

namespace Tensor\Benchmarks\LinearAlgebra;

use Tensor\Vector;

/**
 * @Groups({"LinearAlgebra"})
 * @BeforeMethods({"setUp"})
 */
class VectorPNormBench
{
    /**
     * @var Vector
     */
    protected $a;

    /**
     * @ParamProviders({"sizes"})
     * @param array $params
     */
    public function setUp(array $params) : void
    {
        $this->a = Vector::uniform(1024 * 1024);
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
