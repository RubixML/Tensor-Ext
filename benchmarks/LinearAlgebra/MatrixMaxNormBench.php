<?php

namespace Tensor\Benchmarks\LinearAlgebra;

use Tensor\Matrix;
use Generator;

/**
 * @Groups({"LinearAlgebra"})
 * @BeforeMethods({"setUp"})
 */
class MatrixMaxNormBench
{
    /**
     * @var Matrix
     */
    protected $a;

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
    public function max_norm() : void
    {
        $this->a->maxNorm();
    }
}
