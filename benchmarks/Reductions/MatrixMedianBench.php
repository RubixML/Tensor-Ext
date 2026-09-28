<?php

namespace Tensor\Benchmarks\Reductions;

use Tensor\Matrix;
use Generator;

/**
 * @Groups({"Reductions"})
 * @BeforeMethods({"setUp"})
 */
class MatrixMedianBench
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
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function median() : void
    {
        $this->a->median();
    }
}
