<?php

namespace Tensor\Benchmarks\Arithmetic;

use Tensor\Matrix;
use Tensor\Vector;

/**
 * @Groups({"Arithmetic"})
 * @BeforeMethods({"setUp"})
 */
class MatrixVectorAddBench
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

        $this->b = Vector::uniform($m);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @ParamProviders({"sizes"})
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function add() : void
    {
        $this->a->add($this->b);
    }
}
