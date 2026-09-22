<?php

namespace Tensor\Tests\Parity;

use RuntimeException;
use Throwable;

/**
 * Reads and writes parity transcripts.
 *
 * A transcript is one JSON file per fixture: a header describing the toolchain
 * that produced it, and one encoded record per operation. Both the extension
 * dump and the pure-PHP cross-check produce the same shape, so the same reader
 * and the same comparator serve both.
 *
 * The self-snapshot is a photograph of a specific commit. Its value depends
 * entirely on not being retaken by accident, so writing refuses to overwrite an
 * existing file unless the caller names the commit already recorded in it.
 */
final class Transcript
{
    public const SNAPSHOT = 'snapshot';

    public const ORACLE = 'oracle';

    /**
     * Runs every operation against one fixture's operands.
     *
     * An operation that throws is recorded, not skipped: the exception class and
     * message are public API for a library whose dimensionality errors name the
     * shapes involved, and a migration that stops throwing is a regression.
     *
     * @param Operands $operands
     * @param bool $probeSupport
     * @return array<string, array<string, mixed>>
     */
    public static function record(Operands $operands, bool $probeSupport = false) : array
    {
        $records = [];

        foreach (Operations::all() as $key => $operation) {
            if ($probeSupport && !self::isSupported($operation)) {
                $records[$key] = [
                    'status' => 'unsupported',
                    'reason' => $operation->subjectClass() . '::' . $operation->method() . '() is absent',
                ];

                continue;
            }

            try {
                $result = ($operation->apply)($operands);

                $records[$key] = ['status' => 'ok', 'result' => Encode::result($result)];
            } catch (Throwable $e) {
                $records[$key] = Encode::thrown($e);
            }
        }

        ksort($records);

        return $records;
    }

    /**
     * Whether the implementation under test actually has the method. Probed, never
     * hard-coded: the gap between the extension and the pure-PHP twin is a
     * property of whichever twin version composer resolved.
     * @param Operation $operation
     */
    public static function isSupported(Operation $operation) : bool
    {
        return method_exists($operation->subjectClass(), $operation->method());
    }

    /**
     * @param array<string, mixed> $header
     * @param array<string, array<string, mixed>> $records
     * @param string $path
     * @param bool $allowOverwrite
     * @param ?string $expectedCommit
     * @throws RuntimeException
     */
    public static function write(
        string $path,
        array $header,
        array $records,
        bool $allowOverwrite = false,
        ?string $expectedCommit = null
    ) : void {
        if (is_file($path) && !$allowOverwrite) {
            throw new RuntimeException(
                "{$path} already exists. A recorded transcript is the regression gate for this"
                . ' migration; re-recording it must be deliberate. Pass --allow-overwrite together'
                . ' with --source-commit=<sha> matching the commit already in the file.'
            );
        }

        if (is_file($path) && $allowOverwrite) {
            $existing = self::read($path);
            $recorded = $existing['header']['source_commit'] ?? null;

            if ($expectedCommit === null || $recorded !== $expectedCommit) {
                throw new RuntimeException(sprintf(
                    '%s was recorded at %s; refusing to overwrite it on the word of --source-commit=%s.',
                    $path,
                    is_string($recorded) ? $recorded : '(unknown)',
                    $expectedCommit ?? '(absent)'
                ));
            }
        }

        $directory = dirname($path);

        if (!is_dir($directory) && !mkdir($directory, 0o755, true)) {
            throw new RuntimeException("Cannot create {$directory}.");
        }

        file_put_contents($path, self::encode($header, $records));
    }

    /**
     * @param string $path
     * @throws RuntimeException
     * @return array{header: array<string, mixed>, operations: array<string, array<string, mixed>>}
     */
    public static function read(string $path) : array
    {
        $raw = file_get_contents($path);

        if ($raw === false) {
            throw new RuntimeException("Cannot read {$path}.");
        }

        $decoded = json_decode($raw, true);

        if (!is_array($decoded) || !isset($decoded['header']) || !isset($decoded['operations'])) {
            throw new RuntimeException("{$path} is not a transcript.");
        }

        /** @var array{header: array<string, mixed>, operations: array<string, array<string, mixed>>} $decoded */
        return $decoded;
    }

    public static function path(string $directory, string $fixture) : string
    {
        return rtrim($directory, '/') . '/' . $fixture . '.json';
    }

    /**
     * The environment a transcript's bit-exactness depends on. Compared before
     * the numbers are, so that running on a different toolchain reports "this
     * was recorded elsewhere" rather than a diff of four million values.
     *
     * @return array<string, string>
     */
    public static function environment() : array
    {
        return [
            'php' => PHP_VERSION,
            'openblas_coretype' => (string) getenv('OPENBLAS_CORETYPE'),
        ];
    }

    /**
     * Written with one operation per line rather than through JSON_PRETTY_PRINT.
     *
     * Pretty-printing puts every encoded element on its own indented line, which
     * costs roughly forty bytes per element and ran the four transcripts to
     * 25 MB. One line per operation is the granularity a reader actually wants
     * anyway: a behaviour change shows up as a one-line diff naming the
     * operation, instead of a reshuffle of a million short lines.
     *
     * @param array<string, mixed> $header
     * @param array<string, array<string, mixed>> $records
     * @throws RuntimeException
     */
    private static function encode(array $header, array $records) : string
    {
        $lines = ['{', '"header": ' . self::json($header) . ',', '"operations": {'];

        $last = array_key_last($records);

        foreach ($records as $key => $record) {
            $lines[] = self::json((string) $key) . ': ' . self::json($record)
                . ($key === $last ? '' : ',');
        }

        $lines[] = '}';
        $lines[] = '}';

        return implode("\n", $lines) . "\n";
    }

    /**
     * @param mixed $value
     * @throws RuntimeException
     */
    private static function json(mixed $value) : string
    {
        $json = json_encode($value, JSON_UNESCAPED_SLASHES);

        if ($json === false) {
            throw new RuntimeException('Cannot encode the transcript: ' . json_last_error_msg());
        }

        return $json;
    }
}
