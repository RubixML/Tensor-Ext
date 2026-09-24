<?php

namespace Tensor\Benchmarks\Structural;

use Tensor\Matrix;

/**
 * @Groups({"Structural"})
 * @BeforeMethods({"setUp"})
 */
class MatrixFromArrayBench
{
    /**
     * @var array[]
     */
    protected $a;

    public function setUp() : void
    {
        $a = [];

        for ($i = 0; $i < 2000; ++$i) {
            $row = [];

            for ($j = 0; $j < 2000; ++$j) {
                $row[] = ($i * $j) % 2000;
            }

            $a[] = $row;
        }

        $this->a = $a;
    }

    /**
     * @Subject
     * @Iterations(10)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function fromArray() : void
    {
        Matrix::fromArray($this->a);
    }

    /**
     * @Subject
     * @Iterations(10)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function fromArrayNoValidate() : void
    {
        Matrix::fromArray($this->a, false);
    }
}
