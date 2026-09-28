<?php

namespace Tensor\Benchmarks\Buffer;

use Tensor\Buffer;
use Tensor\TensorBuffer;

/**
 * @Groups({"Buffer"})
 * @BeforeMethods({"setUp"})
 */
class TensorBufferBench
{
    /**
     * @var TensorBuffer
     */
    protected $buffer;

    /**
     * @var int
     */
    protected $offset;

    /**
     * @var int
     */
    protected $length;

    /**
     * @var int
     */
    protected $stride;

    /**
     * @var int<1,max>
     */
    protected $chunkLength;

    /**
     * @var int
     */
    protected $times;

    /**
     * @var TensorBuffer
     */
    protected $concatOther;

    public function setUp() : void
    {
        $a = [];

        for ($i = 0; $i < 10000; ++$i) {
            $a[] = ($i * 31) % 10000;
        }

        $this->buffer = new TensorBuffer(Buffer::fromArray($a));
        $this->concatOther = new TensorBuffer(Buffer::fromArray($a));

        $this->offset = 500;
        $this->length = 2500;
        $this->stride = 3;
        $this->chunkLength = 100;
        $this->times = 10;
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function sort() : void
    {
        $this->buffer->sort();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function slice() : void
    {
        $this->buffer->slice($this->offset, $this->length);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function sliceStrided() : void
    {
        $this->buffer->sliceStrided($this->offset, $this->length, $this->stride);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function concat() : void
    {
        $this->buffer->concat([$this->concatOther]);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function split() : void
    {
        $this->buffer->split($this->chunkLength);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function repeat() : void
    {
        $this->buffer->repeat($this->times);
    }
}
