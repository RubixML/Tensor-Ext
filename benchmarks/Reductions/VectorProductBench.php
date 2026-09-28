<?php

namespace Tensor\Benchmarks\Unary;

use Tensor\Vector;
use Generator;

/**
 * @Groups({"Functions"})
 * @BeforeMethods({"setUp"})
 */
class ProductVectorBench
{
    /**
     * @var Vector
     */
    protected $a;

    /**
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
    public function product() : void
    {
        $this->a->product();
    }
}
