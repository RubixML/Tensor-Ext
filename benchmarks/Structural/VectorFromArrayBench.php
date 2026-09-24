<?php

namespace Tensor\Benchmarks\Structural;

use Tensor\Vector;

/**
 * @Groups({"Structural"})
 * @BeforeMethods({"setUp"})
 */
class VectorFromArrayBench
{
    /**
     * @var list<float>
     */
    protected $a;

    public function setUp() : void
    {
        $a = [];

        for ($i = 0; $i < 4000000; ++$i) {
            $a[] = $i;
        }

        $this->a = $a;
    }

    /**
     * @Subject
     * @Iterations(10)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function fromArray() : void
    {
        Vector::fromArray($this->a);
    }

    /**
     * @Subject
     * @Iterations(10)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function fromArrayNoValidate() : void
    {
        Vector::fromArray($this->a, false);
    }
}
