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

        $decorator->sort();

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

    /**
     * @test
     */
    public function map() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0]));

        $mapped = $decorator->map(static function ($value) : float {
            return $value ** 2;
        });

        $this->assertInstanceOf(TensorBuffer::class, $mapped);
        $this->assertNotSame($decorator->asBuffer(), $mapped->asBuffer());
        $this->assertEqualsWithDelta([1.0, 4.0, 9.0], $mapped->toArray(), self::MAX_DELTA);
        $this->assertEqualsWithDelta([1.0, 2.0, 3.0], $decorator->toArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function sum() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([3.0, 1.0, 2.0]));

        $this->assertEqualsWithDelta(6.0, $decorator->sum(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function sumLongBuffer() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([3, 1, 2], Buffer::TYPE_LONG));

        $this->assertEqualsWithDelta(6.0, $decorator->sum(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function sumEmptyBuffer() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([], Buffer::TYPE_DOUBLE));

        $this->assertEqualsWithDelta(0.0, $decorator->sum(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function product() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([3.0, 1.0, 2.0]));

        $this->assertEqualsWithDelta(6.0, $decorator->product(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function productLongBuffer() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([3, 1, 2], Buffer::TYPE_LONG));

        $this->assertEqualsWithDelta(6.0, $decorator->product(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function productEmptyBuffer() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([], Buffer::TYPE_DOUBLE));

        $this->assertEqualsWithDelta(1.0, $decorator->product(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function min() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([3.0, 1.0, 2.0]));

        $this->assertEqualsWithDelta(1.0, $decorator->min(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function minLongBuffer() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([3, 1, 2], Buffer::TYPE_LONG));

        $this->assertEqualsWithDelta(1.0, $decorator->min(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function minEmptyBuffer() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([], Buffer::TYPE_DOUBLE));

        $this->expectException(\InvalidArgumentException::class);

        $decorator->min();
    }

    /**
     * @test
     */
    public function max() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([3.0, 1.0, 2.0]));

        $this->assertEqualsWithDelta(3.0, $decorator->max(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function maxLongBuffer() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([3, 1, 2], Buffer::TYPE_LONG));

        $this->assertEqualsWithDelta(3.0, $decorator->max(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function maxEmptyBuffer() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([], Buffer::TYPE_DOUBLE));

        $this->expectException(\InvalidArgumentException::class);

        $decorator->max();
    }

    /**
     * @test
     */
    public function argmin() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([3.0, 1.0, 2.0]));

        $this->assertSame(1, $decorator->argmin());
    }

    /**
     * @test
     */
    public function argminLongBuffer() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([3, 1, 2], Buffer::TYPE_LONG));

        $this->assertSame(1, $decorator->argmin());
    }

    /**
     * @test
     */
    public function argminTie() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 1.0, 2.0]));

        $this->assertSame(0, $decorator->argmin());
    }

    /**
     * @test
     */
    public function argminEmptyBuffer() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([], Buffer::TYPE_DOUBLE));

        $this->expectException(\InvalidArgumentException::class);

        $decorator->argmin();
    }

    /**
     * @test
     */
    public function argmax() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([3.0, 1.0, 2.0]));

        $this->assertSame(0, $decorator->argmax());
    }

    /**
     * @test
     */
    public function argmaxLongBuffer() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([3, 1, 2], Buffer::TYPE_LONG));

        $this->assertSame(0, $decorator->argmax());
    }

    /**
     * @test
     */
    public function argmaxEmptyBuffer() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([], Buffer::TYPE_DOUBLE));

        $this->expectException(\InvalidArgumentException::class);

        $decorator->argmax();
    }

    /**
     * @test
     */
    public function sliceStrided() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0, 4.0, 5.0]));

        $slice = $decorator->sliceStrided(1, 2, 2);

        $this->assertInstanceOf(TensorBuffer::class, $slice);
        $this->assertNotSame($decorator->asBuffer(), $slice->asBuffer());
        $this->assertEqualsWithDelta([2.0, 4.0], $slice->toArray(), self::MAX_DELTA);
        $this->assertEqualsWithDelta([1.0, 2.0, 3.0, 4.0, 5.0], $decorator->toArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function sliceStridedPreservesType() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1, 2, 3, 4, 5], Buffer::TYPE_LONG));

        $slice = $decorator->sliceStrided(0, 3, 2);

        $this->assertSame(Buffer::TYPE_LONG, $slice->type());
        $this->assertSame([1, 3, 5], $slice->toArray());
    }

    /**
     * @test
     */
    public function sliceStridedZeroLength() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0, 4.0, 5.0]));

        $slice = $decorator->sliceStrided(4, 0, 2);

        $this->assertSame(0, $slice->count());
    }

    /**
     * @test
     */
    public function sliceStridedOutOfRange() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0, 4.0, 5.0]));

        $this->expectException(InvalidArgumentException::class);

        $decorator->sliceStrided(4, 2, 2);
    }

    /**
     * @test
     */
    public function sliceStridedInvalidStride() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0, 4.0, 5.0]));

        $this->expectException(InvalidArgumentException::class);

        $decorator->sliceStrided(0, 2, 0);
    }

    /**
     * @test
     */
    public function sliceStridedRejectsOverflowingRange() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0, 4.0, 5.0]));

        $this->expectException(InvalidArgumentException::class);

        $decorator->sliceStrided(0, 4097, 4503599627370496);
    }

    /**
     * @test
     */
    public function sliceStridedRejectsHugeStride() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0, 4.0, 5.0]));

        $this->expectException(InvalidArgumentException::class);

        $decorator->sliceStrided(0, 10, 4611686018427387904);
    }

    /**
     * @test
     */
    public function sliceStridedSingleElementHugeStride() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0, 4.0, 5.0]));

        $slice = $decorator->sliceStrided(4, 1, PHP_INT_MAX);

        $this->assertSame([5.0], $slice->toArray());
    }

    /**
     * @test
     */
    public function concat() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0]));
        $other = new TensorBuffer(Buffer::fromArray([3.0, 4.0]));

        $concat = $decorator->concat([$other]);

        $this->assertInstanceOf(TensorBuffer::class, $concat);
        $this->assertEqualsWithDelta([1.0, 2.0, 3.0, 4.0], $concat->toArray(), self::MAX_DELTA);
        $this->assertEqualsWithDelta([1.0, 2.0], $decorator->toArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function concatMany() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0]));

        $concat = $decorator->concat([
            new TensorBuffer(Buffer::fromArray([2.0])),
            new TensorBuffer(Buffer::fromArray([3.0])),
        ]);

        $this->assertEqualsWithDelta([1.0, 2.0, 3.0], $concat->toArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function concatPreservesType() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1, 2], Buffer::TYPE_LONG));
        $other = new TensorBuffer(Buffer::fromArray([3, 4], Buffer::TYPE_LONG));

        $concat = $decorator->concat([$other]);

        $this->assertSame(Buffer::TYPE_LONG, $concat->type());
        $this->assertSame([1, 2, 3, 4], $concat->toArray());
    }

    /**
     * @test
     */
    public function concatEmptyList() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0]));

        $concat = $decorator->concat([]);

        $this->assertEqualsWithDelta([1.0, 2.0], $concat->toArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function split() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0, 4.0, 5.0]));

        $chunks = $decorator->split(2);

        $this->assertCount(3, $chunks);

        foreach ($chunks as $chunk) {
            $this->assertInstanceOf(TensorBuffer::class, $chunk);
        }

        $this->assertEqualsWithDelta([1.0, 2.0], $chunks[0]->toArray(), self::MAX_DELTA);
        $this->assertEqualsWithDelta([3.0, 4.0], $chunks[1]->toArray(), self::MAX_DELTA);
        $this->assertEqualsWithDelta([5.0], $chunks[2]->toArray(), self::MAX_DELTA);
        $this->assertEqualsWithDelta([1.0, 2.0, 3.0, 4.0, 5.0], $decorator->toArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function splitPreservesType() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1, 2, 3], Buffer::TYPE_LONG));

        $chunks = $decorator->split(2);

        $this->assertSame(Buffer::TYPE_LONG, $chunks[0]->type());
        $this->assertSame([1, 2], $chunks[0]->toArray());
        $this->assertSame([3], $chunks[1]->toArray());
    }

    /**
     * @test
     */
    public function splitInvalidChunkLength() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0]));

        $this->expectException(InvalidArgumentException::class);

        $decorator->split(0);
    }

    /**
     * @test
     */
    public function repeat() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0]));

        $repeated = $decorator->repeat(3);

        $this->assertInstanceOf(TensorBuffer::class, $repeated);
        $this->assertEqualsWithDelta([1.0, 2.0, 1.0, 2.0, 1.0, 2.0], $repeated->toArray(), self::MAX_DELTA);
        $this->assertEqualsWithDelta([1.0, 2.0], $decorator->toArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function repeatPreservesType() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1, 2], Buffer::TYPE_LONG));

        $repeated = $decorator->repeat(2);

        $this->assertSame(Buffer::TYPE_LONG, $repeated->type());
        $this->assertSame([1, 2, 1, 2], $repeated->toArray());
    }

    /**
     * @test
     */
    public function repeatInvalidTimes() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0]));

        $this->expectException(InvalidArgumentException::class);

        $decorator->repeat(0);
    }

    /**
     * @test
     */
    public function repeatRejectsOverflowingTimes() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0]));

        $this->expectException(InvalidArgumentException::class);

        $decorator->repeat(PHP_INT_MAX);
    }

    /**
     * @test
     */
    public function splitChunkLargerThanBuffer() : void
    {
        $decorator = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0]));

        $chunks = $decorator->split(PHP_INT_MAX);

        $this->assertCount(1, $chunks);
        $this->assertEqualsWithDelta([1.0, 2.0, 3.0], $chunks[0]->toArray(), self::MAX_DELTA);
    }
}
