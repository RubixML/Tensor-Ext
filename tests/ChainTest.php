<?php

namespace Tensor\Tests;

use Tensor\Chain;
use Tensor\Matrix;
use Tensor\Exceptions\InvalidArgumentException;
use Tensor\Exceptions\DimensionalityMismatch;
use PHPUnit\Framework\TestCase;

/**
 * Tests for the Chain DSL and its planner.
 *
 * The chain is a primitive-only composition -- matmul, add, negate, exp, fork,
 * combineDivide -- and the C planner (ext/include/chain.c) recognises the SiLU
 * composition and swaps it for the fused tensor_silu kernel. The tests cover
 * the matching case (the fused path) and the near-miss case (the sequential
 * fallback), and verify both the arithmetic and the shape bookkeeping.
 *
 * @covers \Tensor\Chain
 */
class ChainTest extends TestCase
{
    /**
     * @var float
     */
    protected const MAX_DELTA = 1e-9;

    /**
     * SiLU is the canonical fusion target. Compose it out of the primitives
     * and verify the planner's output matches the eager reference
     * x / (1 + exp(-x)) elementwise.
     *
     * @test
     */
    public function siluFusionMatchesEager() : void
    {
        $x = $this->ramp(3, 5, 0.5);
        $y = $this->ramp(4, 2, -1.5);

        foreach ([$x, $y] as $index => $input) {
            $expected = [];

            foreach ($input->asArray() as $row) {
                foreach ($row as $value) {
                    $expected[] = $value / (1.0 + exp(-$value));
                }
            }

            $actual = Chain::of($input)
                ->fork()
                ->negate()
                ->exp()
                ->add(1.0)
                ->combineDivide()
                ->done()
              ->asArray();

            $indexIn = 0;

            foreach ($actual as $row) {
                foreach ($row as $value) {
                    $this->assertEqualsWithDelta(
                        $expected[$indexIn],
                        $value,
                        self::MAX_DELTA,
                        "input {$index}: element {$indexIn} diverges from SiLU."
                    );

                    ++$indexIn;
                }
            }
        }
    }

    /**
     * The chain must produce a Matrix of the same shape as the input,
     * regardless of whether the SiLU plan fused or fell through.
     *
     * @test
     */
    public function siluFusionPreservesShape() : void
    {
        $x = Matrix::gaussian(5, 3);

        $result = Chain::of($x)
            ->fork()
            ->negate()
            ->exp()
            ->add(1.0)
            ->combineDivide()
            ->done();

        $this->assertInstanceOf(Matrix::class, $result);
        $this->assertSame(5, $result->m());
        $this->assertSame(3, $result->n());
    }

    /**
     * The non-fusing branch (ADD with a non-1.0 scalar) still runs the
     * primitives in order and produces the right number: x / (exp(-x) + 0.5).
     *
     * @test
     */
    public function nonFusingBranchIsStillCorrect() : void
    {
        $x = $this->ramp(2, 3, 0.25);

        $expected = [];

        foreach ($x->asArray() as $row) {
            foreach ($row as $value) {
                $expected[] = $value / (exp(-$value) + 0.5);
            }
        }

        $actual = Chain::of($x)
            ->fork()
            ->negate()
            ->exp()
            ->add(0.5)
            ->combineDivide()
            ->done()
          ->asArray();

        $indexIn = 0;

        foreach ($actual as $row) {
            foreach ($row as $value) {
                $this->assertEqualsWithDelta(
                    $expected[$indexIn],
                    $value,
                    self::MAX_DELTA,
                    "element {$indexIn} diverges from the sequential reference."
                );

                ++$indexIn;
            }
        }
    }

    /**
     * An empty chain is the identity: it returns the source matrix with the
     * source shape, as an independent object.
     *
     * @test
     */
    public function emptyChainIsIdentity() : void
    {
        $x = $this->ramp(2, 2);
        $y = Chain::of($x)->done();

        $this->assertInstanceOf(Matrix::class, $y);
        $this->assertSame(2, $y->m());
        $this->assertSame(2, $y->n());

        foreach ($x->asArray() as $i => $rowA) {
            foreach ($rowA as $j => $value) {
                $this->assertEqualsWithDelta(
                    $value,
                    $y->asArray()[$i][$j],
                    self::MAX_DELTA
                );
            }
        }

        $this->assertNotSame($x, $y);
    }

    /**
     * A matmul chain should compose the shape correctly through the planner,
     * and the numbers should match an eager matmul.
     *
     * @test
     */
    public function matmulChainTracksShapeAndNumerics() : void
    {
        $x = $this->ramp(2, 3);
        $w = $this->ramp(3, 4);
        $v = $this->ramp(4, 1);

        $result = Chain::of($x)
            ->matmul($w)
            ->matmul($v)
            ->done();

        // 2x3 @ 3x4 -> 2x4, then 2x4 @ 4x1 -> 2x1.
        $this->assertSame(2, $result->m());
        $this->assertSame(1, $result->n());

        $expected = $x->matmul($w)->matmul($v);

        foreach ($expected->asArray() as $i => $rowA) {
            foreach ($rowA as $j => $value) {
                $this->assertEqualsWithDelta(
                    $value,
                    $result->asArray()[$i][$j],
                    self::MAX_DELTA
                );
            }
        }
    }

    /**
     * A scalar add plus a negate exercises the ADD(scalar) and NEGATE
     * fallbacks (neither is in the fuse table), and the shape must come back
     * unchanged.
     *
     * @test
     */
    public function scalarAddAndNegateAreCorrect() : void
    {
        $x = $this->ramp(2, 3);
        $s = 0.5;

        $result = Chain::of($x)
            ->add($s)
            ->negate()
            ->done();

        $this->assertSame(2, $result->m());
        $this->assertSame(3, $result->n());

        foreach ($x->asArray() as $i => $rowA) {
            foreach ($rowA as $j => $value) {
                $this->assertEqualsWithDelta(
                    -1.0 * ($value + $s),
                    $result->asArray()[$i][$j],
                    self::MAX_DELTA
                );
            }
        }
    }

    /**
     * A matmul in the chain whose weight does not agree with the running
     * shape must raise DimensionalityMismatch before any planning happens.
     *
     * @test
     */
    public function matmulDimensionMismatchThrows() : void
    {
        $x = $this->ramp(2, 3);
        $w = $this->ramp(2, 4);

        $this->expectException(DimensionalityMismatch::class);

        Chain::of($x)->matmul($w);
    }

    /**
     * An unsupported operand to add() raises an InvalidArgumentException.
     *
     * @test
     */
    public function addWithUnsupportedOperandThrows() : void
    {
        $x = $this->ramp(2, 2);

        $this->expectException(InvalidArgumentException::class);

        Chain::of($x)->add('not-a-number');
    }

    /**
     * Build a matrix of m x n elements, each seeded linearly.
     *
     * @param int $m
     * @param int $n
     * @param float $offset
     * @return Matrix
     */
    protected function ramp(int $m, int $n, float $offset = 1.0) : Matrix
    {
        $rows = [];

        for ($i = 0; $i < $m; ++$i) {
            $row = [];

            for ($j = 0; $j < $n; ++$j) {
                $row[] = $offset + $i * $n + $j;
            }

            $rows[] = $row;
        }

        return Matrix::fromArray($rows);
    }
}
