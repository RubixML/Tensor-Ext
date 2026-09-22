<?php

namespace Tensor\Tests\Parity;

use Tensor\ColumnVector;
use Tensor\Matrix;
use Tensor\Vector;

/**
 * The inputs every parity operation runs against, derived deterministically
 * from one fixture.
 *
 * Both the extension-side dump and the pure-PHP cross-check build these from
 * the same committed JSON, so the two sides cannot drift apart through their
 * inputs. Nothing here draws from a PRNG or a clock.
 *
 * Shapes are chosen so that every broadcast family has a conforming operand:
 * `$vector` matches the column count, `$columnVector` matches the row count,
 * and `$square` is n x n and symmetric positive semi-definite so the
 * decompositions have something legitimate to chew on.
 */
final class Operands
{
    public Matrix $matrix;

    public Matrix $other;

    public Matrix $transposed;

    public Matrix $square;

    public Vector $vector;

    public Vector $otherVector;

    public ColumnVector $columnVector;

    public Vector $kernel;

    public Matrix $kernelMatrix;

    public float $scalar = 2.5;

    public int $integer = 3;

    public float $probability = 0.75;

    /**
     * @param list<list<float>> $samples
     */
    public static function from(array $samples) : self
    {
        return new self($samples);
    }

    /**
     * A small, fixed callback for map(). An arbitrary user callable cannot be
     * buffer-native, so map() and reduce() are the two operations the migration
     * deliberately leaves array-backed; their transcripts pin that behaviour.
     */
    public static function squareRoot() : callable
    {
        return 'sqrt';
    }

    public static function summation() : callable
    {
        return static fn (float $carry, float $value) : float => $carry + $value;
    }

    /**
     * @param list<list<float>> $samples
     */
    private function __construct(array $samples)
    {
        $this->matrix = Matrix::quick($samples);
        $this->other = Matrix::quick(array_reverse($samples));
        $this->transposed = $this->matrix->transpose();
        $this->square = $this->transposed->matmul($this->matrix);

        $this->vector = Vector::quick($samples[0]);
        $this->otherVector = Vector::quick(array_reverse($samples[0]));
        $this->columnVector = ColumnVector::quick(array_column($samples, 0));

        // Fixed convolution kernels. Not derived from the fixture: a kernel has
        // to be small and shaped independently of the signal it runs over.
        $this->kernel = Vector::quick([0.5, -0.25, 0.25]);
        $this->kernelMatrix = Matrix::quick([
            [1.0, 0.0, -1.0],
            [1.0, 0.0, -1.0],
            [1.0, 0.0, -1.0],
        ]);
    }
}
