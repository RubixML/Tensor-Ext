<?php

namespace Tensor\Benchmarks\Workloads;

use Generator;
use Tensor\Settings;
use Tensor\Tests\Benchmarks\Workloads;
use Tensor\Tests\Support\Loader;

/**
 * The baseline grid: seven chained workloads, two fixture sizes, one and four
 * OpenBLAS threads.
 *
 * These are the numbers Stages 3 and 4 are measured against. Chains, not single
 * operations: a single operation is dominated by the conversion at each end of
 * it, so it understates what moving the storage into C is worth.
 *
 * Sizes were chosen from tools/workload-probe.php rather than guessed. The
 * 5000-row fixture is deliberately absent — its Gram matrix is 25 million PHP
 * floats and its distance matrix peaks at 1.1 GB and 3.6 s per run, which is
 * neither measurable at five iterations nor safe on a 5 GB box.
 *
 * Threads are set inside setUp through Settings, so OPENBLAS_NUM_THREADS must
 * NOT be exported when running these — the environment variable would shadow
 * the runtime call and every row would report the same number.
 *
 * @Groups({"Workloads"})
 * @BeforeMethods({"setUp"})
 * @ParamProviders({"provideCases"})
 */
class WorkloadBench
{
    protected Workloads $workloads;

    /**
     * @param array{fixture: string, threads: int} $params
     */
    public function setUp(array $params) : void
    {
        Settings::setNumThreads($params['threads']);

        $this->workloads = new Workloads(Loader::load($params['fixture'])['samples']);
    }

    /**
     * @Subject
     * @Iterations(5)
     * @Revs(3)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function standardise() : void
    {
        $this->workloads->standardise();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @Revs(3)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function covariance() : void
    {
        $this->workloads->covariance();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @Revs(3)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function gram() : void
    {
        $this->workloads->gram();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @Revs(3)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function leastSquaresGradient() : void
    {
        $this->workloads->leastSquaresGradient();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @Revs(3)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function sigmoid() : void
    {
        $this->workloads->sigmoid();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @Revs(1)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function distanceMatrix() : void
    {
        $this->workloads->distanceMatrix();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @Revs(3)
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function pca() : void
    {
        $this->workloads->pca();
    }

    /**
     * @return Generator<string, array{fixture: string, threads: int}>
     */
    public function provideCases() : Generator
    {
        foreach (['blob-1000x8', 'hyperplane-2000x12'] as $fixture) {
            foreach ([1, 4] as $threads) {
                yield "{$fixture}/t{$threads}" => ['fixture' => $fixture, 'threads' => $threads];
            }
        }
    }
}
