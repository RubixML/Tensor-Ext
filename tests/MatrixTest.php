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
use Tensor\Reductions\REF;
use Tensor\Reductions\RREF;
use Tensor\Decompositions\LU;
use Tensor\Exceptions\RuntimeException;
use Tensor\Exceptions\InvalidArgumentException;
use Tensor\Exceptions\DimensionalityMismatch;
use Tensor\Decompositions\SVD;
use Tensor\Decompositions\Eigen;
use Tensor\Decompositions\Cholesky;
use Tensor\Buffer;
use Tensor\TensorBuffer;
use InvalidArgumentException as SplInvalidArgumentException;
use LengthException;
use PHPUnit\Framework\TestCase;
use Generator;
use ReflectionMethod;

/**
 * @covers \Tensor\Matrix
 */
class MatrixTest extends TestCase
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
        $matrix = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $this->assertInstanceOf(Matrix::class, $matrix);
        $this->assertInstanceOf(Tensor::class, $matrix);
        $this->assertInstanceOf(ArrayLike::class, $matrix);
        $this->assertInstanceOf(Arithmetic::class, $matrix);
        $this->assertInstanceOf(Comparable::class, $matrix);
        $this->assertInstanceOf(Unary::class, $matrix);
        $this->assertInstanceOf(Trigonometric::class, $matrix);
        $this->assertInstanceOf(Special::class, $matrix);
        $this->assertInstanceOf(Reductions::class, $matrix);
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
            $matrix = Matrix::build([
                [1.0, 2.0, 3.0],
                [4.0, 5.0, 6.0],
            ]);

            $this->assertInstanceOf(Matrix::class, $matrix);
        } finally {
            restore_error_handler();
        }

        $this->assertCount(1, $notices);
        $this->assertStringContainsString('Matrix::build()', $notices[0]);
        $this->assertStringContainsString('Matrix::fromArray', $notices[0]);
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
            $matrix = Matrix::quick([
                [1.0, 2.0, 3.0],
                [4.0, 5.0, 6.0],
            ]);

            $this->assertInstanceOf(Matrix::class, $matrix);
        } finally {
            restore_error_handler();
        }

        $this->assertCount(1, $notices);
        $this->assertStringContainsString('Matrix::quick()', $notices[0]);
        $this->assertStringContainsString('Matrix::fromArray', $notices[0]);
    }

    /**
     * @test
     */
    public function buildCastsIntegersToFloatsAndPreservesShape() : void
    {
        $matrix = Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [4.0, 5.0, 6.0],
        ]);

        $this->assertSame([2, 3], $matrix->shape());
        $this->assertSame(6, $matrix->size());

        $result = $matrix->asArray();

        $this->assertCount(2, $result);

        foreach ($result as $row) {
            $this->assertCount(3, $row);

            foreach ($row as $value) {
                $this->assertTrue(is_float($value));
            }
        }

        $this->assertEqualsWithDelta([
            [1.0, 2.0, 3.0],
            [4.0, 5.0, 6.0],
        ], $result, self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function fromBuffer() : void
    {
        $buffer = new TensorBuffer(Buffer::fromArray([
            1.0, 2.0, 3.0,
            4.0, 5.0, 6.0,
        ]));

        $matrix = Matrix::fromBuffer($buffer, 2, 3);

        $this->assertInstanceOf(Matrix::class, $matrix);
        $this->assertSame($buffer, $matrix->buffer());
        $this->assertSame([2, 3], $matrix->shape());
        $this->assertEqualsWithDelta([
            [1.0, 2.0, 3.0],
            [4.0, 5.0, 6.0],
        ], $matrix->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function fromBufferThrowsOnBufferSizeMismatch() : void
    {
        $this->expectException(InvalidArgumentException::class);

        $buffer = new TensorBuffer(Buffer::fromArray([1.0, 2.0, 3.0, 4.0, 5.0, 6.0]));

        Matrix::fromBuffer($buffer, 2, 2);
    }

    /**
     * @test
     */
    public function fromBufferRejectsIntegerBuffers() : void
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage('Argument must wrap a buffer of type double.');

        $buffer = new TensorBuffer(Buffer::fromArray([1, 2, 3, 4, 5, 6], Buffer::TYPE_LONG));

        Matrix::fromBuffer($buffer, 2, 3);
    }

    /**
     * Integer buffers remain a valid *buffer* kind for the structural
     * operations; only the numeric layer refuses them. This guards the
     * distinction the two rejections above depend on.
     *
     * @test
     */
    public function integerBuffersRemainUsableForStructuralOperations() : void
    {
        $buffer = new TensorBuffer(Buffer::fromArray([3, 1, 2], Buffer::TYPE_LONG));

        $this->assertSame(Buffer::TYPE_LONG, $buffer->type());
        $this->assertEquals([3, 1], $buffer->slice(0, 2)->asBuffer()->toArray());
        $this->assertEquals([3, 1, 2, 3, 1, 2], $buffer->repeat(2)->asBuffer()->toArray());

        $buffer->sort();

        $this->assertEquals([1, 2, 3], $buffer->asBuffer()->toArray());
    }

    /**
     * @test
     */
    public function constructorIsProtected() : void
    {
        $this->assertTrue((new ReflectionMethod(Matrix::class, '__construct'))->isProtected());
    }

    /**
     * @test
     */
    public function identity() : void
    {
        $matrix = Matrix::identity(4);

        $expected = Matrix::fromArray([
            [1.0, 0.0, 0.0, 0.0],
            [0.0, 1.0, 0.0, 0.0],
            [0.0, 0.0, 1.0, 0.0],
            [0.0, 0.0, 0.0, 1.0],
        ]);

        $this->assertEquals($expected->asArray(), $matrix->asArray());
    }

    /**
     * @test
     */
    public function zeros() : void
    {
        $matrix = Matrix::zeros(2, 4);

        $expected = Matrix::fromArray([
            [0.0, 0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0, 0.0],
        ]);

        $this->assertEquals($expected->asArray(), $matrix->asArray());
    }

    /**
     * @test
     */
    public function ones() : void
    {
        $matrix = Matrix::ones(4, 2);

        $expected = Matrix::fromArray([
            [1.0, 1.0],
            [1.0, 1.0],
            [1.0, 1.0],
            [1.0, 1.0],
        ]);

        $this->assertEquals($expected->asArray(), $matrix->asArray());
    }

    /**
     * @test
     */
    public function diagonal() : void
    {
        $matrix = Matrix::diagonal([0.0, 1.0, 4.0, 5.0]);

        $expected = Matrix::fromArray([
            [0.0, 0.0, 0.0, 0.0],
            [0.0, 1.0, 0.0, 0.0],
            [0.0, 0.0, 4.0, 0.0],
            [0.0, 0.0, 0.0, 5.0],
        ]);

        $this->assertEquals($expected->asArray(), $matrix->asArray());
    }

    /**
     * @test
     */
    public function diagonalEmpty() : void
    {
        $matrix = Matrix::diagonal([]);

        $this->assertSame([0, 0], $matrix->shape());
        $this->assertSame([], $matrix->asArray());
    }

    /**
     * @test
     */
    public function fill() : void
    {
        $matrix = Matrix::fill(5.0, 4, 4);

        $expected = Matrix::fromArray([
            [5.0, 5.0, 5.0, 5.0],
            [5.0, 5.0, 5.0, 5.0],
            [5.0, 5.0, 5.0, 5.0],
            [5.0, 5.0, 5.0, 5.0],
        ]);

        $this->assertEquals($expected->asArray(), $matrix->asArray());
    }

    /**
     * @test
     */
    public function rand() : void
    {
        $matrix = Matrix::rand(4, 4);

        $this->assertCount(16, $matrix);
    }

    /**
     * @test
     */
    public function gaussian() : void
    {
        $matrix = Matrix::gaussian(3, 3);

        $this->assertCount(9, $matrix);
    }

    /**
     * @test
     */
    public function uniform() : void
    {
        $matrix = Matrix::uniform(3, 3);

        $this->assertCount(9, $matrix);
    }

    /**
     * @test
     */
    public function randHasCorrectBounds() : void
    {
        $flat = $this->toFlatList(Matrix::rand(100, 100));
        $this->assertCount(10000, $flat);

        $this->assertGreaterThanOrEqual(0.0, min($flat));
        $this->assertTrue(max($flat) < 1.0);
    }

    /**
     * @test
     */
    public function randMeanIsCloseToHalf() : void
    {
        $flat = $this->toFlatList(Matrix::rand(100, 100));

        $this->assertEqualsWithDelta(0.5, array_sum($flat) / count($flat), 0.05);
    }

    /**
     * @test
     */
    public function uniformHasCorrectBounds() : void
    {
        $flat = $this->toFlatList(Matrix::uniform(100, 100));

        $this->assertGreaterThanOrEqual(-1.0, min($flat));
        $this->assertLessThanOrEqual(1.0, max($flat));
    }

    /**
     * @test
     */
    public function uniformMeanIsCloseToZero() : void
    {
        $flat = $this->toFlatList(Matrix::uniform(100, 100));

        $this->assertEqualsWithDelta(0.0, array_sum($flat) / count($flat), 0.05);
    }

    /**
     * @test
     */
    public function uniformVarianceIsUnitScale() : void
    {
        $flat = $this->toFlatList(Matrix::uniform(100, 100));
        $n = count($flat);
        $mean = array_sum($flat) / $n;
        $var = array_sum(array_map(function ($x) use ($mean) {
            return ($x - $mean) * ($x - $mean);
        }, $flat)) / $n;

        $this->assertEqualsWithDelta(1.0 / 3.0, $var, 0.1);
    }

    /**
     * @test
     */
    public function gaussianHasZeroMeanAndUnitVariance() : void
    {
        $flat = $this->toFlatList(Matrix::gaussian(100, 100));
        $n = count($flat);
        $mean = array_sum($flat) / $n;
        $var = array_sum(array_map(function ($x) use ($mean) {
            return ($x - $mean) * ($x - $mean);
        }, $flat)) / $n;

        $this->assertEqualsWithDelta(0.0, $mean, 0.05);
        $this->assertEqualsWithDelta(1.0, $var, 0.1);
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
        mt_srand(7777);
        $a = Matrix::rand(3, 3)->asArray();

        mt_srand(7777);
        $b = Matrix::rand(3, 3)->asArray();

        $this->assertEquals($a, $b);
    }

    /**
     * @test
     */
    public function gaussianIsReproducibleUnderMtSrand() : void
    {
        mt_srand(7777);
        $a = Matrix::gaussian(3, 3)->asArray();

        mt_srand(7777);
        $b = Matrix::gaussian(3, 3)->asArray();

        $this->assertEquals($a, $b);
    }

    /**
     * @test
     */
    public function uniformIsReproducibleUnderMtSrand() : void
    {
        mt_srand(7777);
        $a = Matrix::uniform(3, 3)->asArray();

        mt_srand(7777);
        $b = Matrix::uniform(3, 3)->asArray();

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
        mt_srand(7777);
        $a = Matrix::rand(3, 3)->asArray();

        mt_srand(7778);
        $b = Matrix::rand(3, 3)->asArray();

        $this->assertNotEquals($a, $b);
    }

    /**
     * @test
     */
    public function randNegativeMThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::rand(0, 3);
    }

    /**
     * @test
     */
    public function randNegativeNThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::rand(3, 0);
    }

    /**
     * @test
     */
    public function gaussianNegativeMThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::gaussian(0, 3);
    }

    /**
     * @test
     */
    public function gaussianNegativeNThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::gaussian(3, 0);
    }

    /**
     * @test
     */
    public function uniformNegativeMThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::uniform(0, 3);
    }

    /**
     * @test
     */
    public function uniformNegativeNThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::uniform(3, 0);
    }

    /**
     * @test
     */
    public function shape() : void
    {
        $matrix = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $this->assertEquals([3, 3], $matrix->shape());
    }

    /**
     * @test
     */
    public function shapeString() : void
    {
        $matrix = Matrix::fromArray([
            [22.0, -17.0, 12.0, 16.0],
            [4.0, 11.0, -2.0, 18.0],
        ]);

        $this->assertEquals('2 x 4', $matrix->shapeString());
    }

    /**
     * @test
     * @dataProvider isSquareProvider
     *
     * @param Matrix $matrix
     * @param bool $expected
     */
    public function isSquare(Matrix $matrix, $expected) : void
    {
        $this->assertEquals($expected, $matrix->isSquare());
    }

    /**
     * @return Generator<mixed[]>
     */
    public function isSquareProvider() : Generator
    {
        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            true,
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0, 16.0],
                [4.0, 11.0, -2.0, 18.0],
            ]),
            false,
        ];
    }

    /**
     * @test
     */
    public function size() : void
    {
        $matrix = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $this->assertEquals(9, $matrix->size());
    }

    /**
     * @test
     */
    public function m() : void
    {
        $matrix = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $this->assertEquals(3, $matrix->m());
    }

    /**
     * @test
     */
    public function n() : void
    {
        $matrix = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $this->assertEquals(3, $matrix->n());
    }

    /**
     * @test
     */
    public function rowAsVector() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->rowAsVector(1);

        $expected = Vector::fromArray([4.0, 11.0, -2.0]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function columnAsVector() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->columnAsVector(1);

        $expected = ColumnVector::fromArray([-17.0, 11.0, -6.0]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function diagonalAsVector() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->diagonalAsVector();

        $expected = Vector::fromArray([22.0, 11.0, -9.0]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function asArray() : void
    {
        $matrix = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $expected = [
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ];

        $this->assertEquals($expected, $matrix->asArray());
    }

    /**
     * @test
     */
    public function serialization() : void
    {
        $matrix = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
        ]);

        $serialized = serialize($matrix);

        $this->assertStringNotContainsString('TensorBuffer', $serialized);

        $restored = unserialize($serialized);

        $this->assertInstanceOf(Matrix::class, $restored);
        $this->assertEquals([2, 3], $restored->shape());
        $this->assertEquals([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
        ], $restored->asArray());
        $this->assertSame(serialize($matrix), serialize($restored));
    }

    /**
     * @test
     */
    public function asVectors() : void
    {
        $matrix = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $vectors = $matrix->asVectors();

        $expected = [
            Vector::fromArray([22.0, -17.0, 12.0]),
            Vector::fromArray([4.0, 11.0, -2.0]),
            Vector::fromArray([20.0, -6.0, -9.0]),
        ];

        $this->assertEquals(
            array_map(static fn (Vector $vector) => $vector->asArray(), $expected),
            array_map(static fn (Vector $vector) => $vector->asArray(), $vectors)
        );
    }

    /**
     * @test
     */
    public function asColumnVectors() : void
    {
        $matrix = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $vectors = $matrix->asColumnVectors();

        $expected = [
            ColumnVector::fromArray([22.0, 4.0, 20.0]),
            ColumnVector::fromArray([-17.0, 11.0, -6.0]),
            ColumnVector::fromArray([12.0, -2.0, -9.0]),
        ];

        $this->assertEquals(
            array_map(static fn (ColumnVector $vector) => $vector->asArray(), $expected),
            array_map(static fn (ColumnVector $vector) => $vector->asArray(), $vectors)
        );
    }

    /**
     * @test
     */
    public function flatten() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->flatten();

        $expected = Vector::fromArray([22.0, -17.0, 12.0, 4.0, 11.0, -2.0, 20.0, -6.0, -9.0]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function transpose() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->transpose();

        $expected = Matrix::fromArray([
            [22.0, 4.0, 20.0],
            [-17.0, 11.0, -6.0],
            [12.0, -2.0, -9.0],
        ]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * Regression test: passing an array callable [instance, 'method'] to
     * Matrix::map() must not segfault. The previous Zephir dynamic
     * user-callback dispatch inside TensorBuffer::map() was triggered only
     * for arrays holding a non-static instance and crashed on the second
     * and later elements.
     *
     * @test
     */
    public function mapWithInstanceArrayCallable() : void
    {
        $helper = new class() {
            public float $slope = 0.1;

            public function activate(float $v) : float
            {
                return $v > 0.0 ? $v : $v * $this->slope;
            }
        };

        $a = Matrix::fromArray([
            [1.0, -2.0],
            [-3.0, 4.0],
        ]);

        $b = $a->map([$helper, 'activate']);

        $expected = Matrix::fromArray([
            [1.0, -0.2],
            [-0.3, 4.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * A static array callable [Class::class, 'method'] must also work on
     * Matrix::map() over a multi-element matrix.
     *
     * @test
     */
    public function mapWithStaticArrayCallable() : void
    {
        $a = Matrix::fromArray([
            [7.0, -2.5],
            [-3.0, 4.25],
        ]);

        $helper = new class() {
            public static function square(float $v) : float
            {
                return $v * $v;
            }
        };

        $b = $a->map([get_class($helper), 'square']);

        $expected = Matrix::fromArray([
            [49.0, 6.25],
            [9.0, 18.0625],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function inverse() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->inverse();

        $expected = Matrix::fromArray([
            [0.02093549603923048, 0.042436816295737464, 0.018483591097698978],
            [0.0007544322897019996, 0.08261033572236892, -0.017351942663145988],
            [0.04602036967182196, 0.03923047906450396, -0.05846850245190495],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function inverseNonSquareThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
            [5.0, 6.0],
        ])->inverse();
    }

    /**
     * @test
     */
    public function inverseSingularThrows() : void
    {
        // Exactly singular (column 3 = column 0 - column 1 + column 2); the
        // inverse must be rejected rather than return a magnitude ~1e15 matrix
        // that does not satisfy A * A^-1 = I.
        $a = Matrix::fromArray([
            [2.0, 1.0, 0.0, 1.0],
            [1.0, 2.0, 1.0, 0.0],
            [0.0, 1.0, 2.0, 1.0],
            [1.0, 0.0, 1.0, 2.0],
        ]);

        $this->expectException(RuntimeException::class);

        $a->inverse();
    }

    /**
     * @test
     */
    public function pseudoinverse() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
        ]);

        $b = $a->pseudoinverse();

        $expected = Matrix::fromArray([
            [0.03147992432205172, 0.05583000490505223],
            [-0.009144418751313844, 0.07003713825239999],
            [0.01266554551187723, -0.0031357298016957483],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function det() : void
    {
        $a = Matrix::fromArray([
            [4.0, 6.0, -12.0],
            [1.0, 3.0, 5.0],
            [-10.0, -1.0, 14.0],
        ]);

        $this->assertEqualsWithDelta(-544.0, $a->det(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function detSingularIsZero() : void
    {
        // Exactly singular (column 3 = column 0 - column 1 + column 2); the
        // determinant must be zero rather than a spurious ~1e-15 value, and now
        // is zero exactly: see detOfSingularMatrixIsExactlyZero.
        $a = Matrix::fromArray([
            [2.0, 1.0, 0.0, 1.0],
            [1.0, 2.0, 1.0, 0.0],
            [0.0, 1.0, 2.0, 1.0],
            [1.0, 0.0, 1.0, 2.0],
        ]);

        $this->assertSame(0.0, $a->det());
    }

    /**
     * @test
     */
    public function trace() : void
    {
        $a = Matrix::fromArray([
            [4.0, 6.0, -12.0],
            [1.0, 3.0, 5.0],
            [-10.0, -1.0, 14.0],
        ]);

        $this->assertEquals(21.0, $a->trace());
    }

    /**
     * @test
     * @dataProvider symmetricProvider
     *
     * @param Matrix $matrix
     * @param bool $expected
     */
    public function symmetric(Matrix $matrix, $expected) : void
    {
        $this->assertEquals($expected, $matrix->symmetric());
    }

    /**
     * @return Generator<mixed[]>
     */
    public function symmetricProvider() : Generator
    {
        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            false,
        ];

        yield [
            Matrix::fromArray([
                [1.0, 5.0, 2.0],
                [5.0, 1.0, 3.0],
                [2.0, 3.0, 1.0],
            ]),
            true,
        ];

        yield [
            Matrix::fromArray([]),
            true,
        ];

        yield [
            Matrix::fromArray([
                [5.0],
            ]),
            true,
        ];
    }

    /**
     * @test
     */
    public function rank() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $this->assertEquals(3, $a->rank());

        $b = Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [4.0, 5.0, 6.0],
        ]);

        $this->assertEquals(2, $b->rank());

        // A tall (m > n) matrix over-reads the pivot array when counting
        // swaps; the rank must be reported without faulting.
        $tall = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
            [5.0, 6.0],
        ]);

        $this->assertEquals(2, $tall->rank());

        // A 2x1 tall matrix has rank 1.
        $narrow = Matrix::fromArray([
            [1.0],
            [2.0],
        ]);

        $this->assertEquals(1, $narrow->rank());

        // An m x 0 matrix has no columns and therefore rank 0.
        $empty = Matrix::fromArray([[], []]);

        $this->assertEquals(0, $empty->rank());

        // Exactly singular (column 3 = column 0 - column 1 + column 2); the
        // rank must be 3, not 4, even though floating point leaves a ~1e-16
        // residual on the diagonal.
        $c = Matrix::fromArray([
            [2.0, 1.0, 0.0, 1.0],
            [1.0, 2.0, 1.0, 0.0],
            [0.0, 1.0, 2.0, 1.0],
            [1.0, 0.0, 1.0, 2.0],
        ]);

        $this->assertEquals(3, $c->rank());

        // A tall (m > n) matrix of full column rank is reported as min(m, n).
        $rowDependent = Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [2.0, 4.0, 7.0],
            [0.0, 1.0, 0.0],
            [3.0, 6.0, 10.0],
        ]);

        $this->assertEquals(3, $rowDependent->rank());

        // A wide (m < n) matrix of full row rank is reported as min(m, n).
        $wide = Matrix::fromArray([
            [3.0, 3.0, 7.0, 11.0, 13.0],
            [5.0, 4.0, 11.0, 18.0, 21.0],
            [4.0, 7.0, 5.0, 10.0, 14.0],
        ]);

        $this->assertEquals(3, $wide->rank());

        // Tall and singular (column 2 = column 0 + column 1), so the rank is
        // one less than the number of columns.
        $singularTall = Matrix::fromArray([
            [1.0, 0.0, 1.0],
            [0.0, 1.0, 1.0],
            [1.0, 1.0, 2.0],
            [2.0, 1.0, 3.0],
        ]);

        $this->assertEquals(2, $singularTall->rank());
    }

    /**
     * @test
     */
    public function fullRank() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $this->assertTrue($a->fullRank());

        $b = Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [4.0, 5.0, 6.0],
        ]);

        $this->assertTrue($b->fullRank());

        // A tall (m > n) matrix is full rank when its rank equals min(m, n).
        $tall = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
            [5.0, 6.0],
        ]);

        $this->assertTrue($tall->fullRank());

        // An m x 0 matrix has rank 0 === min(m, 0).
        $empty = Matrix::fromArray([[], []]);

        $this->assertTrue($empty->fullRank());

        // Exactly singular 4x4 (see rank() above); fullRank() must be false.
        $c = Matrix::fromArray([
            [2.0, 1.0, 0.0, 1.0],
            [1.0, 2.0, 1.0, 0.0],
            [0.0, 1.0, 2.0, 1.0],
            [1.0, 0.0, 1.0, 2.0],
        ]);

        $this->assertFalse($c->fullRank());

        // A tall matrix of full column rank is full rank, dependency of the
        // rows notwithstanding (see rank() above).
        $rowDependent = Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [2.0, 4.0, 7.0],
            [0.0, 1.0, 0.0],
            [3.0, 6.0, 10.0],
        ]);

        $this->assertTrue($rowDependent->fullRank());

        // A wide matrix of full row rank is full rank.
        $wide = Matrix::fromArray([
            [3.0, 3.0, 7.0, 11.0, 13.0],
            [5.0, 4.0, 11.0, 18.0, 21.0],
            [4.0, 7.0, 5.0, 10.0, 14.0],
        ]);

        $this->assertTrue($wide->fullRank());

        // Tall and singular (see rank() above); fullRank() must be false.
        $singularTall = Matrix::fromArray([
            [1.0, 0.0, 1.0],
            [0.0, 1.0, 1.0],
            [1.0, 1.0, 2.0],
            [2.0, 1.0, 3.0],
        ]);

        $this->assertFalse($singularTall->fullRank());
    }

    /**
     * @test
     */
    public function rankOfZeroMatrixIsZero() : void
    {
        // A matrix of zeros has no pivot in any column. Its rank is zero, it is
        // not full rank, and its determinant is zero -- the reduced form is the
        // zero matrix rather than a division of zero pivots.
        $zeros = Matrix::zeros(3, 3);

        $this->assertSame(0, $zeros->rank());
        $this->assertFalse($zeros->fullRank());
        $this->assertSame(0.0, $zeros->det());
        $this->assertEquals(Matrix::zeros(3, 3)->asArray(), $zeros->rref()->a()->asArray());

        $tall = Matrix::zeros(4, 2);

        $this->assertSame(0, $tall->rank());
        $this->assertFalse($tall->fullRank());

        $wide = Matrix::zeros(2, 4);

        $this->assertSame(0, $wide->rank());
        $this->assertFalse($wide->fullRank());
    }

    /**
     * @test
     */
    public function rankIsRelativeToTheScaleOfTheMatrix() : void
    {
        // A matrix is judged against the size of its own entries, so a matrix on
        // the order of 1e-10 is as invertible as one on the order of 1e10. A
        // threshold that did not scale with the matrix would call the first of
        // these singular and hand back a determinant of zero.
        $rows = [
            [1.0e-10, 2.0e-10],
            [3.0e-10, 4.0e-10],
        ];

        $small = Matrix::fromArray($rows, false);

        $this->assertSame(2, $small->rank());
        $this->assertTrue($small->fullRank());
        $this->assertEqualsWithDelta(-2.0e-20, $small->det(), 1.0e-32);

        // The identity has entries of order 1 while the inverse it is multiplied
        // with has entries of order 1e10, so this is a check about relative
        // accuracy: the products cancel to zero from terms of order 1e10.
        $this->assertMatricesEqualRelative(Matrix::identity(2), $small->matmul($small->inverse()), 1.0e-12);

        $large = Matrix::fromArray([
            [1.0e10, 2.0e10],
            [3.0e10, 4.0e10],
        ], false);

        $this->assertSame(2, $large->rank());
        $this->assertTrue($large->fullRank());
        $this->assertEqualsWithDelta(-2.0e20, $large->det(), 1.0e8);
        $this->assertMatricesEqualRelative(Matrix::identity(2), $large->matmul($large->inverse()), 1.0e-12);

        // The same matrix at both scales is the same matrix, so the same rank.
        foreach ([1.0e-10, 1.0, 1.0e10] as $scale) {
            $scaled = Matrix::fromArray([
                [1.0 * $scale, 2.0 * $scale],
                [3.0 * $scale, 4.0 * $scale],
            ], false);

            $this->assertSame(2, $scaled->rank());
            $this->assertTrue($scaled->fullRank());
        }
    }

    /**
     * @test
     */
    public function rankIgnoresAnEntryBelowTheRelativeTolerance() : void
    {
        // A pivot of order 1e-17 in a matrix whose largest entry is 1 sits below
        // the tolerance, so the matrix is the rank one matrix it is written as
        // and its determinant is zero rather than 1e-17.
        $noise = Matrix::fromArray([
            [1.0, 1.0],
            [1.0, 1.0 + 1.0e-17],
        ], false);

        $this->assertSame(1, $noise->rank());
        $this->assertFalse($noise->fullRank());
        $this->assertSame(0.0, $noise->det());

        // A pivot of order 1e-9 is above it, and the matrix is full rank.
        $signal = Matrix::fromArray([
            [1.0, 1.0],
            [1.0, 1.0 + 1.0e-9],
        ], false);

        $this->assertSame(2, $signal->rank());
        $this->assertTrue($signal->fullRank());
        $this->assertEqualsWithDelta(1.0e-9, $signal->det(), 1.0e-13);

        // The same holds once the matrix is scaled up: the noise moves with the
        // matrix rather than being pinned to an absolute threshold.
        $scaledNoise = Matrix::fromArray([
            [1.0e10, 1.0e10],
            [1.0e10, 1.0e10 + 1.0e-10],
        ], false);

        $this->assertSame(1, $scaledNoise->rank());
        $this->assertFalse($scaledNoise->fullRank());
    }

    /**
     * @test
     */
    public function detOfSingularMatrixIsExactlyZero() : void
    {
        // The elimination leaves a pivot of order 1e-16 in place of a zero one,
        // and the determinant is the product of the pivots, so a determinant that
        // is merely close to zero means the residual was left in the product. The
        // pivot is below the tolerance, so it is not a pivot, so the product is
        // zero exactly.
        $a = Matrix::fromArray([
            [2.0, 1.0, 0.0, 1.0],
            [1.0, 2.0, 1.0, 0.0],
            [0.0, 1.0, 2.0, 1.0],
            [1.0, 0.0, 1.0, 2.0],
        ], false);

        $this->assertSame(0.0, $a->det());

        $b = Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [2.0, 4.0, 6.0],
            [0.0, 1.0, 0.0],
        ], false);

        $this->assertSame(0.0, $b->det());

        // Scaled up by 1e9, where the residual of the elimination is as large as
        // 1e-7 and would pass for a determinant of its own.
        $c = Matrix::fromArray([
            [1.0e9, 2.0e9, 3.0e9],
            [4.0e9, 5.0e9, 6.0e9],
            [5.0e9, 7.0e9, 9.0e9],
        ], false);

        $this->assertSame(0.0, $c->det());
        $this->assertSame(2, $c->rank());
    }

    /**
     * @test
     */
    public function rrefOfRankDeficientMatrixHasZeroRows() : void
    {
        // The rows below the last pivot of a reduced row echelon form are zero
        // rows, and are written as exact zeros rather than as the roundoff the
        // elimination left behind -- the rank of the form is counted from them.
        $a = Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [2.0, 4.0, 6.0],
            [0.0, 1.0, 0.0],
        ], false);

        $this->assertEqualsWithDelta(Matrix::fromArray([
            [1.0, 0.0, 3.0],
            [0.0, 1.0, 0.0],
            [0.0, 0.0, 0.0],
        ], false)->asArray(), $a->rref()->a()->asArray(), self::MAX_DELTA);

        $this->assertSame(2, $a->rank());

        $b = Matrix::fromArray([
            [1.0e9, 2.0e9, 3.0e9],
            [4.0e9, 5.0e9, 6.0e9],
            [5.0e9, 7.0e9, 9.0e9],
        ], false);

        $this->assertEqualsWithDelta(Matrix::fromArray([
            [1.0, 0.0, -1.0],
            [0.0, 1.0, 2.0],
            [0.0, 0.0, 0.0],
        ], false)->asArray(), $b->rref()->a()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function reciprocal() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->reciprocal();

        $expected = Matrix::fromArray([
            [0.045454545454545456, -0.058823529411764705, 0.08333333333333333],
            [0.25, 0.09090909090909091, -0.5],
            [0.05, -0.16666666666666666, -0.1111111111111111],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function map() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $sign = function ($value) {
            return $value >= 0.0 ? 1 : -1;
        };

        $b = $a->map($sign);

        $expected = Matrix::fromArray([
            [1.0, -1.0, 1.0],
            [1.0, 1.0, -1.0],
            [1.0, -1.0, -1.0],
        ]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function reduce() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]);

        $sum = function ($carry, $value) {
            return $carry + $value;
        };

        $this->assertEqualsWithDelta(10.0, $a->reduce($sum), self::MAX_DELTA);

        // Asymmetric callback: pins the (carry, value) argument order.
        $subtract = function ($carry, $value) {
            return $carry - $value;
        };

        $this->assertEqualsWithDelta(-10.0, $a->reduce($subtract), self::MAX_DELTA);
        $this->assertEqualsWithDelta(-8.0, $a->reduce($subtract, 2.0), self::MAX_DELTA);

        // Must match Vector::reduce() for the same data and callback.
        $v = Vector::fromArray([1.0, 2.0, 3.0, 4.0]);

        $this->assertEqualsWithDelta($v->reduce($subtract), $a->reduce($subtract), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function ref() : void
    {
        $matrix = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $ref = $matrix->ref();

        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [0.0, 14.09090909090909, -4.181818181818182],
            [0.0, 0.0, -17.10322580645161],
        ]);

        $expected = new REF($a, 0);

        $this->assertEqualsWithDelta($a->asArray(), $ref->a()->asArray(), self::MAX_DELTA);
        $this->assertEquals(0, $ref->swaps());
    }

    /**
     * @test
     */
    public function rref() : void
    {
        $matrix = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $rref = $matrix->rref();

        $a = Matrix::fromArray([
            [1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            [0.0, 0.0, 1.0],
        ]);

        $expected = new RREF($a);

        $this->assertEqualsWithDelta($a->asArray(), $rref->a()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function lu() : void
    {
        $matrix = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $lu = $matrix->lu();

        $l = Matrix::fromArray([
            [1.0, 0.0, 0.0],
            [0.18181818181818182, 1.0, 0.0],
            [0.9090909090909091, 0.6709677419354838, 1.0],
        ]);

        $u = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [0.0, 14.09090909090909, -4.181818181818182],
            [0.0, 0.0, -17.10322580645161],
        ]);

        $p = Matrix::fromArray([
            [1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            [0.0, 0.0, 1.0],
        ]);

        $expected = new LU($l, $u, $p);

        $this->assertEqualsWithDelta($l->asArray(), $lu->l()->asArray(), self::MAX_DELTA);
        $this->assertEqualsWithDelta($u->asArray(), $lu->u()->asArray(), self::MAX_DELTA);
        $this->assertEqualsWithDelta($p->asArray(), $lu->p()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function luMultiPivot() : void
    {
        $matrix = Matrix::fromArray([
            [0.0,  1.0,  0.0,  0.0],
            [-1.0,  0.0,  0.0,  0.0],
            [0.0,  0.0,  0.0,  2.0],
            [0.0,  0.0,  3.0,  1.0],
        ]);

        $lu = $matrix->lu();

        $pa = $lu->p()->matmul($matrix);
        $luProd = $lu->l()->matmul($lu->u());

        $this->assertEqualsWithDelta($pa->asArray(), $luProd->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function luNegativePivot() : void
    {
        $matrix = Matrix::fromArray([
            [1.0,  2.0,  3.0,  4.0],
            [-9.0,  1.0,  0.0,  0.0],
            [0.5,  0.5,  1.0,  1.0],
            [0.1,  0.2,  0.3,  0.4],
        ]);

        $lu = $matrix->lu();

        $pa = $lu->p()->matmul($matrix);
        $luProd = $lu->l()->matmul($lu->u());

        $this->assertEqualsWithDelta($pa->asArray(), $luProd->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function luSingular() : void
    {
        $this->expectException(RuntimeException::class);

        $matrix = Matrix::fromArray([
            [1.0, 2.0,  0.0,  0.0],
            [0.0, 1.0,  1.0,  0.0],
            [2.0, 4.0,  0.0,  1.0],
            [0.0, 1.0,  1.0,  0.0],
        ]);

        $matrix->lu();
    }

    /**
     * @test
     */
    public function cholesky() : void
    {
        $matrix = Matrix::fromArray([
            [2.0, -1.0, 0.0],
            [-1.0, 2.0, -1.0],
            [0.0, -1.0, 2.0],
        ]);

        $cholesky = $matrix->cholesky();

        $l = Matrix::fromArray([
            [1.4142135623730951, 0.0, 0.0],
            [-0.7071067811865475, 1.224744871391589, 0.0],
            [0.0, -0.8164965809277261, 1.1547005383792515],
        ]);

        $expected = new Cholesky($l);

        $this->assertEqualsWithDelta($l->asArray(), $cholesky->l()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     * @dataProvider eigProvider
     *
     * @param Matrix $matrix
     * @param Eigen $expected
     */
    public function eig(Matrix $matrix, Eigen $expected) : void
    {
        $eig = $matrix->eig(false);

        $this->assertEqualsWithDelta($expected->eigenvalues()->asArray(), $eig->eigenvalues()->asArray(), self::MAX_DELTA);

        // The test matrix has real eigenvalues, so the imaginary parts must be zero.
        $this->assertEqualsWithDelta(array_fill(0, $matrix->n(), 0.0), $eig->eigenvaluesImaginary()->asArray(), self::MAX_DELTA);

        $this->assertEqualsWithDelta($expected->eigenvectors()->asArray(), $eig->eigenvectors()->asArray(), self::MAX_DELTA);
    }

    /**
     * @return Generator<mixed[]>
     */
    public function eigProvider() : Generator
    {
        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            new Eigen(
                Vector::fromArray([
                    -15.096331148319537, 25.108706520450326, 13.9876246278692,
                ]),
                Matrix::fromArray([
                    [0.25848694820886425, -0.11314537870318066, -0.9593657388523845],
                    [-0.8622719261400653, -0.17721179605718698, -0.47442924101375483],
                    [-0.6684472200177011, -0.6126879076802705, -0.42165369894378907],
                ]),
                Vector::fromArray([
                    0.0, 0.0, 0.0,
                ])
            ),
        ];
    }

    /**
     * @test
     */
    public function eigSymmetric() : void
    {
        $matrix = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $eig = $matrix->matmul($matrix)->eig(true);

        $values = Vector::fromArray([-366.30071669298195, 335.92000012383926, 1084.3807165691428]);

        $vectors = Matrix::fromArray([
            [0.5423765325213931, 0.8162941265260668, -0.19872492538460218],
            [-0.04667292577741032, 0.26544998308386847, 0.9629942598375911],
            [-0.8388380862654284, 0.5130304137961217, -0.1820726765627782],
        ]);

        $expected = new Eigen($values, $vectors, Vector::fromArray([0.0, 0.0, 0.0]));

        $this->assertEqualsWithDelta($expected->eigenvalues()->asArray(), $eig->eigenvalues()->asArray(), self::MAX_DELTA);
        $this->assertEqualsWithDelta($expected->eigenvectors()->asArray(), $eig->eigenvectors()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function svd() : void
    {
        $matrix = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $svd = $matrix->svd();

        $u = Matrix::fromArray([
            [-0.8436018806559158, 0.4252547343454771, -0.3278631999333884],
            [0.08179499775610413, -0.5016868397437385, -0.8611735557772425],
            [-0.5307027843302525, -0.7533052009276842, 0.38844025146657923],
        ]);

        $singularValues = Vector::fromArray([
            34.66917512262571, 17.12630582468919, 8.929610580306822,
        ]);

        $vT = Matrix::fromArray([
            [-0.8320393250771425, 0.531457514846513, -0.15894486917903863],
            [-0.4506078135544562, -0.48043370238236727, 0.7524201326246152],
            [-0.3235168618307952, -0.6976649392999047, -0.6392186422366096],
        ]);

        $expected = new SVD($u, $singularValues, $vT);

        $this->assertEqualsWithDelta($u->asArray(), $svd->u()->asArray(), self::MAX_DELTA);
        $this->assertEqualsWithDelta($singularValues->asArray(), $svd->singularValues()->asArray(), self::MAX_DELTA);
        $this->assertEqualsWithDelta($vT->asArray(), $svd->vT()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function matmul() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = Matrix::fromArray([
            [13.0],
            [11.0],
            [9.0],
        ]);

        $c = $a->matmul($b);

        $expected = Matrix::fromArray([
            [207.0], [155.0], [113.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function dotVector() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = Vector::fromArray([2.0, 10.0, -1.0]);

        $c = $a->dot($b);

        $expected = ColumnVector::fromArray([-138.0, 120.0, -11.0]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function convolve() : void
    {
        $a = Matrix::fromArray([
            [3.0, 27.0, 66.0, 29.0, 42.0, 5.0],
            [5.0, 9.0, 15.0, 42.0, 45.0, 16.0],
            [1.0, 5.0, 10.0, 22.0, 66.0, 5.0],
            [0.0, 1.0, 4.0, 9.0, 10.0, 22.0],
            [0.0, 0.0, 3.0, 19.0, 21.0, 25.0],
            [0.0, 0.0, 0.0, 5.0, 2.0, 33.0],
        ]);

        $b = Matrix::fromArray([
            [0.0, 0.0, 1.0],
            [0.0, 1.0, 0.0],
            [1.0, 0.0, 0.0],
        ]);

        // No padding is the "valid" convolution: 6 - 3 + 1 = 4 by 4 outputs, the
        // leading 4 by 4 crop of the "same" result this returned by default
        // before padding was an argument.
        $c = $a->convolve($b, 1);

        $expected = Matrix::fromArray([
            [3.0, 32.0, 75.0, 44.0],
            [32.0, 76.0, 49.0, 94.0],
            [10.0, 20.0, 53.0, 71.0],
            [5.0, 11.0, 26.0, 78.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);

        // One zero on each of the four sides restores the "same" shape, but
        // slides the window by a whole sample, so the alignment differs from the
        // old default by one row and one column: the old result is this one read
        // from index 1, with the first row and column of zeros dropped.
        $padded = $a->convolve($b, 1, 1);

        $this->assertSame([6, 6], $padded->shape());

        $expected = Matrix::fromArray([
            [0.0, 0.0, 3.0, 27.0, 66.0, 29.0],
            [0.0, 3.0, 32.0, 75.0, 44.0, 84.0],
            [3.0, 32.0, 76.0, 49.0, 94.0, 72.0],
            [5.0, 10.0, 20.0, 53.0, 71.0, 91.0],
            [1.0, 5.0, 11.0, 26.0, 78.0, 34.0],
            [0.0, 1.0, 4.0, 12.0, 29.0, 48.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $padded->asArray(), self::MAX_DELTA);
    }

    /**
     * A 1x1 kernel simply samples the input at each stride step, so the result
     * shape is ceil(rows/stride) x ceil(cols/stride).
     *
     * @test
     */
    public function convolveStrideTwo() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0],
            [9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0],
            [17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0, 24.0],
            [25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0, 32.0],
            [33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0],
        ]);

        $b = Matrix::fromArray([
            [1.0],
        ]);

        $c = $a->convolve($b, 2);

        $expected = Matrix::fromArray([
            [1.0, 3.0, 5.0, 7.0],
            [17.0, 19.0, 21.0, 23.0],
            [33.0, 35.0, 37.0, 39.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function convolveStrideThree() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0],
            [9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0],
            [17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0, 24.0],
            [25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0, 32.0],
            [33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0],
        ]);

        $b = Matrix::fromArray([
            [1.0],
        ]);

        $c = $a->convolve($b, 3);

        $expected = Matrix::fromArray([
            [1.0, 4.0, 7.0],
            [25.0, 28.0, 31.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * An even-sized kernel has no single centre sample, so each axis is
     * anchored by the kernel's second sample rather than its third. This pins
     * the alignment, which an mb / 2 centring differs from by one element in
     * both dimensions, and which the old "same" default used: the "same" result
     * for this kernel is the one below read from index 1, with a row and column
     * of zeros dropped.
     *
     * @test
     */
    public function convolveEvenKernelAnchorsSecondSample() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [4.0, 5.0, 6.0],
            [7.0, 8.0, 9.0],
        ]);

        $b = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]);

        $c = $a->convolve($b);

        $expected = Matrix::fromArray([
            [1.0, 4.0],
            [7.0, 23.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);

        $padded = $a->convolve($b, 1, 1);

        $expected = Matrix::fromArray([
            [0.0, 0.0, 0.0, 0.0],
            [0.0, 1.0, 4.0, 7.0],
            [0.0, 7.0, 23.0, 33.0],
            [0.0, 19.0, 53.0, 63.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $padded->asArray(), self::MAX_DELTA);
    }

    /**
     * A kernel larger than the input is rejected unless the padding makes it
     * fit, and the output shape is
     * floor((dimension + 2 * padding - kernel) / stride) + 1 per axis, so a
     * kernel one sample larger than the padded input still leaves a single
     * output.
     *
     * @test
     */
    public function convolvePaddingMakesKernelFit() : void
    {
        $a = Matrix::fromArray([
            [5.0, 2.0, 7.0],
            [1.0, 9.0, 3.0],
            [4.0, 6.0, 8.0],
        ]);

        $b = Matrix::fromArray([
            [1.0, 2.0, 3.0, 4.0],
            [5.0, 6.0, 7.0, 8.0],
            [9.0, 10.0, 11.0, 12.0],
            [13.0, 14.0, 15.0, 16.0],
        ]);

        $expected = Matrix::fromArray([
            [5.0, 12.0],
            [26.0, 51.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $a->convolve($b, 1, 1)->asArray(), self::MAX_DELTA);

        $this->assertEqualsWithDelta([[5.0]], $a->convolve($b, 2, 1)->asArray(), self::MAX_DELTA);
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
        $a = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]);

        $b = Matrix::fromArray([[1.0]]);

        // 2 + 2 * 2 - 1 + 1 = 6 outputs per axis, of which only the four in the
        // middle read an input sample at all.
        $result = $a->convolve($b, 1, 2);

        $this->assertSame([6, 6], $result->shape());

        $expected = Matrix::fromArray([
            [0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            [0.0, 0.0, 1.0, 2.0, 0.0, 0.0],
            [0.0, 0.0, 3.0, 4.0, 0.0, 0.0],
            [0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0, 0.0, 0.0, 0.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $result->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function convolveNegativePaddingThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        (Matrix::fill(1.0, 3, 3))->convolve(Matrix::fromArray([[1.0, 1.0], [1.0, 1.0]]), 1, -1);
    }

    /**
     * Padding is what makes a kernel larger than its input legal, so a kernel
     * still too large once the padding is counted has to be rejected rather than
     * reporting an output dimension of zero or less.
     *
     * @test
     */
    public function convolveKernelLargerThanPaddedMatrixThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        // 3 + 2 * 1 = 5 padded samples per axis cannot take a six-tap kernel.
        (Matrix::fill(1.0, 3, 3))->convolve(Matrix::fill(1.0, 6, 6), 1, 1);
    }

    /**
     * A stride at least as large as either dimension collapses the "same"
     * output to a single element, while a stride below it keeps the rounded-up
     * sub-sampled shape.
     *
     * @test
     */
    public function convolveStrideLargerThanResultReturnsSingleElement() : void
    {
        $a = Matrix::fill(1.0, 4, 3);

        $b = Matrix::fromArray([[1.0]]);

        foreach ([4, 5, PHP_INT_MAX, PHP_INT_MAX - 1] as $stride) {
            $c = $a->convolve($b, $stride);

            $this->assertSame([1, 1], $c->shape(), "stride = {$stride}");

            $this->assertEqualsWithDelta([[1.0]], $c->asArray(), self::MAX_DELTA);
        }

        // ceil(4 / 3) = 2 rows, ceil(3 / 3) = 1 column.
        $this->assertSame([2, 1], $a->convolve($b, 3)->shape());
    }

    /**
     * Regression test for the segmentation fault caused by a signed integer
     * overflow in the output shape. Computing ceil(ma / stride) as
     * (ma + stride - 1) / stride overflowed near PHP_INT_MAX and produced a
     * shape of 0, i.e. a NULL data pointer, while the convolve loop still
     * emitted one element and wrote through it.
     *
     * @test
     */
    public function convolveHugeStrideDoesNotCrash() : void
    {
        foreach ([[4, 2], [5, 3], [6, 2], [8, 8]] as [$m, $n]) {
            $a = Matrix::fill(1.0, $m, $n);

            $b = Matrix::fromArray([[1.0]]);

            foreach ([PHP_INT_MAX, PHP_INT_MAX - 1, PHP_INT_MAX - 2, PHP_INT_MAX - 4] as $stride) {
                $c = $a->convolve($b, $stride);

                $this->assertSame([1, 1], $c->shape(), "{$m} x {$n}, stride = {$stride}");

                $this->assertEqualsWithDelta([[1.0]], $c->asArray(), self::MAX_DELTA);
            }
        }
    }

    /**
     * Cross-check the kernel against a straightforward reference implementation
     * over a wide range of shapes, kernel sizes, strides and paddings,
     * including even-sized kernels.
     *
     * @test
     */
    public function convolveMatchesReference() : void
    {
        mt_srand(4321);

        for ($trial = 0; $trial < 150; ++$trial) {
            $m = mt_rand(1, 9);
            $n = mt_rand(1, 9);
            $padding = mt_rand(0, 3);
            $mb = mt_rand(1, min($m + 2 * $padding, 4));
            $nb = mt_rand(1, min($n + 2 * $padding, 4));
            $stride = mt_rand(1, 4);

            $a = [];
            $b = [];

            for ($i = 0; $i < $m; ++$i) {
                $row = [];

                for ($j = 0; $j < $n; ++$j) {
                    $row[] = mt_rand(-500, 500) / 7.0;
                }

                $a[] = $row;
            }

            for ($i = 0; $i < $mb; ++$i) {
                $row = [];

                for ($j = 0; $j < $nb; ++$j) {
                    $row[] = mt_rand(-500, 500) / 7.0;
                }

                $b[] = $row;
            }

            $expected = $this->referenceConvolve2d($a, $b, $stride, $padding);

            $actual = Matrix::fromArray($a)->convolve(Matrix::fromArray($b), $stride, $padding);

            $this->assertSame(
                [count($expected), count($expected[0])],
                $actual->shape(),
                "{$m} x {$n} by {$mb} x {$nb}, stride = {$stride}, padding = {$padding}"
            );

            $this->assertEqualsWithDelta($expected, $actual->asArray(), self::MAX_DELTA);
        }
    }

    /**
     * The kernel accumulates a tile of 32 output columns of one output row at a
     * time, and only for a unit stride, so convolveMatchesReference() above --
     * whose matrices are all smaller than one tile in both directions -- never
     * reaches that path. The two tile bounds are derived independently, one from
     * the row overhang and one from the column overhang, so a matrix can sit
     * inside the tiled row band while no whole column tile fits at all; this
     * crosses both bounds from either side, and steps the kernel across the tile
     * width and across the stack scratch the reversed kernel is built in so the
     * heap fallback is covered as well. The padding moves both bands, so it is
     * what makes either of them cross the tile grid at a different point.
     *
     * @test
     */
    public function convolveAcrossTileBoundariesMatchesReference() : void
    {
        mt_srand(1234);

        $sizes = [1, 31, 32, 33, 40, 64, 65];
        $paddings = [0, 1, 16, 32, 33];

        foreach ($sizes as $m) {
            foreach ($sizes as $n) {
                foreach ([[1, 1], [3, 3], [2, 5], [16, 16], [17, 17]] as [$mb, $nb]) {
                    foreach ($paddings as $padding) {
                        // The API rejects a kernel that padding cannot make fit.
                        if ($mb > $m + 2 * $padding || $nb > $n + 2 * $padding) {
                            continue;
                        }

                        $a = [];
                        $b = [];

                        for ($i = 0; $i < $m; ++$i) {
                            $row = [];

                            for ($j = 0; $j < $n; ++$j) {
                                $row[] = mt_rand(-500, 500) / 7.0;
                            }

                            $a[] = $row;
                        }

                        for ($i = 0; $i < $mb; ++$i) {
                            $row = [];

                            for ($j = 0; $j < $nb; ++$j) {
                                $row[] = mt_rand(-500, 500) / 7.0;
                            }

                            $b[] = $row;
                        }

                        $expected = $this->referenceConvolve2d($a, $b, 1, $padding);

                        $actual = Matrix::fromArray($a)->convolve(Matrix::fromArray($b), 1, $padding);

                        $this->assertSame(
                            [count($expected), count($expected[0])],
                            $actual->shape(),
                            "{$m} x {$n} by {$mb} x {$nb}, padding = {$padding}"
                        );

                        $this->assertEqualsWithDelta(
                            $expected,
                            $actual->asArray(),
                            self::MAX_DELTA,
                            "{$m} x {$n} by {$mb} x {$nb}, padding = {$padding}"
                        );
                    }
                }
            }
        }

        // The tile path is deliberately not taken for a stride above 1, so the
        // same boundary sizes are worth crossing on the per-output path too.
        foreach ($sizes as $m) {
            foreach ($sizes as $n) {
                foreach ([2, 3] as $stride) {
                    foreach ([0, 1, 17] as $padding) {
                        // A 3 x 3 kernel needs three padded samples an axis.
                        if ($m + 2 * $padding < 3 || $n + 2 * $padding < 3) {
                            continue;
                        }

                        $a = [];
                        $b = [];

                        for ($i = 0; $i < $m; ++$i) {
                            $row = [];

                            for ($j = 0; $j < $n; ++$j) {
                                $row[] = mt_rand(-500, 500) / 7.0;
                            }

                            $a[] = $row;
                        }

                        for ($i = 0; $i < 3; ++$i) {
                            $row = [];

                            for ($j = 0; $j < 3; ++$j) {
                                $row[] = mt_rand(-500, 500) / 7.0;
                            }

                            $b[] = $row;
                        }

                        $expected = $this->referenceConvolve2d($a, $b, $stride, $padding);

                        $actual = Matrix::fromArray($a)->convolve(Matrix::fromArray($b), $stride, $padding);

                        $this->assertSame(
                            [count($expected), count($expected[0])],
                            $actual->shape(),
                            "{$m} x {$n} by 3 x 3, stride = {$stride}, padding = {$padding}"
                        );

                        $this->assertEqualsWithDelta(
                            $expected,
                            $actual->asArray(),
                            self::MAX_DELTA,
                            "{$m} x {$n} by 3 x 3, stride = {$stride}, padding = {$padding}"
                        );
                    }
                }
            }
        }
    }

    /**
     * @test
     * @dataProvider multiplyProvider
     *
     * @param Matrix $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function multiply(Matrix $a, $b, $expected) : void
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
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Matrix::fromArray([
                [4.0, 6.0, -12.0],
                [1.0, 3.0, 5.0],
                [-10.0, -1.0, 14.0],
            ]),
            Matrix::fromArray([
                [88.0, -102.0, -144.0],
                [4.0, 33.0, -10.0],
                [-200.0, 6.0, -126.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Vector::fromArray([2.0, 10.0, -1.0]),
            Matrix::fromArray([
                [44.0, -170.0, -12.0],
                [8.0, 110.0, 2.0],
                [40.0, -60.0, 9.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            ColumnVector::fromArray([2.5, -1.0, 4.8]),
            Matrix::fromArray([
                [55.0, -42.5, 30.0],
                [-4.0, -11.0, 2.0],
                [96.0, -28.8, -43.2],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            2.5,
            Matrix::fromArray([
                [55.0, -42.5, 30.0],
                [10.0, 27.5, -5.0],
                [50.0, -15.0, -22.5],
            ]),
        ];
    }

    /**
     * @test
     * @dataProvider divideProvider
     *
     * @param Matrix $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function divide(Matrix $a, $b, $expected) : void
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
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Matrix::fromArray([
                [4.0, 6.0, -12.0],
                [1.0, 3.0, 5.0],
                [-10.0, -1.0, 14.0],
            ]),
            Matrix::fromArray([
                [5.5, -2.8333333333333335, -1.0],
                [4.0, 3.6666666666666665, -0.4],
                [-2.0, 6.0, -0.6428571428571429],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Vector::fromArray([2.0, 10.0, -1.0]),
            Matrix::fromArray([
                [11.0, -1.7, -12.0],
                [2.0, 1.1, 2.0],
                [10.0, -0.6, 9.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            ColumnVector::fromArray([2.5, -1.0, 4.8]),
            Matrix::fromArray([
                [8.8, -6.8, 4.8],
                [-4.0, -11.0, 2.0],
                [4.166666666666667, -1.25, -1.875],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            2.0,
            Matrix::fromArray([
                [11.0, -8.5, 6.0],
                [2.0, 5.5, -1.0],
                [10.0, -3.0, -4.5],
            ]),
        ];
    }

    /**
     * @test
     * @dataProvider addProvider
     *
     * @param Matrix $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function add(Matrix $a, $b, $expected) : void
    {
        $c = $a->add($b);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function multiplyNumericString() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]);

        $this->assertEqualsWithDelta(
            [
                [2.0, 4.0],
                [6.0, 8.0],
            ],
            $a->multiply('2')->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function addNumericString() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0],
        ]);

        $this->assertEqualsWithDelta(
            [
                [2.5, 3.5],
            ],
            $a->add('1.5')->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function powInfScalar() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0],
        ]);

        $this->assertEqualsWithDelta(
            [
                [1.0, INF],
            ],
            $a->pow(INF)->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function greaterEqualNumericString() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0],
        ]);

        $this->assertEqualsWithDelta(
            [
                [0, 1],
            ],
            $a->greaterEqual('1.5')->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function subtractNumericString() : void
    {
        $a = Matrix::fromArray([
            [5.0, 6.0],
        ]);

        $this->assertEqualsWithDelta(
            [
                [4.5, 5.5],
            ],
            $a->subtract('0.5')->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function divideNumericString() : void
    {
        $a = Matrix::fromArray([
            [4.0, 6.0],
        ]);

        $this->assertEqualsWithDelta(
            [
                [2.0, 3.0],
            ],
            $a->divide('2.0')->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function modNumericString() : void
    {
        $a = Matrix::fromArray([
            [4.0, 7.0],
        ]);

        $this->assertEqualsWithDelta(
            [
                [1.0, 1.0],
            ],
            $a->mod('3')->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @return Generator<mixed[]>
     */
    public function addProvider() : Generator
    {
        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Matrix::fromArray([
                [4.0, 6.0, -12.0],
                [1.0, 3.0, 5.0],
                [-10.0, -1.0, 14.0],
            ]),
            Matrix::fromArray([
                [26.0, -11.0, 0.0],
                [5.0, 14.0, 3.0],
                [10.0, -7.0, 5.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Vector::fromArray([2.0, 10.0, -1.0]),
            Matrix::fromArray([
                [24.0, -7.0, 11.0],
                [6.0, 21.0, -3.0],
                [22.0, 4.0, -10.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            ColumnVector::fromArray([2.5, -1.0, 4.8]),
            Matrix::fromArray([
                [24.5, -14.5, 14.5],
                [3.0, 10.0, -3.0],
                [24.8, -1.2000000000000002, -4.2],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            1.0,
            Matrix::fromArray([
                [23.0, -16.0, 13.0],
                [5.0, 12.0, -1.0],
                [21.0, -5.0, -8.0],
            ]),
        ];
    }

    /**
     * @test
     * @dataProvider subtractProvider
     *
     * @param Matrix $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function subtract(Matrix $a, $b, $expected) : void
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
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Matrix::fromArray([
                [4.0, 6.0, -12.0],
                [1.0, 3.0, 5.0],
                [-10.0, -1.0, 14.0],
            ]),
            Matrix::fromArray([
                [18.0, -23.0, 24.0],
                [3.0, 8.0, -7.0],
                [30.0, -5.0, -23.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Vector::fromArray([2.0, 10.0, -1.0]),
            Matrix::fromArray([
                [20.0, -27.0, 13.0],
                [2.0, 1.0, -1.0],
                [18.0, -16.0, -8.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            ColumnVector::fromArray([2.5, -1.0, 4.8]),
            Matrix::fromArray([
                [19.5, -19.5, 9.5],
                [5.0, 12.0, -1.0],
                [15.2, -10.8, -13.8],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            10.0,
            Matrix::fromArray([
                [12.0, -27.0, 2.0],
                [-6.0, 1.0, -12.0],
                [10.0, -16.0, -19.0],
            ]),
        ];
    }

    /**
     * @test
     * @dataProvider powProvider
     *
     * @param Matrix $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function pow(Matrix $a, $b, $expected) : void
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
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Matrix::fromArray([
                [4.0, 6.0, -12.0],
                [1.0, 3.0, 5.0],
                [-10.0, -1.0, 14.0],
            ]),
            Matrix::fromArray([
                [234256.0, 24137569.0, 1.1215665478461509E-13],
                [4.0, 1331.0, -32.0],
                [9.765625E-14, -0.16666666666666666, 22876792454961.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Vector::fromArray([2.0, 10.0, -1.0]),
            Matrix::fromArray([
                [484.0, 2015993900449.0, 0.08333333333333333],
                [16.0, 25937424601.0, -0.5],
                [400.0, 60466176.0, -0.1111111111111111],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            3.0,
            Matrix::fromArray([
                [10648.0, -4913.0, 1728.0],
                [64.0, 1331.0, -8.0],
                [8000.0, -216.0, -729.0],
            ]),
        ];
    }

    /**
     * @test
     * @dataProvider modProvider
     *
     * @param Matrix $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function mod(Matrix $a, $b, $expected) : void
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
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Matrix::fromArray([
                [4.0, 6.0, -12.0],
                [1.0, 3.0, 5.0],
                [-10.0, -1.0, 14.0],
            ]),
            Matrix::fromArray([
                [2.0, -5.0, 0.0],
                [0.0, 2.0, -2.0],
                [0.0, 0.0, -9.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Vector::fromArray([2.0, 10.0, -1.0]),
            Matrix::fromArray([
                [0.0, -7.0, 0.0],
                [0.0, 1.0, 0.0],
                [0.0, -6.0, 0.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            ColumnVector::fromArray([2.5, -1.0, 4.0]),
            Matrix::fromArray([
                [2.0, -2.0, 2.0],
                [0.0, 0.0, 0.0],
                [0.0, -2.0, -1.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            10.0,
            Matrix::fromArray([
                [2.0, -7.0, 2.0],
                [4.0, 1.0, -2.0],
                [0.0, -6.0, -9.0],
            ]),
        ];
    }

    /**
     * @test
     * @dataProvider equalProvider
     *
     * @param Matrix $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function equal(Matrix $a, $b, $expected) : void
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
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Matrix::fromArray([
                [4.0, 6.0, -12.0],
                [1.0, 3.0, 5.0],
                [-10.0, -1.0, 14.0],
            ]),
            Matrix::fromArray([
                [0.0, 0.0, 0.0],
                [0.0, 0.0, 0.0],
                [0.0, 0.0, 0.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Vector::fromArray([2.0, 10.0, -1.0]),
            Matrix::fromArray([
                [0.0, 0.0, 0.0],
                [0.0, 0.0, 0.0],
                [0.0, 0.0, 0.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            ColumnVector::fromArray([2.5, -1.0, 4.8]),
            Matrix::fromArray([
                [0.0, 0.0, 0.0],
                [0.0, 0.0, 0.0],
                [0.0, 0.0, 0.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            4.0,
            Matrix::fromArray([
                [0.0, 0.0, 0.0],
                [1.0, 0.0, 0.0],
                [0.0, 0.0, 0.0],
            ]),
        ];
    }

    /**
     * @test
     * @dataProvider notEqualProvider
     *
     * @param Matrix $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function notEqual(Matrix $a, $b, $expected) : void
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
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Matrix::fromArray([
                [4.0, 6.0, -12.0],
                [1.0, 3.0, 5.0],
                [-10.0, -1.0, 14.0],
            ]),
            Matrix::fromArray([
                [1.0, 1.0, 1.0],
                [1.0, 1.0, 1.0],
                [1.0, 1.0, 1.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Vector::fromArray([2.0, 10.0, -1.0]),
            Matrix::fromArray([
                [1.0, 1.0, 1.0],
                [1.0, 1.0, 1.0],
                [1.0, 1.0, 1.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            ColumnVector::fromArray([2.5, -1.0, 4.8]),
            Matrix::fromArray([
                [1.0, 1.0, 1.0],
                [1.0, 1.0, 1.0],
                [1.0, 1.0, 1.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            4.0,
            Matrix::fromArray([
                [1.0, 1.0, 1.0],
                [0.0, 1.0, 1.0],
                [1.0, 1.0, 1.0],
            ]),
        ];
    }

    /**
     * @test
     * @dataProvider greaterProvider
     *
     * @param Matrix $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function greater(Matrix $a, $b, $expected) : void
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
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Matrix::fromArray([
                [4.0, 6.0, -12.0],
                [1.0, 3.0, 5.0],
                [-10.0, -1.0, 14.0],
            ]),
            Matrix::fromArray([
                [1.0, 0.0, 1.0],
                [1.0, 1.0, 0.0],
                [1.0, 0.0, 0.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Vector::fromArray([2.0, 10.0, -1.0]),
            Matrix::fromArray([
                [1.0, 0.0, 1.0],
                [1.0, 1.0, 0.0],
                [1.0, 0.0, 0.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            ColumnVector::fromArray([2.5, -1.0, 4.8]),
            Matrix::fromArray([
                [1.0, 0.0, 1.0],
                [1.0, 1.0, 0.0],
                [1.0, 0.0, 0.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            4.0,
            Matrix::fromArray([
                [1.0, 0.0, 1.0],
                [0.0, 1.0, 0.0],
                [1.0, 0.0, 0.0],
            ]),
        ];
    }

    /**
     * @test
     * @dataProvider greaterEqualProvider
     *
     * @param Matrix $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function greaterEqual(Matrix $a, $b, $expected) : void
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
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Matrix::fromArray([
                [4.0, 6.0, -12.0],
                [1.0, 3.0, 5.0],
                [-10.0, -1.0, 14.0],
            ]),
            Matrix::fromArray([
                [1.0, 0.0, 1.0],
                [1.0, 1.0, 0.0],
                [1.0, 0.0, 0.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Vector::fromArray([2.0, 10.0, -1.0]),
            Matrix::fromArray([
                [1.0, 0.0, 1.0],
                [1.0, 1.0, 0.0],
                [1.0, 0.0, 0.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            ColumnVector::fromArray([2.5, -1.0, 4.8]),
            Matrix::fromArray([
                [1.0, 0.0, 1.0],
                [1.0, 1.0, 0.0],
                [1.0, 0.0, 0.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            4.0,
            Matrix::fromArray([
                [1.0, 0.0, 1.0],
                [1.0, 1.0, 0.0],
                [1.0, 0.0, 0.0],
            ]),
        ];
    }

    /**
     * @test
     * @dataProvider lessProvider
     *
     * @param Matrix $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function less(Matrix $a, $b, $expected) : void
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
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Matrix::fromArray([
                [4.0, 6.0, -12.0],
                [1.0, 3.0, 5.0],
                [-10.0, -1.0, 14.0],
            ]),
            Matrix::fromArray([
                [0.0, 1.0, 0.0],
                [0.0, 0.0, 1.0],
                [0.0, 1.0, 1.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Vector::fromArray([2.0, 10.0, -1.0]),
            Matrix::fromArray([
                [0.0, 1.0, 0.0],
                [0.0, 0.0, 1.0],
                [0.0, 1.0, 1.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            ColumnVector::fromArray([2.5, -1.0, 4.8]),
            Matrix::fromArray([
                [0.0, 1.0, 0.0],
                [0.0, 0.0, 1.0],
                [0.0, 1.0, 1.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            10.0,
            Matrix::fromArray([
                [0.0, 1.0, 0.0],
                [1.0, 0.0, 1.0],
                [0.0, 1.0, 1.0],
            ]),
        ];
    }

    /**
     * @test
     * @dataProvider lessEqualProvider
     *
     * @param Matrix $a
     * @param Tensor|float $b
     * @param Tensor|float $expected
     */
    public function lessEqual(Matrix $a, $b, $expected) : void
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
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Matrix::fromArray([
                [4.0, 6.0, -12.0],
                [1.0, 3.0, 5.0],
                [-10.0, -1.0, 14.0],
            ]),
            Matrix::fromArray([
                [0.0, 1.0, 0.0],
                [0.0, 0.0, 1.0],
                [0.0, 1.0, 1.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            Vector::fromArray([2.0, 10.0, -1.0]),
            Matrix::fromArray([
                [0.0, 1.0, 0.0],
                [0.0, 0.0, 1.0],
                [0.0, 1.0, 1.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            ColumnVector::fromArray([2.5, -1.0, 4.8]),
            Matrix::fromArray([
                [0.0, 1.0, 0.0],
                [0.0, 0.0, 1.0],
                [0.0, 1.0, 1.0],
            ]),
        ];

        yield [
            Matrix::fromArray([
                [22.0, -17.0, 12.0],
                [4.0, 11.0, -2.0],
                [20.0, -6.0, -9.0],
            ]),
            10.0,
            Matrix::fromArray([
                [0.0, 1.0, 0.0],
                [1.0, 0.0, 1.0],
                [0.0, 1.0, 1.0],
            ]),
        ];
    }

    /**
     * @test
     */
    public function abs() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->abs();

        $expected = Matrix::fromArray([
            [22.0, 17.0, 12.0],
            [4.0, 11.0, 2.0],
            [20.0, 6.0, 9.0],
        ]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function square() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->square();

        $expected = Matrix::fromArray([
            [484.0, 289.0, 144.0],
            [16.0, 121.0, 4.0],
            [400.0, 36.0, 81.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function sqrt() : void
    {
        $a = Matrix::fromArray([
            [13.0],
            [11.0],
            [9.0],
        ]);

        $b = $a->sqrt();

        $expected = Matrix::fromArray([
            [3.605551275463989],
            [3.3166247903554],
            [3.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function exp() : void
    {
        $a = Matrix::fromArray([
            [13.0],
            [11.0],
            [9.0],
        ]);

        $b = $a->exp();

        $expected = Matrix::fromArray([
            [442413.3920089205],
            [59874.14171519778],
            [8103.08392757538],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function expm1() : void
    {
        $a = Matrix::fromArray([
            [13.0],
            [11.0],
            [9.0],
        ]);

        $b = $a->expm1();

        $expected = Matrix::fromArray([
            [442412.3920089205],
            [59873.14171519782],
            [8102.083927575384],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function log() : void
    {
        $a = Matrix::fromArray([
            [13.0],
            [11.0],
            [9.0],
        ]);

        $b = $a->log();

        $expected = Matrix::fromArray([
            [2.5649493574615367],
            [2.3978952727983707],
            [2.1972245773362196],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function logInvalidBaseThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ])->log(0.0);
    }

    /**
     * @test
     */
    public function logInvalidNegativeBaseThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ])->log(-1.0);
    }

    /**
     * @test
     */
    public function log1p() : void
    {
        $a = Matrix::fromArray([
            [13.0],
            [11.0],
            [9.0],
        ]);

        $b = $a->log1p();

        $expected = Matrix::fromArray([
            [2.6390573296152584],
            [2.4849066497880004],
            [2.302585092994046],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function sigmoid() : void
    {
        $a = Matrix::fromArray([
            [13.0, 1.0, -13.0, 0.5],
            [11.0, -1.0, -11.0, -0.5],
            [9.0, 0.0, -9.0, 2.5],
        ]);

        $b = $a->sigmoid();

        $expected = Matrix::fromArray([
            [0.99999773967570205, 0.7310585786300049, 2.2603242979035746e-6, 0.62245933120185459],
            [0.99998329857815205, 0.2689414213699951, 1.6701421848095181e-5, 0.37754066879814541],
            [0.99987660542401369, 0.5, 0.00012339457598623172, 0.92414181997875655],
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
        $a = Matrix::fromArray([
            [-800.0, 800.0, -1000.0],
            [-745.0, 745.0, 0.0],
        ]);

        $b = $a->sigmoid()->asArray();

        $this->assertSame(0.0, $b[0][0]);
        $this->assertSame(1.0, $b[0][1]);
        $this->assertSame(0.0, $b[0][2]);
        $this->assertSame(0.0, $b[1][0]);
        $this->assertSame(1.0, $b[1][1]);
        $this->assertSame(0.5, $b[1][2]);
    }

    /**
     * The fused kernel must agree with the composition it replaces, elementwise.
     *
     * @test
     */
    public function sigmoidMatchesComposition() : void
    {
        $a = Matrix::rand(17, 23)->multiply(8.0)->subtract(4.0);

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
        $a = Matrix::fromArray([
            [13.0, 1.0, -13.0, 0.5],
            [11.0, -1.0, -11.0, -0.5],
            [9.0, 0.0, -9.0, 2.5],
        ]);

        $b = $a->softplus();

        $expected = Matrix::fromArray([
            [13.000002260327, 1.3132616875182, 2.2603268524372e-6, 0.97407698418011],
            [11.000016701561, 0.31326168751822, 1.6701561318304e-5, 0.47407698418011],
            [9.0001234021897, 0.69314718055995, 0.00012340218972334, 2.5788897342925],
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
        $a = Matrix::fromArray([
            [-800.0, 800.0, -1000.0],
            [-745.0, 745.0, 0.0],
        ]);

        $b = $a->softplus()->asArray();

        $this->assertLessThan(1e-300, $b[0][0]);
        $this->assertSame(800.0, $b[0][1]);
        $this->assertLessThan(1e-300, $b[0][2]);
        $this->assertLessThan(1e-300, $b[1][0]);
        $this->assertSame(745.0, $b[1][1]);
        $this->assertEqualsWithDelta(log(2.0), $b[1][2], self::MAX_DELTA);
    }

    /**
     * The fused kernel must agree with the composition it replaces, elementwise.
     *
     * @test
     */
    public function softplusMatchesComposition() : void
    {
        $a = Matrix::rand(17, 23)->multiply(8.0)->subtract(4.0);

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
        $a = Matrix::fromArray([
            [13.0, 1.0, -13.0, 0.5],
            [11.0, -1.0, -11.0, -0.5],
            [9.0, 0.0, -9.0, 2.5],
        ]);

        $b = $a->tanh();

        $expected = Matrix::fromArray([
            [0.99999999998978184, 0.76159415595576485, -0.99999999998978184, 0.46211715726000974],
            [0.99999999944210638, -0.76159415595576485, -0.99999999944210638, -0.46211715726000974],
            [0.999999969540041, 0.0, -0.999999969540041, 0.98661429815143031],
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
        $a = Matrix::fromArray([
            [100.0, -100.0, 0.0],
            [-800.0, 800.0, 0.5],
        ]);

        $b = $a->tanh()->asArray();

        $this->assertSame(1.0, $b[0][0]);
        $this->assertSame(-1.0, $b[0][1]);
        $this->assertSame(0.0, $b[0][2]);
        $this->assertSame(-1.0, $b[1][0]);
        $this->assertSame(1.0, $b[1][1]);
        $this->assertSame(0.46211715726000974, $b[1][2]);
    }

    /**
     * The fused kernel must agree with the composition it replaces, elementwise.
     *
     * @test
     */
    public function tanhMatchesComposition() : void
    {
        $a = Matrix::rand(17, 23)->multiply(4.0)->subtract(2.0);

        $this->assertEqualsWithDelta(
            $a->exp()->subtract($a->multiply(-1.0)->exp())
                ->divide($a->exp()->add($a->multiply(-1.0)->exp()))
                ->asArray(),
            $a->tanh()->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function sin() : void
    {
        $a = Matrix::fromArray([
            [13.0],
            [11.0],
            [9.0],
        ]);

        $b = $a->sin();

        $expected = Matrix::fromArray([
            [0.4201670368266409],
            [-0.9999902065507035],
            [0.4121184852417566],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function asin() : void
    {
        $a = Matrix::fromArray([
            [0.32],
            [-0.5],
            [0.01],
        ]);

        $b = $a->asin();

        $expected = Matrix::fromArray([
            [0.3257294872946302],
            [-0.5235987755982989],
            [0.010000166674167114],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function cos() : void
    {
        $a = Matrix::fromArray([
            [13.0],
            [11.0],
            [9.0],
        ]);

        $b = $a->cos();

        $expected = Matrix::fromArray([
            [0.9074467814501962],
            [0.004425697988050785],
            [-0.9111302618846769],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function acos() : void
    {
        $a = Matrix::fromArray([
            [0.32],
            [-0.5],
            [0.01],
        ]);

        $b = $a->acos();

        $expected = Matrix::fromArray([
            [1.2450668395002664],
            [2.0943951023931957],
            [1.5607961601207294],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function tan() : void
    {
        $a = Matrix::fromArray([
            [13.0],
            [11.0],
            [9.0],
        ]);

        $b = $a->tan();

        $expected = Matrix::fromArray([
            [0.4630211329364896],
            [-225.95084645419513],
            [-0.45231565944180985],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function atan() : void
    {
        $a = Matrix::fromArray([
            [13.0],
            [11.0],
            [9.0],
        ]);

        $b = $a->atan();

        $expected = Matrix::fromArray([
            [1.4940244355251187],
            [1.4801364395941514],
            [1.460139105621001],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function rad2deg() : void
    {
        $a = Matrix::fromArray([
            [13.0],
            [11.0],
            [9.0],
        ]);

        $b = $a->rad2deg();

        $expected = Matrix::fromArray([
            [744.8451336700701],
            [630.2535746439056],
            [515.6620156177408],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function deg2rad() : void
    {
        $a = Matrix::fromArray([
            [13.0],
            [11.0],
            [9.0],
        ]);

        $b = $a->deg2rad();

        $expected = Matrix::fromArray([
            [0.22689280275926282],
            [0.19198621771937624],
            [0.15707963267948966],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function sum() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->sum();

        $expected = ColumnVector::fromArray([17.0, 13.0, 5.0]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function product() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->product();

        $expected = ColumnVector::fromArray([-4488.0, -88.0, 1080.0]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function min() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->min();

        $expected = ColumnVector::fromArray([-17.0, -2.0, -9.0]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function max() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->max();

        $expected = ColumnVector::fromArray([22.0, 11.0, 20.0]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function argmin() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->argmin();

        $expected = ColumnVector::fromArray([1, 2, 2]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function argmax() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->argmax();

        $expected = ColumnVector::fromArray([0, 1, 0]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function mean() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->mean();

        $expected = ColumnVector::fromArray([5.666666666666667, 4.333333333333333, 1.6666666666666667]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function median() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->median();

        $expected = ColumnVector::fromArray([12.0, 4.0, -6.0]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function quantile() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->quantile(0.4);

        $expected = ColumnVector::fromArray([6.200000000000001, 2.8000000000000003, -6.6]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);

        $max = $a->quantile(1.0);

        $maxExpected = ColumnVector::fromArray([22.0, 11.0, 20.0]);

        $this->assertEqualsWithDelta($maxExpected->asArray(), $max->asArray(), self::MAX_DELTA);

        $single = Matrix::fromArray([
            [5.0],
            [3.0],
            [8.0],
        ]);

        $singleExpected = ColumnVector::fromArray([5.0, 3.0, 8.0]);

        $this->assertEqualsWithDelta(
            $singleExpected->asArray(),
            $single->quantile(0.5)->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function quantileNaNThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        (Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [4.0, 5.0, 6.0],
        ]))->quantile(NAN);
    }

    /**
     * Softmax normalizes along each row, so the three rows below normalize
     * independently of one another.
     *
     * @test
     */
    public function softmax() : void
    {
        $a = Matrix::fromArray([
            [1.0, 5.0, -2.0],
            [2.0, 1.0, 4.0],
            [3.0, 0.0, 0.0],
        ]);

        $b = $a->softmax();

        $expected = Matrix::fromArray([
            [0.017970118068812064, 0.9811352024343174, 0.0008946794968705335],
            [0.11419519938459449, 0.04201006613406605, 0.8437947344813395],
            [0.909442998512742, 0.045278500743629074, 0.045278500743629074],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * The defining property, over shapes that straddle the kernel's block
     * boundary: every row of the result sums to one. Matrix::sum() already runs
     * along a row, so no transpose is needed to read the totals off.
     *
     * @test
     * @dataProvider shapeProvider
     * @param int $m
     * @param int $n
     */
    public function softmaxRowsSumToOne(int $m, int $n) : void
    {
        $a = Matrix::rand($m, $n)->multiply(20.0);

        $totals = $a->softmax()->sum()->asArray();

        $this->assertCount($m, $totals);

        foreach ($totals as $total) {
            $this->assertEqualsWithDelta(1.0, $total, self::MAX_DELTA);
        }
    }

    /**
     * Subtracting a constant from a whole row leaves the exponentials scaled by
     * the same factor, which cancels in the division, so the result is
     * unchanged. This is the property the maximum subtraction inside the kernel
     * relies on, and it is what makes it safe to subtract the maximum rather
     * than nothing.
     *
     * The constant has to be constant down a row: adding a Vector instead would
     * offset each column by a different amount, which is a shift of the matrix
     * across its rows and does move the answer.
     *
     * @test
     */
    public function softmaxIsShiftInvariant() : void
    {
        $a = Matrix::fromArray([
            [1.0, 5.0, -2.0],
            [2.0, 1.0, 4.0],
            [3.0, 0.0, 0.0],
        ]);

        $this->assertEqualsWithDelta(
            $a->softmax()->asArray(),
            $a->add(ColumnVector::fromArray([7.0, -13.0, 100.0]))->softmax()->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * A row of large magnitudes must not overflow the exponential. Taking the
     * maximum of each row is what keeps the exponentials in range: without it
     * the first row would be an infinity throughout, and the second would
     * underflow to nothing but zeroes.
     *
     * @test
     */
    public function softmaxDoesNotOverflow() : void
    {
        $a = Matrix::fromArray([
            [1000.0, -1000.0, 710.0],
            [1001.0, 0.0, -1001.0],
            [-710.0, 1002.0, 0.0],
        ]);

        $b = $a->softmax()->asArray();

        $this->assertEqualsWithDelta([1.0, 0.0, 0.0], $b[0], self::MAX_DELTA);
        $this->assertEqualsWithDelta([1.0, 0.0, 0.0], $b[1], self::MAX_DELTA);
        $this->assertEqualsWithDelta([0.0, 1.0, 0.0], $b[2], self::MAX_DELTA);
    }

    /**
     * The fused kernel must reproduce the maximum, subtract, exponential, sum,
     * divide sequence it replaces. The kernel leans on the same summation
     * helper the sum reduction runs, so this is exact rather than within a
     * tolerance: no clip is needed either, since a row's total is always at
     * least exp(0) = 1.
     *
     * @test
     */
    public function softmaxMatchesComposition() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $z = $a->subtractColumnVector($a->max())->exp();
        $expected = $z->divide($z->sum());

        $this->assertEquals($expected->asArray(), $a->softmax()->asArray());
    }

    /**
     * The kernel writes to a fresh buffer, so the operand must come back
     * untouched.
     *
     * @test
     */
    public function softmaxDoesNotMutate() : void
    {
        $a = Matrix::fromArray([
            [1.0, 5.0, -2.0],
            [2.0, 1.0, 4.0],
            [3.0, 0.0, 0.0],
        ]);

        $before = $a->asArray();
        $a->softmax();

        $this->assertSame($before, $a->asArray());
    }

    /**
     * A single row is one group, so it normalizes to one across its width.
     *
     * @test
     */
    public function softmaxSingleRow() : void
    {
        $a = Matrix::fromArray([[1.0, 2.0, 3.0]]);

        $this->assertEqualsWithDelta(
            [[0.09003057317038046, 0.24472847105479764, 0.6652409557748218]],
            $a->softmax()->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * The degenerate axis: a single column makes every row a one-element group,
     * and a lone element is its own maximum, so the result is all ones. Worth
     * pinning down because it is the one shape where the output carries no
     * information, and it is what a caller gets for a one-hot classifier.
     *
     * @test
     */
    public function softmaxSingleColumn() : void
    {
        $a = Matrix::fromArray([
            [1000.0],
            [0.0],
            [-1000.0],
        ]);

        $this->assertEqualsWithDelta([[1.0], [1.0], [1.0]], $a->softmax()->asArray(), self::MAX_DELTA);
    }

    /**
     * Shapes on both sides of the kernel's lane block: a single row and a single
     * column, a row narrower than a cache line, a row far wider than the
     * working set, and a tall matrix where the derived lane count collapses to
     * one row at a time.
     *
     * @return Generator<mixed[]>
     */
    public function shapeProvider() : Generator
    {
        yield [1, 1];
        yield [1, 200];
        yield [200, 1];
        yield [3, 17];
        yield [17, 3];
        yield [4, 4096];
        yield [4096, 4];
        yield [512, 512];
        yield [129, 257];
    }

    /**
     * @test
     */
    public function variance() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->variance();

        $expected = ColumnVector::fromArray([273.55555555555554, 28.222222222222225, 169.55555555555554]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function varianceRowNonSquare() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [10.0, 20.0, 30.0],
        ]);

        $b = $a->variance();

        $expected = ColumnVector::fromArray([0.6666666666666666, 66.66666666666667]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function covariance() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->covariance();

        $expected = Matrix::fromArray([
            [273.55555555555554, -65.55555555555556, 135.2222222222222],
            [-65.55555555555556, 28.222222222222225, 3.4444444444444406],
            [135.2222222222222, 3.4444444444444406, 169.55555555555554],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);

        $c = Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [4.0, 5.0, 6.0],
        ]);

        $d = $c->covariance();

        $expectedC = Matrix::fromArray([
            [2.0 / 3.0, 2.0 / 3.0],
            [2.0 / 3.0, 2.0 / 3.0],
        ]);

        $this->assertEqualsWithDelta($expectedC->asArray(), $d->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function covarianceNonSquare() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0],
            [4.0, 3.0, 9.0, 2.0, 8.0, 1.0, 5.0],
            [2.0, -5.0, 0.5, 6.0, 3.0, 2.0, -1.0],
        ]);

        $b = $a->covariance();

        $this->assertEquals(3, $b->m());
        $this->assertEquals(3, $b->n());

        $result = $b->asArray();

        $expected = [
            [4.0, -0.28571428571428574, 1.0714285714285714],
            [-0.28571428571428574, 7.673469387755102, -0.5408163265306123],
            [1.0714285714285714, -0.5408163265306123, 10.173469387755102],
        ];

        $this->assertEqualsWithDelta($expected, $result, self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function covarianceSingleRow() : void
    {
        $a = Matrix::fromArray([[1.0, 2.0, 3.0, 4.0, 5.0]]);

        $b = $a->covariance();

        $this->assertEquals(1, $b->m());
        $this->assertEquals(1, $b->n());
        $this->assertEqualsWithDelta([[2.0]], $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function covarianceSingleColumn() : void
    {
        $a = Matrix::fromArray([[1.0], [2.0], [3.0]]);

        $b = $a->covariance();

        $this->assertEquals(3, $b->m());
        $this->assertEquals(3, $b->n());
        $this->assertEqualsWithDelta([[0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0]], $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function covarianceIsSymmetric() : void
    {
        $a = Matrix::rand(24, 40);

        $result = $a->covariance()->asArray();

        for ($i = 0; $i < 24; ++$i) {
            for ($j = 0; $j < 24; ++$j) {
                $this->assertEqualsWithDelta(
                    $result[$i][$j],
                    $result[$j][$i],
                    self::MAX_DELTA,
                    "Covariance is not symmetric at [$i][$j]."
                );
            }
        }
    }

    /**
     * @test
     */
    public function covarianceMatchesComposedOperations() : void
    {
        $a = Matrix::rand(9, 17);

        $mean = $a->mean();
        $centered = $a->subtractColumnVector($mean);

        $composed = $centered->matmul($centered->transpose())->divideScalar($a->n());

        $this->assertEqualsWithDelta(
            $composed->asArray(),
            $a->covariance()->asArray(),
            self::MAX_DELTA
        );
    }

    /**
     * @test
     */
    public function covarianceEmptyThrows() : void
    {
        // A matrix with no rows leaves the fused kernel nothing to centre, and
        // the failure is raised from the C layer as the SPL exception rather
        // than the namespaced one.
        $this->expectException(SplInvalidArgumentException::class);

        Matrix::fromArray([])->covariance();
    }

    /**
     * @test
     */
    public function covarianceEmptyRowsThrows() : void
    {
        $this->expectException(LengthException::class);

        Matrix::fromArray([[], []])->covariance();
    }

    /**
     * @test
     */
    public function round() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->round(2);

        $expected = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function roundFractions() : void
    {
        $a = Matrix::fromArray([
            [1.2345, 2.675, -0.125],
            [0.125, 1.005, 12.345],
            [-12.345, 2.5, -2.5],
        ]);

        $b = $a->round(2);

        $expected = Matrix::fromArray([
            [round(1.2345, 2), round(2.675, 2), round(-0.125, 2)],
            [round(0.125, 2), round(1.005, 2), round(12.345, 2)],
            [round(-12.345, 2), round(2.5, 2), round(-2.5, 2)],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function roundNegativePrecisionThrows() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $this->expectException(InvalidArgumentException::class);

        $a->round(-1);
    }

    /**
     * @test
     */
    public function floor() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->floor();

        $expected = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function ceil() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->ceil();

        $expected = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * The integer fixtures above cannot tell floor and ceil apart, and they
     * never reach the cases where the rounding instruction in include/unary.c
     * has to agree with libm. Fractional magnitudes, the infinities, the NaN and
     * the extremes of the format are where a roundsd-based route could differ
     * from the scalar one, so they are pinned here.
     *
     * A signed zero is deliberately absent. The kernel itself would round
     * toward negative infinity and produce one, but fromArray() loses the sign
     * of a zero on the way in, so such a fixture would only be testing the
     * buffer. That is a separate, pre-existing question from the routing one.
     *
     * @test
     */
    public function floorAndCeilRoundLikeLibm() : void
    {
        $largest = 1.7976931348623157E+308;
        $smallest = 5.0E-324;

        $a = Matrix::fromArray([
            [1.5, -1.5, 0.5, -0.5],
            [2.0, -2.0, 0.0, 7.0],
            [INF, -INF, NAN, 4503599627370496.0],
            [$largest, -$largest, $smallest, -$smallest],
        ]);

        $fl = $a->floor()->asArray();
        $ce = $a->ceil()->asArray();

        // A positive fraction loses its fraction and a negative one loses it
        // toward negative infinity, so floor and ceil part company on sign.
        $this->assertEquals([1.0, -2.0, 0.0, -1.0], $fl[0]);
        $this->assertEquals([2.0, -1.0, 1.0, 0.0], $ce[0]);

        // Exact integers are a fixed point of both.
        $this->assertEquals([2.0, -2.0, 0.0, 7.0], $fl[1]);
        $this->assertEquals([2.0, -2.0, 0.0, 7.0], $ce[1]);

        // The infinities pass through and the NaN poisons its slot.
        $this->assertSame(INF, $fl[2][0]);
        $this->assertSame(-INF, $fl[2][1]);
        $this->assertSame(INF, $ce[2][0]);
        $this->assertSame(-INF, $ce[2][1]);

        // Every double at or above 2^52 is already integral, so the largest
        // magnitudes are their own floor and ceiling.
        $this->assertSame($largest, $fl[3][0]);
        $this->assertSame($largest, $ce[3][0]);
        $this->assertSame(-$largest, $fl[3][1]);
        $this->assertSame(-$largest, $ce[3][1]);

        // A subnormal is not an integer, so it rounds away: the positive one
        // ceilings to 1.0, the negative one floors to -1.0, and the two that
        // round toward zero land on a zero of the opposite sign to the input.
        $this->assertEquals(0.0, $fl[3][2]);
        $this->assertEquals(1.0, $ce[3][2]);
        $this->assertEquals(-1.0, $fl[3][3]);
        $this->assertEquals(0.0, $ce[3][3]);

        // Every non-NaN lane must equal PHP's own floor or ceil exactly, with no
        // delta: these are integer results, so any drift at all is a fault.
        foreach ([[$fl, 'floor'], [$ce, 'ceil']] as [$got, $which]) {
            foreach ($a->asArray() as $row => $values) {
                foreach ($values as $column => $value) {
                    if (is_nan($value)) {
                        $this->assertNan($got[$row][$column]);

                        continue;
                    }

                    $expected = $which === 'floor' ? floor($value) : ceil($value);

                    $this->assertSame(
                        $expected,
                        $got[$row][$column],
                        sprintf('%s returned the wrong double at row %d column %d.', $which, $row, $column)
                    );
                }
            }
        }
    }

    /**
     * @test
     */
    public function l1Norm() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $this->assertEqualsWithDelta(46.0, $a->l1Norm(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function l2Norm() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $this->assertEqualsWithDelta(39.68626966596886, $a->l2Norm(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function infinityNorm() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $this->assertEqualsWithDelta(51.0, $a->infinityNorm(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function maxNorm() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $this->assertEqualsWithDelta(22.0, $a->maxNorm(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function clip() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->clip(0.0, INF);

        $expected = Matrix::fromArray([
            [22.0, 0.0, 12.0],
            [4.0, 11.0, 0.],
            [20.0, 0.0, 0.],
        ]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function clipLower() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->clipLower(5.);

        $expected = Matrix::fromArray([
            [22.0, 5.0, 12.0],
            [5.0, 11.0, 5.],
            [20.0, 5.0, 5.],
        ]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function clipUpper() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->clipUpper(16.0);

        $expected = Matrix::fromArray([
            [16.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [16.0, -6.0, -9.0],
        ]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function sign() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->sign();

        $expected = Matrix::fromArray([
            [1.0, -1.0, 1.0],
            [1.0, 1.0, -1.0],
            [1.0, -1.0, -1.0],
        ]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function negate() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = $a->negate();

        $expected = Matrix::fromArray([
            [-22.0, 17.0, -12.0],
            [-4.0, -11.0, 2.0],
            [-20.0, 6.0, 9.0],
        ]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     */
    public function augmentAbove() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = Matrix::fromArray([
            [4.0, 6.0, -12.0],
            [1.0, 3.0, 5.0],
            [-10.0, -1.0, 14.0],
        ]);

        $c = $a->augmentAbove($b);

        $expected = Matrix::fromArray([
            [4.0, 6.0, -12.0],
            [1.0, 3.0, 5.0],
            [-10.0, -1.0, 14.0],
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @test
     */
    public function augmentBelow() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = Matrix::fromArray([
            [4.0, 6.0, -12.0],
            [1.0, 3.0, 5.0],
            [-10.0, -1.0, 14.0],
        ]);

        $c = $a->augmentBelow($b);

        $expected = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
            [4.0, 6.0, -12.0],
            [1.0, 3.0, 5.0],
            [-10.0, -1.0, 14.0],
        ]);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @test
     */
    public function augmentLeft() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = Matrix::fromArray([
            [13.0],
            [11.0],
            [9.0],
        ]);

        $c = $a->augmentLeft($b);

        $expected = Matrix::fromArray([
            [13.0, 22.0, -17.0, 12.0],
            [11.0, 4.0, 11.0, -2.0],
            [9.0, 20.0, -6.0, -9.0],
        ]);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @test
     */
    public function augmentRight() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $b = Matrix::fromArray([
            [13.0],
            [11.0],
            [9.0],
        ]);

        $c = $a->augmentRight($b);

        $expected = Matrix::fromArray([
            [22.0, -17.0, 12.0, 13.0],
            [4.0, 11.0, -2.0, 11.0],
            [20.0, -6.0, -9.0, 9.0],
        ]);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @test
     */
    public function augmentAboveWithEmptyMatrix() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]);

        $e = Matrix::fromArray([]);

        $above = $a->augmentAbove($e);
        $this->assertSame($a->asArray(), $above->asArray());
        $this->assertSame(2, $above->m());
        $this->assertSame(2, $above->n());

        $aboveEmpty = $e->augmentAbove($a);
        $this->assertSame($a->asArray(), $aboveEmpty->asArray());

        $empty = $e->augmentAbove($e);
        $this->assertSame(0, $empty->m());
        $this->assertSame(0, $empty->n());
    }

    /**
     * @test
     */
    public function augmentBelowWithEmptyMatrix() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]);

        $e = Matrix::fromArray([]);

        $below = $a->augmentBelow($e);
        $this->assertSame($a->asArray(), $below->asArray());

        $belowEmpty = $e->augmentBelow($a);
        $this->assertSame($a->asArray(), $belowEmpty->asArray());

        $empty = $e->augmentBelow($e);
        $this->assertSame(0, $empty->m());
        $this->assertSame(0, $empty->n());
    }

    /**
     * @test
     */
    public function augmentLeftWithEmptyMatrix() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]);

        $e = Matrix::fromArray([]);

        $left = $e->augmentLeft($a);
        $this->assertSame($a->asArray(), $left->asArray());

        $empty = $e->augmentLeft($e);
        $this->assertSame(0, $empty->m());
        $this->assertSame(0, $empty->n());
    }

    /**
     * @test
     */
    public function augmentRightWithEmptyMatrix() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]);

        $e = Matrix::fromArray([]);

        $right = $e->augmentRight($a);
        $this->assertSame($a->asArray(), $right->asArray());

        $empty = $e->augmentRight($e);
        $this->assertSame(0, $empty->m());
        $this->assertSame(0, $empty->n());
    }

    /**
     * @test
     */
    public function repeat() : void
    {
        $a = Matrix::fromArray([
            [13.0],
            [11.0],
            [9.0],
        ]);

        $b = $a->repeat(1, 3);

        $expected = Matrix::fromArray([
            [13.0, 13.0, 13.0, 13.0],
            [11.0, 11.0, 11.0, 11.0],
            [9.0, 9.0, 9.0, 9.0],
            [13.0, 13.0, 13.0, 13.0],
            [11.0, 11.0, 11.0, 11.0],
            [9.0, 9.0, 9.0, 9.0],
        ]);

        $this->assertEquals($expected->asArray(), $b->asArray());
    }

    /**
     * @test
     * @dataProvider repeatOverflowProvider
     *
     * @param int $m
     * @param int $n
     */
    public function repeatOverflowThrows(int $m, int $n) : void
    {
        $a = Matrix::fromArray([
            [13.0, 11.0],
            [9.0, 7.0],
        ]);

        $this->expectException(InvalidArgumentException::class);

        $a->repeat($m, $n);
    }

    /**
     * @return Generator<mixed[]>
     */
    public function repeatOverflowProvider() : Generator
    {
        // times + 1 wraps.
        yield [PHP_INT_MAX, 1];
        yield [1, PHP_INT_MAX];

        // m * (times_m + 1) and n * (times_n + 1) wrap.
        yield [PHP_INT_MAX, PHP_INT_MAX];
        yield [intdiv(PHP_INT_MAX, 2), 2];
        yield [2, intdiv(PHP_INT_MAX, 2)];

        // rows * cols wraps back to a small element count, so the result
        // buffer is allocated far too small for the copy loop.
        yield [4294967295, 4294967295];
        yield [4611686018427387904, 4611686018427387906];
    }

    /**
     * @test
     */
    public function fillNegativeMThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::fill(1.0, 0, 2);
    }

    /**
     * @test
     */
    public function fillNegativeNThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::fill(1.0, 2, 0);
    }

    /**
     * @test
     */
    public function identityNegativeNThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::identity(0);
    }

    /**
     * @test
     */
    public function detNonSquareThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [4.0, 5.0, 6.0],
        ])->det();
    }

    /**
     * tensor_matmul() hands its output to cblas_dgemm() with `beta = 0.0`,
     * which overwrites C and so never reads it. That is what allows the output
     * to come from the oversized buffer cache instead of being zeroed first,
     * but the optimisation is only valid for as long as BLAS keeps ignoring C.
     * If it ever read C, a recycled block still holding a non-finite value
     * would leak into the result, because 0.0 * NaN is NaN.
     *
     * So rather than trust that, this parks a non-finite block in the cache at
     * exactly the size of the product and checks the answer is still exact. A
     * single contaminated element anywhere would fail it.
     *
     * The poisons are built with arithmetic rather than Matrix::fill() because
     * only the arithmetic path allocates through the cache; a filled matrix
     * would never be handed to it.
     *
     * The output size is odd so that it cannot tie with an equally sized block
     * cached elsewhere, since an equal sized incumbent is kept rather than
     * replaced.
     *
     * @test
     */
    public function matmulIgnoresARecycledOutputBuffer() : void
    {
        $rows = 521;
        $inner = 503;
        $cols = 509;

        $a = Matrix::ones($rows, $inner);
        $b = Matrix::ones($inner, $cols);

        // Ones times ones is exactly $inner in every position, so the expected
        // result is exact and any contamination stands out immediately.
        $expected = Matrix::fill((float) $inner, $rows, $cols)->asArray();

        $inf = Vector::ones($rows * $cols)->divide(0.0);
        $nan = Vector::ones($rows * $cols)->divide(0.0)->multiply(0.0);

        $this->assertTrue(is_infinite($inf[0]), 'The Inf poison was not built.');
        $this->assertNan($nan[0], 'The NaN poison was not built.');

        unset($inf, $nan);

        foreach (['Inf', 'NaN'] as $label) {
            $product = $a->matmul($b);
            unset($product);

            $actual = $a->matmul($b)->asArray();

            $this->assertEqualsWithDelta(
                $expected,
                $actual,
                self::MAX_DELTA,
                "A recycled block still holding {$label} contaminated the matmul result, so BLAS is reading C."
            );
        }
    }

    /**
     * The same guarantee as matmulIgnoresARecycledOutputBuffer(), for the
     * matrix-vector product, which reaches cblas_dgemv() with `beta = 0.0` and
     * recycles its output for the same reason.
     *
     * @test
     */
    public function dotVectorIgnoresARecycledOutputBuffer() : void
    {
        $rows = 262_145;

        $a = Matrix::ones($rows, 1);
        $b = Vector::ones(1);

        $inf = Vector::ones($rows)->divide(0.0);
        $nan = Vector::ones($rows)->divide(0.0)->multiply(0.0);

        $this->assertTrue(is_infinite($inf[0]), 'The Inf poison was not built.');
        $this->assertNan($nan[0], 'The NaN poison was not built.');

        unset($inf, $nan);

        foreach (['Inf', 'NaN'] as $label) {
            $product = $a->dot($b);
            unset($product);

            $actual = $a->dot($b)->asArray();

            $this->assertEqualsWithDelta(
                array_fill(0, $rows, 1.0),
                $actual,
                self::MAX_DELTA,
                "A recycled block still holding {$label} contaminated the matrix-vector result, so BLAS is reading C."
            );
        }
    }

    /**
     * @test
     */
    public function matmulDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ])->matmul(Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
            [5.0, 6.0],
        ]));
    }

    /**
     * @test
     */
    public function dotDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [4.0, 5.0, 6.0],
        ])->dot(Vector::fromArray([1.0, 2.0]));
    }

    /**
     * @test
     */
    public function augmentAboveDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        Matrix::fromArray([
            [1.0, 2.0],
        ])->augmentAbove(Matrix::fromArray([
            [1.0, 2.0, 3.0],
        ]));
    }

    /**
     * @test
     */
    public function augmentBelowDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        Matrix::fromArray([
            [1.0, 2.0],
        ])->augmentBelow(Matrix::fromArray([
            [1.0, 2.0, 3.0],
        ]));
    }

    /**
     * @test
     */
    public function augmentLeftDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        Matrix::fromArray([
            [1.0, 2.0],
        ])->augmentLeft(Matrix::fromArray([
            [1.0],
            [2.0],
            [3.0],
        ]));
    }

    /**
     * @test
     */
    public function augmentRightDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        Matrix::fromArray([
            [1.0, 2.0],
        ])->augmentRight(Matrix::fromArray([
            [1.0],
            [2.0],
            [3.0],
        ]));
    }

    /**
     * @test
     */
    public function offsetSetThrows() : void
    {
        $this->expectException(RuntimeException::class);

        $a = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]);

        $a[0] = 10.0;
    }

    /**
     * @test
     */
    public function offsetUnsetThrows() : void
    {
        $this->expectException(RuntimeException::class);

        $a = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]);

        unset($a[0][0]);
    }

    /**
     * @test
     */
    public function offsetGetOutOfBoundsThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        $a = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]);

        $this->assertInstanceOf(Vector::class, $a[10]);
    }

    /**
     * @test
     */
    public function luNonSquareThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [4.0, 5.0, 6.0],
        ])->lu();
    }

    /**
     * @test
     */
    public function choleskyNonSquareThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [4.0, 5.0, 6.0],
        ])->cholesky();
    }

    /**
     * @test
     */
    public function eigReturnsEigen() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]);

        $eig = $a->eig(false);

        $this->assertInstanceOf(Eigen::class, $eig);

        $eigenvalues = $eig->eigenvalues();

        $eigenvectors = $eig->eigenvectors()->asArray();

        // This matrix has real eigenvalues, so the imaginary parts must be zero.
        $this->assertEqualsWithDelta([0.0, 0.0], $eig->eigenvaluesImaginary()->asArray(), 1e-8);

        $aa = $a->asArray();

        for ($j = 0; $j < 2; ++$j) {
            for ($i = 0; $i < 2; ++$i) {
                $sum = $aa[$i][0] * $eigenvectors[$j][0] + $aa[$i][1] * $eigenvectors[$j][1];

                $this->assertEqualsWithDelta($eigenvalues[$j] * $eigenvectors[$j][$i], $sum, 1e-8);
            }
        }
    }

    /**
     * @test
     */
    public function eigSymmetricReturnsEigen() : void
    {
        $a = Matrix::fromArray([
            [9.0, 3.0],
            [3.0, 5.0],
        ]);

        $eig = $a->eig(true);

        $this->assertInstanceOf(Eigen::class, $eig);

        $this->assertEqualsWithDelta([3.3944487241610, 10.605551275464], $eig->eigenvalues()->asArray(), 1e-8);
        $this->assertEqualsWithDelta([0.0, 0.0], $eig->eigenvaluesImaginary()->asArray(), 1e-8);
    }

    /**
     * @test
     */
    public function pseudoinversePreservesTinySingularValues() : void
    {
        $a = Matrix::fromArray([
            [1.0, 0.0],
            [0.0, 1e-9],
        ]);

        $expected = Matrix::fromArray([
            [1.0, 0.0],
            [0.0, 1.0 / 1e-9],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $a->pseudoinverse()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function pseudoinverseDiscardsSingularValuesBelowTolerance() : void
    {
        // The counterpart of pseudoinversePreservesTinySingularValues: a singular
        // value below the rank tolerance is roundoff rather than information
        // about the matrix, and the pseudo-inverse of zero is zero. The tolerance
        // is max(2, 2) * DBL_EPSILON * 1.0, so 1e-16 is far below it.
        $a = Matrix::fromArray([
            [1.0, 0.0],
            [0.0, 1e-16],
        ]);

        $expected = Matrix::fromArray([
            [1.0, 0.0],
            [0.0, 0.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $a->pseudoinverse()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function pseudoinverseRankDeficient() : void
    {
        // Rank 2. The third singular value comes back from the SVD at 4.4e-16
        // rather than at zero, so inverting it inverts roundoff and swamps the
        // product; the rank-deficient input has to be recognized as such.
        $a = Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [4.0, 5.0, 6.0],
            [7.0, 8.0, 9.0],
        ]);

        $this->assertEquals(2, $a->rank());

        $expected = Matrix::fromArray([
            [-0.638888888888888, -0.166666666666667, 0.305555555555556],
            [-0.055555555555556, 0.0, 0.055555555555556],
            [0.527777777777778, 0.166666666666667, -0.194444444444444],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $a->pseudoinverse()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function pseudoinverseRankDeficientWide() : void
    {
        // The same defect through a wide matrix, where the pseudo-inverse is
        // 4x3 and only two of the four columns carry a singular value.
        $a = Matrix::fromArray([
            [1.0, 2.0, 3.0, 4.0],
            [5.0, 6.0, 7.0, 8.0],
            [9.0, 10.0, 11.0, 12.0],
        ]);

        $this->assertEquals(2, $a->rank());

        $expected = Matrix::fromArray([
            [-0.375, -0.1, 0.175],
            [-0.145833333333333, -0.033333333333333, 0.079166666666667],
            [0.083333333333333, 0.033333333333333, -0.016666666666667],
            [0.3125, 0.1, -0.1125],
        ]);

        $b = $a->pseudoinverse();

        $this->assertEquals([4, 3], $b->shape());
        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function pseudoinverseZeroSingularValuesAreZeroed() : void
    {
        // Two of the three singular values are exactly zero, so the pseudo-inverse
        // inverts the one that is not. Inverting zero instead is what turned the
        // whole matrix into NAN.
        $a = Matrix::fromArray([
            [2.0, 0.0, 0.0],
            [0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0],
        ]);

        $expected = Matrix::fromArray([
            [0.5, 0.0, 0.0],
            [0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0],
        ]);

        $b = $a->pseudoinverse();

        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);

        foreach ($this->toFlatList($b) as $v) {
            $this->assertFinite($v);
        }
    }

    /**
     * @test
     */
    public function pseudoinverseOfZeroMatrixIsZero() : void
    {
        $a = Matrix::zeros(4, 3);
        $b = $a->pseudoinverse();

        $this->assertEquals([3, 4], $b->shape());
        $this->assertEqualsWithDelta(Matrix::zeros(3, 4)->asArray(), $b->asArray(), self::MAX_DELTA);

        foreach ($this->toFlatList($b) as $v) {
            $this->assertFinite($v);
        }
    }

    /**
     * @test
     */
    public function pseudoinverseTall() : void
    {
        // A tall matrix has more rows than the singular values it decomposes into,
        // and U is square in a row count the singular values do not fill. The
        // product that assembles the result contracts over the singular values, so
        // the rows of U that are not one of them are not part of it; a contraction
        // over the row count instead read past the end of both factor buffers and
        // returned values of order 1e45 for this input.
        $a = Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [4.0, 5.0, 6.0],
            [7.0, 8.0, 9.0],
            [10.0, 11.0, 12.0],
        ]);

        $this->assertEquals(2, $a->rank());

        $expected = Matrix::fromArray([
            [-0.483333333333333, -0.244444444444444, -0.005555555555556, 0.233333333333333],
            [-0.033333333333333, -0.011111111111111, 0.011111111111111, 0.033333333333333],
            [0.416666666666667, 0.222222222222222, 0.027777777777778, -0.166666666666667],
        ]);

        $b = $a->pseudoinverse();

        $this->assertEquals([3, 4], $b->shape());
        $this->assertEqualsWithDelta($expected->asArray(), $b->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function pseudoinverseSatisfiesMoorePenroseConditions() : void
    {
        // A P A = A, P A P = P, P A = (P A)^T and A P = (A P)^T, which is what
        // makes P a pseudo-inverse rather than merely some matrix A happens to
        // have a large left inverse of. The comparison is relative because the
        // result of inverting a 1e-9 singular value is of order 1e9, and the
        // identities hold to a few units in the last place of that rather than to
        // the absolute 1e-8 the rest of this file asserts against.
        $inputs = [
            [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0], [7.0, 8.0, 9.0]],
            [[1.0, 2.0, 3.0, 4.0], [5.0, 6.0, 7.0, 8.0], [9.0, 10.0, 11.0, 12.0]],
            [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0], [7.0, 8.0, 9.0], [10.0, 11.0, 12.0]],
            [[1.0, 2.0], [2.0, 4.0]],
            [[2.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0]],
            [[1.0, 0.0], [0.0, 1e-9]],
            [[0.0, 0.0], [0.0, 0.0]],
        ];

        foreach ($inputs as $rows) {
            $a = Matrix::fromArray($rows);
            $p = $a->pseudoinverse();

            $this->assertMatricesEqualRelative($a, $a->matmul($p)->matmul($a));
            $this->assertMatricesEqualRelative($p, $p->matmul($a)->matmul($p));

            $pa = $p->matmul($a);
            $ap = $a->matmul($p);

            $this->assertMatricesEqualRelative($pa, $pa->transpose());
            $this->assertMatricesEqualRelative($ap, $ap->transpose());
        }
    }

    /**
     * Assert that two matrices agree to within `relative` of the largest element
     * either of them holds, so a check is judged against the scale of the result
     * it is checking rather than against an absolute constant.
     *
     * @param Matrix $expected
     * @param Matrix $actual
     * @param float $relative
     */
    private function assertMatricesEqualRelative(Matrix $expected, Matrix $actual, float $relative = 1e-12) : void
    {
        $e = $this->toFlatList($expected);
        $a = $this->toFlatList($actual);

        $scale = 0.0;

        foreach ($e as $v) {
            $scale = max($scale, abs($v));
        }

        foreach ($a as $v) {
            $scale = max($scale, abs($v));
        }

        $this->assertEqualsWithDelta($e, $a, $relative * $scale);
    }

    /**
     * Build a flat `list<float>` of elements from a matrix's row arrays.
     *
     * @param Matrix $m
     * @return list<float>
     */
    private function toFlatList(Matrix $m) : array
    {
        $flat = [];

        foreach ($m->asArray() as $row) {
            foreach ($row as $v) {
                $flat[] = (float) $v;
            }
        }

        return $flat;
    }

    /**
     * Naive "valid" convolution sub-sampled every $stride elements, with the
     * kernel anchored by its second sample for even sizes, used as the oracle
     * for convolveMatchesReference().
     *
     * @param list<list<float>> $a
     * @param list<list<float>> $b
     * @param int $stride
     * @param int $padding
     *
     * @return list<list<float>>
     */
    private function referenceConvolve2d(array $a, array $b, int $stride, int $padding = 0) : array
    {
        $m = count($a);
        $n = count($a[0]);
        $mb = count($b);
        $nb = count($b[0]);
        $c0 = ($mb - 1) >> 1;
        $c1 = ($nb - 1) >> 1;
        $om = intdiv($m + 2 * $padding - $mb, $stride) + 1;
        $on = intdiv($n + 2 * $padding - $nb, $stride) + 1;
        $out = array_fill(0, $om, array_fill(0, $on, 0.0));

        // Output ($r, $c) reads the kernel window that the padding slides to
        // $r * $stride - $padding, and each axis is anchored by the kernel's own
        // centre, which is what keeps the sample the kernel is centred on lined
        // up with the output sample at a padding of 0.
        for ($r = 0; $r < $om; ++$r) {
            for ($c = 0; $c < $on; ++$c) {
                $sigma = 0.0;

                for ($k = 0; $k < $mb; ++$k) {
                    $x = $r * $stride - $padding + $c0 - $k;

                    if ($x < 0 || $x >= $m) {
                        continue;
                    }

                    for ($l = 0; $l < $nb; ++$l) {
                        $y = $c * $stride - $padding + $c1 - $l;

                        if ($y >= 0 && $y < $n) {
                            $sigma += $a[$x][$y] * $b[$k][$l];
                        }
                    }
                }

                $out[$r][$c] = $sigma;
            }
        }

        return $out;
    }
}
