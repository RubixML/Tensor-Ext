<?php

namespace Tensor\Benchmarks\Comparison;

use Tensor\Vector;
use Generator;

/**
 * @Groups({"Comparison", "Scalar"})
 * @BeforeMethods({"setUp"})
 */
class VectorScalarGreaterBench
{
    /**
     * @var Vector
     */
    protected $a;

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
    }

    /**
     * @Subject
     * @Iterations(5)
     * @ParamProviders({"sizes"})
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function greaterScalar() : void
    {
        $this->a->greaterScalar(0.5);
    }
}
