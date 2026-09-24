<?php

namespace Tensor\Tests;

use Tensor\Tensor;
use Tensor\Vector;
use Tensor\Matrix;
use Tensor\Special;
use Tensor\ArrayLike;
use Tensor\Algebraic;
use Tensor\Arithmetic;
use Tensor\Comparable;
use Tensor\Statistical;
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
use PHPUnit\Framework\TestCase;
use Generator;

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
        $this->assertInstanceOf(Algebraic::class, $matrix);
        $this->assertInstanceOf(Trigonometric::class, $matrix);
        $this->assertInstanceOf(Statistical::class, $matrix);
        $this->assertInstanceOf(Special::class, $matrix);
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
    public function fromArrayThrowsOnNonArrayRow() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::fromArray([
            [1.0, 2.0],
            3.0,
        ]);
    }

    /**
     * @test
     */
    public function fromArrayThrowsOnRaggedColumns() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::fromArray([
            [1.0, 2.0],
            [3.0],
        ]);
    }

    /**
     * @test
     */
    public function fromArraySkipsValidationWhenValidateFalse() : void
    {
        $matrix = Matrix::fromArray([
            [1.0, 2.0],
            [3.0],
        ], false);

        $this->assertInstanceOf(Matrix::class, $matrix);
        $this->assertSame([2, 2], $matrix->shape());
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
    public function poisson() : void
    {
        $matrix = Matrix::poisson(6, 4, 2.);

        $this->assertCount(24, $matrix);
    }

    /**
     * @test
     */
    public function poissonZeroLambdaIsZero() : void
    {
        $matrix = Matrix::poisson(3, 3, 0.0);

        $expected = Matrix::fill(0.0, 3, 3);

        $this->assertEquals($expected->asArray(), $matrix->asArray());
    }

    /**
     * @test
     */
    public function poissonNegativeLambdaThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Matrix::poisson(1, 1, -1.0);
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
        // determinant must be ~0 rather than a spurious ~1e-15 value.
        $a = Matrix::fromArray([
            [2.0, 1.0, 0.0, 1.0],
            [1.0, 2.0, 1.0, 0.0],
            [0.0, 1.0, 2.0, 1.0],
            [1.0, 0.0, 1.0, 2.0],
        ]);

        $this->assertEqualsWithDelta(0.0, $a->det(), self::MAX_DELTA);
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

        // Exactly singular 4x4 (see rank() above); fullRank() must be false.
        $c = Matrix::fromArray([
            [2.0, 1.0, 0.0, 1.0],
            [1.0, 2.0, 1.0, 0.0],
            [0.0, 1.0, 2.0, 1.0],
            [1.0, 0.0, 1.0, 2.0],
        ]);

        $this->assertFalse($c->fullRank());
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

        $this->assertEqualsWithDelta($expected->eigenvalues(), $eig->eigenvalues(), self::MAX_DELTA);
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
                [
                    -15.096331148319537, 25.108706520450326, 13.9876246278692,
                ],
                Matrix::fromArray([
                    [0.25848694820886425, -0.11314537870318066, -0.9593657388523845],
                    [-0.8622719261400653, -0.17721179605718698, -0.47442924101375483],
                    [-0.6684472200177011, -0.6126879076802705, -0.42165369894378907],
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

        $values = [-366.30071669298195, 335.92000012383926, 1084.3807165691428];

        $vectors = Matrix::fromArray([
            [0.5423765325213931, 0.8162941265260668, -0.19872492538460218],
            [-0.04667292577741032, 0.26544998308386847, 0.9629942598375911],
            [-0.8388380862654284, 0.5130304137961217, -0.1820726765627782],
        ]);

        $expected = new Eigen($values, $vectors);

        $this->assertEqualsWithDelta($expected->eigenvalues(), $eig->eigenvalues(), self::MAX_DELTA);
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

        $singularValues = [
            34.66917512262571, 17.12630582468919, 8.929610580306822,
        ];

        $vT = Matrix::fromArray([
            [-0.8320393250771425, 0.531457514846513, -0.15894486917903863],
            [-0.4506078135544562, -0.48043370238236727, 0.7524201326246152],
            [-0.3235168618307952, -0.6976649392999047, -0.6392186422366096],
        ]);

        $expected = new SVD($u, $singularValues, $vT);

        $this->assertEqualsWithDelta($u->asArray(), $svd->u()->asArray(), self::MAX_DELTA);
        $this->assertEqualsWithDelta($singularValues, $svd->singularValues(), self::MAX_DELTA);
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

        $c = $a->convolve($b, 1);

        $expected = Matrix::fromArray([
            [3.0, 32.0, 75.0, 44.0, 84.0, 50.0],
            [32.0, 76.0, 49.0, 94.0, 72.0, 82.0],
            [10.0, 20.0, 53.0, 71.0, 91.0, 15.0],
            [5.0, 11.0, 26.0, 78.0, 34.0, 43.0],
            [1.0, 4.0, 12.0, 29.0, 48.0, 27.0],
            [0.0, 3.0, 19.0, 26.0, 27.0, 33.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
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

        $this->assertEqualsWithDelta([3.3944487241610, 10.605551275464], $eig->eigenvalues(), 1e-8);
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
}
