<?php

namespace Tensor\Benchmarks\Comparison;

use Tensor\Matrix;
use Generator;

/**
 * @Groups({"Comparison"})
 * @BeforeMethods({"setUp"})
 */
class MatrixMatrixGreaterEqualBench
{
    /**
     * @var Matrix
     */
    protected $a;

    /**
     * @var Matrix
     */
    protected $b;

    public function setUp(array $params) : void
    {
        [$m, $n] = $params['size'];

        $this->a = Matrix::uniform($m, $n);

        $this->b = Matrix::uniform($m, $n);
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
    public function greaterEqual() : void
    {
        $this->a->greaterEqual($this->b);
    }
}
