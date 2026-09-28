<?php

namespace Tensor\Benchmarks\Arithmetic;

use Tensor\Vector;
use Generator;

/**
 * @Groups({"Reductions"})
 * @BeforeMethods({"setUp"})
 */
class VectorArgmaxBench
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
     * @return list<array{size: list<int>}>
     */
    public function sizes() : Generator
    {
        yield 'small' => ['size' => 1024 * 1024];
        yield 'medium' => ['size' => 4096 * 4096];
        yield 'large' => ['size' => 8192 * 8192];
    }

    /**
     * @Subject
     * @Iterations(5)
     * @ParamProviders({"sizes"})
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function argmax() : void
    {
        $this->a->argmax();
    }
}
