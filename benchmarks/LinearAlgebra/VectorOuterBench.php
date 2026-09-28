<?php

namespace Tensor\Benchmarks\LinearAlgebra;

use Tensor\Vector;

/**
 * @Groups({"LinearAlgebra"})
 * @BeforeMethods({"setUp"})
 */
class VectorOuterBench
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
        $size = $params['size'];

        $this->a = Vector::uniform($size);

        $this->b = Vector::uniform($size);
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
    public function outer() : void
    {
        $this->a->outer($this->b);
    }
}
