<?php

namespace Tensor\Tests;

use Tensor\Matrix;
use Tensor\Vector;
use Tensor\ColumnVector;
use Tensor\Reductions\Ref;
use Tensor\Reductions\Rref;
use Tensor\Decompositions\Lu;
use Tensor\Decompositions\Eigen;
use Tensor\Decompositions\Svd;
use Tensor\Decompositions\Cholesky;
use SebastianBergmann\Comparator\Comparator;
use SebastianBergmann\Comparator\ComparisonFailure;

/**
 * @internal
 *
 * Compares two tensor objects for equality by their numerical content.
 *
 * Tensors store their elements in an opaque internal buffer that has no
 * reflection-visible properties, so PHPUnit's default object comparator treats
 * any two same-shape tensors as equal regardless of their contents. This
 * comparator reduces each operand to its plain array of values before
 * delegating back to the comparator factory, so the delta, canonicalize and
 * ignoreCase flags keep their usual meaning.
 */
class TensorComparator extends Comparator
{
    /**
     * The tensor classes whose content lives in an opaque internal buffer.
     *
     * @var list<class-string>
     */
    private const TENSOR_CLASSES = [
        Matrix::class,
        Vector::class,
        ColumnVector::class,
        Ref::class,
        Rref::class,
        Lu::class,
        Eigen::class,
        Svd::class,
        Cholesky::class,
    ];

    /**
     * @param mixed $expected
     * @param mixed $actual
     */
    public function accepts($expected, $actual) : bool
    {
        return is_object($expected)
            && is_object($actual)
            && get_class($expected) === get_class($actual)
            && in_array(get_class($expected), self::TENSOR_CLASSES, true);
    }

    /**
     * @param mixed $expected
     * @param mixed $actual
     * @param float $delta
     * @param bool $canonicalize
     * @param bool $ignoreCase
     */
    public function assertEquals($expected, $actual, $delta = 0.0, $canonicalize = false, $ignoreCase = false) : void
    {
        $expectedValues = $this->toArray($expected);
        $actualValues = $this->toArray($actual);

        try {
            $this->factory->getComparatorFor($expectedValues, $actualValues)
                ->assertEquals($expectedValues, $actualValues, $delta, $canonicalize, $ignoreCase);
        } catch (ComparisonFailure $failure) {
            throw new ComparisonFailure(
                $expected,
                $actual,
                substr_replace($failure->getExpectedAsString(), get_class($expected) . ' Object', 0, 5),
                substr_replace($failure->getActualAsString(), get_class($actual) . ' Object', 0, 5),
                false,
                'Failed asserting that two objects are equal.'
            );
        }
    }

    /**
     * Reduce a tensor object to the plain values that it stores.
     *
     * Nested tensors are left in place so that the factory resolves them back
     * to this comparator and applies the delta to their elements.
     *
     * @param object $object
     * @return array<mixed>
     */
    private function toArray($object) : array
    {
        if ($object instanceof Matrix) {
            return $object->asArray();
        }

        if ($object instanceof Vector) {
            return $object->asArray();
        }

        if ($object instanceof Ref) {
            return [$object->a(), $object->swaps()];
        }

        if ($object instanceof Rref) {
            return [$object->a()];
        }

        if ($object instanceof Lu) {
            return [$object->l(), $object->u(), $object->p()];
        }

        if ($object instanceof Eigen) {
            return [$object->eigenvalues(), $object->eigenvectors()];
        }

        if ($object instanceof Svd) {
            return [$object->u(), $object->singularValues(), $object->vT()];
        }

        if ($object instanceof Cholesky) {
            return [$object->l()];
        }

        throw new \RuntimeException('Unsupported tensor class ' . get_class($object) . '.');
    }
}
