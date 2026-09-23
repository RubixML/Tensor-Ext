<?php

namespace Tensor\Tests;

use Tensor\Matrix;
use Tensor\Vector;
use Tensor\ColumnVector;
use Tensor\Decompositions\Lu;
use PHPUnit\Framework\TestCase;
use PHPUnit\Framework\ExpectationFailedException;

/**
 * @covers \Tensor\Tests\TensorComparator
 */
class TensorComparatorTest extends TestCase
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
    public function comparesMatricesByValue() : void
    {
        $a = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);

        $equal = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);

        $different = Matrix::fromArray([
            [999, 888],
            [777, 666],
        ]);

        $this->assertEquals($a, $equal);
        $this->assertNotEquals($a, $different);
    }

    /**
     * @test
     */
    public function comparesMatricesByShape() : void
    {
        $a = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);

        $transposed = Matrix::fromArray([
            [1, 3],
            [2, 4],
        ]);

        $this->assertNotEquals($a, $transposed);
    }

    /**
     * @test
     */
    public function comparesMatricesWithinDelta() : void
    {
        $a = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);

        $b = Matrix::fromArray([
            [1.0000000001, 2],
            [3, 4],
        ]);

        $this->assertEqualsWithDelta($a, $b, self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function matricesOutsideOfDeltaAreUnequal() : void
    {
        $this->expectException(ExpectationFailedException::class);

        $a = Matrix::fromArray([
            [1, 2],
            [3, 4],
        ]);

        $b = Matrix::fromArray([
            [1.0001, 2],
            [3, 4],
        ]);

        $this->assertEqualsWithDelta($a, $b, self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function comparesVectorsByValue() : void
    {
        $a = Vector::fromArray([1, 2]);
        $equal = Vector::fromArray([1, 2]);
        $different = Vector::fromArray([9, 9]);

        $this->assertEquals($a, $equal);
        $this->assertNotEquals($a, $different);
    }

    /**
     * @test
     */
    public function comparesColumnVectorsByValue() : void
    {
        $a = ColumnVector::fromArray([1, 2]);
        $different = ColumnVector::fromArray([9, 9]);

        $this->assertNotEquals($a, $different);
    }

    /**
     * @test
     */
    public function distinguishesVectorFromColumnVector() : void
    {
        $vector = Vector::fromArray([1, 2]);
        $column = ColumnVector::fromArray([1, 2]);

        $this->assertNotEquals($vector, $column);
    }

    /**
     * @test
     */
    public function comparesDecompositionsByValue() : void
    {
        $a = new Lu(
            Matrix::fromArray([[1, 0], [0, 1]]),
            Matrix::fromArray([[2, 0], [0, 2]]),
            Matrix::fromArray([[1, 0], [0, 1]])
        );

        $b = new Lu(
            Matrix::fromArray([[1, 0], [0, 1]]),
            Matrix::fromArray([[999, 0], [0, 2]]),
            Matrix::fromArray([[1, 0], [0, 1]])
        );

        $this->assertNotEquals($a, $b);
    }
}
