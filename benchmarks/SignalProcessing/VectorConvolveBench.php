<?php

namespace Tensor\Benchmarks\SignalProcessing;

use Tensor\Vector;

/**
 * @Groups({"Signal Processing"})
 * @BeforeMethods({"setUp"})
 */
class VectorConvolveBench
{
    /**
     * @var Vector
     */
    protected $a;

    /**
     * @var Vector
     */
    protected $kernel;

    public function setUp() : void
    {
        $this->a = Vector::uniform(250000);

        $this->kernel = Vector::uniform(100);
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

    /**
     * Padding the input by half the kernel restores the "same" output length,
     * so this is the shape the old default produced. Benchmarked separately
     * because every output now also carries the padding offset, and a change
     * that only speeds the unpadded case up should not be reported as speeding
     * this one up.
     *
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function convolvePadded() : void
    {
        $this->a->convolve($this->kernel, 1, 50);
    }
}
