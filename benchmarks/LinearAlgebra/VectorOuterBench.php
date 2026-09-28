<?php

namespace Tensor\Benchmarks\LinearAlgebra;

use Tensor\Vector;
use Generator;

/**
 * @Groups({"LinearAlgebra"})
 * @BeforeMethods({"setUp"})
 */
class VectorOuterBench
{
    /**
     * @var Vector
     */
    protected $a;

    /**
     * @var Vector
     */
    protected $b;

    public function setUp(array $params) : void
    {
        $this->a = Vector::uniform($params['size']);

        $this->b = Vector::uniform($params['size']);
    }

    /**
     * @return list<array{size: list<int>}>
     */
    public function sizes() : Generator
    {
        yield 'small' => ['size' => 1024];
        yield 'medium' => ['size' => 4096];
        yield 'large' => ['size' => 8192];
    }

    /**
     * @Subject
     * @Iterations(5)
     * @ParamProviders({"sizes"})
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function outer() : void
    {
        $this->a->outer($this->b);
    }
}
