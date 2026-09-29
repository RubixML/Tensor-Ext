<?php

namespace Tensor\Benchmarks\Special;

use Tensor\Vector;
use Generator;

/**
 * Softmax as a fused kernel, against the maximum/subtract/exp/sum/divide
 * composition it replaces.
 *
 * @Groups({"Functions"})
 * @BeforeMethods({"setUp"})
 */
class VectorSoftmaxBench
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
        $this->a = Vector::uniform($params['size'])->multiply(20.0);
    }

    /**
     * @return list<array{size: int}>
     */
    public function sizes() : Generator
    {
        yield 'small' => ['size' => 1024];
        yield 'medium' => ['size' => 65536];
        yield 'large' => ['size' => 1048576];
    }

    /**
     * @Subject
     * @Iterations(5)
     * @ParamProviders({"sizes"})
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function benchFused() : void
    {
        $this->a->softmax();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @ParamProviders({"sizes"})
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function benchComposed() : void
    {
        $e = $this->a->subtract($this->a->max())->exp();

        $e->divide($e->sum());
    }
}
