<?php

namespace Tensor\Benchmarks\Arithmetic;

use Tensor\Vector;
use Generator;

/**
 * @Groups({"Arithmetic"})
 * @BeforeMethods({"setUp"})
 */
class VectorVectorPowBench
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
     * @return list<array{size: int}>
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
    public function pow() : void
    {
        $this->a->pow($this->b);
    }
}
