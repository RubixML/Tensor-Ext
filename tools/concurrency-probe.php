<?php

declare(strict_types=1);

/**
 * Measures what OpenBLAS threading is worth when several PHP processes are
 * running at once, which is how this library is actually deployed.
 *
 * A single-process benchmark says threading is a large win. It is not the
 * question. Under php-fpm there are N workers, and OpenBLAS here is a pthread
 * build whose per-process pool busy-waits after each parallel region: N workers
 * times T threads oversubscribes the machine, and the busy-wait turns idle time
 * into contention. The plan measured 4 workers at 4 threads running 2.4x
 * *slower* per operation than 4 workers at 1 thread, which is why
 * config.json pins openblas_set_num_threads(1) at MINIT.
 *
 * That measurement was taken before the migration. The marshalling it was
 * competing with is now gone, so it has to be retaken before the default can be
 * defended or changed.
 *
 * Usage:
 *   php tools/concurrency-probe.php --so=<path> [--n=512] [--reps=5] [--workers=1,2,4] [--threads=1,4]
 *   php tools/concurrency-probe.php --worker --so=<path> --n=512 --reps=5 --threads=4
 */
$options = getopt('', ['so:', 'n::', 'reps::', 'workers::', 'threads::', 'worker']);

$so = $options['so'] ?? null;

if (!is_string($so)) {
    fwrite(STDERR, "Usage: concurrency-probe.php --so=<path to tensor_ext.so>\n");

    exit(1);
}

$n = (int) ($options['n'] ?? 512);
$reps = (int) ($options['reps'] ?? 5);

/**
 * One worker: build the operands, then time `reps` matmuls and report the mean
 * milliseconds per operation on stdout.
 */
if (array_key_exists('worker', $options)) {
    $threads = (int) ($options['threads'] ?? 1);

    Tensor\Settings::setNumThreads($threads);

    mt_srand(3);

    $samples = [];

    for ($i = 0; $i < $n; ++$i) {
        $row = [];

        for ($j = 0; $j < $n; ++$j) {
            $row[] = mt_rand(-1000, 1000) / 1000.0;
        }

        $samples[] = $row;
    }

    $a = Tensor\Matrix::quick($samples);
    $b = Tensor\Matrix::quick($samples);

    $a->matmul($b);

    $start = hrtime(true);

    for ($r = 0; $r < $reps; ++$r) {
        $a->matmul($b);
    }

    printf("%.4f\n", (hrtime(true) - $start) / 1e6 / $reps);

    exit(0);
}

/**
 * @param list<int> $list
 * @param string $value
 */
function parseList(string $value) : array
{
    return array_map('intval', explode(',', $value));
}

$workerCounts = parseList(is_string($options['workers'] ?? null) ? $options['workers'] : '1,2,4');
$threadCounts = parseList(is_string($options['threads'] ?? null) ? $options['threads'] : '1,4');

printf("matmul(%d), %d reps per worker, on %s\n\n", $n, $reps, trim((string) shell_exec('nproc')) . ' cores');
printf("%-9s %-9s %12s %12s %12s\n", 'workers', 'threads', 'mean ms/op', 'worst', 'total ops/s');

$results = [];

foreach ($workerCounts as $workers) {
    foreach ($threadCounts as $threads) {
        $processes = [];
        $pipes = [];

        $command = sprintf(
            '%s -n -d memory_limit=4G -d extension=%s %s --worker --so=%s --n=%d --reps=%d --threads=%d',
            escapeshellarg(PHP_BINARY),
            escapeshellarg($so),
            escapeshellarg(__FILE__),
            escapeshellarg($so),
            $n,
            $reps,
            $threads
        );

        for ($w = 0; $w < $workers; ++$w) {
            $descriptors = [1 => ['pipe', 'w'], 2 => ['pipe', 'w']];
            $handle = proc_open($command, $descriptors, $pipe);

            if (!is_resource($handle)) {
                fwrite(STDERR, "Cannot start worker {$w}.\n");

                exit(1);
            }

            $processes[$w] = $handle;
            $pipes[$w] = $pipe;
        }

        $times = [];

        foreach ($processes as $w => $handle) {
            $out = stream_get_contents($pipes[$w][1]);
            $err = stream_get_contents($pipes[$w][2]);

            fclose($pipes[$w][1]);
            fclose($pipes[$w][2]);

            $status = proc_close($handle);

            if ($status !== 0) {
                fwrite(STDERR, "Worker {$w} exited {$status}: {$err}\n");

                exit(1);
            }

            $times[] = (float) trim((string) $out);
        }

        $mean = array_sum($times) / count($times);
        $worst = max($times);

        $results["{$workers}x{$threads}"] = $mean;

        printf(
            "%-9d %-9d %12.3f %12.3f %12.1f\n",
            $workers,
            $threads,
            $mean,
            $worst,
            $workers / ($mean / 1000.0)
        );
    }
}

echo "\n";

foreach ($workerCounts as $workers) {
    $one = $results["{$workers}x1"] ?? null;
    $many = $results["{$workers}x4"] ?? null;

    if ($one === null || $many === null) {
        continue;
    }

    printf(
        "%d worker%s: 4 threads is %.2fx %s than 1 thread\n",
        $workers,
        $workers === 1 ? '' : 's',
        $many > $one ? $many / $one : $one / $many,
        $many > $one ? 'SLOWER' : 'faster'
    );
}
