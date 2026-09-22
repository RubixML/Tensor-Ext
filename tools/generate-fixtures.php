<?php

declare(strict_types=1);

/**
 * Generates the committed synthetic fixtures from Rubix ML's dataset generators.
 *
 * This is the only file in the repository that depends on rubix/ml, and it is
 * deliberately not wired into composer.json: rubix/ml requires php >= 8.3 while
 * this package supports 8.1, so adding it would break the 8.1 and 8.2 CI legs.
 * Run it from a throwaway sandbox instead — see tests/fixtures/synthetic/README.md
 * — and commit the JSON it produces. CI never runs this.
 *
 * Usage: php -d serialize_precision=-1 generate-fixtures.php <output-dir>
 */

use Rubix\ML\Datasets\Generators\Agglomerate;
use Rubix\ML\Datasets\Generators\Blob;
use Rubix\ML\Datasets\Generators\Circle;
use Rubix\ML\Datasets\Generators\Generator;
use Rubix\ML\Datasets\Generators\HalfMoon;
use Rubix\ML\Datasets\Generators\Hyperplane;
use Rubix\ML\Datasets\Generators\SwissRoll;

const SEED = 20260921;

/**
 * Small enough for full-fidelity parity transcripts. The rest drive benchmarks.
 */
const PARITY_FIXTURES = ['blob-100x4', 'circle-500x2', 'halfmoon-500x2', 'swissroll-1000x3'];

$autoload = getcwd() . '/vendor/autoload.php';

if (!is_file($autoload)) {
    fwrite(STDERR, 'vendor/autoload.php not found in ' . getcwd() . ".\n"
        . "Run this from a sandbox that has done `composer require rubix/ml`.\n");

    exit(1);
}

require $autoload;

$out = $argv[1] ?? null;

if ($out === null) {
    fwrite(STDERR, "Usage: php generate-fixtures.php <output-dir>\n");

    exit(1);
}

if (!is_dir($out) && !mkdir($out, 0o755, true)) {
    fwrite(STDERR, "Cannot create {$out}.\n");

    exit(1);
}

/** @var array<string, array{0: Generator, 1: int}> $specs */
$specs = [
    'blob-100x4' => [new Blob([0.0, 1.0, -2.0, 0.5], 0.25), 100],
    'blob-1000x8' => [new Blob([0.0, 1.0, -2.0, 0.5, 3.0, -1.0, 0.25, 7.5], 1.0), 1000],
    'agglomerate-5000x16' => [
        new Agglomerate([
            'a' => new Blob(array_fill(0, 16, 0.0), 1.0),
            'b' => new Blob(array_fill(0, 16, 4.0), 1.5),
            'c' => new Blob(array_fill(0, 16, -4.0), 0.5),
        ], [3.5, 4.0, 5.0]),
        5000,
    ],
    'hyperplane-2000x12' => [new Hyperplane(range(-1.0, 1.0, 2.0 / 11), 150.0, 0.25), 2000],
    'swissroll-1000x3' => [new SwissRoll(5.5, 1.5, -2.0, 10.0, 21.0, 0.2), 1000],
    'circle-500x2' => [new Circle(0.0, 0.0, 100.0, 0.1), 500],
    'halfmoon-500x2' => [new HalfMoon(4.0, 0.0, 6.0, 180.0, 0.2), 500],
];

$manifest = [];

foreach ($specs as $name => [$generator, $n]) {
    // The generators draw from the global PRNG, so reseed before every one.
    mt_srand(SEED);
    srand(SEED);

    $dataset = $generator->generate($n);

    $samples = $dataset->samples();
    $labels = method_exists($dataset, 'labels') ? $dataset->labels() : [];

    assertFinite($samples, $name);

    $payload = [
        'generator' => $generator::class,
        'seed' => SEED,
        'n' => $n,
        'samples' => $samples,
        'labels' => $labels,
    ];

    // Pretty-printing agglomerate-5000x16 alone costs ~10 MB of committed diff.
    $flags = JSON_PRESERVE_ZERO_FRACTION
        | (in_array($name, PARITY_FIXTURES, true) ? JSON_PRETTY_PRINT : 0);

    $json = json_encode($payload, $flags);

    if ($json === false) {
        fwrite(STDERR, "{$name}: " . json_last_error_msg() . "\n");

        exit(1);
    }

    // Fail loudly rather than commit a lossy fixture.
    $decoded = json_decode($json, true);

    if (!is_array($decoded) || $decoded['samples'] !== $samples || $decoded['labels'] !== $labels) {
        fwrite(STDERR, "{$name} does not survive a JSON round-trip;"
            . " check that serialize_precision is -1.\n");

        exit(1);
    }

    file_put_contents("{$out}/{$name}.json", $json . "\n");

    $manifest[$name] = [
        'rows' => count($samples),
        'columns' => count($samples[0]),
        'sha256' => hash_file('sha256', "{$out}/{$name}.json"),
    ];

    printf("%-22s %5d x %-3d %s\n", $name, count($samples), count($samples[0]), $manifest[$name]['sha256']);
}

file_put_contents(
    "{$out}/MANIFEST.json",
    json_encode($manifest, JSON_PRETTY_PRINT) . "\n"
);

/**
 * @param list<list<float>> $samples
 * @param string $name
 */
function assertFinite(array $samples, string $name) : void
{
    foreach ($samples as $i => $row) {
        foreach ($row as $j => $value) {
            if (!is_finite($value)) {
                fwrite(STDERR, "{$name}: non-finite value at [{$i}][{$j}].\n");

                exit(1);
            }
        }
    }
}
