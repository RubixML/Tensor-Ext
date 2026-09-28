<?php

namespace Tensor\Benchmarks\Factories;

use Tensor\Vector;

/**
 * @Groups({"Factories"})
 */
class VectorUniformBench
{
    /**
     * @return list<array{size: int}>
     */
    public function sizes() : array
    {
        return [
            ['size' => 1024 * 1024],
            ['size' => 8192 * 8192],
        ];
    }

    /**
     * @Subject
     * @Iterations(5)
     * @ParamProviders({"sizes"})
     * @OutputTimeUnit("milliseconds", precision=3)
     * @param array $params
     */
    public function uniform(array $params) : void
    {
        Vector::uniform($params['size']);
    }
}
