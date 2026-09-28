<?php

namespace Tensor\Benchmarks\Structural;

use Tensor\Vector;

/**
 * @Groups({"Structural"})
 * @BeforeMethods({"setUp"})
 */
class ReshapeVectorBench
{
    /**
     * @var Vector
     */
    protected $a;

    /**
     * @param array $params
     */
    public function setUp(array $params) : void
    {
        $this->a = Vector::uniform($params['size']);
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
    public function reshape() : void
    {
        $this->a->reshape(500, 500);
    }
}
