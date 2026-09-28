<?php

namespace Tensor\Benchmarks\Comparison;

use Tensor\Matrix;

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
     */
    public function greaterEqual() : void
    {
        $this->a->greaterEqual($this->b);
    }
}
