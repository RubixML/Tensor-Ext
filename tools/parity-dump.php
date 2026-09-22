<?php

declare(strict_types=1);

/**
 * Records the self-snapshot: what this build of the extension does, operation by
 * operation, over every parity fixture.
 *
 * This transcript is the hard gate for the buffer migration. Its worth depends
 * on having been recorded from a build that the migration has not touched, so
 * run it against a .so produced by plain `phpize && ./configure && make` from
 * the committed ext/ — never one produced by `composer compile`, which would
 * bake the new toolchain into the control.
 *
 * Usage:
 *   php -n -d memory_limit=4G -d extension=<path>/tensor_ext.so \
 *       tools/parity-dump.php --out=tests/fixtures/parity/snapshot --commit=<sha> [--so=<path>]
 *
 * Re-recording an existing transcript additionally requires --allow-overwrite
 * and a --commit that matches the one already in the file.
 */

use Tensor\Settings;
use Tensor\Tests\Parity\Operands;
use Tensor\Tests\Parity\Transcript;
use Tensor\Tests\Support\Loader;

require dirname(__DIR__) . '/vendor/autoload.php';

if (!extension_loaded('tensor_ext')) {
    fwrite(STDERR, "The self-snapshot must be recorded from the compiled extension.\n"
        . "Re-run with -d extension=<path>/tensor_ext.so.\n");

    exit(1);
}

$options = getopt('', ['out:', 'commit:', 'so::', 'allow-overwrite', 'quiet']);

$out = $options['out'] ?? null;
$commit = $options['commit'] ?? null;

if (!is_string($out) || !is_string($commit)) {
    fwrite(STDERR, "Usage: parity-dump.php --out=<dir> --commit=<sha> [--so=<path>] [--allow-overwrite]\n");

    exit(1);
}

$so = is_string($options['so'] ?? null) && $options['so'] !== ''
    ? $options['so']
    : dirname(__DIR__) . '/ext/modules/tensor_ext.so';

$allowOverwrite = array_key_exists('allow-overwrite', $options);
$quiet = array_key_exists('quiet', $options);

/**
 * Deviations from the pinned commit that the baseline deliberately includes.
 *
 * The snapshot is meant to be a photograph of the commit named below, so any
 * departure from it has to be visible in the transcript itself rather than
 * living in a commit message. tensor_pseudoinverse read out of bounds for every
 * tall matrix, which made the operation non-deterministic and therefore
 * impossible to transcribe at all; baselining the unfixed behaviour would have
 * meant baselining undefined behaviour.
 */
const PATCHES = [
    'ext/include/linear_algebra.c: tensor_pseudoinverse dgemm K argument m -> MIN(m, n)'
        . ' (out-of-bounds read of vvt for every m > n)',
];

$header = [
    'oracle' => Transcript::SNAPSHOT,
    'source_commit' => $commit,
    'patches' => PATCHES,
    'ext_sha256' => is_file($so) ? hash_file('sha256', $so) : 'unknown',
    // Recorded so that a transcript retaken after the compiler switch is
    // self-evidently not the control any more.
    'zephir' => 'none (built from the committed ext/)',
    'threads' => Settings::numThreads(),
    'operations' => count(Tensor\Tests\Parity\Operations::all()),
    'env' => Transcript::environment(),
];

if ($header['threads'] !== 1) {
    fwrite(STDERR, "OpenBLAS is set to {$header['threads']} threads. Record with one:\n"
        . "dgemm's reduction order, and so its last ulps, varies with thread count.\n");

    exit(1);
}

foreach (Loader::parityNames() as $fixture) {
    $operands = Operands::from(Loader::load($fixture)['samples']);
    $records = Transcript::record($operands);

    $path = Transcript::path($out, $fixture);

    try {
        Transcript::write($path, $header, $records, $allowOverwrite, $allowOverwrite ? $commit : null);
    } catch (RuntimeException $e) {
        fwrite(STDERR, $e->getMessage() . "\n");

        exit(1);
    }

    if (!$quiet) {
        $threw = count(array_filter($records, static fn (array $r) : bool => $r['status'] === 'threw'));

        printf(
            "%-20s %4d ops  %3d threw  %8d bytes  %s\n",
            $fixture,
            count($records),
            $threw,
            (int) filesize($path),
            substr(hash_file('sha256', $path), 0, 16)
        );
    }
}
