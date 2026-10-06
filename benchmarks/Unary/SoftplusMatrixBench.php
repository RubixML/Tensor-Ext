<?php

namespace Tensor\Benchmarks\Unary;

use Tensor\Matrix;
use Generator;

/**
 * Softplus as a fused kernel, against the exp/add/log composition it replaces.
 *
 * @Groups({"Functions"})
 * @BeforeMethods({"setUp"})
 */
class SoftplusMatrixBench
{
    /**
     * @var Matrix
     */
    protected $a;

    /**
     * @param array $params
     */
    public function setUp(array $params) : void
    {
        $this->a = Matrix::uniform(...$params['size']);
    }

    /**
     * @return list<array{size: list<int>}>
     */
    public function sizes() : Generator
    {
        yield 'small' => ['size' => [1024, 1024]];
        yield 'medium' => ['size' => [4096, 4096]];
        yield 'large' => ['size' => [8192, 8192]];
    }

    /**
     * @Subject
     * @Iterations(5)
     * @ParamProviders({"sizes"})
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function benchFused() : void
    {
        $this->a->softplus();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @ParamProviders({"sizes"})
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function benchComposed() : void
    {
        $e = $this->a->exp();

        $e->add(1.0)->log();
    }
}
