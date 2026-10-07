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
use Tensor\Buffer;
use Tensor\TensorBuffer;
use PHPUnit\Framework\TestCase;
use ReflectionMethod;

/**
 * @covers \Tensor\ColumnVector
 */
class ColumnVectorTest extends TestCase
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
        $vector = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $this->assertInstanceOf(ColumnVector::class, $vector);
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
    public function fromBuffer() : void
    {
        $buffer = new TensorBuffer(Buffer::fromArray([-15.0, 25.0, 35.0]));

        $vector = ColumnVector::fromBuffer($buffer);

        $this->assertInstanceOf(ColumnVector::class, $vector);
        $this->assertSame($buffer, $vector->buffer());
        $this->assertSame([3], $vector->shape());
        $this->assertEqualsWithDelta([-15.0, 25.0, 35.0], $vector->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function fromBufferRejectsIntegerBuffers() : void
    {
        $this->expectException(InvalidArgumentException::class);
        $this->expectExceptionMessage('Argument must wrap a buffer of type double.');

        $buffer = new TensorBuffer(Buffer::fromArray([-15, 25, 35], Buffer::TYPE_LONG));

        ColumnVector::fromBuffer($buffer);
    }

    /**
     * @test
     */
    public function constructorIsProtected() : void
    {
        $this->assertTrue((new ReflectionMethod(ColumnVector::class, '__construct'))->isProtected());
    }

    /**
     * @test
     */
    public function shape() : void
    {
        $vector = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $this->assertEquals([3], $vector->shape());
    }

    /**
     * @test
     */
    public function shapeString() : void
    {
        $vector = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $this->assertEquals('3', $vector->shapeString());
    }

    /**
     * @test
     */
    public function size() : void
    {
        $vector = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $this->assertEquals(3, $vector->size());
    }

    /**
     * @test
     */
    public function serialization() : void
    {
        $vector = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $serialized = serialize($vector);

        $this->assertStringNotContainsString('TensorBuffer', $serialized);

        $restored = unserialize($serialized);

        $this->assertInstanceOf(ColumnVector::class, $restored);
        $this->assertEquals([-15.0, 25.0, 35.0], $restored->asArray());
        $this->assertSame(serialize($vector), serialize($restored));
    }

    /**
     * @test
     */
    public function m() : void
    {
        $vector = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $this->assertEquals(3, $vector->m());
    }

    /**
     * @test
     */
    public function n() : void
    {
        $vector = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $this->assertEquals(1, $vector->n());
    }

    /**
     * @test
     */
    public function multiply() : void
    {
        $a = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $b = Matrix::fromArray([
            [6.23, -1.0, 0.03],
            [0.01, 2.01, 1.0],
            [1.1, 5.0, -5.0],
        ]);

        $c = $a->multiply($b);

        $expected = Matrix::fromArray([
            [-93.45, 15.0, -0.44999999999999996],
            [0.25, 50.24999999999999, 25.0],
            [38.5, 175.0, -175.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function divide() : void
    {
        $a = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $b = Matrix::fromArray([
            [6.23, -1.0, 0.03],
            [0.01, 2.01, 1.0],
            [1.1, 5.0, -5.0],
        ]);

        $c = $a->divide($b);

        $expected = Matrix::fromArray([
            [-2.407704654895666, 15.0, -500.],
            [2500.0, 12.437810945273633, 25.0],
            [31.818181818181817, 7.0, -7.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function add() : void
    {
        $a = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $b = Matrix::fromArray([
            [6.23, -1.0, 0.03],
            [0.01, 2.01, 1.0],
            [1.1, 5.0, -5.0],
        ]);

        $c = $a->add($b);

        $expected = Matrix::fromArray([
            [-8.77, -16.0, -14.97],
            [25.01, 27.009999999999998, 26.0],
            [36.1, 40.0, 30.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function subtract() : void
    {
        $a = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $b = Matrix::fromArray([
            [6.23, -1.0, 0.03],
            [0.01, 2.01, 1.0],
            [1.1, 5.0, -5.0],
        ]);

        $c = $a->subtract($b);

        $expected = Matrix::fromArray([
            [-21.23, -14.0, -15.03],
            [24.99, 22.990000000000002, 24.0],
            [33.9, 30.0, 40.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function equal() : void
    {
        $a = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $b = Matrix::fromArray([
            [6.23, -1.0, 0.03],
            [0.01, 2.01, 1.0],
            [1.1, 5.0, -5.0],
        ]);

        $c = $a->equal($b);

        $expected = Matrix::fromArray([
            [0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0],
        ]);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @test
     */
    public function notEqual() : void
    {
        $a = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $b = Matrix::fromArray([
            [6.23, -1.0, 0.03],
            [0.01, 2.01, 1.0],
            [1.1, 5.0, -5.0],
        ]);

        $c = $a->notEqual($b);

        $expected = Matrix::fromArray([
            [1.0, 1.0, 1.0],
            [1.0, 1.0, 1.0],
            [1.0, 1.0, 1.0],
        ]);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @test
     */
    public function greater() : void
    {
        $a = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $b = Matrix::fromArray([
            [6.23, -1.0, 0.03],
            [0.01, 2.01, 1.0],
            [1.1, 5.0, -5.0],
        ]);

        $c = $a->greater($b);

        $expected = Matrix::fromArray([
            [0.0, 0.0, 0.0],
            [1.0, 1.0, 1.0],
            [1.0, 1.0, 1.0],
        ]);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @test
     */
    public function greaterEqual() : void
    {
        $a = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $b = Matrix::fromArray([
            [6.23, -1.0, 0.03],
            [0.01, 2.01, 1.0],
            [1.1, 5.0, -5.0],
        ]);

        $c = $a->greaterEqual($b);

        $expected = Matrix::fromArray([
            [0.0, 0.0, 0.0],
            [1.0, 1.0, 1.0],
            [1.0, 1.0, 1.0],
        ]);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @test
     */
    public function less() : void
    {
        $a = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $b = Matrix::fromArray([
            [6.23, -1.0, 0.03],
            [0.01, 2.01, 1.0],
            [1.1, 5.0, -5.0],
        ]);

        $c = $a->less($b);

        $expected = Matrix::fromArray([
            [1.0, 1.0, 1.0],
            [0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0],
        ]);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @test
     */
    public function lessEqual() : void
    {
        $a = ColumnVector::fromArray([-15.0, 25.0, 35.0]);

        $b = Matrix::fromArray([
            [6.23, -1.0, 0.03],
            [0.01, 2.01, 1.0],
            [1.1, 5.0, -5.0],
        ]);

        $c = $a->lessEqual($b);

        $expected = Matrix::fromArray([
            [1.0, 1.0, 1.0],
            [0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0],
        ]);

        $this->assertEquals($expected->asArray(), $c->asArray());
    }

    /**
     * @test
     */
    public function transposeReturnsVector() : void
    {
        $a = ColumnVector::fromArray([1.0, 2.0, 3.0]);

        $b = $a->transpose();

        $this->assertInstanceOf(Vector::class, $b);
        $this->assertEquals(Vector::fromArray([1.0, 2.0, 3.0])->asArray(), $b->asArray());
    }

    /**
     * The univariate operations are inherited from Vector, so the only thing
     * worth pinning down here is that they stay a ColumnVector when they are.
     *
     * @test
     */
    public function softmaxReturnsColumnVector() : void
    {
        $a = ColumnVector::fromArray([1.0, 2.0, 3.0]);

        $b = $a->softmax();

        $this->assertInstanceOf(ColumnVector::class, $b);
        $this->assertEqualsWithDelta(
            Vector::fromArray([0.09003057317038046, 0.24472847105479764, 0.6652409557748218])->asArray(),
            $b->asArray(),
            1e-8
        );
    }

    /**
     * @test
     */
    public function sigmoidReturnsColumnVector() : void
    {
        $b = ColumnVector::fromArray([1.0, -1.0, 0.0])->sigmoid();

        $this->assertInstanceOf(ColumnVector::class, $b);
        $this->assertEqualsWithDelta([0.7310585786300049, 0.2689414213699951, 0.5], $b->asArray(), 1e-8);
    }

    /**
     * @test
     */
    public function softplusReturnsColumnVector() : void
    {
        $b = ColumnVector::fromArray([1.0, -1.0, 0.0])->softplus();

        $this->assertInstanceOf(ColumnVector::class, $b);
        $this->assertEqualsWithDelta([1.3132616875182, 0.31326168751822, 0.69314718055995], $b->asArray(), 1e-8);
    }

    /**
     * @test
     */
    public function tanhReturnsColumnVector() : void
    {
        $b = ColumnVector::fromArray([1.0, -1.0, 0.0])->tanh();

        $this->assertInstanceOf(ColumnVector::class, $b);
        $this->assertEqualsWithDelta([0.76159415595576485, -0.76159415595576485, 0.0], $b->asArray(), 1e-8);
    }

    /**
     * @test
     */
    public function sinhReturnsColumnVector() : void
    {
        $b = ColumnVector::fromArray([1.0, -1.0, 0.0])->sinh();

        $this->assertInstanceOf(ColumnVector::class, $b);
        $this->assertEqualsWithDelta([1.1752011936438014, -1.1752011936438014, 0.0], $b->asArray(), 1e-8);
    }

    /**
     * @test
     */
    public function coshReturnsColumnVector() : void
    {
        $b = ColumnVector::fromArray([1.0, -1.0, 0.0])->cosh();

        $this->assertInstanceOf(ColumnVector::class, $b);
        $this->assertEqualsWithDelta([1.5430806348152437, 1.5430806348152437, 1.0], $b->asArray(), 1e-8);
    }

    /**
     * @test
     */
    public function erfReturnsColumnVector() : void
    {
        $b = ColumnVector::fromArray([1.0, -1.0, 0.0])->erf();

        $this->assertInstanceOf(ColumnVector::class, $b);
        $this->assertEqualsWithDelta([0.8427007929497148, -0.8427007929497148, 0.0], $b->asArray(), 1e-8);
    }

    /**
     * @test
     */
    public function rsqrtReturnsColumnVector() : void
    {
        $b = ColumnVector::fromArray([4.0, 9.0, 0.25])->rsqrt();

        $this->assertInstanceOf(ColumnVector::class, $b);
        $this->assertEqualsWithDelta([0.5, 0.3333333333333333, 2.0], $b->asArray(), 1e-8);
    }

    /**
     * @test
     */
    public function sizes() : void
    {
        $a = ColumnVector::fromArray([1.0, 2.0, 3.0]);

        $this->assertEquals(3, $a->m());
        $this->assertEquals(1, $a->n());
        $this->assertEquals(3, $a->size());
    }

    /**
     * @test
     */
    public function matmul() : void
    {
        $a = ColumnVector::fromArray([1.0, 2.0, 3.0]);

        $b = Matrix::fromArray([
            [1.0, 2.0, 3.0],
        ]);

        $c = $a->matmul($b);

        $expected = Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [2.0, 4.0, 6.0],
            [3.0, 6.0, 9.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function matmulDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        (ColumnVector::fromArray([1.0, 2.0, 3.0]))->matmul(Matrix::fromArray([
            [1.0, 2.0, 3.0, 4.0],
            [5.0, 6.0, 7.0, 8.0],
            [9.0, 10.0, 11.0, 12.0],
        ]));
    }

    /**
     * @test
     */
    public function powMatrix() : void
    {
        $a = ColumnVector::fromArray([2.0, 3.0, 4.0]);

        $b = Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [1.0, 1.0, 1.0],
            [2.0, 0.0, 1.0],
        ]);

        $c = $a->powMatrix($b);

        $expected = Matrix::fromArray([
            [2.0, 4.0, 8.0],
            [3.0, 3.0, 3.0],
            [16.0, 1.0, 4.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function powMatrixDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        (ColumnVector::fromArray([1.0, 2.0, 3.0]))->powMatrix(Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]));
    }

    /**
     * @test
     */
    public function modMatrix() : void
    {
        $a = ColumnVector::fromArray([10.0, 12.0, 15.0]);

        $b = Matrix::fromArray([
            [3.0, 4.0, 5.0],
            [2.0, 3.0, 4.0],
            [5.0, 6.0, 7.0],
        ]);

        $c = $a->modMatrix($b);

        $expected = Matrix::fromArray([
            [1.0, 2.0, 0.0],
            [0.0, 0.0, 0.0],
            [0.0, 3.0, 1.0],
        ]);

        $this->assertEqualsWithDelta($expected->asArray(), $c->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function modMatrixDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        (ColumnVector::fromArray([1.0, 2.0, 3.0]))->modMatrix(Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]));
    }

    /**
     * @test
     */
    public function multiplyMatrixDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        (ColumnVector::fromArray([1.0, 2.0, 3.0]))->multiplyMatrix(Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]));
    }

    /**
     * @test
     */
    public function divideMatrixDimensionMismatchThrows() : void
    {
        $this->expectException(DimensionalityMismatch::class);

        (ColumnVector::fromArray([1.0, 2.0, 3.0]))->divideMatrix(Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]));
    }
}
