<?php

namespace Tensor\Benchmarks\Comparison;

use Tensor\Matrix;
use Tensor\Vector;

/**
 * @Groups({"Comparison"})
 * @BeforeMethods({"setUp"})
 */
class MatrixVectorGreaterBench
{
    /**
     * @var Matrix
     */
    protected $a;

    /**
     * @var Vector
     */
    protected $b;

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
    public function greater() : void
    {
        $this->a->greater($this->b);
    }
}
