<?php

namespace Tensor\Tests;

use Tensor\Matrix;
use Tensor\Vector;
use PHPUnit\Framework\TestCase;

/**
 * Output buffers at or above ZEND_MM_CHUNK_SIZE (2 MB) recycle a single cached
 * block per request, see the oversized buffer cache in ext/include/buffer.c.
 * The block is handed out again once the previous holder is destroyed, which
 * means a bug here would show up as two live tensors sharing one block.
 *
 * Aliasing is therefore the risk these tests exist to rule out: every case
 * keeps results alive across further allocations and then re-reads them, so a
 * shared block would surface as silently corrupted numbers.
 *
 * @covers \Tensor\Buffer
 * @covers \Tensor\TensorBuffer
 */
class BufferReuseTest extends TestCase
{
    /**
     * The maximum error tolerated due to varying numerical precision.
     *
     * @var float
     */
    protected const MAX_DELTA = 1e-9;

    /**
     * Element counts whose buffers clear the 2 MB threshold, so the cache is
     * actually in play, and one that stays under it, so the untouched path is
     * covered too.
     *
     * @var int
     */
    protected const OVER_THRESHOLD = 400_000;

    /**
     * @var int
     */
    protected const UNDER_THRESHOLD = 100_000;

    /**
     * Element counts used by the cache liveness tests. Both are deliberately
     * odd so that neither can tie with a round-numbered block cached by an
     * unrelated test, because an equal sized incumbent is kept rather than
     * replaced.
     *
     * @var int
     */
    protected const CACHE_SMALL = 262_145;

    /**
     * @var int
     */
    protected const CACHE_LARGE = 8_388_609;

    /**
     * The core aliasing case. Each result is retained while the next one is
     * built, so the cache is refilled and drained underneath live tensors. If
     * two of them ever shared a block, the later write would corrupt the
     * earlier result and the fingerprints would diverge.
     *
     * @test
     */
    public function retainedResultsNeverShareABlock() : void
    {
        $n = self::OVER_THRESHOLD;
        $a = $this->ramp($n, 1.0);
        $b = $this->ramp($n, 2.0);

        $expected = $this->fingerprint($a->add($b));

        /* Built with the previous one still held, so nothing is ever free at
         * the moment the next allocation happens. */
        $held = [];

        for ($i = 0; $i < 8; ++$i) {
            $held[] = $a->add($b);
        }

        $this->assertCount(8, $held);

        foreach ($held as $i => $result) {
            $this->assertEqualsWithDelta(
                $expected,
                $this->fingerprint($result),
                self::MAX_DELTA,
                "Retained result {$i} changed while later results were built, so two tensors shared a block."
            );
        }

        // And they must still be independent objects.
        $this->assertNotSame($held[0], $held[1]);
    }

    /**
     * Results freed between operations are the case the cache is built for.
     * Correctness must not depend on the previous result having been freed in
     * any particular pattern.
     *
     * @test
     */
    public function repeatedSameSizedOperationsStayCorrect() : void
    {
        $n = self::OVER_THRESHOLD;
        $a = $this->ramp($n, 3.0);
        $b = $this->ramp($n, 4.0);

        $expected = $this->fingerprint($a->add($b));

        for ($i = 0; $i < 12; ++$i) {
            $result = $a->add($b);

            $this->assertEqualsWithDelta(
                $expected,
                $this->fingerprint($result),
                self::MAX_DELTA,
                "Iteration {$i} produced a different result."
            );

            unset($result);
        }
    }

    /**
     * Interleaving sizes must not let a cached block satisfy a request it is
     * too small for. The cache only reuses a block at least as large as the
     * request, so a small block must never be handed to a larger request.
     *
     * @test
     */
    public function mixedSizesDoNotCollide() : void
    {
        $sizes = [300_000, 400_000, 500_000, 400_000, 300_000, 700_000, 300_000];

        foreach ($sizes as $index => $n) {
            $a = $this->ramp($n, 1.0 + $index);
            $b = $this->ramp($n, 2.0 + $index);
            $expected = $this->fingerprint($a->add($b));

            $result = $a->add($b);

            $this->assertEqualsWithDelta(
                $expected,
                $this->fingerprint($result),
                self::MAX_DELTA,
                "Size {$n} at position {$index} produced a different result."
            );

            unset($result);
        }
    }

    /**
     * Shrinking then growing: a cached small block must not be reused for a
     * larger request, which would write past the end of the allocation.
     *
     * @test
     */
    public function growingBeyondACachedBlockIsSafe() : void
    {
        $sizes = [400_000, 900_000, 400_000, 900_000, 1_500_000, 400_000];

        foreach ($sizes as $index => $n) {
            $a = $this->ramp($n, 1.0 + $index);
            $b = $this->ramp($n, 2.0 + $index);
            $expected = $this->fingerprint($a->add($b));

            $result = $a->add($b);

            $this->assertEqualsWithDelta(
                $expected,
                $this->fingerprint($result),
                self::MAX_DELTA,
                "Growing to {$n} at position {$index} produced a different result."
            );

            unset($result);
        }
    }

    /**
     * The under-threshold path must be untouched, including the part where a
     * large cached block is resident and a small allocation happens alongside
     * it.
     *
     * @test
     */
    public function underThresholdOperationsAreUnaffected() : void
    {
        $small = $this->ramp(self::UNDER_THRESHOLD, 5.0);
        $large = $this->ramp(self::OVER_THRESHOLD, 6.0);

        $expectedSmall = $this->fingerprint($small->add($small));
        $expectedLarge = $this->fingerprint($large->add($large));

        for ($i = 0; $i < 6; ++$i) {
            $a = $small->add($small);
            $b = $large->add($large);

            $this->assertEqualsWithDelta($expectedSmall, $this->fingerprint($a), self::MAX_DELTA, "Small op {$i} is wrong.");
            $this->assertEqualsWithDelta($expectedLarge, $this->fingerprint($b), self::MAX_DELTA, "Large op {$i} is wrong.");

            unset($a, $b);
        }
    }

    /**
     * Every operation sharing the allocation path has to survive recycling,
     * not just addition.
     *
     * @test
     */
    public function otherOperationsSurviveRecycling() : void
    {
        $n = self::OVER_THRESHOLD;
        $a = $this->ramp($n, 7.0);
        $b = $this->ramp($n, 8.0);

        $cases = [
            'subtract' => [$a->subtract($b), fn (Vector $x, Vector $y) => $x->subtract($y)],
            'multiply' => [$a->multiply($b), fn (Vector $x, Vector $y) => $x->multiply($y)],
            'abs' => [$a->abs(), fn (Vector $x) => $x->abs()],
            'negate' => [$a->negate(), fn (Vector $x) => $x->negate()],
        ];

        foreach ($cases as $name => [$first, $rebuild]) {
            $expected = $this->fingerprint($first);

            unset($first);

            for ($i = 0; $i < 4; ++$i) {
                $result = $rebuild($a, $b);

                $this->assertEqualsWithDelta(
                    $expected,
                    $this->fingerprint($result),
                    self::MAX_DELTA,
                    "{$name} iteration {$i} produced a different result."
                );

                unset($result);
            }
        }
    }

    /**
     * Matrix output buffers take a different create path and must be recycled
     * just as safely.
     *
     * @test
     */
    public function matrixTransposeSurvivesRecycling() : void
    {
        $side = 700;
        $data = [];

        for ($i = 0; $i < $side; ++$i) {
            $row = [];

            for ($j = 0; $j < $side; ++$j) {
                $row[] = 1.0 + $i * $side + $j;
            }

            $data[] = $row;
        }

        $a = Matrix::fromArray($data);

        $expected = $this->fingerprint(Vector::fromArray(array_merge(...$a->transpose()->asArray())));

        for ($i = 0; $i < 5; ++$i) {
            $result = $a->transpose();
            $actual = $this->fingerprint(Vector::fromArray(array_merge(...$result->asArray())));

            $this->assertEqualsWithDelta($expected, $actual, self::MAX_DELTA, "Transpose iteration {$i} is wrong.");

            unset($result);
        }
    }

    /**
     * A block sitting in the cache is still emalloc()ed, so it keeps being
     * accounted for by memory_get_usage(). Retention is therefore a direct
     * read on whether the cache is live, with no timing involved and no
     * dependence on how fast the machine happens to be.
     *
     * Only a block strictly larger than the cached incumbent can be observed
     * this way, since a smaller one is released to keep the more useful
     * incumbent. CACHE_LARGE is odd so that it can never tie with a
     * round-numbered block cached by an unrelated test.
     *
     * The cache must additionally not be able to wedge itself shut.
     *
     * A block cached for a small allocation used to be left in place when a
     * larger allocation arrived. The larger one was emalloc()ed, and its
     * release then found the slot occupied and efree()d itself instead of
     * caching, so the undersized block stayed forever. Every subsequent
     * allocation in the request missed as well, leaving recycling silently
     * disabled for the rest of the request while all the correctness tests
     * still passed.
     *
     * This fails both if the cache never engages and if it wedges.
     *
     * @test
     */
    public function cacheStaysLiveWhenLargerAllocationsFollow() : void
    {
        $small = Vector::fill(1.0, self::CACHE_SMALL);

        $scratch = $small->add($small);
        unset($scratch, $small);

        $baseline = memory_get_usage();

        $large = Vector::fill(2.0, self::CACHE_LARGE);

        $scratch = $large->add($large);
        unset($scratch, $large);

        $this->assertGreaterThan(
            self::CACHE_LARGE * 8 / 2,
            memory_get_usage() - $baseline,
            'A large allocation was not cached, so the cache is not engaging or has wedged on the smaller block.'
        );
    }

    /**
     * Build a vector of `len` distinct, reproducible values.
     *
     * @param int $len
     * @param float $offset
     * @return Vector
     */
    protected function ramp(int $len, float $offset) : Vector
    {
        $values = [];

        for ($i = 0; $i < $len; ++$i) {
            $values[] = $offset + $i * 0.5;
        }

        return Vector::fromArray($values);
    }

    /**
     * Reduce a vector to a cheap but collision resistant fingerprint, so a
     * whole 400k element buffer can be compared without building a giant
     * expected array for every case.
     *
     * @param Vector $vector
     * @return float
     */
    protected function fingerprint(Vector $vector) : float
    {
        $sum = 0.0;
        $index = 0;

        foreach ($vector->asArray() as $value) {
            // Weight by position so a swap of two elements cannot cancel out.
            $sum += $value * (1.0 + ($index % 7));
            ++$index;
        }

        return $sum;
    }
}
