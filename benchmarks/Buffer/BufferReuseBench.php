<?php

namespace Tensor\Benchmarks\Buffer;

use Tensor\Vector;

/**
 * Output buffers at or above ZEND_MM_CHUNK_SIZE (2 MB) recycle one cached block
 * per request, see the oversized buffer cache in ext/include/buffer.c. Whether
 * that helps is entirely a question of what the caller does with the result, so
 * the subjects here cover both shapes rather than just the favourable one.
 *
 * @Groups({"Buffer"})
 * @BeforeMethods({"setUp"})
 */
class BufferReuseBench
{
    /**
     * Element count whose output buffer is comfortably over the 2 MB threshold.
     *
     * @var int
     */
    public const OVER = 4_000_000;

    /**
     * Element count whose output buffer stays under the 2 MB threshold, so the
     * cache must not engage and this acts as the control.
     *
     * @var int
     */
    public const UNDER = 200_000;

    /**
     * Operations per subject. The pool only reaches a steady state after the
     * first result has been freed and handed back, and the vectors built in
     * setUp() already drain the slot, so a single operation per subject would
     * only ever measure the cold path.
     *
     * @var int
     */
    public const OPS = 8;

    /**
     * @var Vector
     */
    protected $a;

    /**
     * @var Vector
     */
    protected $b;

    /**
     * @var Vector
     */
    protected $smallA;

    /**
     * @var Vector
     */
    protected $smallB;

    /**
     * @var Vector[]
     */
    protected $retained = [];

    public function setUp() : void
    {
        $this->a = Vector::linspace(1.0, 2.0, self::OVER);
        $this->b = Vector::linspace(2.0, 3.0, self::OVER);
        $this->smallA = Vector::linspace(1.0, 2.0, self::UNDER);
        $this->smallB = Vector::linspace(2.0, 3.0, self::UNDER);
    }

    /**
     * The case the cache exists for: each result is dropped before the next
     * operation, so its block returns to the slot and the next allocation gets
     * it back still mapped and faulted in. Only the first operation is cold.
     *
     * The unset() is load bearing and not just tidiness. Reassigning
     * `$result = ...` instead would evaluate the new operation while the
     * previous result is still alive, because PHP assigns only once the right
     * hand side is done, and the slot would then be empty for every operation.
     *
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function addOversizedRecycled() : void
    {
        for ($i = 0; $i < self::OPS; ++$i) {
            $result = $this->a->add($this->b);
            unset($result);
        }
    }

    /**
     * The case the cache cannot help: every result is kept, so no block is
     * ever freed and every operation must mmap and fault a fresh one. This is
     * the honest baseline for the subject above, and the per-operation cost
     * should match the subject's cold first operation.
     *
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function addOversizedRetained() : void
    {
        for ($i = 0; $i < self::OPS; ++$i) {
            $this->retained[] = $this->a->add($this->b);
        }
    }

    /**
     * Control for the under-threshold path, which the cache must leave
     * completely alone.
     *
     * @Subject
     * @Iterations(5)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function addUnderThresholdRecycled() : void
    {
        for ($i = 0; $i < self::OPS; ++$i) {
            $result = $this->smallA->add($this->smallB);
            unset($result);
        }
    }
}
