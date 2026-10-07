<?php

namespace Tensor\Tests;

use Tensor\Tensor;
use Tensor\Vector;
use Tensor\Matrix;
use Tensor\Reductions;
use Tensor\ArrayLike;
use Tensor\Unary;
use Tensor\Arithmetic;
use Tensor\Comparable;
use Tensor\Special;
use Tensor\ColumnVector;
use Tensor\Trigonometric;
use Tensor\Exceptions\DimensionalityMismatch;
use Tensor\Exceptions\InvalidArgumentException;
use Tensor\Exceptions\RuntimeException;
use Tensor\Buffer;
use Tensor\TensorBuffer;
use PHPUnit\Framework\TestCase;
use Generator;
use ReflectionClass;
use ReflectionMethod;

/**
 * @covers \Tensor\Vector
 */
class VectorTest extends TestCase
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
        $vector = Vector::fromArray([1.0, 2.0, 3.0, 4.0, 5.0]);

        $this->assertInstanceOf(Vector::class, $vector);
        $this->assertInstanceOf(Tensor::class, $vector);
        $this->assertInstanceOf(ArrayLike::class, $vector);
        $this->assertInstanceOf(Arithmetic::class, $vector);
        $this->assertInstanceOf(Comparable::class, $vector);
        $this->assertInstanceOf(Unary::class, $vector);
        $this->assertInstanceOf(Trigonometric::class, $vector);
        $this->assertInstanceOf(Special::class, $vector);
        $this->assertInstanceOf(Reductions::class, $vector);
    }

    /**
     * @test
     */
    public function buildEmitsDeprecationNotice() : void
    {
        $notices = [];

        set_error_handler(
            function ($errno, $errstr) use (&$notices) : bool {
                if ($errno === E_USER_DEPRECATED) {
                    $notices[] = $errstr;

                    return true;
                }

                return false;
            },
            E_USER_DEPRECATED
        );

        try {
            $vector = Vector::build([1.0, 2.0, 3.0]);

            $this->assertInstanceOf(Vector::class, $vector);
        } finally {
            restore_error_handler();
        }

        $this->assertCount(1, $notices);
        $this->assertStringContainsString('Vector::build()', $notices[0]);
        $this->assertStringContainsString('Vector::fromArray', $notices[0]);
    }

    /**
     * @test
     */
    public function quickEmitsDeprecationNotice() : void
    {
        $notices = [];

        set_error_handler(
            function ($errno, $errstr) use (&$notices) : bool {
                if ($errno === E_USER_DEPRECATED) {
                    $notices[] = $errstr;

                    return true;
                }

                return false;
            },
            E_USER_DEPRECATED
        );

        try {
            $vector = Vector::quick([1.0, 2.0, 3.0]);

            $this->assertInstanceOf(Vector::class, $vector);
        } finally {
            restore_error_handler();
        }

        $this->assertCount(1, $notices);
        $this->assertStringContainsString('Vector::quick()', $notices[0]);
        $this->assertStringContainsString('Vector::fromArray', $notices[0]);
    }

    /**
     * @test
     */
    public function unaryInterfaceSurface() : void
    {
        $methods = array_map(
            fn (ReflectionMethod $method) : string => $method->getName(),
            (new ReflectionClass(Unary::class))->getMethods()
        );

        sort($methods);

        $this->assertEquals([
            'abs',
            'ceil',
            'clip',
            'clipLower',
            'clipUpper',
            'exp',
            'expm1',
            'floor',
            'log',
            'log1p',
            'negate',
            'round',
            'sign',
            'sqrt',
            'square',
        ], $methods);

        $this->assertEquals(2.7182818284590452354, Unary::M_E);
    }

    /**
     * @test
     */
    public function reductionsInterfaceSurface() : void
    {
        $methods = array_map(
            fn (ReflectionMethod $method) : string => $method->getName(),
            (new ReflectionClass(Reductions::class))->getMethods()
        );

        sort($methods);

        $this->assertEquals([
            'argmax',
            'argmin',
            'max',
            'mean',
            'median',
            'min',
            'product',
            'quantile',
            'sum',
            'variance',
        ], $methods);
    }

    /**
     * @test
     */
    public function buildCastsIntegersToFloats() : void
    {
        $vector = Vector::fromArray([1.0, 2.0, 3.0, 4.0, 5.0]);

        $this->assertSame(5, $vector->size());

        $result = $vector->asArray();

        $this->assertCount(5, $result);

        foreach ($result as $value) {
            $this->assertTrue(is_float($value));
        }

        $this->assertEqualsWithDelta([1.0, 2.0, 3.0, 4.0, 5.0], $result, self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function fromBuffer() : void
    {
        $buffer = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0, 4.0, 5.0]));

        $vector = Vector::fromBuffer($buffer);

        $this->assertInstanceOf(Vector::class, $vector);
        $this->assertSame($buffer, $vector->buffer());
        $this->assertSame([5], $vector->shape());
        $this->assertEqualsWithDelta([1.0, 2.0, 3.0, 4.0, 5.0], $vector->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function fromBufferRejectsIntegerBuffers() : void
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage('Argument must wrap a buffer of type double.');

        $buffer = new TensorBuffer(Buffer::fromArray([1, 2, 3, 4, 5], Buffer::TYPE_LONG));

        Vector::fromBuffer($buffer);
    }

    /**
     * @test
     */
    public function constructorIsProtected() : void
    {
        $this->assertTrue((new ReflectionMethod(Vector::class, '__construct'))->isProtected());
    }

    /**
     * @test
     */
    public function zeros() : void
    {
        $zeros = Vector::zeros(4);

        $expected = Vector::fromArray([0.0, 0.0, 0.0, 0.0]);

        $this->assertEquals($expected->asArray(), $zeros->asArray());
    }

    /**
     * @test
     */
    public function ones() : void
    {
        $ones = Vector::ones(4);

        $expected = Vector::fromArray([1.0, 1.0, 1.0, 1.0]);

        $this->assertEquals($expected->asArray(), $ones->asArray());
    }

    /**
     * @test
     */
    public function fill() : void
    {
        $vector = Vector::fill(16.0, 4);

        $expected = Vector::fromArray([16.0, 16.0, 16.0, 16.0]);

        $this->assertEquals($expected->asArray(), $vector->asArray());
    }

    /**
     * @test
     */
    public function rand() : void
    {
        $vector = Vector::rand(4);

        $this->assertCount(4, $vector);
    }

    /**
     * @test
     */
    public function gaussian() : void
    {
        $vector = Vector::gaussian(4);

        $this->assertCount(4, $vector);
    }

    /**
     * @test
     */
    public function uniform() : void
    {
        $vector = Vector::uniform(4);

        $this->assertCount(4, $vector);
    }

    /**
     * @test
     */
    public function randHasCorrectBounds() : void
    {
        $vector = Vector::rand(10000);

        $this->assertCount(10000, $vector);

        // A standard uniform on [0,1) must never produce a value outside [0, 1).
        $this->assertGreaterThanOrEqual(0.0, $vector->min());
        $this->assertTrue($vector->max() < 1.0);
    }

    /**
     * @test
     */
    public function randMeanIsCloseToHalf() : void
    {
        $vector = Vector::rand(10000);

        // A standard uniform on [0,1) has mean 1/2 and std sqrt(1/12).
        $this->assertEqualsWithDelta(0.5, $vector->mean(), 0.05);
    }

    /**
     * @test
     */
    public function uniformHasCorrectBounds() : void
    {
        $vector = Vector::uniform(10000);

        $this->assertCount(10000, $vector);

        // A standard uniform on [-1,1] must never produce a value outside that range.
        $this->assertGreaterThanOrEqual(-1.0, $vector->min());
        $this->assertLessThanOrEqual(1.0, $vector->max());
    }

    /**
     * @test
     */
    public function uniformMeanIsCloseToZero() : void
    {
        $vector = Vector::uniform(10000);

        // A standard uniform on [-1,1] has mean 0 and std sqrt(1/3).
        $this->assertEqualsWithDelta(0.0, $vector->mean(), 0.05);
    }

    /**
     * @test
     */
    public function uniformVarianceIsUnitScale() : void
    {
        $vector = Vector::uniform(10000);

        // Variance of a standard uniform on [-1,1] is 1/3.
        $this->assertEqualsWithDelta(1.0 / 3.0, $vector->variance(), 0.1);
    }

    /**
     * @test
     */
    public function gaussianHasZeroMeanAndUnitVariance() : void
    {
        $vector = Vector::gaussian(10000);

        $this->assertCount(10000, $vector);

        $this->assertEqualsWithDelta(0.0, $vector->mean(), 0.05);

        // Variance of a standard normal is 1.
        $this->assertEqualsWithDelta(1.0, $vector->variance(), 0.1);
    }

    /**
     * The random factories draw from PHP's MT19937 stream, so the exact same
     * seed must reproduce the exact same values. This guards against a future
     * regression that swaps in a different RNG (e.g. libc rand()).
     *
     * @test
     */
    public function randIsReproducibleUnderMtSrand() : void
    {
        mt_srand(4321);
        $a = Vector::rand(8)->asArray();

        mt_srand(4321);
        $b = Vector::rand(8)->asArray();

        $this->assertEquals($a, $b);
    }

    /**
     * @test
     */
    public function gaussianIsReproducibleUnderMtSrand() : void
    {
        mt_srand(4321);
        $a = Vector::gaussian(8)->asArray();

        mt_srand(4321);
        $b = Vector::gaussian(8)->asArray();

        $this->assertEquals($a, $b);
    }

    /**
     * @test
     */
    public function uniformIsReproducibleUnderMtSrand() : void
    {
        mt_srand(4321);
        $a = Vector::uniform(8)->asArray();

        mt_srand(4321);
        $b = Vector::uniform(8)->asArray();

        $this->assertEquals($a, $b);
    }

    /**
     * A different seed must yield a different sample, i.e. the output actually
     * depends on the seed rather than being constant.
     *
     * @test
     */
    public function randChangesWithSeed() : void
    {
        mt_srand(4321);
        $a = Vector::rand(8)->asArray();

        mt_srand(4322);
        $b = Vector::rand(8)->asArray();

        $this->assertNotEquals($a, $b);
    }

    /**
     * `Vector::rand(2)` consumes two draws from the same MT19937 state that
     * `mt_rand()` reads, so the element equals `mt_rand()/getrandmax()` and the
     * *following* `mt_rand()` is deterministic. Proves the factory shares
     * PHP's stream (order matters, so a separate RNG would not pass).
     *
     * @test
     */
    public function randSharesStreamWithMtRand() : void
    {
        mt_srand(123);
        $v = Vector::rand(2);
        $third = mt_rand();

        mt_srand(123);
        $r1 = mt_rand();
        $r2 = mt_rand();
        $r3 = mt_rand();

        $max = getrandmax();

        $this->assertEqualsWithDelta($r1 / $max, $v->asArray()[0], 0.0);
        $this->assertEqualsWithDelta($r2 / $max, $v->asArray()[1], 0.0);
        $this->assertSame($r3, $third);
    }

    /**
     * @test
     */
    public function range() : void
    {
        $vector = Vector::range(5.0, 12.0, 2.0);

        $expected = Vector::fromArray([5.0, 7.0, 9.0, 11.0]);

        $this->assertEquals($expected->asArray(), $vector->asArray());
    }

    /**
     * @test
     */
    public function linspace() : void
    {
        $vector = Vector::linspace(-5.0, 5.0, 10);

        $expected = Vector::fromArray([
            -5.0, -3.888888888888889, -2.7777777777777777, -1.6666666666666665, -0.5555555555555554,
            0.5555555555555558, 1.666666666666667, 2.777777777777778, 3.8888888888888893, 5.0,
        ]);

        $this->assertEquals($expected->asArray(), $vector->asArray());
    }

    /**
     * @test
     * @dataProvider shapeProvider
     *
     * @param Vector $vector
     * @param array<int> $expected
     */
    public function shape(Vector $vector, array $expected) : void
    {
        $this->assertEquals($expected, $vector->shape());
    }

    /**
     * @return Generator<mixed[]>
     */
    public function shapeProvider() : Generator
    {
        yield [Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]), [8]];

        yield [Vector::fromArray([0.25]), [1]];

        yield [Vector::fromArray([]), [0]];
    }

    /**
     * @test
     */
    public function shapeString() : void
    {
        $vector = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEquals('8', $vector->shapeString());
    }

    /**
     * @test
     */
    public function size() : void
    {
        $vector = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEquals(8, $vector->size());
    }

    /**
     * @test
     */
    public function m() : void
    {
        $vector = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEquals(1, $vector->m());
    }

    /**
     * @test
     */
    public function n() : void
    {
        $vector = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEquals(8, $vector->n());
    }

    /**
     * @test
     */
    public function asArray() : void
    {
        $vector = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $expected = [-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0];

        $this->assertEquals($expected, $vector->asArray());
    }

    /**
     * @test
     */
    public function serialization() : void
    {
        $vector = Vector::fromArray([-15.0, 25.0, 35.0, -36.0]);

        $serialized = serialize($vector);

        $this->assertStringNotContainsString('TensorBuffer', $serialized);

        $restored = unserialize($serialized);

        $this->assertInstanceOf(Vector::class, $restored);
        $this->assertEquals([-15.0, 25.0, 35.0, -36.0], $restored->asArray());
        $this->assertSame(serialize($vector), serialize($restored));
    }

    /**
     * @test
     */
    public function asRowMatrix() : void
    {
        $vector = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $matrix = $vector->asRowMatrix();

        $expected = Matrix::fromArray([
            [-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0],
        ]);

        $this->assertEquals($expected->asArray(), $matrix->asArray());
    }

    /**
     * @test
     */
    public function asColumnMatrix() : void
    {
        $vector = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $matrix = $vector->asColumnMatrix();

        $expected = Matrix::fromArray([[-15.0], [25.0], [35.0], [-36.0], [-72.0], [89.0], [106.0], [45.0]]);

        $this->assertEquals($expected->asArray(), $matrix->asArray());
    }

    /**
     * @test
     */
    public function reshape() : void
    {
        $vector = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $matrix = $vector->reshape(4, 2);

        $expected = Matrix::fromArray([
            [-15.0, 25.0],
            [35.0, -36.0],
            [-72.0, 89.0],
            [106.0, 45.0],
        ]);

        $this->assertEquals($expected->asArray(), $matrix->asArray());
    }

    /**
     * @test
     */
    public function transpose() : void
    {
        $vector = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $vector = $vector->transpose();

        $expected = ColumnVector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEquals($expected->asArray(), $vector->asArray());
    }

    /**
     * @test
     */
    public function map() : void
    {
        $vector = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $sign = function ($value) {
            return $value >= 0.0 ? 1 : 0;
        };

        $vector = $vector->map($sign);

        $expected = Vector::fromArray([0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0]);

        $this->assertEquals($expected->asArray(), $vector->asArray());
    }

    /**
     * @test
     */
    public function reduce() : void
    {
        $vector = Vector::fromArray([1.0, 2.0, 3.0]);

        $sum = function ($carry, $value) {
            return $carry + $value;
        };

        $this->assertEqualsWithDelta(6.0, $vector->reduce($sum), self::MAX_DELTA);

        // Asymmetric callback: pins the (carry, value) argument order.
        $subtract = function ($carry, $value) {
            return $carry - $value;
        };

        $this->assertEqualsWithDelta(-6.0, $vector->reduce($subtract), self::MAX_DELTA);
        $this->assertEqualsWithDelta(-4.0, $vector->reduce($subtract, 2.0), self::MAX_DELTA);

        // Must match Matrix::reduce() for the same data and callback.
        $matrix = Matrix::fromArray([[1.0, 2.0, 3.0]]);

        $this->assertEqualsWithDelta($vector->reduce($subtract), $matrix->reduce($subtract), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function reciprocal() : void
    {
        $vector = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $vector = $vector->reciprocal();

        $expected = Vector::fromArray([
            -0.06666666666666667, 0.04, 0.02857142857142857, -0.027777777777777776,
            -0.013888888888888888, 0.011235955056179775, 0.009433962264150943, 0.022222222222222223,
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $vector->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function dot() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $b = Vector::fromArray([0.25, 0.1, 2.0, -0.5, -1.0, -3.0, 3.3, 2.0]);

        $c = $a->dot($b);

        $this->assertEqualsWithDelta(331.54999999999995, $c, self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function matmul() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = Matrix::fromArray([
            [1.1, 0.01, 6.23],
            [5.0, 2.01, -1.0],
            [-5.0, 1.0, 0.03],
            [30.0, 0.02, -0.01],
            [-0.005, 0.05, -0.5],
            [-0.001, -1.0, 2.0],
        ]);

        $c = $a->matmul($b);

        $expected = Matrix::fromArray([
            [622.3751, 4.634999999999993, 40.807],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function inner() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $b = Vector::fromArray([0.25, 0.1, 2.0, -0.5, -1.0, -3.0, 3.3, 2.0]);

        $c = $a->inner($b);

        $this->assertEqualsWithDelta(331.54999999999995, $c, self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function outer() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $b = Vector::fromArray([0.25, 0.1, 2.0, -0.5, -1.0, -3.0, 3.3, 2.0]);

        $c = $a->outer($b);

        $expected = Matrix::fromArray([
            [-3.75, -1.5, -30.0, 7.5, 15.0, 45.0, -49.5, -30.],
            [6.25, 2.5, 50.0, -12.5, -25.0, -75.0, 82.5, 50.],
            [8.75, 3.5, 70.0, -17.5, -35.0, -105.0, 115.5, 70.],
            [-9.0, -3.6, -72.0, 18.0, 36.0, 108.0, -118.8, -72.],
            [-18.0, -7.2, -144.0, 36.0, 72.0, 216.0, -237.6, -144.],
            [22.25, 8.9, 178.0, -44.5, -89.0, -267.0, 293.7, 178.],
            [26.5, 10.600000000000001, 212.0, -53.0, -106.0, -318.0, 349.79999999999995, 212.],
            [11.25, 4.5, 90.0, -22.5, -45.0, -135.0, 148.5, 90.],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function convolve() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $b = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        // No padding is the "valid" convolution: the eight samples and the six
        // kernel taps give three outputs, each a full six-tap window.
        $this->assertEqualsWithDelta(
            [370.1, 462.20000000000005, 10.000000000000114],
            $a->convolve($b, 1)->asArray(),
            self::MAX_DELTA
        );

        // One zero at each end adds one output on either side, the first and
        // last of which read four and two real samples respectively.
        $this->assertEqualsWithDelta(
            [40.5, 370.1, 462.20000000000005, 10.000000000000114, 1764.3000000000002],
            $a->convolve($b, 1, 1)->asArray(),
            self::MAX_DELTA
        );

        // Five zeros at each end is the whole of the "full" convolution, which
        // is what this used to return with no padding argument at all.
        $this->assertEqualsWithDelta(
            [
                -60.0, 2.5, 259.0, -144.0, 40.5, 370.1, 462.20000000000005,
                10.000000000000114, 1764.3000000000002, 1625.1, 2234.7, 1378.4, 535.5,
            ],
            $a->convolve($b, 1, 5)->asArray(),
            self::MAX_DELTA
        );

        // The stride selects every n-th output of the same padded convolution,
        // so a padding of nB - 1 at stride 2 is the sampling of the "full" one
        // that convolveStrideTwo() below still asserts.
        $this->assertEqualsWithDelta(
            [-144.0, 370.1, 10.000000000000114, 1625.1],
            $a->convolve($b, 2, 2)->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * Regression test for the out-of-bounds read in the final output slot
     * reached when an output index equals the size of A (na), i.e. when the
     * size of A is a multiple of the stride and nb >= 2.
     *
     * Only the "full" convolution reaches an output index of na, so this needs
     * a padding of nb - 1 to arrive at the slot that used to be the last one.
     *
     * @test
     */
    public function convolveFullReadsNoOutOfBounds() : void
    {
        $a = Vector::fromArray([5.0, 2.0, 7.0, 1.0, 9.0, 3.0]);

        $b = Vector::fromArray([1.0, 2.0, 3.0]);

        $c = $a->convolve($b, 1, 2);

        $expected = Vector::fromArray([5.0, 12.0, 26.0, 21.0, 32.0, 24.0, 33.0, 9.0]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function convolveStrideTwo() : void
    {
        $a = Vector::fromArray([5.0, 2.0, 7.0, 1.0, 9.0, 3.0]);

        $b = Vector::fromArray([1.0, 2.0, 3.0]);

        $c = $a->convolve($b, 2);

        $this->assertEqualsWithDelta([26.0, 32.0], $c->asArray(), self::MAX_DELTA);
    }

    /**
     * The unpadded length is na - nb + 1, so a stride at least as large as that
     * samples exactly one output element, and a smaller stride emits one per
     * stride step of it.
     *
     * @test
     */
    public function convolveStrideLargerThanResultReturnsSingleElement() : void
    {
        $a = Vector::fromArray([5.0, 2.0, 7.0]);

        $b = Vector::fromArray([1.0, 2.0]);

        // The "valid" convolution has 3 - 2 + 1 = 2 samples, so a stride of 2
        // and above emits only the first of them, a[1] * b[0] + a[0] * b[1].
        $this->assertEqualsWithDelta([12.0], $a->convolve($b, 2)->asArray(), self::MAX_DELTA);

        $this->assertEqualsWithDelta([12.0], $a->convolve($b, 3)->asArray(), self::MAX_DELTA);

        $this->assertEqualsWithDelta([12.0], $a->convolve($b, 4)->asArray(), self::MAX_DELTA);

        // A stride of 1 emits both, the second being a[2] * b[0].
        $this->assertEqualsWithDelta([12.0, 11.0], $a->convolve($b, 1)->asArray(), self::MAX_DELTA);
    }

    /**
     * Regression test for the segmentation fault caused by a signed integer
     * overflow in the output length. Computing the length as
     * (na + 2 * padding - nb + stride - 1) / stride overflowed near PHP_INT_MAX
     * and produced a length of 0, i.e. a NULL data pointer, while the convolve
     * loop still emitted one sample and wrote through it.
     *
     * @test
     */
    public function convolveHugeStrideDoesNotCrash() : void
    {
        $a = Vector::fromArray([1.0, 2.0, 3.0]);

        $b = Vector::fromArray([1.0, 1.0]);

        foreach ([PHP_INT_MAX, PHP_INT_MAX - 1, PHP_INT_MAX - 2, intdiv(PHP_INT_MAX, 2)] as $stride) {
            $c = $a->convolve($b, $stride);

            $this->assertCount(1, $c);
            $this->assertEqualsWithDelta([3.0], $c->asArray(), self::MAX_DELTA);
        }

        // The padding is added before the division, so a padding large enough to
        // overflow on its own has to saturate rather than wrap round into a
        // length of zero.
        $c = $a->convolve($b, PHP_INT_MAX, 1);

        $this->assertCount(1, $c);
        $this->assertEqualsWithDelta([1.0], $c->asArray(), self::MAX_DELTA);
    }

    /**
     * An empty kernel has no samples to accumulate, so it is rejected rather
     * than silently returning a zero-filled result of an arbitrary length.
     *
     * @test
     */
    public function convolveEmptyKernelThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        (Vector::fromArray([1.0, 2.0, 3.0]))->convolve(Vector::fromArray([]));
    }

    /**
     * Convolving two empty vectors is rejected with the reason for it rather
     * than reaching the constructor with an unset result buffer, which used to
     * surface as a TypeError about the protected constructor.
     *
     * @test
     */
    public function convolveEmptyVectorsThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage('Vector B cannot be empty.');

        (Vector::fromArray([]))->convolve(Vector::fromArray([]));
    }

    /**
     * Cross-check the kernel against a straightforward reference implementation
     * over a wide range of lengths, kernel sizes, strides and paddings.
     *
     * @test
     */
    public function convolveMatchesReference() : void
    {
        mt_srand(4321);

        for ($trial = 0; $trial < 200; ++$trial) {
            $na = mt_rand(1, 12);
            $padding = mt_rand(0, 4);
            $nb = mt_rand(1, min($na + 2 * $padding, 6));
            $stride = mt_rand(1, 5);

            $a = [];
            $b = [];

            for ($i = 0; $i < $na; ++$i) {
                $a[] = mt_rand(-500, 500) / 7.0;
            }

            for ($i = 0; $i < $nb; ++$i) {
                $b[] = mt_rand(-500, 500) / 7.0;
            }

            $expected = $this->referenceConvolve1d($a, $b, $stride, $padding);

            $actual = Vector::fromArray($a)->convolve(Vector::fromArray($b), $stride, $padding)->asArray();

            $this->assertCount(count($expected), $actual, "na = {$na}, nb = {$nb}, stride = {$stride}, padding = {$padding}");

            $this->assertEqualsWithDelta($expected, $actual, self::MAX_DELTA);
        }
    }

    /**
     * The kernel accumulates 32 outputs at a time whenever consecutive outputs
     * read consecutive samples, so convolveMatchesReference() above -- whose
     * inputs are all shorter than one tile -- never reaches that path. This
     * crosses it from both sides: the lengths and kernel sizes step either side
     * of the tile width and of the stack scratch the reversed kernel is built
     * in, and the padding steps the band of whole tiles across the input. The
     * padding is what makes the band move at all, so the two cases worth
     * covering separately are a band that is present and one that is empty
     * because the output is shorter than a tile.
     *
     * @test
     */
    public function convolveAcrossTileBoundariesMatchesReference() : void
    {
        mt_srand(1234);

        $lengths = [1, 31, 32, 33, 63, 64, 65, 100, 257];
        $kernels = [1, 2, 3, 31, 32, 33, 64, 257];
        $paddings = [0, 1, 16, 32, 33, 64];

        foreach ($lengths as $na) {
            foreach ($kernels as $nb) {
                foreach ($paddings as $padding) {
                    // The API rejects a kernel that padding cannot make fit.
                    if ($nb > $na + 2 * $padding) {
                        continue;
                    }

                    $a = [];
                    $b = [];

                    for ($i = 0; $i < $na; ++$i) {
                        $a[] = mt_rand(-500, 500) / 7.0;
                    }

                    for ($i = 0; $i < $nb; ++$i) {
                        $b[] = mt_rand(-500, 500) / 7.0;
                    }

                    $expected = $this->referenceConvolve1d($a, $b, 1, $padding);

                    $actual = Vector::fromArray($a)->convolve(Vector::fromArray($b), 1, $padding)->asArray();

                    $this->assertCount(count($expected), $actual, "na = {$na}, nb = {$nb}, padding = {$padding}");

                    $this->assertEqualsWithDelta($expected, $actual, self::MAX_DELTA, "na = {$na}, nb = {$nb}, padding = {$padding}");
                }
            }
        }

        // The tile path is deliberately not taken for a stride above 1, so the
        // same boundary lengths are worth crossing on the per-output path too.
        foreach ($lengths as $na) {
            foreach ([1, 2, 3] as $stride) {
                foreach ([0, 1, 17, 33] as $padding) {
                    $a = [];
                    $b = [];

                    for ($i = 0; $i < $na; ++$i) {
                        $a[] = mt_rand(-500, 500) / 7.0;
                    }

                    for ($i = 0; $i < min($na + 2 * $padding, 33); ++$i) {
                        $b[] = mt_rand(-500, 500) / 7.0;
                    }

                    $expected = $this->referenceConvolve1d($a, $b, $stride, $padding);

                    $actual = Vector::fromArray($a)->convolve(Vector::fromArray($b), $stride, $padding)->asArray();

                    $this->assertCount(count($expected), $actual, "na = {$na}, stride = {$stride}, padding = {$padding}");

                    $this->assertEqualsWithDelta($expected, $actual, self::MAX_DELTA, "na = {$na}, stride = {$stride}, padding = {$padding}");
                }
            }
        }
    }

    /**
     * A kernel wider than the input is rejected unless the padding makes it
     * fit, and the output length is
     * floor((na + 2 * padding - nb) / stride) + 1, so a kernel exactly one
     * sample wider than the padded input still leaves a single output.
     *
     * @test
     */
    public function convolvePaddingMakesKernelFit() : void
    {
        $a = Vector::fromArray([5.0, 2.0, 7.0]);

        $b = Vector::fromArray([1.0, 2.0, 3.0, 4.0]);

        // 3 + 2 * 1 - 4 + 1 = 2 outputs, the first of which still sees the
        // whole kernel and the second only its first three taps.
        $this->assertEqualsWithDelta([26.0, 40.0], $a->convolve($b, 1, 1)->asArray(), self::MAX_DELTA);

        $this->assertEqualsWithDelta([26.0], $a->convolve($b, 2, 1)->asArray(), self::MAX_DELTA);
    }

    /**
     * A padding past the point where every sample is inside some output window
     * is still zero padding: the extra outputs are zeros rather than being
     * dropped.
     *
     * @test
     */
    public function convolvePaddingLargerThanNeededIsStillZeroPadded() : void
    {
        $a = Vector::fromArray([5.0, 2.0, 7.0, 1.0]);

        $b = Vector::fromArray([1.0, 1.0]);

        // 4 + 2 * 3 - 2 + 1 = 9 outputs, of which the five at each end read
        // nothing at all.
        $result = $a->convolve($b, 1, 3)->asArray();

        $this->assertCount(9, $result);
        $this->assertEqualsWithDelta([0.0, 0.0, 5.0, 7.0, 9.0, 8.0, 1.0, 0.0, 0.0], $result, self::MAX_DELTA);
    }

    /**
     * @test
     * @dataProvider multiplyProvider
     *
     * @param Vector $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function multiply(Vector $a, $b, $expected) : void
    {
        $c = $a->multiply($b);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @return Generator<mixed[]>
     */
    public function multiplyProvider() : Generator
    {
        yield [
            Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]),
            Matrix::fromArray([
                [6.23, -1.0, 0.03, -0.01, -0.5, 2.0],
                [0.01, 2.01, 1.0, 0.02, 0.05, -1.0],
                [1.1, 5.0, -5.0, 30.0, -0.005, -0.001],
            ]),
            Matrix::fromArray([
                [24.92, -6.5, 0.087, -0.2, -1.3, 23.8],
                [0.04, 13.064999999999998, 2.9, 0.4, 0.13, -11.9],
                [4.4, 32.5, -14.5, 600.0, -0.013000000000000001, -0.0119],
            ]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            Vector::fromArray([0.25, 0.1, 2.0, -0.5, -1.0, -3.0, 3.3, 2.0]),
            Vector::fromArray([-3.75, 2.5, 70.0, 18.0, 72.0, -267.0, 349.79999999999995, 90.0]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            2.0,
            Vector::fromArray([-30.0, 50.0, 70.0, -72.0, -144.0, 178.0, 212.0, 90.0]),
        ];
    }

    /**
     * @test
     * @dataProvider divideProvider
     *
     * @param Vector $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function divide(Vector $a, $b, $expected) : void
    {
        $c = $a->divide($b);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @return Generator<mixed[]>
     */
    public function divideProvider() : Generator
    {
        yield [
            Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]),
            Matrix::fromArray([
                [6.23, -1.0, 0.03, -0.01, -0.5, 2.0],
                [0.01, 2.01, 1.0, 0.02, 0.05, -1.0],
                [1.1, 5.0, -5.0, 30.0, -0.005, -0.001],
            ]),
            Matrix::fromArray([
                [0.6420545746388443, -6.5, 96.66666666666667, -2000.0, -5.2, 5.95],
                [400.0, 3.2338308457711444, 2.9, 1000.0, 52.0, -11.9],
                [3.6363636363636362, 1.3, -0.58, 0.6666666666666666, -520.0, -11900.],
            ]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            Vector::fromArray([0.25, 0.1, 2.0, -0.5, -1.0, -3.0, 3.3, 2.0]),
            Vector::fromArray([-60.0, 250.0, 17.5, 72.0, 72.0, -29.666666666666668, 32.121212121212125, 22.5]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            2.0,
            Vector::fromArray([-7.5, 12.5, 17.5, -18.0, -36.0, 44.5, 53.0, 22.5]),
        ];
    }

    /**
     * @test
     * @dataProvider addProvider
     *
     * @param Vector $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function add(Vector $a, $b, $expected) : void
    {
        $c = $a->add($b);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @return Generator<mixed[]>
     */
    public function addProvider() : Generator
    {
        yield [
            Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]),
            Matrix::fromArray([
                [6.23, -1.0, 0.03, -0.01, -0.5, 2.0],
                [0.01, 2.01, 1.0, 0.02, 0.05, -1.0],
                [1.1, 5.0, -5.0, 30.0, -0.005, -0.001],
            ]),
            Matrix::fromArray([
                [10.23, 5.5, 2.9299999999999997, 19.99, 2.1, 13.9],
                [4.01, 8.51, 3.9, 20.02, 2.65, 10.9],
                [5.1, 11.5, -2.1, 50.0, 2.595, 11.899000000000001],
            ]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            Vector::fromArray([0.25, 0.1, 2.0, -0.5, -1.0, -3.0, 3.3, 2.0]),
            Vector::fromArray([-14.75, 25.1, 37.0, -36.5, -73.0, 86.0, 109.3, 47.0]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            10.0,
            Vector::fromArray([-5.0, 35.0, 45.0, -26.0, -62.0, 99.0, 116.0, 55.0]),
        ];
    }

    /**
     * @test
     * @dataProvider subtractProvider
     *
     * @param Vector $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function subtract(Vector $a, $b, $expected) : void
    {
        $c = $a->subtract($b);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @return Generator<mixed[]>
     */
    public function subtractProvider() : Generator
    {
        yield [
            Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]),
            Matrix::fromArray([
                [6.23, -1.0, 0.03, -0.01, -0.5, 2.0],
                [0.01, 2.01, 1.0, 0.02, 0.05, -1.0],
                [1.1, 5.0, -5.0, 30.0, -0.005, -0.001],
            ]),
            Matrix::fromArray([
                [-2.2300000000000004, 7.5, 2.87, 20.01, 3.1, 9.9],
                [3.99, 4.49, 1.9, 19.98, 2.5500000000000003, 12.9],
                [2.9, 1.5, 7.9, -10.0, 2.605, 11.901],
            ]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            Vector::fromArray([0.25, 0.1, 2.0, -0.5, -1.0, -3.0, 3.3, 2.0]),
            Vector::fromArray([-15.25, 24.9, 33.0, -35.5, -71.0, 92.0, 102.7, 43.0]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            10.0,
            Vector::fromArray([-25.0, 15.0, 25.0, -46.0, -82.0, 79.0, 96.0, 35.0]),
        ];
    }

    /**
     * @test
     * @dataProvider powProvider
     *
     * @param Vector $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function power(Vector $a, $b, $expected) : void
    {
        $c = $a->pow($b);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @return Generator<mixed[]>
     */
    public function powProvider() : Generator
    {
        yield [
            Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]),
            Matrix::fromArray([
                [6.23, -1.0, 0.03, -0.01, -0.5, 2.0],
                [0.01, 2.01, 1.0, 0.02, 0.05, -1.0],
                [1.1, 5.0, -5.0, 30.0, -0.005, -0.001],
            ]),
            Matrix::fromArray([
                [5634.219287100394, 0.15384615384615385, 1.0324569211337775, 0.9704869503929601, 0.6201736729460423, 141.61],
                [1.013959479790029, 43.048284263459465, 2.9, 1.0617459178549786, 1.0489352187366092, 0.08403361344537814],
                [4.59479341998814, 11602.90625, 0.004875397277841432, 1.073741824E+39, 0.9952338371484033, 0.9975265256911376],
            ]),
        ];

        yield [
            Vector::fromArray([3.0, 6.0, 9.0]),
            Vector::fromArray([3.0, 2.0, 1.0]),
            Vector::fromArray([27.0, 36.0, 9.0]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            4.0,
            Vector::fromArray([
                50625.0, 390625.0, 1500625.0, 1679616.0, 26873856.0, 62742241.0, 126247696.0, 4100625.0
            ]),
        ];
    }

    /**
     * @test
     * @dataProvider equalProvider
     *
     * @param Vector $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function equal(Vector $a, $b, $expected) : void
    {
        $c = $a->equal($b);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @return Generator<mixed[]>
     */
    public function equalProvider() : Generator
    {
        yield [
            Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]),
            Matrix::fromArray([
                [4.0, -1.0, 0.03, -0.01, -0.5, 2.0],
                [0.01, 2.01, 1.0, 20.0, 0.05, -1.0],
                [1.1, 5.0, -5.0, 30.0, -0.005, 11.9],
            ]),
            Matrix::fromArray([
                [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
                [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
                [0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
            ]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            Vector::fromArray([0.25, 0.1, 2.0, -36.0, -1.0, -3.0, 3.3, 2.0]),
            Vector::fromArray([0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            25.0,
            Vector::fromArray([0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]),
        ];
    }

    /**
     * @test
     * @dataProvider notEqualProvider
     *
     * @param Vector $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function notEqual(Vector $a, $b, $expected) : void
    {
        $c = $a->notEqual($b);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @return Generator<mixed[]>
     */
    public function notEqualProvider() : Generator
    {
        yield [
            Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]),
            Matrix::fromArray([
                [4.0, -1.0, 0.03, -0.01, -0.5, 2.0],
                [0.01, 2.01, 1.0, 20.0, 0.05, -1.0],
                [1.1, 5.0, -5.0, 30.0, -0.005, 11.9],
            ]),
            Matrix::fromArray([
                [0.0, 1.0, 1.0, 1.0, 1.0, 1.0],
                [1.0, 1.0, 1.0, 0.0, 1.0, 1.0],
                [1.0, 1.0, 1.0, 1.0, 1.0, 0.0],
            ]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            Vector::fromArray([0.25, 0.1, 2.0, -36.0, -1.0, -3.0, 3.3, 2.0]),
            Vector::fromArray([1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            25.0,
            Vector::fromArray([1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]),
        ];
    }

    /**
     * @test
     */
    public function notEqualMatrixDimensionMismatch() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        $a = Vector::fromArray([1.0, 2.0, 3.0]);

        $b = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]);

        $a->notEqualMatrix($b);
    }

    /**
     * @test
     */
    public function multiplyMatrix() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = Matrix::fromArray([
            [4.0, -1.0, 0.03, -0.01, -0.5, 2.0],
            [0.01, 2.01, 1.0, 20.0, 0.05, -1.0],
            [1.1, 5.0, -5.0, 30.0, -0.005, 11.9],
        ]);

        $c = $a->multiplyMatrix($b);

        $expected = Matrix::fromArray([
            [16.0, -6.5, 0.087, -0.2, -1.3, 23.8],
            [0.04, 13.065, 2.9, 400.0, 0.13, -11.9],
            [4.4, 32.5, -14.5, 600.0, -0.013, 141.61],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function divideMatrix() : void
    {
        $a = Vector::fromArray([10.0, 20.0, 30.0]);

        $b = Matrix::fromArray([
            [2.0, 4.0, 5.0],
            [5.0, 10.0, 3.0],
            [3.0, 2.0, 1.0],
        ]);

        $c = $a->divideMatrix($b);

        $expected = Matrix::fromArray([
            [5.0, 5.0, 6.0],
            [2.0, 2.0, 10.0],
            [3.3333333333333335, 10.0, 30.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function addMatrix() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = Matrix::fromArray([
            [4.0, -1.0, 0.03, -0.01, -0.5, 2.0],
            [0.01, 2.01, 1.0, 20.0, 0.05, -1.0],
            [1.1, 5.0, -5.0, 30.0, -0.005, 11.9],
        ]);

        $c = $a->addMatrix($b);

        $expected = Matrix::fromArray([
            [8.0, 5.5, 2.93, 19.99, 2.1, 13.9],
            [4.01, 8.51, 3.9, 40.0, 2.65, 10.9],
            [5.1, 11.5, -2.1, 50.0, 2.595, 23.8],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function subtractMatrix() : void
    {
        $a = Vector::fromArray([10.0, 20.0, 30.0]);

        $b = Matrix::fromArray([
            [4.0, 15.0, 12.0],
            [4.0, 15.0, 12.0],
            [4.0, 15.0, 12.0],
        ]);

        $c = $a->subtractMatrix($b);

        $expected = Matrix::fromArray([
            [6.0, 5.0, 18.0],
            [6.0, 5.0, 18.0],
            [6.0, 5.0, 18.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function powMatrix() : void
    {
        $a = Vector::fromArray([2.0, 10.0, 2.0]);

        $b = Matrix::fromArray([
            [1.0, 1.0, 1.5],
            [2.0, 0.0, 1.0],
            [3.0, 2.0, 0.5],
        ]);

        $c = $a->powMatrix($b);

        $expected = Matrix::fromArray([
            [2.0, 10.0, 2.8284271247461903],
            [4.0, 1.0, 2.0],
            [8.0, 100.0, 1.4142135623730951],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function modMatrix() : void
    {
        $a = Vector::fromArray([1.0, 20.0, 7.5]);

        $b = Matrix::fromArray([
            [0.5, 5.0, 3.0],
            [0.5, 10.0, 3.0],
            [0.5, 7.5, 3.0],
        ]);

        $c = $a->modMatrix($b);

        $expected = Matrix::fromArray([
            [0.0, 0.0, 1.5],
            [0.0, 0.0, 1.5],
            [0.0, 5.0, 1.5],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function equalMatrix() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = Matrix::fromArray([
            [4.0, -1.0, 0.03, -0.01, -0.5, 2.0],
            [0.01, 6.5, 2.9, 20.0, 2.6, 11.9],
            [1.1, 5.0, -5.0, 30.0, -0.005, 11.9],
        ]);

        $c = $a->equalMatrix($b);

        $expected = Matrix::fromArray([
            [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            [0.0, 1.0, 1.0, 1.0, 1.0, 1.0],
            [0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function notEqualMatrix() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = Matrix::fromArray([
            [4.0, -1.0, 0.03, -0.01, -0.5, 2.0],
            [0.01, 6.5, 2.9, 20.0, 2.6, 11.9],
            [1.1, 5.0, -5.0, 30.0, -0.005, 11.9],
        ]);

        $c = $a->notEqualMatrix($b);

        $expected = Matrix::fromArray([
            [0.0, 1.0, 1.0, 1.0, 1.0, 1.0],
            [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            [1.0, 1.0, 1.0, 1.0, 1.0, 0.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function greaterMatrix() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = Matrix::fromArray([
            [4.0, -1.0, 0.03, -0.01, -0.5, 2.0],
            [0.01, 6.5, 2.9, 20.0, 0.05, -1.0],
            [1.1, 5.0, -5.0, 30.0, -0.005, 11.9],
        ]);

        $c = $a->greaterMatrix($b);

        $expected = Matrix::fromArray([
            [0.0, 1.0, 1.0, 1.0, 1.0, 1.0],
            [1.0, 0.0, 0.0, 0.0, 1.0, 1.0],
            [1.0, 1.0, 1.0, 0.0, 1.0, 0.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function greaterEqualMatrix() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = Matrix::fromArray([
            [4.0, -1.0, 0.03, -0.01, -0.5, 2.0],
            [0.01, 6.5, 2.9, 20.0, 0.05, -1.0],
            [1.1, 5.0, -5.0, 30.0, -0.005, 11.9],
        ]);

        $c = $a->greaterEqualMatrix($b);

        $expected = Matrix::fromArray([
            [1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
            [1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
            [1.0, 1.0, 1.0, 0.0, 1.0, 1.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function lessMatrix() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = Matrix::fromArray([
            [4.0, -1.0, 0.03, -0.01, -0.5, 2.0],
            [0.01, 6.5, 2.9, 20.0, 0.05, -1.0],
            [1.1, 5.0, -5.0, 30.0, -0.005, 11.9],
        ]);

        $c = $a->lessMatrix($b);

        $expected = Matrix::fromArray([
            [0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function lessEqualMatrix() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = Matrix::fromArray([
            [4.0, -1.0, 0.03, -0.01, -0.5, 2.0],
            [0.01, 6.5, 2.9, 20.0, 0.05, -1.0],
            [1.1, 5.0, -5.0, 30.0, -0.005, 11.9],
        ]);

        $c = $a->lessEqualMatrix($b);

        $expected = Matrix::fromArray([
            [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            [0.0, 1.0, 1.0, 1.0, 0.0, 0.0],
            [0.0, 0.0, 0.0, 1.0, 0.0, 1.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     * @dataProvider greaterProvider
     *
     * @param Vector $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function greater(Vector $a, $b, $expected) : void
    {
        $c = $a->greater($b);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @return Generator<mixed[]>
     */
    public function greaterProvider() : Generator
    {
        yield [
            Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]),
            Matrix::fromArray([
                [4.0, -1.0, 0.03, -0.01, -0.5, 2.0],
                [0.01, 2.01, 1.0, 20.0, 0.05, -1.0],
                [1.1, 5.0, -5.0, 30.0, -0.005, 11.9],
            ]),
            Matrix::fromArray([
                [0.0, 1.0, 1.0, 1.0, 1.0, 1.0],
                [1.0, 1.0, 1.0, 0.0, 1.0, 1.0],
                [1.0, 1.0, 1.0, 0.0, 1.0, 0.0],
            ]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            Vector::fromArray([0.25, 0.1, 2.0, -36.0, -1.0, -3.0, 3.3, 2.0]),
            Vector::fromArray([0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            1.0,
            Vector::fromArray([0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0]),
        ];
    }

    /**
     * @test
     * @dataProvider greaterEqualProvider
     *
     * @param Vector $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function greaterEqual(Vector $a, $b, $expected) : void
    {
        $c = $a->greaterEqual($b);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @return Generator<mixed[]>
     */
    public function greaterEqualProvider() : Generator
    {
        yield [
            Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]),
            Matrix::fromArray([
                [4.0, -1.0, 0.03, -0.01, -0.5, 2.0],
                [0.01, 2.01, 1.0, 20.0, 0.05, -1.0],
                [1.1, 5.0, -5.0, 30.0, -0.005, 11.9],
            ]),
            Matrix::fromArray([
                [1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
                [1.0, 1.0, 1.0, 1.0, 1.0, 1.0],
                [1.0, 1.0, 1.0, 0.0, 1.0, 1.0],
            ]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            Vector::fromArray([0.25, 0.1, 2.0, -36.0, -1.0, -3.0, 3.3, 2.0]),
            Vector::fromArray([0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            25.0,
            Vector::fromArray([0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0]),
        ];
    }

    /**
     * @test
     * @dataProvider lessProvider
     *
     * @param Vector $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function less(Vector $a, $b, $expected) : void
    {
        $c = $a->less($b);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @return Generator<mixed[]>
     */
    public function lessProvider() : Generator
    {
        yield [
            Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]),
            Matrix::fromArray([
                [4.0, -1.0, 0.03, -0.01, -0.5, 2.0],
                [0.01, 2.01, 1.0, 20.0, 0.05, -1.0],
                [1.1, 5.0, -5.0, 30.0, -0.005, 11.9],
            ]),
            Matrix::fromArray([
                [0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
                [0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
                [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
            ]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            Vector::fromArray([0.25, 0.1, 2.0, -36.0, -1.0, -3.0, 3.3, 2.0]),
            Vector::fromArray([1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            25.0,
            Vector::fromArray([1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0]),
        ];
    }

    /**
     * @test
     * @dataProvider lessEqualProvider
     *
     * @param Vector $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function lessEqual(Vector $a, $b, $expected) : void
    {
        $c = $a->lessEqual($b);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @return Generator<mixed[]>
     */
    public function lessEqualProvider() : Generator
    {
        yield [
            Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]),
            Matrix::fromArray([
                [4.0, -1.0, 0.03, -0.01, -0.5, 2.0],
                [0.01, 2.01, 1.0, 20.0, 0.05, -1.0],
                [1.1, 5.0, -5.0, 30.0, -0.005, 11.9],
            ]),
            Matrix::fromArray([
                [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
                [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
                [0.0, 0.0, 0.0, 1.0, 0.0, 1.0],
            ]),

        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            Vector::fromArray([0.25, 0.1, 2.0, -36.0, -1.0, -3.0, 3.3, 2.0]),
            Vector::fromArray([1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            25.0,
            Vector::fromArray([1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0]),
        ];
    }

    /**
     * @test
     * @dataProvider modProvider
     *
     * @param Vector $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function mod(Vector $a, $b, $expected) : void
    {
        $c = $a->mod($b);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @return Generator<mixed[]>
     */
    public function modProvider() : Generator
    {
        yield [
            Vector::fromArray([3.5, -7.0, 12.75, 0.375, -1.5]),
            Vector::fromArray([2.0, 3.0, 4.5, 0.5, 2.0]),
            Vector::fromArray([1.5, -1.0, 3.75, 0.375, -1.5]),
        ];

        yield [
            Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]),
            4.0,
            Vector::fromArray([-3.0, 1.0, 3.0, 0.0, 0.0, 1.0, 2.0, 1.0]),
        ];
    }

    /**
     * @test
     */
    public function abs() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $b = $a->abs();

        $expected = Vector::fromArray([15.0, 25.0, 35.0, 36.0, 72.0, 89.0, 106.0, 45.0]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function square() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $b = $a->square();

        $expected = Vector::fromArray([225.0, 625.0, 1225.0, 1296.0, 5184.0, 7921.0, 11236.0, 2025.0]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function pow() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $b = $a->pow(3.0);

        $expected = Vector::fromArray([-3375.0, 15625.0, 42875.0, -46656.0, -373248.0, 704969.0, 1191016.0, 91125.0]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function sqrt() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = $a->sqrt();

        $expected = Vector::fromArray([
            2.0, 2.5495097567963922, 1.70293863659264, 4.47213595499958,
            1.61245154965971, 3.449637662132068,
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function exp() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = $a->exp();

        $expected = Vector::fromArray([
            54.598150033144236, 665.1416330443618, 18.17414536944306,
            485165195.4097903, 13.463738035001692, 147266.6252405527,
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function log() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = $a->log();

        $expected = Vector::fromArray([
            1.3862943611198906, 1.8718021769015913, 1.0647107369924282,
            2.995732273553991, 0.9555114450274363, 2.4765384001174837,
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function sigmoid() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = $a->sigmoid();

        $expected = Vector::fromArray([
            0.98201379003790845, 0.99849881774326299, 0.94784643692158232,
            0.99999999793884631, 0.93086157965665328, 0.99999320964130201,
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * A large negative input must not overflow the exponential that sigmoid
     * evaluates, and a large positive one must saturate to exactly 1.0.
     *
     * @test
     */
    public function sigmoidSaturates() : void
    {
        $a = Vector::fromArray([-800.0, 800.0, 0.0, -1000.0]);

        $this->assertSame([0.0, 1.0, 0.5, 0.0], $a->sigmoid()->asArray());
    }

    /**
     * The fused kernel must agree with the composition it replaces, elementwise.
     *
     * @test
     */
    public function sigmoidMatchesComposition() : void
    {
        $a = Vector::rand(257)->multiply(8.0)->subtract(4.0);

        $e = $a->exp();

        $this->assertEqualsWithDelta(
            $e->divide($e->add(1.0))->asArray(),
            $a->sigmoid()->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function softplus() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = $a->softplus();

        $expected = Vector::fromArray([
            4.0181499279178, 6.5015023101598, 2.953562776218,
            20.000000002061, 2.6716446919677, 11.900006790382,
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * A large positive input must not overflow the exponential that softplus
     * evaluates and must saturate to x, and a large negative one to 0.
     *
     * @test
     */
    public function softplusSaturates() : void
    {
        $a = Vector::fromArray([-800.0, 800.0, 0.0, -1000.0]);

        $b = $a->softplus()->asArray();

        $this->assertSame(0.0, $b[0]);
        $this->assertSame(800.0, $b[1]);
        $this->assertEqualsWithDelta(log(2.0), $b[2], self::MAX_DELTA);
        $this->assertSame(0.0, $b[3]);
    }

    /**
     * The fused kernel must agree with the composition it replaces, elementwise.
     *
     * @test
     */
    public function softplusMatchesComposition() : void
    {
        $a = Vector::rand(257)->multiply(8.0)->subtract(4.0);

        $e = $a->exp();

        $this->assertEqualsWithDelta(
            $e->add(1.0)->log()->asArray(),
            $a->softplus()->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function tanh() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = $a->tanh();

        $expected = Vector::fromArray([
            0.99932929973906703, 0.9999954793514042, 0.9939631673505831,
            1.0, 0.98902740220109919, 0.99999999990778077,
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * A large input must saturate to exactly +/-1.0 rather than returning an
     * infinity.
     *
     * @test
     */
    public function tanhSaturates() : void
    {
        $a = Vector::fromArray([100.0, -100.0, 0.0, -800.0]);

        $this->assertSame([1.0, -1.0, 0.0, -1.0], $a->tanh()->asArray());
    }

    /**
     * The fused kernel must agree with the composition it replaces, elementwise.
     *
     * @test
     */
    public function tanhMatchesComposition() : void
    {
        $a = Vector::rand(257)->multiply(8.0)->subtract(4.0);

        $e = $a->exp();
        $em = $a->multiply(-1.0)->exp();

        $this->assertEqualsWithDelta(
            $e->subtract($em)->divide($e->add($em))->asArray(),
            $a->tanh()->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function sinh() : void
    {
        $a = Vector::fromArray([0.0, 1.0, -1.0, 0.5, -0.5, 2.0]);

        $this->assertSame(0.0, $a->sinh()->asArray()[0]);

        $this->assertEqualsWithDelta(
            array_map('sinh', $a->asArray()),
            $a->sinh()->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * The kernel must agree with the exp difference composition it replaces.
     *
     * @test
     */
    public function sinhMatchesComposition() : void
    {
        $a = Vector::rand(257)->multiply(0.5)->subtract(0.25);

        $e = $a->exp();
        $em = $a->multiply(-1.0)->exp();

        $this->assertEqualsWithDelta(
            $e->subtract($em)->multiply(0.5)->asArray(),
            $a->sinh()->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function cosh() : void
    {
        $a = Vector::fromArray([0.0, 1.0, -1.0, 0.5, -0.5, 2.0]);

        $this->assertSame(1.0, $a->cosh()->asArray()[0]);

        $this->assertEqualsWithDelta(
            array_map('cosh', $a->asArray()),
            $a->cosh()->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * The kernel must agree with the exp sum composition it replaces.
     *
     * @test
     */
    public function coshMatchesComposition() : void
    {
        $a = Vector::rand(257)->multiply(0.5)->subtract(0.25);

        $e = $a->exp();
        $em = $a->multiply(-1.0)->exp();

        $this->assertEqualsWithDelta(
            $e->add($em)->multiply(0.5)->asArray(),
            $a->cosh()->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * A vector softmax normalizes the whole vector, as a single row, so the
     * result sums to one.
     *
     * @test
     */
    public function softmax() : void
    {
        $a = Vector::fromArray([1.0, 2.0, 3.0]);

        $b = $a->softmax();

        $expected = Vector::fromArray([0.09003057317038046, 0.24472847105479764, 0.6652409557748218]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
        $this->assertEqualsWithDelta(1.0, $b->sum(), self::MAX_DELTA);
    }

    /**
     * The defining property, over sizes that straddle the kernel's block
     * boundary.
     *
     * @test
     * @dataProvider lengthProvider
     * @param int $length
     */
    public function softmaxSumsToOne(int $length) : void
    {
        $a = Vector::rand($length)->multiply(20.0);

        $this->assertEqualsWithDelta(1.0, $a->softmax()->sum(), self::MAX_DELTA);
    }

    /**
     * Subtracting a constant leaves the exponentials scaled by the same factor,
     * which cancels in the division, so the result is unchanged. This is the
     * property the maximum subtraction inside the kernel relies on.
     *
     * @test
     */
    public function softmaxIsShiftInvariant() : void
    {
        $a = Vector::fromArray([1.0, 2.0, 3.0, -4.0]);

        $this->assertEqualsWithDelta(
            $a->softmax()->asArray(),
            $a->add(13.0)->softmax()->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * Large magnitudes must not overflow the exponential.
     *
     * @test
     */
    public function softmaxDoesNotOverflow() : void
    {
        $a = Vector::fromArray([1000.0, 1001.0, 1002.0]);

        $expected = Vector::fromArray([0.09003057317038046, 0.24472847105479764, 0.6652409557748218]);

        $this->assertEqualsWithDelta($expected->asArray(), $a->softmax()->asArray(), self::MAX_DELTA);
    }

    /**
     * The fused kernel must reproduce the transpose, maximum, subtract,
     * exponential, sum, clip, divide, transpose sequence it replaces.
     *
     * @test
     */
    public function softmaxMatchesComposition() : void
    {
        $a = Vector::fromArray([22.0, -17.0, 12.0, 4.0, 11.0, -2.0]);

        $e = $a->subtract($a->max())->exp();

        $this->assertEqualsWithDelta(
            $e->divide($e->sum())->asArray(),
            $a->softmax()->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * The kernel writes to a fresh buffer, so the operand must come back
     * untouched.
     *
     * @test
     */
    public function softmaxDoesNotMutate() : void
    {
        $a = Vector::fromArray([1.0, 2.0, 3.0]);

        $before = $a->asArray();
        $a->softmax();

        $this->assertSame($before, $a->asArray());
    }

    /**
     * @test
     */
    public function erf() : void
    {
        $a = Vector::fromArray([0.0, 1.0, -1.0, 0.5, -0.5, 2.0, 10.0]);

        $b = $a->erf();

        $expected = Vector::fromArray([
            0.0, 0.8427007929497148, -0.8427007929497148,
            0.5204998778130465, -0.5204998778130465,
            0.9953222650189527, 1.0,
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * A large input saturates to exactly +/-1.0 rather than an infinity.
     *
     * @test
     */
    public function erfSaturates() : void
    {
        $a = Vector::fromArray([100.0, -100.0]);

        $this->assertSame([1.0, -1.0], $a->erf()->asArray());
    }

    /**
     * Lengths on both sides of the kernel's block boundary: a single element, a
     * block narrower than the floor, and one wider than the cap.
     *
     * @return Generator<mixed[]>
     */
    public function lengthProvider() : Generator
    {
        yield [1];
        yield [2];
        yield [31];
        yield [32];
        yield [33];
        yield [127];
        yield [128];
        yield [129];
        yield [1000];
        yield [4096];
    }

    /**
     * @test
     */
    public function sin() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = $a->sin();

        $expected = Vector::fromArray([
            -0.7568024953079282, 0.21511998808781552, 0.23924932921398243,
            0.9129452507276277, 0.5155013718214642, -0.6181371122370333,
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function asin() : void
    {
        $a = Vector::fromArray([0.1, 0.3, -0.5]);

        $b = $a->asin();

        $expected = Vector::fromArray([
            0.1001674211615598, 0.3046926540153975, -0.5235987755982989,
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function cos() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = $a->cos();

        $expected = Vector::fromArray([
            -0.6536436208636119, 0.9765876257280235, -0.9709581651495905,
            0.40808206181339196, -0.8568887533689473, 0.7860702961410393,
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function acos() : void
    {
        $a = Vector::fromArray([0.1, 0.3, -0.5]);

        $b = $a->acos();

        $expected = Vector::fromArray([
            1.4706289056333368, 1.2661036727794992, 2.0943951023931957,
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function tan() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = $a->tan();

        $expected = Vector::fromArray([
            1.1578212823495777, 0.22027720034589682, -0.24640539397196634,
            2.237160944224742, -0.6015966130897586, -0.7863636563696398,
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function atan() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = $a->atan();

        $expected = Vector::fromArray([
            1.3258176636680326, 1.4181469983996315, 1.2387368592520112,
            1.5208379310729538, 1.2036224929766774, 1.486959684726482,
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function rad2deg() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = $a->rad2deg();

        $expected = Vector::fromArray([
            229.1831180523293, 372.42256683503507, 166.15776058793872,
            1145.9155902616465, 148.96902673401405, 681.8197762056797,
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function deg2rad() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = $a->deg2rad();

        $expected = Vector::fromArray([
            0.06981317007977318, 0.11344640137963141, 0.05061454830783556,
            0.3490658503988659, 0.04537856055185257, 0.2076941809873252,
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function sum() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEqualsWithDelta(177.0, $a->sum(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function product() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEqualsWithDelta(-14442510600000.0, $a->product(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function min() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEquals(-72.0, $a->min());
    }

    /**
     * @test
     */
    public function max() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEquals(106.0, $a->max());
    }

    /**
     * @test
     */
    public function argmin() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertSame(4, $a->argmin());
    }

    /**
     * @test
     */
    public function argminTie() : void
    {
        $a = Vector::fromArray([2.0, 1.0, 1.0, 3.0]);

        $this->assertSame(1, $a->argmin());
    }

    /**
     * @test
     */
    public function argminEmptyVector() : void
    {
        $a = Vector::fromArray([], false);

        $this->expectException(\InvalidArgumentException::class);

        $a->argmin();
    }

    /**
     * @test
     */
    public function argmax() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertSame(6, $a->argmax());
    }

    /**
     * @test
     */
    public function argmaxTie() : void
    {
        $a = Vector::fromArray([2.0, 3.0, 3.0, 1.0]);

        $this->assertSame(1, $a->argmax());
    }

    /**
     * @test
     */
    public function argmaxEmptyVector() : void
    {
        $a = Vector::fromArray([], false);

        $this->expectException(\InvalidArgumentException::class);

        $a->argmax();
    }

    /**
     * @test
     */
    public function mean() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEqualsWithDelta(22.125, $a->mean(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function median() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEquals(30.0, $a->median());
    }

    /**
     * @test
     */
    public function medianEmptyVectorThrows() : void
    {
        $a = Vector::fromArray([], false);

        $this->expectException(InvalidArgumentException::class);

        $a->median();
    }

    /**
     * @test
     */
    public function quantile() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEqualsWithDelta(30.0, $a->quantile(0.5), self::MAX_DELTA);
        $this->assertEqualsWithDelta(-72.0, $a->quantile(0.0), self::MAX_DELTA);
        $this->assertEqualsWithDelta(106.0, $a->quantile(1.0), self::MAX_DELTA);

        $single = Vector::fromArray([5.0]);

        $this->assertEqualsWithDelta(5.0, $single->quantile(0.0), self::MAX_DELTA);
        $this->assertEqualsWithDelta(5.0, $single->quantile(0.5), self::MAX_DELTA);
        $this->assertEqualsWithDelta(5.0, $single->quantile(1.0), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function quantileEmptyVectorThrows() : void
    {
        $a = Vector::fromArray([], false);

        $this->expectException(InvalidArgumentException::class);

        $a->quantile(0.5);
    }

    /**
     * @test
     */
    public function variance() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEqualsWithDelta(3227.609375, $a->variance(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function round() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = $a->round(2);

        $expected = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function floor() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = $a->floor();

        $expected = Vector::fromArray([4.0, 6.0, 2.0, 20.0, 2.0, 11.0]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function ceil() : void
    {
        $a = Vector::fromArray([4.0, 6.5, 2.9, 20.0, 2.6, 11.9]);

        $b = $a->ceil();

        $expected = Vector::fromArray([4.0, 7.0, 3.0, 20.0, 3.0, 12.0]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function l1Norm() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEqualsWithDelta(423.0, $a->l1Norm(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function l2Norm() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEqualsWithDelta(172.4441938715247, $a->l2Norm(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function pNorm() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEqualsWithDelta(135.15554088861361, $a->pNorm(3.0), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function maxNorm() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $this->assertEqualsWithDelta(106.0, $a->maxNorm(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function clip() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $b = $a->clip(0.0, 100.0);

        $expected = Vector::fromArray([0.0, 25.0, 35.0, 0.0, 0.0, 89.0, 100.0, 45.0]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function clipLower() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $b = $a->clipLower(60.0);

        $expected = Vector::fromArray([60.0, 60.0, 60.0, 60.0, 60.0, 89.0, 106.0, 60.0]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function clipUpper() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $b = $a->clipUpper(50.0);

        $expected = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 50.0, 50.0, 45.0]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function sign() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $b = $a->sign();

        $expected = Vector::fromArray([-1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function negate() : void
    {
        $a = Vector::fromArray([-15.0, 25.0, 35.0, -36.0, -72.0, 89.0, 106.0, 45.0]);

        $b = $a->negate();

        $expected = Vector::fromArray([15.0, -25.0, -35.0, 36.0, 72.0, -89.0, -106.0, -45.0]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function fillNegativeNThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Vector::fill(1.0, 0);
    }

    /**
     * @test
     */
    public function zerosNegativeNThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Vector::zeros(0);
    }

    /**
     * @test
     */
    public function onesNegativeNThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Vector::ones(0);
    }

    /**
     * @test
     */
    public function randNegativeNThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Vector::rand(0);
    }

    /**
     * @test
     */
    public function gaussianNegativeNThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Vector::gaussian(0);
    }

    /**
     * @test
     */
    public function uniformNegativeNThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Vector::uniform(0);
    }

    /**
     * @test
     */
    public function linspaceMinimumGreaterThanMaximumThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Vector::linspace(5.0, 1.0, 5);
    }

    /**
     * @test
     */
    public function linspaceTooFewElementsThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Vector::linspace(0.0, 1.0, 1);
    }

    /**
     * @test
     */
    public function reshapeSizeMismatchThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        (Vector::fromArray([1.0, 2.0, 3.0, 4.0]))->reshape(2, 3);
    }

    /**
     * @test
     */
    public function quantileOutOfRangeThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        (Vector::fromArray([1.0, 2.0, 3.0]))->quantile(-0.1);
    }

    /**
     * @test
     */
    public function quantileAboveOneThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        (Vector::fromArray([1.0, 2.0, 3.0]))->quantile(1.1);
    }

    /**
     * @test
     */
    public function quantileNaNThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        (Vector::fromArray([1.0, 2.0, 3.0]))->quantile(NAN);
    }

    /**
     * @test
     */
    public function pNormNonPositiveThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        (Vector::fromArray([1.0, 2.0, 3.0]))->pNorm(0.0);
    }

    /**
     * @test
     */
    public function dotDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        (Vector::fromArray([1.0, 2.0, 3.0]))->dot(Vector::fromArray([1.0, 2.0]));
    }

    /**
     * @test
     */
    public function multiplyDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        (Vector::fromArray([1.0, 2.0, 3.0]))->multiply(Vector::fromArray([1.0, 2.0]));
    }

    /**
     * @test
     */
    public function divideDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        (Vector::fromArray([1.0, 2.0, 3.0]))->divide(Vector::fromArray([1.0, 2.0]));
    }

    /**
     * @test
     */
    public function addDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        (Vector::fromArray([1.0, 2.0, 3.0]))->add(Vector::fromArray([1.0, 2.0]));
    }

    /**
     * @test
     */
    public function subtractDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        (Vector::fromArray([1.0, 2.0, 3.0]))->subtract(Vector::fromArray([1.0, 2.0]));
    }

    /**
     * @test
     */
    public function powDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        (Vector::fromArray([1.0, 2.0, 3.0]))->pow(Vector::fromArray([1.0, 2.0]));
    }

    /**
     * @test
     */
    public function modDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        (Vector::fromArray([1.0, 2.0, 3.0]))->mod(Vector::fromArray([1.0, 2.0]));
    }

    /**
     * @test
     * @dataProvider wrongOperandTypeProvider
     * @param callable $operation
     */
    public function arithmeticWithWrongOperandTypeThrows(callable $operation) : void
    {
        $this->expectException(InvalidArgumentException::class);

        $operation();
    }

    /**
     * @return Generator<callable[]|mixed[]>
     */
    public function wrongOperandTypeProvider() : Generator
    {
        yield 'multiply' => [function () {
            (Vector::fromArray([1.0, 2.0, 3.0]))->multiply('not a valid operand');
        }];

        yield 'divide' => [function () {
            (Vector::fromArray([1.0, 2.0, 3.0]))->divide('not a valid operand');
        }];

        yield 'add' => [function () {
            (Vector::fromArray([1.0, 2.0, 3.0]))->add('not a valid operand');
        }];

        yield 'subtract' => [function () {
            (Vector::fromArray([1.0, 2.0, 3.0]))->subtract('not a valid operand');
        }];

        yield 'pow' => [function () {
            (Vector::fromArray([1.0, 2.0, 3.0]))->pow('not a valid operand');
        }];

        yield 'mod' => [function () {
            (Vector::fromArray([1.0, 2.0, 3.0]))->mod('not a valid operand');
        }];

        yield 'equal' => [function () {
            (Vector::fromArray([1.0, 2.0, 3.0]))->equal('not a valid operand');
        }];

        yield 'notEqual' => [function () {
            (Vector::fromArray([1.0, 2.0, 3.0]))->notEqual('not a valid operand');
        }];

        yield 'greater' => [function () {
            (Vector::fromArray([1.0, 2.0, 3.0]))->greater('not a valid operand');
        }];

        yield 'greaterEqual' => [function () {
            (Vector::fromArray([1.0, 2.0, 3.0]))->greaterEqual('not a valid operand');
        }];

        yield 'less' => [function () {
            (Vector::fromArray([1.0, 2.0, 3.0]))->less('not a valid operand');
        }];

        yield 'lessEqual' => [function () {
            (Vector::fromArray([1.0, 2.0, 3.0]))->lessEqual('not a valid operand');
        }];
    }

    /**
     * @test
     */
    public function multiplyNumericString() : void
    {
        $a = Vector::fromArray([1.0, 2.0, 3.0]);

        $this->assertEqualsWithDelta(
            [2.0, 4.0, 6.0],
            $a->multiply('2')->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function addNumericString() : void
    {
        $a = Vector::fromArray([1.0, 2.0]);

        $this->assertEqualsWithDelta(
            [2.5, 3.5],
            $a->add('1.5')->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function powInfScalar() : void
    {
        $a = Vector::fromArray([1.0, 2.0]);

        $this->assertEqualsWithDelta(
            [1.0, INF],
            $a->pow(INF)->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function greaterEqualNumericString() : void
    {
        $a = Vector::fromArray([1.0, 2.0]);

        $this->assertEqualsWithDelta(
            [0, 1],
            $a->greaterEqual('1.5')->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function subtractNumericString() : void
    {
        $a = Vector::fromArray([5.0, 6.0]);

        $this->assertEqualsWithDelta(
            [4.5, 5.5],
            $a->subtract('0.5')->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function divideNumericString() : void
    {
        $a = Vector::fromArray([4.0, 6.0]);

        $this->assertEqualsWithDelta(
            [2.0, 3.0],
            $a->divide('2')->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function modNumericString() : void
    {
        $a = Vector::fromArray([4.0, 7.0]);

        $this->assertEqualsWithDelta(
            [1.0, 1.0],
            $a->mod('3')->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function greaterInfScalar() : void
    {
        $a = Vector::fromArray([1.0, INF]);

        $this->assertEqualsWithDelta(
            [0, 0],
            $a->greater(INF)->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function convolveStrideLessThanOneThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        (Vector::fromArray([1.0, 2.0, 3.0]))->convolve(Vector::fromArray([1.0, 1.0]), 0);
    }

    /**
     * @test
     */
    public function convolveKernelLargerThanVectorThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        (Vector::fromArray([1.0, 2.0]))->convolve(Vector::fromArray([1.0, 2.0, 3.0]));
    }

    /**
     * @test
     */
    public function convolveNegativePaddingThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        (Vector::fromArray([1.0, 2.0, 3.0]))->convolve(Vector::fromArray([1.0, 1.0]), 1, -1);
    }

    /**
     * Padding is what makes a kernel wider than its input legal, so a kernel
     * still too wide once the padding is counted has to be rejected rather than
     * reporting an output length of zero or less.
     *
     * @test
     */
    public function convolveKernelLargerThanPaddedVectorThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        // 3 + 2 * 1 = 5 padded samples still cannot take a six-tap kernel.
        (Vector::fromArray([1.0, 2.0, 3.0]))->convolve(Vector::fromArray([1.0, 2.0, 3.0, 4.0, 5.0, 6.0]), 1, 1);
    }

    /**
     * @test
     */
    public function offsetSetThrows() : void
    {
        $this->expectException(RuntimeException::class);

        $a = Vector::fromArray([1.0, 2.0, 3.0]);

        $a[0] = 4.0;
    }

    /**
     * @test
     */
    public function offsetUnsetThrows() : void
    {
        $this->expectException(RuntimeException::class);

        $a = Vector::fromArray([1.0, 2.0, 3.0]);

        unset($a[0]);
    }

    /**
     * @test
     */
    public function offsetGetOutOfBoundsThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        $a = Vector::fromArray([1.0, 2.0, 3.0]);

        $this->assertEquals(0.0, $a[10]);
    }

    /**
     * Naive "valid" convolution sampled every $stride samples, used as the
     * oracle for convolveMatchesReference().
     *
     * @param list<float> $a
     * @param list<float> $b
     * @param int $stride
     * @param int $padding
     *
     * @return list<float>
     */
    private function referenceConvolve1d(array $a, array $b, int $stride, int $padding = 0) : array
    {
        $na = count($a);
        $nb = count($b);
        $nc = intdiv($na + 2 * $padding - $nb, $stride) + 1;
        $out = [];

        for ($j = 0; $j < $nc; ++$j) {
            $sigma = 0.0;

            // Output $j is the $j-th of floor((na + 2 * padding - nb) / stride) + 1
            // outputs, and it reads the nb-sample window of the padded input
            // that starts $padding samples before the $j * stride-th one, against
            // the kernel reversed. A window reaching past either end of the input
            // contributes only the taps that do not.
            $start = $j * $stride - $padding;

            for ($k = 0; $k < $nb; ++$k) {
                $x = $start + $k;

                if ($x >= 0 && $x < $na) {
                    $sigma += $a[$x] * $b[$nb - 1 - $k];
                }
            }

            $out[] = $sigma;
        }

        return $out;
    }
}
