<?php

namespace Tensor\Benchmarks\Arithmetic;

use Tensor\ColumnVector;
use Tensor\Matrix;
use Generator;

/**
 * @Groups({"Arithmetic"})
 * @BeforeMethods({"setUp"})
 */
class ColumnVectorMatrixAddBench
{
    /**
     * @var ColumnVector
     */
    protected $a;

    /**
     * @var Matrix
     */
    protected $b;

    public function setUp(array $params) : void
    {
        [$m, $n] = $params['size'];

        $this->a = ColumnVector::uniform($m);

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
    public function add() : void
    {
        $this->a->add($this->b);
    }
}
