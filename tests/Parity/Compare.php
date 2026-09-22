<?php

namespace Tensor\Tests\Parity;

/**
 * Compares two encoded operation records.
 *
 * Two modes, because the two oracles answer different questions. The
 * self-snapshot is exact: the same source, the same compiler and the same BLAS
 * must produce the same bits, and anything else is a behaviour change. The
 * pure-PHP cross-check is tolerant: it is a different implementation, so only
 * the numbers need to agree, and only to a stated precision.
 *
 * Both report the first few differences by path rather than dumping two arrays
 * of a thousand hex strings at the reader.
 */
final class Compare
{
    /**
     * How many differing paths a failure message names before truncating.
     */
    public const MAX_REPORTED = 5;

    /**
     * Fields that a tolerant comparison must not look at.
     *
     * `digest` is a sha256 over the raw bytes — bit-exact by construction, so
     * comparing it tolerantly is a contradiction. `t` records each element's PHP
     * type, which two independent implementations are entitled to disagree about
     * without disagreeing about the numbers: rubix/tensor's Matrix::diagonal()
     * fills the off-diagonal with int 0, the extension with float 0.0, and the
     * values are identical either way.
     *
     * Both fields are compared strictly by exact(), which is what the
     * self-snapshot uses. Only the cross-check relaxes them.
     *
     * @var list<string>
     */
    public const UNCOMPARABLE_UNDER_TOLERANCE = ['digest', 't'];

    /**
     * @param array<string, mixed> $expected
     * @param array<string, mixed> $actual
     * @return list<string>
     */
    public static function exact(array $expected, array $actual) : array
    {
        return self::walk($expected, $actual, '', null);
    }

    /**
     * Exact, except that integer-typed elements may have become double-typed,
     * provided every value is bit-identical.
     *
     * That one widening is the whole accepted behaviour change of the storage
     * migration: a double buffer cannot hold an integer, so the comparison and
     * modulo families, which returned PHP integers, return doubles instead. The
     * values do not move, and the transcript proves it — they are compared as
     * raw IEEE-754 bytes either way.
     *
     * Applied at any depth, because a decomposition result is a composite whose
     * components carry their own type strings: Lu has l, u and p, and it was the
     * permutation matrix that was integer-typed.
     *
     * The allowance is one-directional and per element. A double that became an
     * integer fails, a value that moved fails, and a type string that gained a
     * kind other than double fails. SnapshotTest pins the set of operations
     * needing it, so it cannot quietly spread.
     *
     * @param array<string, mixed> $expected
     * @param array<string, mixed> $actual
     * @return array{0: list<string>, 1: bool} the differences, and whether the allowance was used
     */
    public static function exactAllowingIntToDouble(array $expected, array $actual) : array
    {
        $differences = self::exact($expected, $actual);

        if ($differences === []) {
            return [[], false];
        }

        if (!self::onlyWidensIntegers($expected, $actual)) {
            return [$differences, false];
        }

        $remaining = self::exact(self::withoutTypes($expected), self::withoutTypes($actual));

        return $remaining === [] ? [[], true] : [$differences, false];
    }

    /**
     * @param array<string, mixed> $expected
     * @param array<string, mixed> $actual
     * @param float $tolerance
     * @return list<string>
     */
    public static function withinRelative(array $expected, array $actual, float $tolerance) : array
    {
        foreach (self::UNCOMPARABLE_UNDER_TOLERANCE as $field) {
            unset($expected[$field], $actual[$field]);
        }

        return self::walk($expected, $actual, '', $tolerance);
    }

    /**
     * @param list<string> $differences
     * @param string $key
     */
    public static function describe(string $key, array $differences) : string
    {
        if ($differences === []) {
            return '';
        }

        $shown = array_slice($differences, 0, self::MAX_REPORTED);
        $extra = count($differences) - count($shown);

        return sprintf(
            "%s differs from the recorded transcript:\n  %s%s",
            $key,
            implode("\n  ", $shown),
            $extra > 0 ? "\n  ... and {$extra} more" : ''
        );
    }

    /**
     * True when every type string that differs does so only by integers having
     * become doubles.
     *
     * @param mixed $expected
     * @param mixed $actual
     */
    private static function onlyWidensIntegers(mixed $expected, mixed $actual) : bool
    {
        if (!is_array($expected) || !is_array($actual)) {
            return true;
        }

        if (isset($expected['t'], $actual['t']) && is_string($expected['t']) && is_string($actual['t'])
            && $expected['t'] !== $actual['t']) {
            $count = is_int($expected['n'] ?? null) ? $expected['n'] : 0;

            $before = self::expandTypes($expected['t'], $count);
            $after = self::expandTypes($actual['t'], $count);

            if (strlen($before) !== strlen($after)) {
                return false;
            }

            for ($i = 0, $length = strlen($before); $i < $length; ++$i) {
                if ($before[$i] === $after[$i]) {
                    continue;
                }

                if ($before[$i] !== 'i' || $after[$i] !== 'd') {
                    return false;
                }
            }
        }

        foreach ($expected as $key => $value) {
            if (!self::onlyWidensIntegers($value, $actual[$key] ?? null)) {
                return false;
            }
        }

        return true;
    }

    /**
     * Encode::compactTypes() writes a uniform run as two characters; expand it
     * so the two sides can be compared position by position.
     * @param string $types
     * @param int $count
     */
    private static function expandTypes(string $types, int $count) : string
    {
        if (strlen($types) === 2 && $types[1] === '*') {
            return str_repeat($types[0], $count);
        }

        return $types;
    }

    /**
     * @param mixed $record
     * @return mixed
     */
    private static function withoutTypes(mixed $record) : mixed
    {
        if (!is_array($record)) {
            return $record;
        }

        unset($record['t']);

        foreach ($record as $key => $value) {
            $record[$key] = self::withoutTypes($value);
        }

        return $record;
    }

    /**
     * @param mixed $expected
     * @param mixed $actual
     * @param string $path
     * @param ?float $tolerance
     * @return list<string>
     */
    private static function walk(mixed $expected, mixed $actual, string $path, ?float $tolerance) : array
    {
        if (is_array($expected) && is_array($actual)) {
            return self::walkArray($expected, $actual, $path, $tolerance);
        }

        if (is_array($expected) !== is_array($actual)) {
            return [self::at($path) . 'shape changed: ' . get_debug_type($expected)
                . ' vs ' . get_debug_type($actual)];
        }

        if ($expected === $actual) {
            return [];
        }

        // A tolerant comparison only relaxes the encoded doubles; everything
        // else — kinds, shapes, element types, exception classes — stays strict.
        if ($tolerance !== null && is_string($expected) && is_string($actual)
            && self::looksLikeDouble($expected) && self::looksLikeDouble($actual)) {
            if (self::closeEnough($expected, $actual, $tolerance)) {
                return [];
            }

            return [self::at($path) . sprintf(
                '%.17g vs %.17g exceeds a relative tolerance of %g',
                Encode::decodeF64($expected),
                Encode::decodeF64($actual),
                $tolerance
            )];
        }

        return [self::at($path) . self::render($expected) . ' vs ' . self::render($actual)];
    }

    /**
     * @param array<array-key, mixed> $expected
     * @param array<array-key, mixed> $actual
     * @param string $path
     * @param ?float $tolerance
     * @return list<string>
     */
    private static function walkArray(array $expected, array $actual, string $path, ?float $tolerance) : array
    {
        $differences = [];

        foreach ($expected as $key => $value) {
            if ($tolerance !== null && in_array($key, self::UNCOMPARABLE_UNDER_TOLERANCE, true)) {
                continue;
            }

            if (!array_key_exists($key, $actual)) {
                $differences[] = self::at("{$path}.{$key}") . 'missing';

                continue;
            }

            foreach (self::walk($value, $actual[$key], "{$path}.{$key}", $tolerance) as $difference) {
                $differences[] = $difference;
            }
        }

        foreach ($actual as $key => $value) {
            if ($tolerance !== null && in_array($key, self::UNCOMPARABLE_UNDER_TOLERANCE, true)) {
                continue;
            }

            if (!array_key_exists($key, $expected)) {
                $differences[] = self::at("{$path}.{$key}") . 'unexpected';
            }
        }

        return $differences;
    }

    private static function looksLikeDouble(string $value) : bool
    {
        return strlen($value) === 16 && ctype_xdigit($value);
    }

    private static function closeEnough(string $expected, string $actual, float $tolerance) : bool
    {
        $a = Encode::decodeF64($expected);
        $b = Encode::decodeF64($actual);

        if ($a === $b) {
            return true;
        }

        if (!is_finite($a) || !is_finite($b)) {
            return false;
        }

        $scale = max(abs($a), abs($b));

        return $scale === 0.0 ? true : abs($a - $b) / $scale <= $tolerance;
    }

    private static function at(string $path) : string
    {
        return ($path === '' ? '(root)' : ltrim($path, '.')) . ': ';
    }

    private static function render(mixed $value) : string
    {
        if (is_string($value) && self::looksLikeDouble($value)) {
            return sprintf('%.17g', Encode::decodeF64($value));
        }

        return is_scalar($value) || $value === null
            ? var_export($value, true)
            : get_debug_type($value);
    }
}
