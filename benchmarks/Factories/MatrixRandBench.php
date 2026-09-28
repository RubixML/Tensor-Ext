<?php

namespace Tensor\Benchmarks\Factories;

use Tensor\Matrix;

/**
 * @Groups({"Factories"})
 */
class MatrixRandBench
{
    /**
     * @return list<array{size: list<int>}>
     */
    public function sizes() : array
    {
        return [
            ['size' => [1024, 1024]],
            ['size' => [4096, 4096]],
            ['size' => [8192, 8192]],
        ];
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
