<?php

use Tensor\Matrix;
use Tensor\Settings;
use Tensor\Vector;

/**
 * Forward pass through a 6-layer dense feed-forward network.
 *
 * Each layer is a 512-wide dense matmul, a learned bias (broadcast Vector
 *  add), and a SiLU activation composed from the primitives the extension
 * ships:  silu(x) = x / (1 + exp(-x)).
 *
 * Run:
 *     php examples/DenseNetworkExample.php
 *     php examples/DenseNetworkExample.php --batch=256 --warmups=5 --epochs=1
 *     php examples/DenseNetworkExample.php --sweep
 *
 * With --sweep the example re-runs the forward at thread counts 1, 2, 4, 8,
 * 16, 32 so the expected split is visible: matmul (the only threaded call in
 * this extension, via OpenBLAS) should shrink as threads rise; bias-add and
 * the SiLU activation (hand-written AVX loops, never threaded by design)
 * should stay flat.
 * @param Matrix $x
 */
function silu(Matrix $x) : Matrix
{
    $den = $x->negate()->exp()->add(1.0);

    return $x->divide($den);
}

/**
 * Forward through $layers 512-wide dense layers, accumulating per-operation
 * wall-clock time and call counts in $stats.
 *
 * @param int $layers number of dense layers
 * @param int $width neurons per layer (fixed by the example)
 * @param Matrix $input batch x width input
 * @param array[] $weights per-layer weights (width x width)
 * @param Vector[] $biases per-layer biases (width)
 * @param float[] $stats accumulator [ [t,c], [t,c], [t,c] ] for
 *                       matmul / bias-add / activation, resp.
 * @param Matrix|int|null $threads thread count to force before measuring, or null
 *
 * @return Matrix
 */
function forward(int $layers, int $width, Matrix $input, array $weights, array $biases, array &$stats, $threads = null) : Matrix
{
    if ($threads !== null) {
        Settings::setNumThreads((int) $threads);
    }

    $h = $input;
    $l = 0;

    while ($l < $layers) {
        $t0 = hrtime(true);
        $h = $h->matmul($weights[$l]);
        $t1 = hrtime(true);
        $h = $h->add($biases[$l]);
        $t2 = hrtime(true);
        $h = silu($h);
        $t3 = hrtime(true);

        $stats[0][0] += ($t1 - $t0) / 1000000.0;
        $stats[0][1] += 1;
        $stats[1][0] += ($t2 - $t1) / 1000000.0;
        $stats[1][1] += 1;
        $stats[2][0] += ($t3 - $t2) / 1000000.0;
        $stats[2][1] += 1;

        ++$l;
    }

    return $h;
}

/**
 * Print a per-operation table with ms/call and the elementwise share.
 * @param array $stats
 * @param string $label
 */
function printStats(array $stats, string $label) : void
{
    $total = 0.0;

    foreach ($stats as $k) {
        $total += $k[0];
    }

    $elem = $stats[1][0] + $stats[2][0];
    $share = $total > 0.0 ? 100.0 * $elem / $total : 0.0;

    printf(
        "%-10s | %12s %8s | %12s %8s | %12s %8s | %10s\n",
        $label,
        number_format($stats[0][0] / max(1, (int) $stats[0][1]), 3) . 'ms',
        $stats[0][1] . 'x',
        number_format($stats[1][0] / max(1, (int) $stats[1][1]), 3) . 'ms',
        $stats[1][1] . 'x',
        number_format($stats[2][0] / max(1, (int) $stats[2][1]), 3) . 'ms',
        $stats[2][1] . 'x',
        number_format($share, 2) . '% elem'
    );
}

/**
 * Run a warmup + measurement pass; returns the accumulated stats.
 * @param int $layers
 * @param int $width
 * @param Matrix $input
 * @param array $weights
 * @param array $biases
 * @param int $warmups
 * @param int $epochs
 * @param null|mixed $threads
 */
function runPass(int $layers, int $width, Matrix $input, array $weights, array $biases, int $warmups, int $epochs, $threads = null) : array
{
    $stats = [
        [0.0, 0],
        [0.0, 0],
        [0.0, 0],
    ];

    $i = 0;

    while ($i < $warmups) {
        forward($layers, $width, $input, $weights, $biases, $stats, $threads);
        ++$i;
    }

    $j = 0;

    while ($j < $epochs) {
        forward($layers, $width, $input, $weights, $biases, $stats, $threads);
        ++$j;
    }

    return $stats;
}

/**
 * Check every element of a Matrix is finite. Returns a bool.
 *
 * @param Matrix $m
 *
 * @return bool
 */
function allFinite(Matrix $m) : bool
{
    $a = $m->asArray();
    $r = 0;

    while ($r < count($a)) {
        $row = $a[$r];
        $c = 0;

        while ($c < count($row)) {
            if (!is_finite((float) $row[$c])) {
                return false;
            }
            ++$c;
        }
        ++$r;
    }

    return true;
}

/**
 * Numeric spot-check of the composed SiLU against the hand-evaluated
 * formula  x / (1 + exp(-x)).
 */
function checkSilu() : void
{
    // 4 hand-crafted values, one row of the input.
    $xs = [-123.456, -1e-6, 0.0, 3.14159];

    $m = Matrix::fromArray([$xs]);

    $y = silu($m)->asArray()[0];

    $max = 0.0;
    $i = 0;
    $values = [];

    while ($i < count($xs)) {
        $x = (float) $xs[$i];
        $expect = $x / (1.0 + exp(-$x));
        $got = (float) $y[$i];
        $diff = abs($got - $expect);

        if ($diff > $max) {
            $max = $diff;
        }
        $values[] = number_format($x, 6) . ' -> ' . number_format($got, 6);
        ++$i;
    }

    printf("silu spot-check: |max - expect| = %.3e  (%s)\n", $max, implode(', ', $values));

    if ($max > 1e-9) {
        exit(1);
    }
    printf("silu ok\n");
}

function main(array $argv) : void
{
    $batch = 256;
    $layers = 6;
    $width = 512;
    $warmups = 5;
    $epochs = 1;
    $sweep = false;

    foreach ($argv as $arg) {
        if (str_starts_with($arg, '--batch=')) {
            $batch = (int) substr($arg, 8);
        }

        if (str_starts_with($arg, '--layers=')) {
            $layers = (int) substr($arg, 9);
        }

        if (str_starts_with($arg, '--width=')) {
            $width = (int) substr($arg, 8);
        }

        if (str_starts_with($arg, '--warmups=')) {
            $warmups = (int) substr($arg, 10);
        }

        if (str_starts_with($arg, '--epochs=')) {
            $epochs = (int) substr($arg, 9);
        }

        if ($arg === '--sweep') {
            $sweep = true;
        }
    }

    printf(
        "Dense MLP forward pass: %d layers x %d wide (batch=%d)\n",
        $layers,
        $width,
        $batch
    );

    // SiLU correctness spot-check, independent of the main workload.
    checkSilu();

    // Build the network once.
    $weights = [];
    $biases = [];
    $i = 0;

    while ($i < $layers) {
        $weights[] = Matrix::gaussian($width, $width);
        $biases[] = Vector::gaussian($width);
        ++$i;
    }

    $input = Matrix::gaussian($batch, $width);

    /*
     * Reference pass: run once at the current default thread count to
     * populate $stats, then report.
     */
    $threads0 = Settings::numThreads();
    printf(
        "\nthreads=%d, batch=%d, %d warmups + %d epoch:\n",
        $threads0,
        $batch,
        $warmups,
        $epochs
    );

    $stats = runPass($layers, $width, $input, $weights, $biases, $warmups, $epochs, $threads0);
    printStats($stats, 'per-op');

    // Output sanity: shape and finiteness.
    $outStats = [[0.0, 0], [0.0, 0], [0.0, 0]];
    $out = forward($layers, $width, $input, $weights, $biases, $outStats);
    printf(
        "\noutput shape = [%d, %d]  expected [%d, %d]\n",
        $out->shape()[0],
        $out->shape()[1],
        $batch,
        $width
    );
    printf("output finite = %s\n", allFinite($out) ? 'yes' : 'NO');

    if (!allFinite($out) || $out->shape()[1] !== $width) {
        exit(1);
    }

    if ($sweep) {
        printf("\nsweep over OpenBLAS thread counts (per-op ms/call):\n");
        printf(
            "%-9s | %12s %8s | %12s %8s | %12s %8s | %10s\n",
            'threads',
            'matmul',
            'calls',
            'bias',
            'calls',
            'act',
            'calls',
            'elem%'
        );

        foreach ([1, 2, 4, 8, 16, 32] as $t) {
            $wstats = [[0.0, 0], [0.0, 0], [0.0, 0]];
            $warm = 0;

            while ($warm < $warmups) {
                forward($layers, $width, $input, $weights, $biases, $wstats, $t);
                ++$warm;
            }

            $stats = [[0.0, 0], [0.0, 0], [0.0, 0]];
            $ep = 0;

            while ($ep < $epochs) {
                forward($layers, $width, $input, $weights, $biases, $stats, $t);
                ++$ep;
            }

            $total = $stats[0][0] + $stats[1][0] + $stats[2][0];
            $share = $total > 0.0 ? 100.0 * ($stats[1][0] + $stats[2][0]) / $total : 0.0;

            printf(
                "%-9d | %12s %8d | %12s %8d | %12s %8d | %10s\n",
                $t,
                number_format($stats[0][0] / max(1, (int) $stats[0][1]), 3) . 'ms',
                (int) $stats[0][1],
                number_format($stats[1][0] / max(1, (int) $stats[1][1]), 3) . 'ms',
                (int) $stats[1][1],
                number_format($stats[2][0] / max(1, (int) $stats[2][1]), 3) . 'ms',
                (int) $stats[2][1],
                number_format($share, 2) . '%'
            );
        }
    }
}

main(array_slice($argv, 1));
