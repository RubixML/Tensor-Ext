<?php

namespace Tensor\Tests\Support;

use RuntimeException;

/**
 * Reads the committed synthetic fixtures under tests/fixtures/synthetic.
 *
 * The fixtures come from Rubix ML's dataset generators and are generated once,
 * out of tree, by tools/generate-fixtures.php. They are committed so that CI
 * stays hermetic and so that parity transcripts are reproducible: the generators
 * draw from the global PRNG, which is not stable across PHP versions.
 */
final class Loader
{
    /**
     * Every committed fixture, smallest first.
     *
     * @var list<string>
     */
    public const ALL = [
        'blob-100x4',
        'circle-500x2',
        'halfmoon-500x2',
        'swissroll-1000x3',
        'blob-1000x8',
        'hyperplane-2000x12',
        'agglomerate-5000x16',
    ];

    /**
     * The subset small enough for full-fidelity parity transcripts. The larger
     * fixtures drive benchmarks and digest-only comparisons — a 5000x5000 result
     * is 25 million elements, which no transcript can hold verbatim.
     *
     * @var list<string>
     */
    public const PARITY = [
        'blob-100x4',
        'circle-500x2',
        'halfmoon-500x2',
        'swissroll-1000x3',
    ];

    /**
     * @return list<string>
     */
    public static function names() : array
    {
        return self::ALL;
    }

    /**
     * @return list<string>
     */
    public static function parityNames() : array
    {
        return self::PARITY;
    }

    public static function directory() : string
    {
        return dirname(__DIR__) . '/fixtures/synthetic';
    }

    public static function path(string $name) : string
    {
        return self::directory() . '/' . $name . '.json';
    }

    /**
     * @param string $name
     * @throws RuntimeException
     * @return array{generator: string, seed: int, n: int, samples: list<list<float>>, labels: list<float|string>}
     */
    public static function load(string $name) : array
    {
        $path = self::path($name);
        $raw = file_get_contents($path);

        if ($raw === false) {
            throw new RuntimeException("Cannot read fixture {$path}.");
        }

        $decoded = json_decode($raw, true);

        if (!is_array($decoded)) {
            throw new RuntimeException("Fixture {$name} is not a JSON object.");
        }

        foreach (['generator', 'seed', 'n', 'samples', 'labels'] as $key) {
            if (!array_key_exists($key, $decoded)) {
                throw new RuntimeException("Fixture {$name} is missing '{$key}'.");
            }
        }

        $generator = $decoded['generator'];
        $seed = $decoded['seed'];
        $n = $decoded['n'];
        $samples = $decoded['samples'];
        $labels = $decoded['labels'];

        if (!is_string($generator) || !is_int($seed) || !is_int($n)) {
            throw new RuntimeException("Fixture {$name} has a malformed header.");
        }

        if (!is_array($samples) || $samples === []) {
            throw new RuntimeException("Fixture {$name} has no samples.");
        }

        if (!is_array($labels)) {
            throw new RuntimeException("Fixture {$name} has malformed labels.");
        }

        return [
            'generator' => $generator,
            'seed' => $seed,
            'n' => $n,
            'samples' => self::normalizeSamples($samples, $name),
            'labels' => self::normalizeLabels($labels, $name),
        ];
    }

    /**
     * @throws RuntimeException
     * @return array<string, array{rows: int, columns: int, sha256: string}>
     */
    public static function manifest() : array
    {
        $raw = file_get_contents(self::directory() . '/MANIFEST.json');

        if ($raw === false) {
            throw new RuntimeException('Cannot read the fixture manifest.');
        }

        $decoded = json_decode($raw, true);

        if (!is_array($decoded)) {
            throw new RuntimeException('The fixture manifest is malformed.');
        }

        /** @var array<string, array{rows: int, columns: int, sha256: string}> $decoded */
        return $decoded;
    }

    /**
     * Rectangularity and finiteness are checked here rather than at the call
     * site, so every consumer gets the same guarantee.
     *
     * @param array<mixed> $samples
     * @param string $name
     * @throws RuntimeException
     * @return list<list<float>>
     */
    private static function normalizeSamples(array $samples, string $name) : array
    {
        $rows = [];
        $width = null;

        foreach ($samples as $i => $row) {
            if (!is_array($row)) {
                throw new RuntimeException("Fixture {$name} row {$i} is not an array.");
            }

            $width ??= count($row);

            if (count($row) !== $width) {
                throw new RuntimeException("Fixture {$name} row {$i} has "
                    . count($row) . " columns, expected {$width}.");
            }

            $values = [];

            foreach ($row as $j => $value) {
                if (!is_int($value) && !is_float($value)) {
                    throw new RuntimeException("Fixture {$name} [{$i}][{$j}] is not numeric.");
                }

                $value = (float) $value;

                if (!is_finite($value)) {
                    throw new RuntimeException("Fixture {$name} [{$i}][{$j}] is not finite.");
                }

                $values[] = $value;
            }

            $rows[] = $values;
        }

        return $rows;
    }

    /**
     * @param array<mixed> $labels
     * @param string $name
     * @throws RuntimeException
     * @return list<float|string>
     */
    private static function normalizeLabels(array $labels, string $name) : array
    {
        $out = [];

        foreach ($labels as $i => $label) {
            if (is_string($label)) {
                $out[] = $label;

                continue;
            }

            if (!is_int($label) && !is_float($label)) {
                throw new RuntimeException("Fixture {$name} label {$i} is neither numeric nor a string.");
            }

            $out[] = (float) $label;
        }

        return $out;
    }
}
