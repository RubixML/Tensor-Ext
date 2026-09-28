<?php

namespace Tensor\Benchmarks\Arithmetic;

use Tensor\Matrix;

/**
 * @Groups({"Arithmetic"})
 * @BeforeMethods({"setUp"})
 */
class MatrixScalarMultiplyBench
{
    /**
     * @var Matrix
     */
    protected $a;

    /**
     * @var float
     */
    protected $b = M_E;

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
