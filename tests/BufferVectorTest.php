<?php

namespace Tensor\Tests;

use Tensor\Buffer;
use Tensor\BufferVector;
use Tensor\Exceptions\DimensionalityMismatch;
use Tensor\Tests\Support\Loader;
use Tensor\Vector;
use PHPUnit\Framework\TestCase;

/**
 * BufferVector is the measurement prototype for the storage migration: a rank-1
 * tensor whose elements live in a Tensor\Buffer instead of a PHP array. It
 * carries only the three operations the issue identifies as dominated by
 * pack/unpack overhead, so the two representations can be benchmarked on
 * identical work.
 *
 * It is not a replacement for Vector and is not part of the public API. What
 * these tests establish is that the buffer path computes the same numbers as
 * the array path — if it did not, the benchmark would be measuring the wrong
 * thing.
 *
 * @covers \Tensor\BufferVector
 */
class BufferVectorTest extends TestCase
{
    protected const MAX_DELTA = 1e-8;

    /**
     * @var list<float>
     */
    protected array $a = [];

    /**
     * @var list<float>
     */
    protected array $b = [];

    protected function setUp() : void
    {
        $samples = Loader::load('blob-100x4')['samples'];

        $this->a = array_column($samples, 0);
        $this->b = array_column($samples, 1);
    }

    /**
     * @test
     */
    public function buildRoundTripsThroughTheBuffer() : void
    {
        $this->assertSame($this->a, BufferVector::build($this->a)->asArray());
    }

    /**
     * @test
     */
    public function theUnderlyingStorageIsABuffer() : void
    {
        $vector = BufferVector::build($this->a);

        $this->assertInstanceOf(Buffer::class, $vector->buffer());
        $this->assertSame(Buffer::TYPE_DOUBLE, $vector->buffer()->type());
        $this->assertCount(count($this->a), $vector->buffer());
        $this->assertSame(count($this->a), $vector->n());
    }

    /**
     * @test
     */
    public function additionAgreesWithTheArrayPath() : void
    {
        $this->assertEqualsWithDelta(
            Vector::quick($this->a)->add(Vector::quick($this->b))->asArray(),
            BufferVector::build($this->a)->add(BufferVector::build($this->b))->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function multiplicationAgreesWithTheArrayPath() : void
    {
        $this->assertEqualsWithDelta(
            Vector::quick($this->a)->multiply(Vector::quick($this->b))->asArray(),
            BufferVector::build($this->a)->multiply(BufferVector::build($this->b))->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * The array path hand-rolls this accumulator; the buffer path reaches
     * cblas_ddot. They still have to agree.
     *
     * @test
     */
    public function innerProductAgreesWithTheArrayPath() : void
    {
        $this->assertEqualsWithDelta(
            Vector::quick($this->a)->dot(Vector::quick($this->b)),
            BufferVector::build($this->a)->dot(BufferVector::build($this->b)),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function aChainStaysInTheBuffer() : void
    {
        $expected = Vector::quick($this->a)
            ->add(Vector::quick($this->b))
            ->multiply(Vector::quick($this->a))
            ->asArray();

        $actual = BufferVector::build($this->a)
            ->add(BufferVector::build($this->b))
            ->multiply(BufferVector::build($this->a))
            ->asArray();

        $this->assertEqualsWithDelta($expected, $actual, self::MAX_DELTA);
    }

    /**
     * Length validation happens in Zephir, before the C handler is reached, and
     * raises the library's own exception type. The handlers do carry their own
     * guard, but it throws a plain Error and is only reachable by calling them
     * directly, which nothing does.
     *
     * @test
     * @dataProvider mismatchedOperationProvider
     * @param string $method
     */
    public function mismatchedLengthsRaiseADimensionalityMismatch(string $method) : void
    {
        $a = BufferVector::build([1.0, 2.0, 3.0]);
        $b = BufferVector::build([1.0, 2.0]);

        $this->expectException(DimensionalityMismatch::class);

        /** @var callable $callable */
        $callable = [$a, $method];
        $callable($b);
    }

    /**
     * @return \Generator<array{0: string}>
     */
    public function mismatchedOperationProvider() : \Generator
    {
        yield 'add' => ['add'];
        yield 'multiply' => ['multiply'];
        yield 'dot' => ['dot'];
    }

    /**
     * @test
     */
    public function anEmptyVectorIsLegal() : void
    {
        $vector = BufferVector::build([]);

        $this->assertSame(0, $vector->n());
        $this->assertSame([], $vector->asArray());
        $this->assertSame(0.0, $vector->dot(BufferVector::build([])));
    }
}
