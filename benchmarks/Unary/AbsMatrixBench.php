<?php

namespace Tensor\Benchmarks\Unary;

use Tensor\Matrix;
use Generator;

/**
 * @Groups({"Functions"})
 * @BeforeMethods({"setUp"})
 */
class AbsMatrixBench
{
    /**
     * @var Matrix
     */
    protected $a;

    /**
     * @param list<int> $size
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
    public function abs() : void
    {
        $this->a->abs();
    }
}
