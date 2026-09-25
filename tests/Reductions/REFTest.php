<?php

namespace Tensor\Tests\Reductions;

use Tensor\Matrix;
use Tensor\Reductions\REF;
use Tensor\Exceptions\InvalidArgumentException;
use PHPUnit\Framework\TestCase;

/**
 * @covers \Tensor\Reductions\REF
 */
class REFTest extends TestCase
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
    public function reduce2x2() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
        ]);

        $ref = REF::reduce($a);

        // Partial pivoting selects the largest magnitude in each column, so the
        // rows are swapped to bring 3 (col 0) to the top and 4 (col 1) to the right.
        $expectedA = Matrix::fromArray([
            [3.0, 4.0],
            [0.0, 2.0 / 3.0],
        ]);

        $this->assertEquals(1, $ref->swaps());
        $this->assertEqualsWithDelta($expectedA->asArray(), $ref->a()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function reduce3x3() : void
    {
        $a = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [4.0, 11.0, -2.0],
            [20.0, -6.0, -9.0],
        ]);

        $ref = REF::reduce($a);

        $expectedA = Matrix::fromArray([
            [22.0, -17.0, 12.0],
            [0.0, 14.09090909090909, -4.181818181818182],
            [0.0, 0.0, -17.10322580645161],
        ]);

        $this->assertEqualsWithDelta($expectedA->asArray(), $ref->a()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function reduce2x3Rectangular() : void
    {
        $a = Matrix::fromArray([
            [1.0, 2.0, 3.0],
            [4.0, 5.0, 6.0],
        ]);

        $ref = REF::reduce($a);

        // Partial pivoting brings 4 to the top of column 0.
        $expectedA = Matrix::fromArray([
            [4.0, 5.0, 6.0],
            [0.0, 0.75, 1.5],
        ]);

        $this->assertEquals(1, $ref->swaps());
        $this->assertEqualsWithDelta($expectedA->asArray(), $ref->a()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function reduce3x2Tall() : void
    {
        // A tall (m > n) matrix over-reads the pivot array when counting swaps:
        // dgetrf writes only min(m, n) pivots, so the loop must be bounded
        // accordingly.
        $a = Matrix::fromArray([
            [1.0, 2.0],
            [3.0, 4.0],
            [5.0, 6.0],
        ]);

        $ref = REF::reduce($a);

        // Partial pivoting brings 5 (col 0) to the top.
        $expectedA = Matrix::fromArray([
            [5.0, 6.0],
            [0.0, 0.8],
            [0.0, 0.0],
        ]);

        // Two swaps (one per column): col 0 -> bring 5 to top from row 2,
        // col 1 -> bring 0.8 to row 1 from row 2.
        $this->assertEquals(2, $ref->swaps());
        $this->assertEqualsWithDelta($expectedA->asArray(), $ref->a()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function reduceEmptyMatrix() : void
    {
        // An m x 0 matrix has no columns: it is trivially in REF and the
        // zero-length pivot buffer must not be over-read.
        $a = Matrix::fromArray([[], []]);

        $ref = REF::reduce($a);

        $this->assertEquals(0, $ref->swaps());
        $this->assertEquals([2, 0], $ref->a()->shape());
        $this->assertEquals([], $ref->a()->asArray());
    }

    /**
     * @test
     */
    public function reduceRequiresPivoting() : void
    {
        // First column is [0, 5] - a row swap is required to pivot.
        $a = Matrix::fromArray([
            [0.0, 1.0],
            [5.0, 2.0],
        ]);

        $ref = REF::reduce($a);

        // REF should still be valid: the second row's leading element
        // should be used as the pivot, and the first row below it zeroed.
        $this->assertGreaterThanOrEqual(1, $ref->swaps());

        $expectedA = Matrix::fromArray([
            [5.0, 2.0],
            [0.0, 1.0],
        ]);

        $this->assertEqualsWithDelta($expectedA->asArray(), $ref->a()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function reduce1x1() : void
    {
        $a = Matrix::fromArray([[7.0]]);

        $ref = REF::reduce($a);

        $expectedA = Matrix::fromArray([[7.0]]);

        $this->assertEquals(0, $ref->swaps());
        $this->assertEqualsWithDelta($expectedA->asArray(), $ref->a()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function reduceDiagonal() : void
    {
        $a = Matrix::fromArray([
            [2.0, 0.0],
            [0.0, 3.0],
        ]);

        $ref = REF::reduce($a);

        // Diagonal is already in row echelon form.
        $expectedA = Matrix::fromArray([
            [2.0, 0.0],
            [0.0, 3.0],
        ]);

        $this->assertEquals(0, $ref->swaps());
        $this->assertEqualsWithDelta($expectedA->asArray(), $ref->a()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function reduceZeroRow() : void
    {
        // One zero row at the top, non-zero row at the bottom.
        $a = Matrix::fromArray([
            [0.0, 0.0],
            [1.0, 2.0],
        ]);

        $ref = REF::reduce($a);

        // The zero row remains zero. After reduction the pivots should appear
        // in the first column of the non-zero row and the zero row should have
        // all-zero entries.
        $aOut = $ref->a()->asArray();

        $this->assertEquals([0.0, 0.0], $aOut[1]);
    }

    /**
     * @test
     */
    public function reduceSingularKeepsPivotScale() : void
    {
        // A rank-1 matrix is singular: Gaussian elimination must fail and the
        // row reduction fallback must produce the same (non-normalised) REF
        // convention as Gaussian elimination - the pivot keeps its value.
        $a = Matrix::fromArray([
            [2.0, 4.0],
            [1.0, 2.0],
        ]);

        $ref = REF::reduce($a);

        $expectedA = Matrix::fromArray([
            [2.0, 4.0],
            [0.0, 0.0],
        ]);

        $this->assertEqualsWithDelta($expectedA->asArray(), $ref->a()->asArray(), self::MAX_DELTA);
    }

    /**
     * @test
     */
    public function constructorWithNegativeSwapsThrows() : void
    {
        $this->expectException(InvalidArgumentException::class);

        new REF(Matrix::fromArray([[1.0]]), -1);
    }

    /**
     * @test
     */
    public function constructorWithZeroSwaps() : void
    {
        $a = Matrix::fromArray([[1.0]]);

        $ref = new REF($a, 0);

        $this->assertEqualsWithDelta($a->asArray(), $ref->a()->asArray(), self::MAX_DELTA);
        $this->assertEquals(0, $ref->swaps());
    }

    /**
     * @test
     */
    public function constructorWithPositiveSwaps() : void
    {
        $a = Matrix::fromArray([
            [1.0, 0.0],
            [0.0, 1.0],
        ]);

        $ref = new REF($a, 2);

        $this->assertEqualsWithDelta($a->asArray(), $ref->a()->asArray(), self::MAX_DELTA);

        $this->assertEquals(2, $ref->swaps());
    }
}
