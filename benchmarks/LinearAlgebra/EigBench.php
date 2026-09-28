<?php

namespace Tensor\Benchmarks\LinearAlgebra;

use Tensor\Matrix;

/**
 * @Groups({"LinearAlgebra"})
 */
class EigBench
{
    /**
     * @var Matrix
     */
    protected $a;

    public function setUpGeneral() : void
    {
        $this->a = Matrix::uniform(512, 1536);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @BeforeMethods({"setUpGeneral"})
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function eigGeneral() : void
    {
        $this->a->eig(false);
    }

    public function setUpSymmetric() : void
    {
        $this->a = Matrix::uniform(1024, 1024);

        $this->a = $this->a->matmul($this->a);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @BeforeMethods({"setUpSymmetric"})
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function eigSymmetric() : void
    {
        $this->a->eig(true);
    }
}
