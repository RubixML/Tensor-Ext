#!/usr/bin/env php
<?php

/**
 * Compare the serialized representation of a plain PHP array against each layer
 * of the Tensor/Buffer object chain:
 *
 *   array -> Buffer -> TensorBuffer -> Vector / ColumnVector / Matrix
 *
 * For every representation the script reports:
 *   - byte size of serialize() (and ratio vs the plain-array baseline)
 *   - structure: class, own properties with their visibility, __serialize presence
 *   - round-trip fidelity: unserialize -> class survived -> values match
 *
 * It then sweeps array size to separate the fixed wrapper overhead from the
 * per-element cost, and exercises the empty-buffer edge case.
 *
 * Usage:
 *   php scripts/compare-serialization.php
 *
 * The `tensor` extension must be loaded. If it is not, the exact command to
 * load a locally built shared object is printed.
 */
if (!class_exists('\Tensor\Vector')) {
    fwrite(STDERR, "The 'tensor' extension is not loaded.\n");

    $localSo = __DIR__ . '/../ext/modules/tensor.so';

    if (file_exists($localSo)) {
        fwrite(
            STDERR,
            "Try again with:\n" .
            '  php -n -d extension=' . $localSo . ' ' . __FILE__ . "\n"
        );
    }

    exit(1);
}

echo 'PHP ' . phpversion() . ' | ', "\n";
$extName = extension_loaded('tensor') ? 'tensor' : 'tensor_ext';
echo 'extension: ', $extName, "\n\n";

/*
 * The canonical payload: the same four doubles reused at every layer so that
 * only representation overhead varies, never the data.
 */
$elements = [1.5, -2.25, 3.0, 4.75];
$matrixRows = [
    [1.5, -2.25],
    [3.0, 4.75],
];

$maxDelta = 1e-8;

/**
 * The own properties of an object, with their visibility.
 *
 * @param object $subject
 *
 * @return array<int, array{name: string, visibility: string}>
 */
function ownProperties(object $subject) : array
{
    $reflection = new ReflectionObject($subject);

    $properties = [];

    foreach ($reflection->getProperties() as $property) {
        if ($property->isStatic()) {
            continue;
        }

        $visibility = $property->isPrivate()
            ? 'private'
            : ($property->isProtected() ? 'protected' : 'public');

        $properties[] = ['name' => $property->getName(), 'visibility' => $visibility];
    }

    return $properties;
}

function declaresSerialize(object $subject) : bool
{
    return $subject instanceof Tensor\Buffer || method_exists($subject, '__serialize');
}

/**
 * Extract the element values from a tensor or plain array, for round-trip checks.
 *
 * @param object|array<int>|array<int, array<float>> $subject
 *
 * @return array
 */
function elementValues($subject) : array
{
    if (is_array($subject)) {
        return $subject;
    }

    if (method_exists($subject, 'toArray')) {
        return $subject->toArray();
    }

    if (method_exists($subject, 'asArray')) {
        return $subject->asArray();
    }

    throw new LogicException('Unable to read values from ' . get_class($subject));
}

function valuesEqual(array $a, array $b, float $delta) : bool
{
    $a = array_map('floatval', array_values($a));
    $b = array_map('floatval', array_values($b));

    $count = count($a);

    if ($count !== count($b)) {
        return false;
    }

    for ($i = 0; $i < $count; ++$i) {
        if (abs($a[$i] - $b[$i]) > $delta) {
            return false;
        }
    }

    return true;
}

/**
 * @param object|array $subject
 * @param float $maxDelta
 * @return array{class: string, ok: bool}
 */
function roundTrip($subject, float $maxDelta) : array
{
    $originalClass = is_object($subject) ? get_class($subject) : 'array';
    $expected = elementValues($subject);

    $deserialized = unserialize(serialize($subject));

    $newClass = is_object($deserialized) ? get_class($deserialized) : 'array';

    return [
        'class' => $newClass,
        'ok' => $newClass === $originalClass && valuesEqual(elementValues($deserialized), $expected, $maxDelta),
    ];
}

function bytes($payload) : int
{
    return strlen(serialize($payload));
}

echo "=== A. Size, structure and round-trip ===\n\n";

$representations = [
    'array (baseline)' => fn () => $elements,
    'Buffer::fromArray()' => fn () => Tensor\Buffer::fromArray($elements),
    'new TensorBuffer()' => fn () => new Tensor\TensorBuffer(Tensor\Buffer::fromArray($elements)),
    'Vector::fromArray()' => fn () => Tensor\Vector::fromArray($elements),
    'ColumnVector::fromArray()' => fn () => Tensor\ColumnVector::fromArray($elements),
    'Matrix::fromArray() 2x2' => fn () => Tensor\Matrix::fromArray($matrixRows),
];

$arrayBytes = bytes($elements);

foreach ($representations as $label => $build) {
    $object = $build();
    $size = bytes($object);
    $ratio = round($size / $arrayBytes, 2);

    echo str_pad($label, 24) . "  {$size} B  ({$ratio}x)\n";

    if (is_object($object)) {
        $properties = ownProperties($object);
        $propertyList = $properties
            ? implode(', ', array_map(static fn ($p) => $p['visibility'] . ' ' . $p['name'], $properties))
            : '(none)';

        echo '     class:      ', get_class($object), "\n";
        echo "     properties: {$propertyList}\n";
        echo '     __serialize:', declaresSerialize($object) ? 'yes' : 'no', "\n";
    } else {
        echo "     class:      array\n";
        echo "     __serialize: n/a\n";
    }

    $raw = serialize($object);
    echo '     raw:        ', substr($raw, 0, 120), (strlen($raw) > 120 ? '...' : ''), "\n";

    $result = roundTrip($object, $maxDelta);
    echo '     round-trip: ', $result['ok'] ? 'OK' : 'FAILED', " (as {$result['class']})\n\n";
}

echo "=== B. Size vs element count (wrapper overhead vs per-element cost) ===\n\n";

echo "  n            array      Buffer       Vector      Matrix      ratio(V/array)\n";

foreach ([4, 64, 1024, 8192] as $n) {
    $data = array_map(fn (int $i) => ($i % 9) - 4.0, range(0, $n - 1));

    $arraySize = strlen(serialize($data));
    $bufferSize = bytes(Tensor\Buffer::fromArray($data));
    $vectorSize = bytes(new Tensor\Vector($data));
    $matrixSize = $n % 2 === 0 ? bytes(Tensor\Matrix::fromArray(array_chunk($data, $n / 2))) : null;
    $ratio = round($vectorSize / $arraySize, 2);

    $matrixCell = $matrixSize === null ? '      n/a' : (string) $matrixSize;

    printf(
        "  %-11d %-10d %-11d %-11d %-11s %s\n",
        $n,
        $arraySize,
        $bufferSize,
        $vectorSize,
        $matrixCell,
        $ratio . 'x'
    );
}

echo "\n";
echo "  As n grows, the fixed wrapper (class name, property keys, shape ints)\n";
echo "  amortises and the ratio converges toward ~1x (the raw element array).\n\n";

echo "=== C. Empty-buffer edge case ===\n\n";

$empty = [];

foreach ([
    'array' => static fn () => $empty,
    'Buffer' => static function () use ($empty) {
        return Tensor\Buffer::fromArray($empty);
    },
    'TensorBuffer' => static function () use ($empty) {
        return new Tensor\TensorBuffer(Tensor\Buffer::fromArray($empty));
    },
    'Vector' => static function () use ($empty) {
        return new Tensor\Vector($empty);
    },
] as $label => $build) {
    $object = $build();
    $size = bytes($object);
    $result = roundTrip($object, $maxDelta);

    echo '  ', str_pad($label, 14), '  ', $size, ' B    round-trip: ', $result['ok'] ? 'OK' : 'FAILED', "\n";
}

echo "\n";
echo "=== Summary (canonical payload: {$elements[0]}, {$elements[1]}, {$elements[2]}, {$elements[3]}) ===\n\n";

echo '  Array      ', str_pad((string) strlen(serialize($elements)), 8), "B  (baseline)\n";
echo '  Buffer     ', str_pad((string) bytes(Tensor\Buffer::fromArray($elements)), 8), "B\n";
echo '  TensorBuffer ', str_pad((string) bytes(new Tensor\TensorBuffer(Tensor\Buffer::fromArray($elements))), 8), "B\n";
echo '  Vector     ', str_pad((string) bytes(new Tensor\Vector($elements)), 8), "B\n";
echo '  ColumnVector ', str_pad((string) bytes(new Tensor\ColumnVector($elements)), 8), "B\n";
echo '  Matrix 2x2 ', str_pad((string) bytes(Tensor\Matrix::fromArray($matrixRows)), 8), "B\n\n";

echo "  Overhead: a plain array and the raw Buffer carry a little per-element\n";
echo "  overhead. Vector, ColumnVector and Matrix declare __serialize/__unserialize,\n";
echo "  so the element payload is written value-only (a nested array) and rebuilt\n";
echo "  into a TensorBuffer on unserialize - only the object envelope remains.\n";
