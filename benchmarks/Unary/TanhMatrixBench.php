<?php

namespace Tensor\Benchmarks\Unary;

use Tensor\Matrix;
use Generator;

/**
 * Tanh as a fused kernel, against the exp/exp/divide composition it replaces.
 * The composition needs two exponentials where the kernel needs one, so this is
 * where fusing a transcendental pays off most directly.
 *
 * @Groups({"Functions"})
 * @BeforeMethods({"setUp"})
 */
class TanhMatrixBench
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
        $this->a = Matrix::uniform(...$params['size'])->multiply(8.0)->subtract(4.0);
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
        $this->a->tanh();
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
        $em = $this->a->multiply(-1.0)->exp();

        $e->subtract($em)->divide($e->add($em));
    }
}
