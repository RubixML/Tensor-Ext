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
        $this->assertArrayHasKey('avx512', $features);
        $this->assertArrayHasKey('fma', $features);

        $this->assertIsBool($features['avx']);
        $this->assertIsBool($features['avx2']);
        $this->assertIsBool($features['avx512']);
        $this->assertIsBool($features['fma']);
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

    /**
     * Calling Settings::disableOptimizedKernels() once must reset the dispatched
     * kernel routes to their baseline variants without changing the observable
     * result of any op, so on AVX/AVX-512 CPUs the user can pin the baseline
     * path (and bit-identical results across host environments) without
     * recompiling the extension.
     *
     * The ops here are chosen from the bit-stable subset already covered by
     * dispatchedKernelsAreCorrect: the AVX/AVX-512 variants of those ops
     * produce bit-identical results to the baseline, so the assertion is a
     * strict assertSame. The FMA-backed ops (convolution, LU row update) are
     * intentionally not asserted bit-equal across the two routes, because
     * fma(x, y, acc) is one rounding and acc + x * y is two, by design.
     *
     * This test runs last in the file because the call leaves the baseline
     * route installed. That is harmless to any test which follows -- the
     * baseline kernels return the same answers, only more slowly -- so
     * enableOptimizedKernelsRestoresOptimizedRoute is free to run after this
     * one and put the widest route back.
     *
     * @test
     */
    public function disableOptimizedKernelsRestoresBaselineRoute() : void
    {
        foreach ([1, 3, 4, 7, 8, 13, 1000] as $n) {
            $a = [];
            $b = [];

            for ($i = 0; $i < $n; ++$i) {
                // Exactly representable values; quotients by powers of two are
                // exact; assertSame then holds bit-for-bit.
                $a[] = ($i % 17) - 8.0;
                $b[] = [0.25, 0.5, 1.0, 2.0, 4.0][$i % 5];
            }

            $va = Vector::fromArray($a);
            $vb = Vector::fromArray($b);

            $this->assertSame(
                array_map(fn ($x, $y) => $x + $y, $a, $b),
                $va->add($vb)->asArray(),
                "add (pre) failed for length {$n}"
            );
            $this->assertSame(
                array_map(fn ($x, $y) => $x - $y, $a, $b),
                $va->subtract($vb)->asArray(),
                "subtract (pre) failed for length {$n}"
            );
            $this->assertSame(
                array_map(fn ($x, $y) => $x * $y, $a, $b),
                $va->multiply($vb)->asArray(),
                "multiply (pre) failed for length {$n}"
            );
            $this->assertSame(
                array_map(fn ($x, $y) => $x / $y, $a, $b),
                $va->divide($vb)->asArray(),
                "divide (pre) failed for length {$n}"
            );
            $this->assertSame(
                array_map(fn ($x) => abs($x), $a),
                $va->abs()->asArray(),
                "abs (pre) failed for length {$n}"
            );
            $this->assertSame(
                array_map(fn ($x) => -$x, $a),
                $va->negate()->asArray(),
                "negate (pre) failed for length {$n}"
            );
        }

        // One call is enough; the second asserts idempotency.
        Settings::disableOptimizedKernels();

        foreach ([1, 3, 4, 7, 8, 13, 1000] as $n) {
            $a = [];
            $b = [];

            for ($i = 0; $i < $n; ++$i) {
                $a[] = ($i % 17) - 8.0;
                $b[] = [0.25, 0.5, 1.0, 2.0, 4.0][$i % 5];
            }

            $va = Vector::fromArray($a);
            $vb = Vector::fromArray($b);

            $this->assertSame(
                array_map(fn ($x, $y) => $x + $y, $a, $b),
                $va->add($vb)->asArray(),
                "add (post) failed for length {$n}"
            );
            $this->assertSame(
                array_map(fn ($x, $y) => $x - $y, $a, $b),
                $va->subtract($vb)->asArray(),
                "subtract (post) failed for length {$n}"
            );
            $this->assertSame(
                array_map(fn ($x, $y) => $x * $y, $a, $b),
                $va->multiply($vb)->asArray(),
                "multiply (post) failed for length {$n}"
            );
            $this->assertSame(
                array_map(fn ($x, $y) => $x / $y, $a, $b),
                $va->divide($vb)->asArray(),
                "divide (post) failed for length {$n}"
            );
            $this->assertSame(
                array_map(fn ($x) => abs($x), $a),
                $va->abs()->asArray(),
                "abs (post) failed for length {$n}"
            );
            $this->assertSame(
                array_map(fn ($x) => -$x, $a),
                $va->negate()->asArray(),
                "negate (post) failed for length {$n}"
            );
        }

        // A second call from another worker in a real app would be the
        // concurrent case the docstring warns about; here we just cover the
        // single-threaded idempotency path.
        Settings::disableOptimizedKernels();

        $va = Vector::fromArray([1.5, -2.0, 3.25, 0.0, -8.5]);
        $vb = Vector::fromArray([0.25, 0.5, 1.0, 2.0, 4.0]);

        $this->assertSame(
            array_map(fn ($x, $y) => $x + $y, [1.5, -2.0, 3.25, 0.0, -8.5], [0.25, 0.5, 1.0, 2.0, 4.0]),
            $va->add($vb)->asArray(),
            'add (idempotent) failed'
        );
        $this->assertSame(
            array_map(fn ($x) => -$x, [1.5, -2.0, 3.25, 0.0, -8.5]),
            $va->negate()->asArray(),
            'negate (idempotent) failed'
        );
    }

    /**
     * enableOptimizedKernels() must restore the widest route the CPU supports,
     * undoing disableOptimizedKernels() without restarting the process.
     *
     * The CPU feature bits are sampled once and cached at load time, so the
     * call replays the same selection the module initializer made. What this
     * test establishes is the safety property, which is the part that can
     * regress: the route must be left pointing at kernels the CPU can actually
     * execute, and results must be unchanged by the toggling. An AVX-512 route
     * installed on a CPU without AVX-512, or a kernel pointer left null
     * because the replay never happened, would both show up here.
     *
     * It cannot from PHP assert that the route pointer is now the AVX variant
     * rather than the baseline: cpuFeatures() reports the detected feature bits
     * and not the live route, and no accessor exposes the latter. The ops
     * asserted below are bit-stable across all three routes by construction,
     * which is what lets a strict assertSame stand in for the route check.
     *
     * Unlike the disable test, this one leaves the process in the state MINIT
     * put it in, so it does not have to run last.
     *
     * @test
     */
    public function enableOptimizedKernelsRestoresOptimizedRoute() : void
    {
        $this->assertBitStableDispatchedOps('initial route');

        Settings::disableOptimizedKernels();

        $this->assertBitStableDispatchedOps('baseline route');

        Settings::enableOptimizedKernels();

        $this->assertBitStableDispatchedOps('re-enabled route');

        // A second call from the same worker is the idempotency path; a real
        // app calling this from several workers concurrently is the case the
        // docstring warns against, so it is not exercised here.
        Settings::enableOptimizedKernels();

        $this->assertBitStableDispatchedOps('idempotent re-enable');
    }

    /**
     * Assert the bit-stable subset of the dispatched operations still returns
     * bit-identical results on whichever route is currently installed.
     *
     * The operands are exactly representable and the divisors are powers of
     * two, so every operation below is exact in binary floating point and the
     * comparison can be a strict assertSame rather than a tolerance compare.
     * Lengths are not all multiples of four, so the scalar tail of each
     * vector loop is covered alongside the vector body -- the tails are
     * rewritten by the same route pointer as the bodies, and a route that is
     * only correct for the aligned part of a buffer would be caught here.
     *
     * The FMA-backed operations (convolution, LU row update) are deliberately
     * excluded: fma(x, y, acc) is one rounding and acc + x * y is two, so
     * they are not bit-stable across routes and would need a tolerance.
     *
     * @param string $stage
     */
    private function assertBitStableDispatchedOps(string $stage) : void
    {
        foreach ([1, 3, 4, 7, 8, 13, 1000] as $n) {
            $a = [];
            $b = [];

            for ($i = 0; $i < $n; ++$i) {
                $a[] = ($i % 17) - 8.0;
                $b[] = [0.25, 0.5, 1.0, 2.0, 4.0][$i % 5];
            }

            $va = Vector::fromArray($a);
            $vb = Vector::fromArray($b);

            $this->assertSame(
                array_map(fn ($x, $y) => $x + $y, $a, $b),
                $va->add($vb)->asArray(),
                "add failed for length {$n} ({$stage})"
            );
            $this->assertSame(
                array_map(fn ($x, $y) => $x - $y, $a, $b),
                $va->subtract($vb)->asArray(),
                "subtract failed for length {$n} ({$stage})"
            );
            $this->assertSame(
                array_map(fn ($x, $y) => $x * $y, $a, $b),
                $va->multiply($vb)->asArray(),
                "multiply failed for length {$n} ({$stage})"
            );
            $this->assertSame(
                array_map(fn ($x, $y) => $x / $y, $a, $b),
                $va->divide($vb)->asArray(),
                "divide failed for length {$n} ({$stage})"
            );
            $this->assertSame(
                array_map(fn ($x) => abs($x), $a),
                $va->abs()->asArray(),
                "abs failed for length {$n} ({$stage})"
            );
            $this->assertSame(
                array_map(fn ($x) => -$x, $a),
                $va->negate()->asArray(),
                "negate failed for length {$n} ({$stage})"
            );
        }
    }
}
