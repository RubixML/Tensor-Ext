<?php

namespace Tensor\Benchmarks\Factories;

use Tensor\Matrix;
use Generator;

/**
 * @Groups({"Factories"})
 */
class MatrixRandBench
{
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
     * @param array $params
     */
    public function rand(array $params) : void
    {
        Matrix::rand(...$params['size']);
    }
}
