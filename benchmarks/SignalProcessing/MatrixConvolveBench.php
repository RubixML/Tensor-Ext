<?php

namespace Tensor\Benchmarks\SignalProcessing;

use Tensor\Matrix;

/**
 * @Groups({"Signal Processing"})
 * @BeforeMethods({"setUp"})
 */
class MatrixConvolveBench
{
    /**
     * @var Matrix
     */
    protected $a;

    /**
     * @var Matrix
     */
    protected $kernel;

    public function setUp() : void
    {
        $this->a = Matrix::uniform(500, 500);

        $this->kernel = Matrix::uniform(10, 10);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function convolve() : void
    {
        $this->a->convolve($this->kernel);
    }

    /**
     * A stride above 1 leaves consecutive outputs reading samples a stride
     * apart, so the tiled path is deliberately not taken for it and every
     * output is accumulated on its own. Benchmarked separately because it
     * exercises a different kernel, and because a change that only speeds the
     * tiled path up should not be reported as speeding this one up.
     *
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function convolveStrideTwo() : void
    {
        $this->a->convolve($this->kernel, 2);
    }
}
