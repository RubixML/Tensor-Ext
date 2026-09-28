<?php

namespace Tensor\Benchmarks\LinearAlgebra;

use Tensor\Matrix;

/**
 * @Groups({"LinearAlgebra"})
 */
class CovarianceBench
{
    /**
     * @var Matrix
     */
    protected $a;

    /**
     * @var Matrix
     */
    protected $wide;

    /**
     * @var Matrix
     */
    protected $tall;

    public function setUp() : void
    {
        $this->a = Matrix::uniform(500, 500);
    }

    /**
     * @BeforeMethods({"setUpWide"})
     *
     * The wide case reduces n samples per row, so computing the mean costs more
     * relative to the m * m product than it does in the square case.
     *
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function covarianceWide() : void
    {
        $this->wide->covariance();
    }

    /**
     * @BeforeMethods({"setUpTall"})
     *
     * The tall case inverts that balance, leaving the m * m product in charge.
     *
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function covarianceTall() : void
    {
        $this->tall->covariance();
    }

    /**
     * @BeforeMethods({"setUp"})
     *
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("seconds", precision=3)
     */
    public function covariance() : void
    {
        $this->a->covariance();
    }

    public function setUpWide() : void
    {
        $this->wide = Matrix::uniform(500, 2000);
    }

    public function setUpTall() : void
    {
        $this->tall = Matrix::uniform(2000, 500);
    }
}
