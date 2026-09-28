<?php

namespace Tensor\Benchmarks\Arithmetic;

use Tensor\Matrix;
use Tensor\Vector;

/**
 * @Groups({"Arithmetic"})
 * @BeforeMethods({"setUp"})
 */
class MatrixVectorMultiplyBench
{
    /**
     * @var Matrix
     */
    protected $a;

    /**
     * @var Vector
     */
    protected $b;

    /**
     * @ParamProviders({"sizes"})
     * @param array $params
     */
    public function setUp(array $params) : void
    {
        [$m, $n] = $params['size'];

        $this->a = Matrix::uniform($m, $n);

        $this->b = Vector::uniform($m);
    }

    /**
     * @return list<array{size: list<int>}>
     */
    public function sizes() : array
    {
        return [
            ['size' => [1024, 1024]],
            ['size' => [4096, 4096]],
        ];
    }

    /**
     * @Subject
     * @Iterations(5)
     * @ParamProviders({"sizes"})
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function multiply() : void
    {
        $this->a->multiply($this->b);
    }
}
