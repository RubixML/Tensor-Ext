<?php

namespace Tensor\Tests\Parity;

use Closure;
use Tensor\ColumnVector;
use Tensor\Matrix;
use Tensor\Vector;

/**
 * The single list of probes that both parity entry points run.
 *
 * tools/parity-dump.php (extension) and tools/oracle-dump.php (pure-PHP twin)
 * both require this file, so the two transcripts cannot measure different
 * things. It is inside the tests directory but is not named *Test.php, so
 * PHPUnit does not collect it — that is deliberate, do not "fix" it.
 *
 * Completeness is enforced mechanically: OperationsTest asserts that every
 * public method of Vector, ColumnVector and Matrix either appears here or is in
 * EXCLUDED with a stated reason.
 */
final class Operations
{
    /**
     * Families that exist in four spellings each: a polymorphic entry point
     * that dispatches on the operand type, plus one explicit method per operand
     * kind. Both are probed — the polymorphic dispatch is behaviour too.
     *
     * @var list<string>
     */
    public const FAMILIES = [
        'add', 'subtract', 'multiply', 'divide', 'pow', 'mod',
        'equal', 'notEqual', 'greater', 'greaterEqual', 'less', 'lessEqual',
    ];

    /**
     * Element-wise unary maths. These are the methods the migration replaces
     * with a single C handler over a buffer; today every one of them is an
     * array_map with a PHP function-call frame per element.
     *
     * @var list<string>
     */
    public const UNARY = [
        'abs', 'acos', 'asin', 'atan', 'ceil', 'cos', 'deg2rad', 'exp', 'expm1',
        'floor', 'log1p', 'negate', 'rad2deg', 'reciprocal', 'sign', 'sin',
        'sqrt', 'square', 'tan',
    ];

    /**
     * Zero-argument reductions and shape accessors.
     *
     * @var list<string>
     */
    public const NULLARY = [
        'sum', 'product', 'min', 'max', 'mean', 'median',
        'l1Norm', 'l2Norm', 'maxNorm',
        'count', 'm', 'n', 'size', 'shape', 'shapeString', 'asArray', 'getIterator',
    ];

    /**
     * Methods deliberately not probed, each with the reason. Anything added to a
     * tensor class must land here or in all(); OperationsTest enforces it.
     *
     * @var array<string, string>
     */
    public const EXCLUDED = [
        'rand' => 'draws from the global PRNG; no stable transcript exists',
        'gaussian' => 'draws from the global PRNG; no stable transcript exists',
        'poisson' => 'draws from the global PRNG; no stable transcript exists',
        'uniform' => 'draws from the global PRNG; no stable transcript exists',
    ];

    /**
     * Individual probes that cannot be recorded, keyed by operation.
     *
     * Unlike EXCLUDED these name one spelling on one subject, because the same
     * method is fine elsewhere: Vector::modMatrix() throws DivisionByZeroError
     * cleanly, while ColumnVector::modMatrix() aborts the process.
     *
     * ColumnVector's twelve *Matrix overrides are the only element-wise family
     * still written as nested Zephir loops rather than routed through a C
     * handler. When an arithmetic exception is raised part-way through one of
     * those loops the memory frame is not unwound, and the next ZEPHIR_OBS_VAR
     * on the same slot trips a kernel assertion:
     *
     *     Tensor\ColumnVector::quick([1.5, 2.5])
     *         ->modMatrix(Tensor\Matrix::quick([[0.5, 2.0], [0.5, 2.0]]));
     *     // Variable 0x... is already observed -> SIGABRT, exit 134
     *
     * It was not specific to mod: divideMatrix by an exact 0.0 aborted the same
     * way. It was a hard crash, so it could not be caught and could not be
     * recorded.
     *
     * Stage 3 replaced all twelve of those loops with tensor_buffer_broadcast_rows(),
     * which unwinds properly, and the operation now raises a catchable
     * DivisionByZeroError. These two probes stay excluded all the same: the
     * snapshot is a photograph of the behaviour before the migration, and what
     * it photographed here was an abort. There is nothing to compare against.
     * ColumnVectorTest covers the fixed behaviour directly instead.
     *
     * @var array<string, string>
     */
    public const EXCLUDED_OPS = [
        'columnVector.mod.matrix' => 'the recorded baseline for this is a process abort; see the docblock',
        'columnVector.modMatrix' => 'the recorded baseline for this is a process abort; see the docblock',
    ];

    /**
     * @return array<string, Operation>
     */
    public static function all() : array
    {
        $ops = [];

        foreach ([...self::rankOne('vector'), ...self::rankOne('columnVector'), ...self::matrix()] as $op) {
            if (array_key_exists($op->key, self::EXCLUDED_OPS)) {
                continue;
            }

            $ops[$op->key] = $op;
        }

        ksort($ops);

        return $ops;
    }

    /**
     * Methods accounted for on a given subject by an EXCLUDED_OPS entry rather
     * than by a probe, so the coverage assertion stays complete.
     *
     * @param string $subject
     * @return list<string>
     */
    public static function excludedMethodsFor(string $subject) : array
    {
        $methods = [];

        foreach (array_keys(self::EXCLUDED_OPS) as $key) {
            $parts = explode('.', $key);

            if (($parts[0] ?? '') === $subject && isset($parts[1])) {
                $methods[] = $parts[1];
            }
        }

        return array_values(array_unique($methods));
    }

    /**
     * @return list<string>
     */
    public static function keys() : array
    {
        return array_keys(self::all());
    }

    /**
     * Vector and ColumnVector share a surface; only the broadcast conformance
     * differs. A Vector matches the column count of the fixture matrix, a
     * ColumnVector matches the row count, so each gets the operand that
     * conforms.
     *
     * @param string $subject
     * @return list<Operation>
     */
    private static function rankOne(string $subject) : array
    {
        $self = static fn (Operands $o) : Vector => $subject === 'columnVector'
            ? $o->columnVector
            : $o->vector;

        $peer = static fn (Operands $o) : Vector => $subject === 'columnVector'
            ? ColumnVector::quick(array_reverse($o->columnVector->asArray()))
            : $o->otherVector;

        $ops = [];

        foreach (self::UNARY as $method) {
            $ops[] = self::op(
                "{$subject}.{$method}",
                $subject,
                static fn (Operands $o) : mixed => self::call($self($o), $method),
                ['elementwise']
            );
        }

        foreach (self::NULLARY as $method) {
            $ops[] = self::op(
                "{$subject}.{$method}",
                $subject,
                static fn (Operands $o) : mixed => self::call($self($o), $method),
                ['reduction']
            );
        }

        foreach (self::FAMILIES as $family) {
            $ops[] = self::op(
                "{$subject}.{$family}.vector",
                $subject,
                static fn (Operands $o) : mixed => self::call($self($o), $family, [$peer($o)]),
                ['elementwise']
            );
            $ops[] = self::op(
                "{$subject}.{$family}.matrix",
                $subject,
                static fn (Operands $o) : mixed => self::call($self($o), $family, [$o->matrix]),
                ['broadcast']
            );
            $ops[] = self::op(
                "{$subject}.{$family}.scalar",
                $subject,
                static fn (Operands $o) : mixed => self::call($self($o), $family, [$o->scalar]),
                ['elementwise']
            );
            $ops[] = self::op(
                "{$subject}.{$family}Vector",
                $subject,
                static fn (Operands $o) : mixed => self::call($self($o), $family . 'Vector', [$peer($o)]),
                ['elementwise']
            );
            $ops[] = self::op(
                "{$subject}.{$family}Matrix",
                $subject,
                static fn (Operands $o) : mixed => self::call($self($o), $family . 'Matrix', [$o->matrix]),
                ['broadcast']
            );
            $ops[] = self::op(
                "{$subject}.{$family}Scalar",
                $subject,
                static fn (Operands $o) : mixed => self::call($self($o), $family . 'Scalar', [$o->scalar]),
                ['elementwise']
            );
        }

        return [
            ...$ops,
            self::op(
                "{$subject}.dot",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->dot($peer($o)),
                ['blas']
            ),
            self::op(
                "{$subject}.inner",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->inner($peer($o)),
                ['blas']
            ),
            self::op(
                "{$subject}.outer",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->outer($peer($o)),
                ['blas']
            ),
            self::op(
                "{$subject}.matmul",
                $subject,
                static fn (Operands $o) : mixed => $subject === 'columnVector'
                    ? $o->columnVector->matmul($o->matrix->matmul($o->transposed))
                    : $o->vector->matmul($o->square),
                ['blas']
            ),
            self::op(
                "{$subject}.transpose",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->transpose()
            ),
            self::op(
                "{$subject}.asColumnMatrix",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->asColumnMatrix()
            ),
            self::op(
                "{$subject}.asRowMatrix",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->asRowMatrix()
            ),
            self::op(
                "{$subject}.log",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->log(),
                ['elementwise']
            ),
            self::op(
                "{$subject}.log.base10",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->log(10.0),
                ['elementwise']
            ),
            self::op(
                "{$subject}.round",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->round(),
                ['elementwise']
            ),
            self::op(
                "{$subject}.round.p2",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->round(2),
                ['elementwise']
            ),
            self::op(
                "{$subject}.pNorm",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->pNorm(3.0),
                ['reduction']
            ),
            self::op(
                "{$subject}.quantile",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->quantile($o->probability),
                ['reduction']
            ),
            self::op(
                "{$subject}.variance",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->variance(),
                ['reduction']
            ),
            self::op(
                "{$subject}.clipLower",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->clipLower(-1.0),
                ['elementwise']
            ),
            self::op(
                "{$subject}.clipUpper",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->clipUpper(1.0),
                ['elementwise']
            ),
            self::op(
                "{$subject}.clip",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->clip(-1.0, 1.0),
                ['elementwise']
            ),
            self::op(
                "{$subject}.reshape",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->reshape(1, $self($o)->size())
            ),
            self::op(
                "{$subject}.convolve",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->convolve($o->kernel, 1),
                ['signal']
            ),
            self::op(
                "{$subject}.map.sqrt",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->map(Operands::squareRoot()),
                ['callable']
            ),
            self::op(
                "{$subject}.reduce.sum",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->reduce(Operands::summation(), 0.0),
                ['callable']
            ),
            self::op(
                "{$subject}.offsetGet",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->offsetGet(0)
            ),
            self::op(
                "{$subject}.offsetExists",
                $subject,
                static fn (Operands $o) : mixed => $self($o)->offsetExists(0)
            ),
            self::op(
                "{$subject}.offsetSet",
                $subject,
                static function (Operands $o) use ($self) : mixed {
                    // Declared void; probed for the exception it raises, not a value.
                    $self($o)->offsetSet(0, 1.0);

                    return null;
                },
                ['throws']
            ),
            self::op(
                "{$subject}.offsetUnset",
                $subject,
                static function (Operands $o) use ($self) : mixed {
                    $self($o)->offsetUnset(0);

                    return null;
                },
                ['throws']
            ),
            self::op(
                "{$subject}.build",
                $subject,
                static fn (Operands $o) : mixed => $subject === 'columnVector'
                    ? ColumnVector::build($o->columnVector->asArray())
                    : Vector::build($o->vector->asArray()),
                ['factory']
            ),
            self::op(
                "{$subject}.quick",
                $subject,
                static fn (Operands $o) : mixed => $subject === 'columnVector'
                    ? ColumnVector::quick($o->columnVector->asArray())
                    : Vector::quick($o->vector->asArray()),
                ['factory']
            ),
            self::op(
                "{$subject}.zeros",
                $subject,
                static fn (Operands $o) : mixed => Vector::zeros(5),
                ['factory']
            ),
            self::op(
                "{$subject}.ones",
                $subject,
                static fn (Operands $o) : mixed => Vector::ones(5),
                ['factory']
            ),
            self::op(
                "{$subject}.fill",
                $subject,
                static fn (Operands $o) : mixed => Vector::fill(1.5, 5),
                ['factory']
            ),
            self::op(
                "{$subject}.linspace",
                $subject,
                static fn (Operands $o) : mixed => Vector::linspace(0.0, 1.0, 10),
                ['factory']
            ),
            self::op(
                "{$subject}.range",
                $subject,
                static fn (Operands $o) : mixed => Vector::range(0.0, 1.0, 0.25),
                ['factory']
            ),
        ];
    }

    /**
     * @return list<Operation>
     */
    private static function matrix() : array
    {
        $ops = [];

        foreach (self::UNARY as $method) {
            $ops[] = self::op(
                "matrix.{$method}",
                'matrix',
                static fn (Operands $o) : mixed => self::call($o->matrix, $method),
                ['elementwise']
            );
        }

        foreach (self::NULLARY as $method) {
            $ops[] = self::op(
                "matrix.{$method}",
                'matrix',
                static fn (Operands $o) : mixed => self::call($o->matrix, $method),
                ['reduction']
            );
        }

        foreach (self::FAMILIES as $family) {
            $ops[] = self::op(
                "matrix.{$family}.matrix",
                'matrix',
                static fn (Operands $o) : mixed => self::call($o->matrix, $family, [$o->other]),
                ['elementwise']
            );
            $ops[] = self::op(
                "matrix.{$family}.vector",
                'matrix',
                static fn (Operands $o) : mixed => self::call($o->matrix, $family, [$o->vector]),
                ['broadcast']
            );
            $ops[] = self::op(
                "matrix.{$family}.columnVector",
                'matrix',
                static fn (Operands $o) : mixed => self::call($o->matrix, $family, [$o->columnVector]),
                ['broadcast']
            );
            $ops[] = self::op(
                "matrix.{$family}.scalar",
                'matrix',
                static fn (Operands $o) : mixed => self::call($o->matrix, $family, [$o->scalar]),
                ['elementwise']
            );
            $ops[] = self::op(
                "matrix.{$family}Matrix",
                'matrix',
                static fn (Operands $o) : mixed => self::call($o->matrix, $family . 'Matrix', [$o->other]),
                ['elementwise']
            );
            $ops[] = self::op(
                "matrix.{$family}Vector",
                'matrix',
                static fn (Operands $o) : mixed => self::call($o->matrix, $family . 'Vector', [$o->vector]),
                ['broadcast']
            );
            $ops[] = self::op(
                "matrix.{$family}ColumnVector",
                'matrix',
                static fn (Operands $o) : mixed => self::call($o->matrix, $family . 'ColumnVector', [$o->columnVector]),
                ['broadcast']
            );
            $ops[] = self::op(
                "matrix.{$family}Scalar",
                'matrix',
                static fn (Operands $o) : mixed => self::call($o->matrix, $family . 'Scalar', [$o->scalar]),
                ['elementwise']
            );
        }

        return [...$ops, ...self::matrixStructural(), ...self::matrixLinearAlgebra()];
    }

    /**
     * @return list<Operation>
     */
    private static function matrixStructural() : array
    {
        return [
            self::op('matrix.transpose', 'matrix', static fn (Operands $o) : mixed => $o->matrix->transpose(), ['columnwise']),
            self::op('matrix.flatten', 'matrix', static fn (Operands $o) : mixed => $o->matrix->flatten(), ['shape']),
            self::op('matrix.augmentAbove', 'matrix', static fn (Operands $o) : mixed => $o->matrix->augmentAbove($o->other), ['shape']),
            self::op('matrix.augmentBelow', 'matrix', static fn (Operands $o) : mixed => $o->matrix->augmentBelow($o->other), ['shape']),
            self::op('matrix.augmentLeft', 'matrix', static fn (Operands $o) : mixed => $o->matrix->augmentLeft($o->other), ['shape']),
            self::op('matrix.augmentRight', 'matrix', static fn (Operands $o) : mixed => $o->matrix->augmentRight($o->other), ['shape']),
            self::op('matrix.repeat', 'matrix', static fn (Operands $o) : mixed => $o->square->repeat(2, 2), ['shape']),
            self::op('matrix.rowAsVector', 'matrix', static fn (Operands $o) : mixed => $o->matrix->rowAsVector(0), ['copy']),
            self::op('matrix.columnAsVector', 'matrix', static fn (Operands $o) : mixed => $o->matrix->columnAsVector(0), ['columnwise']),
            self::op('matrix.diagonalAsVector', 'matrix', static fn (Operands $o) : mixed => $o->square->diagonalAsVector(), ['copy']),
            self::op('matrix.asVectors', 'matrix', static fn (Operands $o) : mixed => $o->matrix->asVectors(), ['copy']),
            self::op('matrix.asColumnVectors', 'matrix', static fn (Operands $o) : mixed => $o->matrix->asColumnVectors(), ['copy']),
            self::op('matrix.symmetric', 'matrix', static fn (Operands $o) : mixed => $o->square->symmetric()),
            self::op('matrix.isSquare', 'matrix', static fn (Operands $o) : mixed => $o->matrix->isSquare()),
            self::op('matrix.log', 'matrix', static fn (Operands $o) : mixed => $o->matrix->log(), ['elementwise']),
            self::op('matrix.log.base10', 'matrix', static fn (Operands $o) : mixed => $o->matrix->log(10.0), ['elementwise']),
            self::op('matrix.round', 'matrix', static fn (Operands $o) : mixed => $o->matrix->round(), ['elementwise']),
            self::op('matrix.round.p2', 'matrix', static fn (Operands $o) : mixed => $o->matrix->round(2), ['elementwise']),
            self::op('matrix.clip', 'matrix', static fn (Operands $o) : mixed => $o->matrix->clip(-1.0, 1.0), ['elementwise']),
            self::op('matrix.clipLower', 'matrix', static fn (Operands $o) : mixed => $o->matrix->clipLower(-1.0), ['elementwise']),
            self::op('matrix.clipUpper', 'matrix', static fn (Operands $o) : mixed => $o->matrix->clipUpper(1.0), ['elementwise']),
            self::op('matrix.infinityNorm', 'matrix', static fn (Operands $o) : mixed => $o->matrix->infinityNorm(), ['reduction']),
            self::op('matrix.quantile', 'matrix', static fn (Operands $o) : mixed => $o->matrix->quantile($o->probability), ['reduction']),
            self::op('matrix.variance', 'matrix', static fn (Operands $o) : mixed => $o->matrix->variance(), ['reduction']),
            self::op('matrix.covariance', 'matrix', static fn (Operands $o) : mixed => $o->square->covariance(), ['reduction']),
            self::op('matrix.convolve', 'matrix', static fn (Operands $o) : mixed => $o->matrix->convolve($o->kernelMatrix, 1), ['signal']),
            self::op('matrix.map.sqrt', 'matrix', static fn (Operands $o) : mixed => $o->matrix->map(Operands::squareRoot()), ['callable']),
            self::op('matrix.reduce.sum', 'matrix', static fn (Operands $o) : mixed => $o->matrix->reduce(Operands::summation(), 0.0), ['callable']),
            self::op('matrix.offsetGet', 'matrix', static fn (Operands $o) : mixed => $o->matrix->offsetGet(0)),
            self::op('matrix.offsetExists', 'matrix', static fn (Operands $o) : mixed => $o->matrix->offsetExists(0)),
            self::op('matrix.offsetSet', 'matrix', static function (Operands $o) : mixed {
                $o->matrix->offsetSet(0, [1.0]);

                return null;
            }, ['throws']),
            self::op('matrix.offsetUnset', 'matrix', static function (Operands $o) : mixed {
                $o->matrix->offsetUnset(0);

                return null;
            }, ['throws']),
            self::op('matrix.build', 'matrix', static fn (Operands $o) : mixed => Matrix::build($o->matrix->asArray()), ['factory']),
            self::op('matrix.quick', 'matrix', static fn (Operands $o) : mixed => Matrix::quick($o->matrix->asArray()), ['factory']),
            self::op('matrix.zeros', 'matrix', static fn (Operands $o) : mixed => Matrix::zeros(3, 3), ['factory']),
            self::op('matrix.ones', 'matrix', static fn (Operands $o) : mixed => Matrix::ones(3, 3), ['factory']),
            self::op('matrix.fill', 'matrix', static fn (Operands $o) : mixed => Matrix::fill(1.5, 3, 3), ['factory']),
            self::op('matrix.identity', 'matrix', static fn (Operands $o) : mixed => Matrix::identity(4), ['factory']),
            self::op('matrix.diagonal', 'matrix', static fn (Operands $o) : mixed => Matrix::diagonal([1.0, 2.0, 3.0]), ['factory']),
        ];
    }

    /**
     * BLAS and LAPACK. These run on `square`, which is symmetric positive
     * semi-definite by construction, so the decompositions have something
     * legitimate to work on. Their tolerance is looser on the cross-check:
     * eigenvector sign and ordering are not canonical between implementations.
     *
     * @return list<Operation>
     */
    private static function matrixLinearAlgebra() : array
    {
        return [
            self::op('matrix.matmul', 'matrix', static fn (Operands $o) : mixed => $o->matrix->matmul($o->transposed), ['blas']),
            self::op('matrix.dot', 'matrix', static fn (Operands $o) : mixed => $o->matrix->dot($o->vector), ['blas']),
            self::op('matrix.trace', 'matrix', static fn (Operands $o) : mixed => $o->square->trace(), ['reduction']),
            self::op('matrix.det', 'matrix', static fn (Operands $o) : mixed => $o->square->det(), ['lapack'], 1e-7),
            self::op('matrix.rank', 'matrix', static fn (Operands $o) : mixed => $o->square->rank(), ['lapack']),
            self::op('matrix.fullRank', 'matrix', static fn (Operands $o) : mixed => $o->square->fullRank(), ['lapack']),
            self::op('matrix.inverse', 'matrix', static fn (Operands $o) : mixed => $o->square->inverse(), ['lapack'], 1e-7),
            self::op('matrix.pseudoinverse', 'matrix', static fn (Operands $o) : mixed => $o->matrix->pseudoinverse(), ['lapack'], 1e-7),
            self::op('matrix.ref', 'matrix', static fn (Operands $o) : mixed => $o->square->ref(), ['lapack'], 1e-7),
            self::op('matrix.rref', 'matrix', static fn (Operands $o) : mixed => $o->square->rref(), ['lapack'], 1e-7),
            self::op('matrix.cholesky', 'matrix', static fn (Operands $o) : mixed => $o->square->cholesky(), ['lapack'], 1e-7),
            self::op('matrix.lu', 'matrix', static fn (Operands $o) : mixed => $o->square->lu(), ['lapack'], 1e-7),
            self::op('matrix.eig', 'matrix', static fn (Operands $o) : mixed => $o->square->eig(false), ['lapack', 'signAmbiguous'], 1e-6),
            self::op('matrix.eig.normalized', 'matrix', static fn (Operands $o) : mixed => $o->square->eig(true), ['lapack', 'signAmbiguous'], 1e-6),
            self::op('matrix.svd', 'matrix', static fn (Operands $o) : mixed => $o->square->svd(), ['lapack', 'signAmbiguous'], 1e-6),
        ];
    }

    /**
     * @param Closure(Operands): mixed $apply
     * @param list<string> $tags
     * @param string $key
     * @param string $subject
     * @param float $tolerance
     */
    private static function op(
        string $key,
        string $subject,
        Closure $apply,
        array $tags = [],
        float $tolerance = 1e-9
    ) : Operation {
        return new Operation($key, $subject, $apply, $tolerance, true, $tags);
    }

    /**
     * Dynamic dispatch by method name, kept in one place so the family loops
     * stay readable and static analysis has a single thing to understand.
     *
     * @param list<mixed> $args
     * @param object $target
     * @param string $method
     */
    private static function call(object $target, string $method, array $args = []) : mixed
    {
        /** @var callable $callable */
        $callable = [$target, $method];

        return $callable(...$args);
    }
}
