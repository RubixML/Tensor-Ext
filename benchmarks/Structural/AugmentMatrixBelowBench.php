<?php

namespace Tensor\Benchmarks\Structural;

use Tensor\Matrix;
use Generator;

/**
 * @Groups({"Structural"})
 * @BeforeMethods({"setUp"})
 */
class AugmentMatrixBelowBench
{
    /**
     * @var Matrix
     */
    protected $a;

    /**
     * @var Matrix
     */
    protected $b;

    /**
     * @param array $params
     */
    public function setUp(array $params) : void
    {
        $this->a = Matrix::uniform(...$params['size']);

        $this->b = Matrix::uniform(...$params['size']);
    }

    /**
     * @return list<array{size: list<int>}>
     */
    public function sizes() : Generator
    {
        yield 'small' => ['size' => [1024, 1024]];
        yield 'medium' => ['size' => [4096, 4096]];
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function augmentBelow() : void
    {
        $this->a->augmentBelow($this->b);
    }
}
