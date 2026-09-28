<?php

namespace Tensor\Benchmarks\Arithmetic;

use Tensor\Matrix;
use Tensor\ColumnVector;
use Generator;

/**
 * @Groups({"Arithmetic"})
 * @BeforeMethods({"setUp"})
 */
class MatrixColumnVectorDivideBench
{
    /**
     * @var Matrix
     */
    protected $a;

    /**
     * @var ColumnVector
     */
    protected $b;

    public function setUp(array $params) : void
    {
        [$m, $n] = $params['size'];

        $this->a = Matrix::uniform($m, $n);

        $this->b = ColumnVector::uniform($m);
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
    public function divide() : void
    {
        $this->a->divide($this->b);
    }
}
