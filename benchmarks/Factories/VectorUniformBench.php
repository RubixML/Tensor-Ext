<?php

namespace Tensor\Benchmarks\Factories;

use Tensor\Vector;
use Generator;

/**
 * @Groups({"Factories"})
 */
class VectorUniformBench
{
    /**
     * @return list<array{size: int}>
     */
    public function sizes() : Generator
    {
        yield 'small' => ['size' => 1024 * 1024];
        yield 'medium' => ['size' => 4096 * 4096];
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
