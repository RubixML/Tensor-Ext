<?php

namespace Tensor\Tests;

use Tensor\Buffer;
use Tensor\TensorBuffer;
use Tensor\Exceptions\InvalidArgumentException;
use PHPUnit\Framework\TestCase;

/**
 * @covers \Tensor\TensorBuffer
 */
class TensorBufferTest extends TestCase
{
    /**
     * The maximum error tolerated due to varying numerical precision.
     *
     * @var float
     */
    protected const MAX_DELTA = 1e-8;

    /**
     * @test
     */
    public function build() : void
    {
        $buffer = Buffer::fromArray([1, 2, 3]);

        $decorator = new TensorBuffer($buffer);

        $this->assertInstanceOf(TensorBuffer::class, $decorator);
        $this->assertSame($buffer, $decorator->asBuffer());
        $this->assertSame(3, $decorator->count());
        $this->assertSame(Buffer::TYPE_DOUBLE, $decorator->type());
    }

    /**
     * @test
     */
    public function toArray() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1, 2, 3]));

        $expected = [1.0, 2.0, 3.0];

        $this->assertEqualsWithDelta($expected, $decorator->toArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function longType() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1, 2, 3], Buffer::TYPE_LONG));

        $this->assertSame(Buffer::TYPE_LONG, $decorator->type());
        $this->assertSame([1, 2, 3], $decorator->toArray());
    }

    /**
     * @test
     */
    public function get() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([3.0, 1.0, 2.0]));

        $this->assertSame(1.0, $decorator->get(1));
    }

    /**
     * @test
     */
    public function set() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([3.0, 1.0, 2.0]));

        $decorator->set(0, 4.0);

        $this->assertEqualsWithDelta([4.0, 1.0, 2.0], $decorator->toArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function sortAscending() : void
    {
        $buffer = Buffer::fromArray([3.0, 1.0, 2.0]);

        $decorator = new TensorBuffer($buffer);

        $result = $decorator->sort();

        $this->assertNull($result);
        $this->assertSame($buffer, $decorator->asBuffer());
        $this->assertEqualsWithDelta([1.0, 2.0, 3.0], $decorator->toArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function sortDescending() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([3.0, 1.0, 2.0]));

        $decorator->sort(false);

        $this->assertEqualsWithDelta([3.0, 2.0, 1.0], $decorator->toArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function sortLongBuffer() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([3, 1, 2], Buffer::TYPE_LONG));

        $decorator->sort();

        $this->assertSame(Buffer::TYPE_LONG, $decorator->type());
        $this->assertSame([1, 2, 3], $decorator->toArray());
    }

    /**
     * @test
     */
    public function slice() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0]));

        $slice = $decorator->slice(1, 2);

        $this->assertInstanceOf(TensorBuffer::class, $slice);
        $this->assertNotSame($decorator->asBuffer(), $slice->asBuffer());
        $this->assertEqualsWithDelta([2.0, 3.0], $slice->toArray(), self::MAX_DELTA);
        $this->assertEqualsWithDelta([1.0, 2.0, 3.0], $decorator->toArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function sliceIsACopy() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0]));

        $slice = $decorator->slice(0, 3);

        $slice->set(0, 100.0);

        $this->assertEqualsWithDelta([1.0, 2.0, 3.0], $decorator->toArray(), self::MAX_DELTA);
        $this->assertEqualsWithDelta([100.0, 2.0, 3.0], $slice->toArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function sliceZeroLength() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0]));

        $slice = $decorator->slice(2, 0);

        $this->assertSame(0, $slice->count());
    }

    /**
     * @test
     */
    public function slicePreservesType() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1, 2, 3], Buffer::TYPE_LONG));

        $slice = $decorator->slice(1, 1);

        $this->assertSame(Buffer::TYPE_LONG, $slice->type());
        $this->assertSame([2], $slice->toArray());
    }

    /**
     * @test
     */
    public function sliceOutOfRange() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0]));

        $this->expectException(InvalidArgumentException::class);

        $decorator->slice(2, 2);
    }

    /**
     * @test
     */
    public function sliceNegativeOffset() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0]));

        $this->expectException(InvalidArgumentException::class);

        $decorator->slice(-1, 1);
    }

    /**
     * @test
     */
    public function sliceNegativeLength() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0]));

        $this->expectException(InvalidArgumentException::class);

        $decorator->slice(0, -1);
    }
}
