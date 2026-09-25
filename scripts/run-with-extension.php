<?php

/**
 * Run a PHP script with the tensor extension available.
 *
 * The extension is normally installed system wide and loaded through php.ini.
 * Some environments however ship with the shared object present but disabled,
 * which makes every test and benchmark fail with "Class Tensor\Vector does not
 * exist". When that happens fall back to the local build produced by
 * "composer compile" so the command still runs against the working tree.
 *
 * The target is always executed through the current PHP binary so that scripts
 * do not need to carry the executable bit.
 *
 * Usage:
 *
 *     php scripts/run-with-extension.php <script> [...arguments]
 */
$command = array_slice($argv, 1);

if (!$command) {
    fwrite(STDERR, "Usage: php scripts/run-with-extension.php <script> [...arguments]\n");
    exit(1);
}

$prefix = [PHP_BINARY];

$library = dirname(__DIR__) . '/ext/modules/tensor.so';
$injected = false;

if (extension_loaded('tensor')) {
    // Already enabled through php.ini, nothing to add.
} elseif (is_file($library)) {
    $prefix[] = '-d';
    $prefix[] = 'extension=' . $library;

    $injected = true;

    fwrite(STDERR, "tensor is not enabled in php.ini, using $library\n\n");
} else {
    fwrite(STDERR, "tensor is not enabled in php.ini and no local build was found at $library.\n");
    fwrite(STDERR, "Run \"composer compile\" to build it.\n");

    exit(1);
}

/**
 * PHPBench re-spawns PHP for every subject, so the extension has to be handed
 * to it as a runner setting rather than as a flag on this process. The path has
 * to be absolute because those child processes run from a temporary directory.
 */
if ($injected && str_starts_with(basename($command[0]), 'phpbench')) {
    $command[] = '--php-config=' . json_encode(['extension' => $library]);
}

$process = proc_open(array_merge($prefix, $command), [0 => STDIN, 1 => STDOUT, 2 => STDERR], $pipes);

if (!is_resource($process)) {
    fwrite(STDERR, 'Failed to run: ' . implode(' ', $command) . "\n");

    exit(1);
}

exit(proc_close($process));
