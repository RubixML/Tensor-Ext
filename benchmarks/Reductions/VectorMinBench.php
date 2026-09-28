<?php

namespace Tensor\Benchmarks\Arithmetic;

use Tensor\Vector;
use Generator;

/**
 * @Groups({"Reductions"})
 * @BeforeMethods({"setUp"})
 */
class VectorMinBench
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
        $this->a = Vector::uniform($params['size']);
    }

    /**
     * @return list<array{size: int}>
     */
    public function sizes() : Generator
    {
        yield 'small' => ['size' => 1024 * 1024];
    }

    /**
     * @Subject
     * @Iterations(5)
     * @ParamProviders({"sizes"})
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function min() : void
    {
        $this->a->min();
    }
}
