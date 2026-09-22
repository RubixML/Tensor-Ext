<?php

namespace Tensor\Benchmarks\Workloads;

use Tensor\BufferVector;
use Tensor\Vector;

/**
 * Tensor\Vector against the BufferVector prototype.
 *
 * In Stage 2 these measured a PHP array against a contiguous C buffer, and the
 * subjects were named accordingly. Stage 3 moved Vector onto a buffer too, so
 * the comparison is now Vector against the hand-written prototype it was
 * modelled on: the question is no longer "how much is the representation
 * worth" but "did the migrated class actually get there".
 *
 * The subjects were renamed with that change, so a --ref against the
 * stage2_prototype tag will not line them up. The Stage 2 figures are recorded
 * in docs/plans/baseline.md instead.
 *
 * The two boundary conversions are still measured separately, because they are
 * what caps the win on a single operation and what makes it scale with chain
 * length.
 *
 * @Groups({"Workloads", "Buffer"})
 * @BeforeMethods({"setUp"})
 */
class BufferVectorBench
{
    /**
     * A million elements: large enough that per-element cost dominates.
     */
    protected const N = 1000000;

    /**
     * Ten revolutions per iteration, so both paths are measured warm.
     *
     * This matters more than it looks. A single cold call to dot() at this size
     * takes 0.83 ms while a warm one takes 0.37 ms, because two million doubles
     * are 16 MB and this box sustains 71 GB/s from cache against 23 GB/s from
     * DRAM. Measuring one path warm and the other cold would say more about the
     * cache than about the representation, so both get the same treatment.
     */

    /**
     * @var list<float>
     */
    protected array $rawA = [];

    /**
     * @var list<float>
     */
    protected array $rawB = [];

    protected Vector $a;

    protected Vector $b;

    protected BufferVector $bufferA;

    protected BufferVector $bufferB;

    public function setUp() : void
    {
        // Deterministic, so the two representations see the same bits and
        // consecutive runs are comparable.
        mt_srand(20260921);

        $a = [];
        $b = [];

        for ($i = 0; $i < self::N; ++$i) {
            $a[] = mt_rand(-1000000, 1000000) / 1000.0;
            $b[] = mt_rand(-1000000, 1000000) / 1000.0;
        }

        $this->rawA = $a;
        $this->rawB = $b;

        $this->a = Vector::quick($a);
        $this->b = Vector::quick($b);

        $this->bufferA = BufferVector::build($a);
        $this->bufferB = BufferVector::build($b);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @Revs(10)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function vectorAdd() : void
    {
        $this->a->add($this->b);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @Revs(10)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function prototypeAdd() : void
    {
        $this->bufferA->add($this->bufferB);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @Revs(10)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function vectorMultiply() : void
    {
        $this->a->multiply($this->b);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @Revs(10)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function prototypeMultiply() : void
    {
        $this->bufferA->multiply($this->bufferB);
    }

    /**
     * The array path hand-rolls an accumulator; the buffer path reaches
     * cblas_ddot. This is the largest single ratio in the set.
     *
     * @Subject
     * @Iterations(5)
     * @Revs(10)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function vectorDot() : void
    {
        $this->a->dot($this->b);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @Revs(10)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function prototypeDot() : void
    {
        $this->bufferA->dot($this->bufferB);
    }

    /**
     * Two operations rather than one: the buffer path pays its conversion once
     * at each end regardless of how long the chain is, so this is where the
     * representation starts to pay for itself.
     *
     * @Subject
     * @Iterations(5)
     * @Revs(10)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function vectorChain() : void
    {
        $this->a->add($this->b)->multiply($this->a);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @Revs(10)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function prototypeChain() : void
    {
        $this->bufferA->add($this->bufferB)->multiply($this->bufferA);
    }

    /**
     * The boundary going in.
     *
     * @Subject
     * @Iterations(5)
     * @Revs(10)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function prototypeBuild() : void
    {
        BufferVector::build($this->rawA);
    }

    /**
     * The boundary coming out. Deliberately measured: a migration that called
     * this once per operation would be slower than the arrays it replaced.
     *
     * @Subject
     * @Iterations(5)
     * @Revs(10)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function prototypeAsArray() : void
    {
        $this->bufferA->asArray();
    }
}
