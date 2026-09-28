<?php

namespace Tensor\Benchmarks\Factories;

use Tensor\Vector;

/**
 * @Groups({"Factories"})
 */
class VectorGaussianBench
{
    /**
     * @return list<array{size: int}>
     */
    public function sizes() : array
    {
        return [
            ['size' => 1024 * 1024],
            ['size' => 4096 * 4096],
        ];
    }

    /**
     * @Subject
     * @Iterations(5)
     * @ParamProviders({"sizes"})
     * @OutputTimeUnit("milliseconds", precision=3)
     * @param array $params
     */
    public function gaussian(array $params) : void
    {
        Vector::gaussian($params['size']);
    }
}
