<?php

namespace Tensor\Tests\Parity;

use ArrayIterator;
use RuntimeException;
use Throwable;
use Traversable;

/**
 * Turns the result of a tensor operation into a byte-stable JSON record.
 *
 * Two things drive the shape of that record.
 *
 * Exactness: floats are encoded as `bin2hex(pack('E', $x))`, the raw big-endian
 * IEEE-754 bytes. That is exact by construction, locale-free, preserves -0.0,
 * and sidesteps serialize_precision entirely — which matters because the fixture
 * generator and the test runner use different ini.
 *
 * Size: a full-fidelity transcript of every element of every operation over
 * every fixture runs to several megabytes, and a single matmul on the largest
 * fixture is 25 million elements. So the gate is a sha256 over the concatenated
 * raw bytes, which is exactly as strong as storing them, and the values
 * themselves are kept only when there are few enough to be worth reading.
 */
final class Encode
{
    /**
     * Results with at most this many elements keep their values verbatim. Larger
     * ones are compared by digest, with head and tail retained to localise a
     * failure. Asserted in OperationsTest so it cannot drift silently.
     *
     * The digest is a sha256 over the concatenated raw IEEE-754 bytes, so it is
     * exactly as strong a gate as storing every element; the verbatim values buy
     * readability, not safety. At this threshold rank-1 results stay readable
     * while rank-2 results, which are quadratically larger and rarely read by
     * eye, are compared by digest.
     */
    public const FULL_FIDELITY_LIMIT = 256;

    /**
     * How many leading and trailing elements a digest-only record retains.
     */
    public const EDGE = 8;

    /**
     * Component accessors for the objects the decomposition and reduction
     * methods return. Explicit rather than reflected: a silent change in one of
     * these classes should break the transcript, not be absorbed by it.
     *
     * @var array<class-string, list<string>>
     */
    private const COMPONENTS = [
        'Tensor\Decompositions\Lu' => ['l', 'u', 'p'],
        'Tensor\Decompositions\Svd' => ['u', 'singularValues', 's', 'v', 'vT'],
        'Tensor\Decompositions\Cholesky' => ['l', 'lT'],
        'Tensor\Decompositions\Eigen' => ['eigenvalues', 'eigenvectors'],
        'Tensor\Reductions\Ref' => ['a', 'swaps'],
        'Tensor\Reductions\Rref' => ['a'],
    ];

    /**
     * One IEEE-754 double as 16 lowercase hex characters, or a token for the
     * values that have no meaningful bit pattern. NaN payloads differ between
     * libm implementations and carry no information, so they are normalised.
     * @param float $value
     */
    public static function f64(float $value) : string
    {
        if (is_nan($value)) {
            return 'NAN';
        }

        if (!is_finite($value)) {
            return $value > 0.0 ? 'INF' : '-INF';
        }

        return bin2hex(pack('E', $value));
    }

    public static function decodeF64(string $encoded) : float
    {
        if ($encoded === 'NAN') {
            return NAN;
        }

        if ($encoded === 'INF' || $encoded === '-INF') {
            return $encoded === 'INF' ? INF : -INF;
        }

        $bytes = hex2bin($encoded);

        if ($bytes === false) {
            throw new RuntimeException("Cannot decode '{$encoded}' as a double.");
        }

        /** @var array{1: float} $unpacked */
        $unpacked = unpack('E', $bytes);

        return $unpacked[1];
    }

    /**
     * @param mixed $value
     * @return array<string, mixed>
     */
    public static function result(mixed $value) : array
    {
        if (is_object($value)) {
            return self::object($value);
        }

        if (is_array($value)) {
            return self::numeric(self::flatten($value), self::shapeOf($value), 'array');
        }

        return self::scalar($value);
    }

    /**
     * @param Throwable $e
     * @return array{status: string, class: class-string, message: string}
     */
    public static function thrown(Throwable $e) : array
    {
        return [
            'status' => 'threw',
            'class' => $e::class,
            'message' => $e->getMessage(),
        ];
    }

    /**
     * @param mixed $value
     * @return array<string, mixed>
     */
    private static function scalar(mixed $value) : array
    {
        if (is_float($value)) {
            return ['k' => 'float', 'v' => self::f64($value)];
        }

        if (is_int($value)) {
            return ['k' => 'int', 'v' => $value];
        }

        if (is_bool($value)) {
            return ['k' => 'bool', 'v' => $value];
        }

        if (is_string($value)) {
            return ['k' => 'string', 'v' => $value];
        }

        if ($value === null) {
            return ['k' => 'null'];
        }

        return ['k' => 'unknown', 'v' => get_debug_type($value)];
    }

    /**
     * @param object $value
     * @return array<string, mixed>
     */
    private static function object(object $value) : array
    {
        foreach (self::COMPONENTS as $class => $accessors) {
            if ($value instanceof $class) {
                $parts = ['k' => 'composite', 'class' => $value::class];

                foreach ($accessors as $accessor) {
                    /** @var callable $callable */
                    $callable = [$value, $accessor];
                    $parts[$accessor] = self::result($callable());
                }

                return $parts;
            }
        }

        // asArray() is tried before Traversable on purpose: every tensor is an
        // IteratorAggregate, so the iterator branch would otherwise swallow all
        // of them and record a rank-2 result as a flat list of the wrong kind.
        if (method_exists($value, 'asArray')) {
            /** @var array<mixed> $array */
            $array = $value->asArray();

            return self::numeric(self::flatten($array), self::shapeOf($array), self::kindOf($value));
        }

        if ($value instanceof ArrayIterator || $value instanceof Traversable) {
            $items = iterator_to_array($value);

            return self::numeric(self::flatten($items), self::shapeOf($items), 'iterator');
        }

        return ['k' => 'object', 'class' => $value::class];
    }

    private static function kindOf(object $value) : string
    {
        return match ($value::class) {
            'Tensor\Matrix' => 'matrix',
            'Tensor\ColumnVector' => 'columnVector',
            'Tensor\Vector' => 'vector',
            default => $value::class,
        };
    }

    /**
     * @param list<int|float|bool|string|null> $flat
     * @param list<int> $shape
     * @param string $kind
     * @return array<string, mixed>
     */
    private static function numeric(array $flat, array $shape, string $kind) : array
    {
        $n = count($flat);

        $types = '';
        $raw = '';
        $hex = [];

        foreach ($flat as $value) {
            $types .= self::typeChar($value);

            $encoded = is_float($value) || is_int($value)
                ? self::f64((float) $value)
                : null;

            if ($encoded === null) {
                // Non-numeric elements (labels, bools) are folded in verbatim so
                // the digest still depends on them.
                $raw .= serialize($value);
                $hex[] = self::scalar($value);

                continue;
            }

            // Non-finite values go into the digest as their token, never as raw
            // bytes: a NaN payload is libm-dependent and would make the digest
            // vary by toolchain rather than by behaviour.
            $raw .= self::isToken($encoded) ? $encoded : pack('E', (float) $value);
            $hex[] = $encoded;
        }

        $record = [
            'k' => $kind,
            'shape' => $shape,
            'n' => $n,
            't' => self::compactTypes($types),
            'digest' => 'sha256:' . hash('sha256', $raw),
        ];

        if ($n <= self::FULL_FIDELITY_LIMIT) {
            $record['v'] = $hex;

            return $record;
        }

        $record['head'] = array_slice($hex, 0, self::EDGE);
        $record['tail'] = array_slice($hex, -self::EDGE);

        return $record;
    }

    private static function isToken(string $encoded) : bool
    {
        return $encoded === 'NAN' || $encoded === 'INF' || $encoded === '-INF';
    }

    private static function typeChar(mixed $value) : string
    {
        return match (true) {
            is_float($value) => 'd',
            is_int($value) => 'i',
            is_bool($value) => 'b',
            is_string($value) => 's',
            default => '?',
        };
    }

    /**
     * Almost every result is uniformly typed, so record that as two characters
     * rather than one per element. The distinction is load-bearing: quick()
     * stores integers today, and the Stage 3 migration coerces them to floats.
     * A value-only transcript cannot see that change; this can.
     * @param string $types
     */
    private static function compactTypes(string $types) : string
    {
        if ($types === '') {
            return '';
        }

        $first = $types[0];

        return strspn($types, $first) === strlen($types) ? $first . '*' : $types;
    }

    /**
     * @param array<mixed> $value
     * @return list<int>
     */
    private static function shapeOf(array $value) : array
    {
        $shape = [count($value)];
        $first = $value === [] ? null : reset($value);

        if (is_array($first)) {
            $shape[] = count($first);
        }

        return $shape;
    }

    /**
     * @param array<mixed> $value
     * @return list<int|float|bool|string|null>
     */
    private static function flatten(array $value) : array
    {
        $out = [];

        foreach ($value as $item) {
            if (is_array($item)) {
                foreach (self::flatten($item) as $inner) {
                    $out[] = $inner;
                }

                continue;
            }

            if (is_object($item)) {
                // Matrix::asVectors() and friends yield tensor objects.
                if (method_exists($item, 'asArray')) {
                    /** @var array<mixed> $nested */
                    $nested = $item->asArray();

                    foreach (self::flatten($nested) as $inner) {
                        $out[] = $inner;
                    }

                    continue;
                }

                $out[] = $item::class;

                continue;
            }

            /** @var int|float|bool|string|null $item */
            $out[] = $item;
        }

        return $out;
    }
}
