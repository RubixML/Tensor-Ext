<?php

namespace Tensor\Benchmarks\LinearAlgebra;

use Tensor\Matrix;
use Generator;

/**
 * @Groups({"LinearAlgebra"})
 * @BeforeMethods({"setUp"})
 */
class CholeskyBench
{
    /**
     * @var Matrix
     */
    protected $a;

    public function setUp(array $params) : void
    {
        $a = Matrix::rand(...$params['size']);

        $this->a = $a->transpose()->matmul($a);
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
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function cholesky() : void
    {
        $this->a->cholesky();
    }
}
