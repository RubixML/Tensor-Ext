<?php

namespace Tensor\Benchmarks\LinearAlgebra;

use Tensor\Vector;

/**
 * @Groups({"LinearAlgebra"})
 * @BeforeMethods({"setUp"})
 */
class VectorDotBench
{
    /**
     * @var Vector
     */
    protected $a;

    /**
     * @var Vector
     */
    protected $b;

    public function setUp(array $params) : void
    {
        $this->a = Vector::uniform($params['size']);

        $this->b = Vector::uniform($params['size']);
    }

    /**
     * @return list<array{size: list<int>}>
     */
    public function sizes() : array
    {
        return [
            ['size' => 1024 * 1024],
            ['size' => 4096 * 4096],
            ['size' => 8192 * 8192],
        ];
    }

    /**
     * @Subject
     * @Iterations(5)
     * @ParamProviders({"sizes"})
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function dot() : void
    {
        $this->a->dot($this->b);
    }
}
