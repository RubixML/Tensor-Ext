<?php

declare(strict_types=1);

/**
 * Classifies how the current build differs from the recorded snapshot.
 *
 * SnapshotTest answers "did anything move" with a yes or no. During the storage
 * migration that is not enough: moving Vector onto a double buffer legitimately
 * turns integer results into floats, while leaving every value bit-identical.
 * That is an accepted, documented change. A value that actually moved is not.
 *
 * This tool separates the two, so each sub-step of the migration can be reviewed
 * on the shape of its diff rather than on a pass/fail bit.
 *
 * Usage:
 *   php -n -d memory_limit=4G -d extension=<...>.so tools/parity-diff.php [--verbose] [--fixture=NAME]
 */

use Tensor\Tests\Parity\Compare;
use Tensor\Tests\Parity\Encode;
use Tensor\Tests\Parity\Operands;
use Tensor\Tests\Parity\Operations;
use Tensor\Tests\Parity\SnapshotTest;
use Tensor\Tests\Parity\Transcript;
use Tensor\Tests\Support\Loader;

require dirname(__DIR__) . '/vendor/autoload.php';

if (!extension_loaded('tensor_ext')) {
    fwrite(STDERR, "Load the extension: -d extension=ext/modules/tensor_ext.so\n");

    exit(1);
}

$options = getopt('', ['verbose', 'fixture:']);
$verbose = array_key_exists('verbose', $options);
$only = is_string($options['fixture'] ?? null) ? $options['fixture'] : null;

$directory = dirname(__DIR__) . '/tests/fixtures/parity/snapshot';

/**
 * Operations excused in SnapshotTest::REASSOCIATED, kept in step with it by
 * reading the constant rather than restating the list.
 *
 * @var list<string>
 */
$reassociated = array_keys(SnapshotTest::REASSOCIATED);

/** @var array<string, list<string>> $buckets */
$buckets = ['typeOnly' => [], 'reassociated' => [], 'value' => [], 'status' => [], 'shape' => [], 'other' => []];

$total = 0;
$identical = 0;

foreach (Loader::parityNames() as $fixture) {
    if ($only !== null && $fixture !== $only) {
        continue;
    }

    $recorded = Transcript::read(Transcript::path($directory, $fixture))['operations'];
    $operands = Operands::from(Loader::load($fixture)['samples']);

    foreach (Operations::all() as $key => $operation) {
        ++$total;

        $expected = $recorded[$key] ?? null;

        if (!is_array($expected)) {
            $buckets['other'][] = "{$fixture}/{$key}: absent from the transcript";

            continue;
        }

        try {
            $actual = ['status' => 'ok', 'result' => Encode::result(($operation->apply)($operands))];
        } catch (Throwable $e) {
            $actual = Encode::thrown($e);
        }

        if (Compare::exact($expected, $actual) === []) {
            ++$identical;

            continue;
        }

        $bucket = classify($expected, $actual);

        if ($bucket === 'value' && in_array($key, $reassociated, true)) {
            $bucket = 'reassociated';
        }

        $buckets[$bucket][] = describe($fixture, $key, $expected, $actual, $verbose);
    }
}

/**
 * @param array<string, mixed> $expected
 * @param array<string, mixed> $actual
 */
function classify(array $expected, array $actual) : string
{
    // Delegated rather than restated: this tool exists to be trusted about
    // exactly the distinction Compare draws, so it has to draw the same one.
    [, $widened] = Compare::exactAllowingIntToDouble($expected, $actual);

    if ($widened) {
        return 'typeOnly';
    }

    if (($expected['status'] ?? null) !== ($actual['status'] ?? null)) {
        return 'status';
    }

    if (($expected['status'] ?? null) !== 'ok') {
        return 'other';
    }

    $a = $expected['result'];
    $b = $actual['result'];

    if (!is_array($a) || !is_array($b)) {
        return 'other';
    }

    if (($a['shape'] ?? null) !== ($b['shape'] ?? null) || ($a['n'] ?? null) !== ($b['n'] ?? null)) {
        return 'shape';
    }

    return 'value';
}

/**
 * @param array<string, mixed> $expected
 * @param array<string, mixed> $actual
 * @param string $fixture
 * @param string $key
 * @param bool $verbose
 */
function describe(string $fixture, string $key, array $expected, array $actual, bool $verbose) : string
{
    $line = "{$fixture}/{$key}";

    if (!$verbose) {
        return $line;
    }

    return $line . "\n      " . implode("\n      ", array_slice(Compare::exact($expected, $actual), 0, 3));
}

$labels = [
    'typeOnly' => 'element type changed, every value identical  (ACCEPTED: int -> float)',
    'reassociated' => 'last ulps moved, reduction reassociated    (ACCEPTED: see SnapshotTest::REASSOCIATED)',
    'value' => 'a value changed                              (REGRESSION unless justified)',
    'status' => 'started or stopped throwing                  (REGRESSION unless justified)',
    'shape' => 'shape or element count changed               (REGRESSION unless justified)',
    'other' => 'unclassified',
];

printf("%d of %d operation/fixture pairs reproduce the snapshot exactly.\n\n", $identical, $total);

$exit = 0;

foreach ($buckets as $bucket => $entries) {
    if ($entries === []) {
        continue;
    }

    if ($bucket !== 'typeOnly' && $bucket !== 'reassociated') {
        $exit = 1;
    }

    printf("%s  [%d]\n  %s\n", strtoupper($bucket), count($entries), $labels[$bucket]);

    foreach (array_slice($entries, 0, $verbose ? 200 : 40) as $entry) {
        echo "    {$entry}\n";
    }

    if (count($entries) > ($verbose ? 200 : 40)) {
        printf("    ... and %d more\n", count($entries) - ($verbose ? 200 : 40));
    }

    echo "\n";
}

exit($exit);
