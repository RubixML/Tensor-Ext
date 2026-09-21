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

        $this->offset = 2500;
        $this->length = 5000;
        $this->stride = 2;
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

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function sum() : void
    {
        $this->buffer->sum();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function sumNative() : int|float
    {
        return array_sum($this->buffer->toArray());
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function product() : void
    {
        $this->buffer->product();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function productNative() : int|float
    {
        return array_product($this->buffer->toArray());
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function min() : void
    {
        $this->buffer->min();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function minNative() : mixed
    {
        return min($this->buffer->toArray());
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function max() : void
    {
        $this->buffer->max();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function maxNative() : mixed
    {
        return max($this->buffer->toArray());
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function argmin() : void
    {
        $this->buffer->argmin();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function argmax() : void
    {
        $this->buffer->argmax();
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
    public function sliceStridedNative() : void
    {
        $a = $this->buffer->toArray();

        $b = [];

        for ($i = $this->offset; $i < $this->offset + $this->length * $this->stride; $i += $this->stride) {
            $b[] = $a[$i];
        }

        new TensorBuffer(Buffer::fromArray($b));
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
    public function concatNative() : void
    {
        $a = $this->buffer->toArray();
        $b = $this->concatOther->toArray();

        new TensorBuffer(Buffer::fromArray(array_merge($a, $b)));
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
     * @return array<int, list<mixed>>
     */
    public function splitNative() : array
    {
        return array_chunk($this->buffer->toArray(), $this->chunkLength);
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

    /**
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function repeatNative() : void
    {
        $a = $this->buffer->toArray();

        $b = [];

        for ($i = 0; $i < $this->times; ++$i) {
            $b = array_merge($b, $a);
        }

        new TensorBuffer(Buffer::fromArray($b));
    }
}
