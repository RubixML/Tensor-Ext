<?php

declare(strict_types=1);

/**
 * Runs each workload once per candidate fixture and reports peak memory.
 *
 * The benchmark suite measures chains whose results are m x m: a Gram matrix
 * over a 5000-row fixture is 25 million PHP floats, several hundred megabytes
 * per intermediate, and this box has 5 GB. Measuring that before starting a
 * twenty-minute baseline run is cheaper than discovering it half way through.
 *
 * Exits non-zero if any pair exceeds the budget, naming the offender.
 *
 * Usage: php -n -d memory_limit=3G -d extension=<...>.so tools/workload-probe.php [budgetMB]
 */

use Tensor\Tests\Benchmarks\Workloads;
use Tensor\Tests\Support\Loader;

require dirname(__DIR__) . '/vendor/autoload.php';

if (!extension_loaded('tensor_ext')) {
    fwrite(STDERR, "Load the extension: -d extension=ext/modules/tensor_ext.so\n");

    exit(1);
}

$budget = (int) ($argv[1] ?? 2048);

$fixtures = Loader::names();
$failures = 0;

printf("%-22s %-24s %10s %10s\n", 'fixture', 'workload', 'peak MB', 'ms');

foreach ($fixtures as $fixture) {
    $samples = Loader::load($fixture)['samples'];

    foreach (Workloads::NAMES as $workload) {
        gc_collect_cycles();

        $before = memory_get_peak_usage(true);
        $start = hrtime(true);

        $workloads = new Workloads($samples);
        $workloads->run($workload);

        $elapsed = (hrtime(true) - $start) / 1e6;
        $peak = (memory_get_peak_usage(true) - $before) / 1048576;

        unset($workloads);

        $flag = $peak > $budget ? '  <-- OVER BUDGET' : '';

        if ($peak > $budget) {
            ++$failures;
        }

        printf("%-22s %-24s %10.1f %10.1f%s\n", $fixture, $workload, $peak, $elapsed, $flag);
    }
}

if ($failures > 0) {
    fwrite(STDERR, "\n{$failures} workload/fixture pairs exceed {$budget} MB. Shrink them before baselining.\n");

    exit(1);
}

echo "\nAll pairs fit within {$budget} MB.\n";
