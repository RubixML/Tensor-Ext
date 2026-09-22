<?php

namespace Tensor\Tests\Benchmarks;

use RuntimeException;
use Tensor\ColumnVector;
use Tensor\Matrix;
use Tensor\Vector;

/**
 * The seven chained sequences the baseline measures.
 *
 * Chains rather than single operations, because that is what the buffer
 * migration changes. A single operation is dominated by the array-to-buffer
 * conversion at each end; a chain pays that once and keeps the elements in C in
 * between, so the payoff scales with the length of the chain.
 *
 * Defined once here and driven from both tools/workload-probe.php and the
 * phpbench subjects, so what is measured and what is memory-checked cannot
 * drift apart.
 */
final class Workloads
{
    /**
     * @var list<string>
     */
    public const NAMES = [
        'standardise',
        'covariance',
        'gram',
        'leastSquaresGradient',
        'sigmoid',
        'distanceMatrix',
        'pca',
    ];

    private Matrix $x;

    private Matrix $centred;

    private ColumnVector $rowMean;

    private ColumnVector $rowDeviation;

    private Vector $weights;

    private ColumnVector $target;

    /**
     * @param list<list<float>> $samples
     */
    public function __construct(array $samples)
    {
        $this->x = Matrix::quick($samples);

        // Matrix::mean() and ::variance() reduce along rows, so these have one
        // element per row and conform to the *ColumnVector broadcast family.
        $this->rowMean = $this->x->mean();

        $this->rowDeviation = ColumnVector::quick(array_map(
            // A zero-variance row would otherwise turn the chain into a divide by zero.
            static fn (float $value) : float => sqrt($value) ?: 1.0,
            $this->x->variance()->asArray()
        ));

        // Centring for the covariance is per column, so it takes the *Vector family.
        $columnMean = Vector::quick($this->x->transpose()->mean()->asArray());

        $this->centred = $this->x->subtractVector($columnMean);

        $this->weights = Vector::quick(array_fill(0, count($samples[0]), 0.5));
        $this->target = ColumnVector::quick(array_fill(0, count($samples), 1.0));
    }

    /**
     * @param string $name
     * @throws RuntimeException
     */
    public function run(string $name) : mixed
    {
        return match ($name) {
            'standardise' => $this->standardise(),
            'covariance' => $this->covariance(),
            'gram' => $this->gram(),
            'leastSquaresGradient' => $this->leastSquaresGradient(),
            'sigmoid' => $this->sigmoid(),
            'distanceMatrix' => $this->distanceMatrix(),
            'pca' => $this->pca(),
            default => throw new RuntimeException("Unknown workload {$name}."),
        };
    }

    /**
     * Per-row scalar broadcast: one C call and one result array per row today.
     */
    public function standardise() : Matrix
    {
        return $this->x
            ->subtractColumnVector($this->rowMean)
            ->divideColumnVector($this->rowDeviation);
    }

    /**
     * dgemm, plus the pack/unpack around it.
     */
    public function covariance() : Matrix
    {
        return $this->centred->transpose()
            ->matmul($this->centred)
            ->divideScalar((float) max(1, $this->x->m() - 1));
    }

    /**
     * dgemm with an m x m result, and array_column for the transpose.
     */
    public function gram() : Matrix
    {
        return $this->x->matmul($this->x->transpose());
    }

    /**
     * Two dgemv calls, neither of which threads below n = 1024.
     */
    public function leastSquaresGradient() : ColumnVector
    {
        return $this->x->transpose()->dot(
            $this->x->dot($this->weights)->subtract($this->target)
        );
    }

    /**
     * Four chained array_map / hash walks — the largest ratio in the whole set.
     */
    public function sigmoid() : Matrix
    {
        return $this->x->negate()->exp()->addScalar(1.0)->reciprocal();
    }

    /**
     * dgemm plus a broadcast of the squared norms along both axes.
     */
    public function distanceMatrix() : Matrix
    {
        $gram = $this->x->matmul($this->x->transpose());
        $norms = $gram->diagonalAsVector();

        return $gram->multiplyScalar(-2.0)
            ->addColumnVector(ColumnVector::quick($norms->asArray()))
            ->addVector($norms)
            ->clipLower(0.0)
            ->sqrt();
    }

    /**
     * Covariance followed by a symmetric eigendecomposition.
     */
    public function pca() : Matrix
    {
        return $this->covariance()->eig(true)->eigenvectors();
    }
}
