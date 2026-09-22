<?php

namespace Tensor\Tests;

use Tensor\Exceptions\InvalidArgumentException;
use Tensor\Settings;
use PHPUnit\Framework\TestCase;

/**
 * Pins the shipped OpenBLAS threading default.
 *
 * The default is one thread, set in config.json's module initializer, and it is
 * there on measured grounds rather than caution. Threading a single process is
 * a large win — 3.34x on matmul(512) on four cores — but this library runs under
 * php-fpm, where several worker processes are doing the same thing at once.
 * OpenBLAS here is a pthread build whose per-process pool busy-waits after each
 * parallel region, so N workers times T threads oversubscribes the machine and
 * the busy-wait turns idle time into contention.
 *
 * Measured with tools/concurrency-probe.php after the buffer migration, on four
 * cores, matmul(512):
 *
 *     workers  threads   mean ms/op   total ops/s
 *           1        1       16.915          59.1
 *           1        4        5.065         197.4
 *           4        1       17.680         226.3
 *           4        4       34.346         116.5
 *
 * Four workers at four threads is 1.94x slower per operation and delivers half
 * the throughput. The migration narrowed the gap — it was 2.4x before, because
 * the parallel fraction is now higher — but did not close it, and did not come
 * close to reversing it.
 *
 * So the default stays at one, and Settings::setNumThreads() remains the opt-in
 * for workloads that own the machine. There is no timing assertion here: timings
 * do not belong in a unit suite. What is asserted is that the default has not
 * been changed without someone revisiting the measurement above.
 *
 * @coversNothing
 */
class ThreadingTest extends TestCase
{
    protected function tearDown() : void
    {
        // Other tests, and the parity snapshot, assume one thread.
        Settings::setNumThreads(1);
    }

    /**
     * @test
     */
    public function theShippedDefaultIsOneThread() : void
    {
        $this->assertSame(1, Settings::numThreads());
    }

    /**
     * @test
     */
    public function threadCountIsSettableAtRuntime() : void
    {
        Settings::setNumThreads(4);

        $this->assertSame(4, Settings::numThreads());

        Settings::setNumThreads(1);

        $this->assertSame(1, Settings::numThreads());
    }

    /**
     * @test
     */
    public function aNonPositiveThreadCountIsRejected() : void
    {
        $this->expectException(InvalidArgumentException::class);

        Settings::setNumThreads(0);
    }
}
