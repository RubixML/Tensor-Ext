<?php

namespace Tensor\Benchmarks\Arithmetic;

use Tensor\Matrix;
use Tensor\ColumnVector;

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

    /**
     * @return list<array{size: list<int>}>
     */
    public function sizes() : array
    {
        return [
            ['size' => [1024, 1024]],
            ['size' => [8192, 8192]],
        ];
    }

    public function setUp(array $parameters) : void
    {
        [$m, $n] = $parameters['size'];

        $this->a = Matrix::uniform($m, $n);

        $this->b = ColumnVector::uniform($m);
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
