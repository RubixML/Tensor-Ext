<?php

namespace Tensor\Tests;

use Tensor\Settings;
use Tensor\Vector;
use Tensor\Matrix;
use Tensor\ColumnVector;
use PHPUnit\Framework\TestCase;

/**
 * @covers \Tensor\Settings
 */
class SettingsTest extends TestCase
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
    public function cpuFeatures() : void
    {
        $features = Settings::cpuFeatures();

        $this->assertIsArray($features);
        $this->assertArrayHasKey('avx', $features);
        $this->assertArrayHasKey('avx2', $features);
        $this->assertArrayHasKey('dispatch', $features);

        $this->assertIsBool($features['avx']);
        $this->assertIsBool($features['avx2']);
        $this->assertIsString($features['dispatch']);
    }

    /**
     * The dispatch route is chosen by, and only by, the AVX bit: the kernels
     * are switched over when AVX is usable and left at their baseline otherwise.
     *
     * @test
     */
    public function cpuFeaturesDispatchFollowsAvx() : void
    {
        $features = Settings::cpuFeatures();

        $expected = $features['avx'] ? 'avx' : 'scalar';

        $this->assertSame($expected, $features['dispatch']);
    }

    /**
     * AVX2 is an extension of AVX, so it cannot be present without it. A report
     * claiming otherwise would mean the detection was wrong.
     *
     * @test
     */
    public function cpuFeaturesWideningImpliesAvx() : void
    {
        $features = Settings::cpuFeatures();

        if ($features['avx2']) {
            $this->assertTrue($features['avx']);
        }
    }

    /**
     * The dispatched kernels must produce the same answers as the baseline ones
     * whether or not the AVX route was installed. The operations exercised here
     * are dispatched, so on an AVX CPU these run through the widened loops; on
     * anything else they run through the baseline. Either way the results have
     * to be right, which is what makes the detection safe to ship.
     *
     * Lengths are deliberately not multiples of four so the scalar tail of each
     * vector loop is covered as well as the vector body.
     *
     * @test
     */
    public function dispatchedKernelsAreCorrect() : void
    {
        foreach ([1, 3, 4, 7, 8, 13, 1000, 4099] as $n) {
            $a = [];
            $b = [];

            for ($i = 0; $i < $n; ++$i) {
                // Exactly representable values, so the expectations below hold
                // bit-for-bit rather than within a tolerance. The divisors are
                // powers of two and none of them is zero, which keeps the
                // quotients exact and the comparison strict.
                $a[] = ($i % 17) - 8.0;
                $b[] = [0.25, 0.5, 1.0, 2.0, 4.0][$i % 5];
            }

            $va = Vector::fromArray($a);
            $vb = Vector::fromArray($b);

            $this->assertSame(
                array_map(fn ($x, $y) => $x + $y, $a, $b),
                $va->add($vb)->asArray(),
                "add failed for length {$n}"
            );

            $this->assertSame(
                array_map(fn ($x, $y) => $x - $y, $a, $b),
                $va->subtract($vb)->asArray(),
                "subtract failed for length {$n}"
            );

            $this->assertSame(
                array_map(fn ($x, $y) => $x * $y, $a, $b),
                $va->multiply($vb)->asArray(),
                "multiply failed for length {$n}"
            );

            $this->assertSame(
                array_map(fn ($x, $y) => $x / $y, $a, $b),
                $va->divide($vb)->asArray(),
                "divide failed for length {$n}"
            );

            $this->assertSame(
                array_map(fn ($x) => $x * 3.0, $a),
                $va->multiply(3.0)->asArray(),
                "multiplyScalar failed for length {$n}"
            );

            $this->assertSame(
                array_map(fn ($x) => $x + 3.0, $a),
                $va->add(3.0)->asArray(),
                "addScalar failed for length {$n}"
            );

            $this->assertSame(
                array_map(fn ($x) => abs($x), $a),
                $va->abs()->asArray(),
                "abs failed for length {$n}"
            );

            $this->assertSame(
                array_map(fn ($x) => -$x, $a),
                $va->negate()->asArray(),
                "negate failed for length {$n}"
            );
        }
    }

    /**
     * The column and row broadcast forms are dispatched too, and their dimension
     * checks have to behave identically whichever route is installed.
     *
     * Row counts and column counts are swept independently and deliberately
     * include lengths that are not multiples of four, so both the vector body
     * and its scalar tail are covered in each of the two nesting directions.
     *
     * @test
     */
    public function dispatchedBroadcastKernelsAreCorrect() : void
    {
        foreach ([1, 3, 5, 7, 33] as $rows) {
            foreach ([1, 2, 4, 6, 9] as $columns) {
                $elements = [];
                $expected = [];

                for ($i = 0; $i < $rows * $columns; ++$i) {
                    $elements[] = ($i % 13) - 6.0;
                }

                foreach (range(0, $rows - 1) as $r) {
                    $expected[] = array_slice($elements, $r * $columns, $columns);
                }

                $matrix = Matrix::fromArray($expected);

                $columnValues = [];
                $rowValues = [];

                for ($i = 0; $i < $rows; ++$i) {
                    $columnValues[] = ($i % 7) - 3.0;
                }

                for ($j = 0; $j < $columns; ++$j) {
                    $rowValues[] = 0.25 * ($j % 3) - 0.5;
                }

                $column = ColumnVector::fromArray($columnValues);
                $row = Vector::fromArray($rowValues);

                // A column vector contributes one scalar to a whole row, so the
                // expectation has to be built per row index. Deriving it with a
                // single array_map over the row and the column would silently
                // truncate to whichever array is shorter.
                $added = [];

                foreach ($expected as $r => $elements) {
                    $added[$r] = array_map(fn ($x) => $x + $columnValues[$r], $elements);
                }

                $this->assertEqualsWithDelta(
                    $added,
                    $matrix->addColumnVector($column)->asArray(),
                    self::MAX_DELTA,
                    "addColumnVector failed for {$rows}x{$columns}"
                );

                $this->assertEqualsWithDelta(
                    array_map(
                        fn ($r) => array_map(fn ($x, $v) => $x * $v, $r, $rowValues),
                        $expected
                    ),
                    $matrix->multiplyVector($row)->asArray(),
                    self::MAX_DELTA,
                    "multiplyVector failed for {$rows}x{$columns}"
                );
            }
        }
    }
}
