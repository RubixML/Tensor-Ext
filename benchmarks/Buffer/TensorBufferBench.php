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

    public function setUp() : void
    {
        $a = [];

        for ($i = 0; $i < 10000; ++$i) {
            $a[] = ($i * 31) % 10000;
        }

        $this->buffer = new TensorBuffer(Buffer::fromArray($a));

        $this->offset = 2500;
        $this->length = 5000;
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
    public function sortNative() : void
    {
        $a = $this->buffer->toArray();

        sort($a);

        $this->buffer = new TensorBuffer(Buffer::fromArray($a));
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
    public function sliceNative() : void
    {
        $a = $this->buffer->toArray();

        $a = array_slice($a, $this->offset, $this->length);

        new TensorBuffer(Buffer::fromArray($a));
    }
}
