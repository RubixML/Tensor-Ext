<?php

declare(strict_types=1);

/**
 * Records the cross-check oracle: what the pure-PHP rubix/tensor computes for
 * the same operations, over the same fixtures.
 *
 * This is a second opinion, not the gate. The twin is a different
 * implementation, so its numbers are compared within a tolerance and its
 * exception wording is not compared at all. What it is good for is catching a
 * migration that changes the extension and the recorded snapshot in the same
 * direction — something a self-snapshot alone cannot see.
 *
 * The twin declares the same class names as the extension, so the two cannot
 * coexist in one process. Run this one WITHOUT the extension.
 *
 * Usage:
 *   php -n -d memory_limit=4G tools/oracle-dump.php --out=tests/fixtures/parity/oracle
 */

use Tensor\Tests\Parity\Operands;
use Tensor\Tests\Parity\Operations;
use Tensor\Tests\Parity\Transcript;
use Tensor\Tests\Support\Loader;

if (extension_loaded('tensor_ext')) {
    fwrite(STDERR, "tensor_ext is loaded. The oracle must run against the pure-PHP rubix/tensor,\n"
        . "otherwise it compares the extension with itself. Re-run with -n and no extension.\n");

    exit(1);
}

// Root autoloader first: its Tensor\Tests\ prefix is longer than the twin's
// Tensor\, so the harness classes resolve here and only the tensor classes
// themselves fall through to the twin.
require dirname(__DIR__) . '/vendor/autoload.php';

$oracleAutoload = __DIR__ . '/oracle/vendor/autoload.php';

if (!is_file($oracleAutoload)) {
    fwrite(STDERR, "tools/oracle/vendor is missing. Run:\n"
        . "  composer --working-dir=tools/oracle install\n");

    exit(1);
}

require $oracleAutoload;

$options = getopt('', ['out:', 'allow-overwrite', 'quiet']);

$out = $options['out'] ?? null;

if (!is_string($out)) {
    fwrite(STDERR, "Usage: oracle-dump.php --out=<dir> [--allow-overwrite]\n");

    exit(1);
}

$allowOverwrite = array_key_exists('allow-overwrite', $options);
$quiet = array_key_exists('quiet', $options);

$version = 'unknown';
$lock = __DIR__ . '/oracle/composer.lock';

if (is_file($lock)) {
    /** @var array{packages: list<array{name: string, version: string}>} $decoded */
    $decoded = json_decode((string) file_get_contents($lock), true);

    foreach ($decoded['packages'] as $package) {
        if ($package['name'] === 'rubix/tensor') {
            $version = $package['version'];
        }
    }
}

// Probed, never hard-coded: which methods the twin lacks is a property of
// whichever version composer resolved, and pinning a guess would make the gap
// look smaller or larger than it is.
$unsupported = [];

foreach (Operations::all() as $key => $operation) {
    if (!Transcript::isSupported($operation)) {
        $unsupported[] = $key;
    }
}

$header = [
    'oracle' => Transcript::ORACLE,
    'package' => 'rubix/tensor',
    'version' => $version,
    'operations' => count(Operations::all()),
    'unsupported' => count($unsupported),
    'env' => ['php' => PHP_VERSION],
];

foreach (Loader::parityNames() as $fixture) {
    $operands = Operands::from(Loader::load($fixture)['samples']);
    $records = Transcript::record($operands, true);

    $path = Transcript::path($out, $fixture);

    try {
        Transcript::write($path, $header, $records, true, null);
    } catch (RuntimeException $e) {
        // The oracle is advisory and cheap to regenerate, so it has none of the
        // snapshot's re-recording ceremony; only the gate needs that.
        fwrite(STDERR, $e->getMessage() . "\n");

        exit(1);
    }

    if (!$quiet) {
        $threw = count(array_filter($records, static fn (array $r) : bool => $r['status'] === 'threw'));
        $absent = count(array_filter($records, static fn (array $r) : bool => $r['status'] === 'unsupported'));

        printf(
            "%-20s %4d ops  %3d threw  %3d unsupported  %8d bytes\n",
            $fixture,
            count($records),
            $threw,
            $absent,
            (int) filesize($path)
        );
    }
}

sort($unsupported);

file_put_contents(
    rtrim($out, '/') . '/unsupported.json',
    json_encode($unsupported, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES) . "\n"
);

if (!$quiet) {
    printf("\nrubix/tensor %s lacks %d of %d operations.\n", $version, count($unsupported), count(Operations::all()));
}
