<?php

namespace Tensor\Benchmarks\LinearAlgebra;

use Tensor\Vector;

/**
 * @Groups({"LinearAlgebra"})
 * @BeforeMethods({"setUp"})
 */
class VectorOuterProductBench
{
    /**
     * @var Vector
     */
    protected $a;

    /**
     * @var Vector
     */
    protected $b;

    /**
     * @var Vector
     */
    protected $c;

    /**
     * @var Vector
     */
    protected $d;

    public function setUp() : void
    {
        $this->a = Vector::uniform(500);

        $this->b = Vector::uniform(500);

        $this->c = Vector::uniform(2000);

        $this->d = Vector::uniform(2000);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function outer() : void
    {
        $this->a->outer($this->b);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function outerLarge() : void
    {
        $this->c->outer($this->d);
    }
}
