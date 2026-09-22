<?php

namespace Tensor\Tests\Parity;

use Generator;
use Throwable;
use Tensor\Tests\Support\Loader;
use PHPUnit\Framework\TestCase;

/**
 * Compares the extension against the pure-PHP rubix/tensor.
 *
 * Advisory, unlike SnapshotTest. The twin is a separate implementation, so only
 * the numbers are compared and only within each operation's stated tolerance;
 * exception wording is not compared, because it drifts between the two.
 *
 * What this catches that a self-snapshot cannot: a migration that moves the
 * extension and the recorded transcript together. The snapshot answers "did
 * behaviour change"; this answers "is the behaviour right".
 *
 * @covers \Tensor\Tests\Parity\Compare
 * @covers \Tensor\Tests\Parity\Transcript
 */
class CrossCheckTest extends TestCase
{
    /**
     * Operations where the two implementations are entitled to disagree beyond
     * a tolerance, with the reason. Eigenvectors and singular vectors are
     * determined only up to sign and, for repeated values, up to rotation
     * within the eigenspace; LAPACK and a hand-written PHP routine have no
     * obligation to make the same choice.
     *
     * @var array<string, string>
     */
    public const DIVERGENT = [
        'matrix.eig' => 'eigenvector sign and ordering are not canonical',
        'matrix.eig.normalized' => 'eigenvector sign and ordering are not canonical',
        'matrix.svd' => 'singular vector sign is not canonical',
        'matrix.lu' => 'LAPACK dgetrf pivots differently from the twin\'s Gaussian elimination,'
            . ' so L, U and P differ while P*A = L*U holds for both',
        'matrix.ref' => 'row echelon form is not unique; the two pivot differently',
        'matrix.rref' => 'reached from ref(), so it inherits the pivoting difference',
        'matrix.symmetric' => 'an exact equality test over a floating-point product: on'
            . ' swissroll-1000x3 the extension\'s cblas_dgemm leaves two entries of A^T A'
            . ' differing from their transpose in the last ulp, which BLAS never promised not'
            . ' to do, while the twin\'s PHP loop happens to be bitwise symmetric. The'
            . ' snapshot still covers this operation exactly, so nothing is lost here.',
    ];

    /**
     * @var array<string, Operands>
     */
    private static array $operands = [];

    /**
     * @var array<string, array{header: array<string, mixed>, operations: array<string, array<string, mixed>>}>
     */
    private static array $transcripts = [];

    private static function operands(string $fixture) : Operands
    {
        return self::$operands[$fixture] ??= Operands::from(Loader::load($fixture)['samples']);
    }

    /**
     * @param string $fixture
     * @return array{header: array<string, mixed>, operations: array<string, array<string, mixed>>}
     */
    private static function transcript(string $fixture) : array
    {
        return self::$transcripts[$fixture] ??= Transcript::read(
            Transcript::path(dirname(__DIR__) . '/fixtures/parity/oracle', $fixture)
        );
    }

    public static function tearDownAfterClass() : void
    {
        self::$operands = [];
        self::$transcripts = [];
    }

    /**
     * @test
     * @dataProvider operationProvider
     * @param string $fixture
     * @param string $key
     */
    public function extensionAgreesWithTheTwin(string $fixture, string $key) : void
    {
        $expected = self::transcript($fixture)['operations'][$key] ?? null;

        $this->assertIsArray($expected);

        if ($expected['status'] === 'unsupported') {
            $this->markTestSkipped("rubix/tensor does not implement {$key}.");
        }

        if (array_key_exists($key, self::DIVERGENT)) {
            $this->markTestSkipped($key . ': ' . self::DIVERGENT[$key]);
        }

        $operation = Operations::all()[$key];
        $actual = $this->execute($operation, $fixture);

        $this->assertSame(
            $expected['status'],
            $actual['status'],
            "{$fixture}/{$key}: the extension and the twin disagree about whether this throws."
        );

        if ($expected['status'] === 'threw') {
            // Only the class is compared; the twin words its messages differently.
            $this->assertSame($expected['class'], $actual['class'], "{$fixture}/{$key} throws a different class.");

            return;
        }

        $differences = Compare::withinRelative(
            $expected['result'],
            $actual['result'],
            $operation->tolerance
        );

        $this->assertSame([], $differences, Compare::describe("{$fixture}/{$key}", $differences));
    }

    /**
     * The gap between the extension and the twin is a reviewable number, not a
     * background fact. If the twin ever stops implementing something, that shows
     * up here as a one-line diff rather than as silently thinner coverage.
     *
     * @test
     */
    public function theUnsupportedSetIsExactlyAsRecorded() : void
    {
        $recorded = json_decode(
            (string) file_get_contents(dirname(__DIR__) . '/fixtures/parity/oracle/unsupported.json'),
            true
        );

        $this->assertIsArray($recorded);

        $actual = [];

        foreach (self::transcript(Loader::parityNames()[0])['operations'] as $key => $record) {
            if ($record['status'] === 'unsupported') {
                $actual[] = $key;
            }
        }

        sort($actual);

        $this->assertSame($recorded, $actual);
    }

    /**
     * @return Generator<array{0: string, 1: string}>
     */
    public function operationProvider() : Generator
    {
        foreach (Loader::parityNames() as $fixture) {
            foreach (Operations::keys() as $key) {
                yield "{$fixture}/{$key}" => [$fixture, $key];
            }
        }
    }

    /**
     * @param Operation $operation
     * @param string $fixture
     * @return array<string, mixed>
     */
    private function execute(Operation $operation, string $fixture) : array
    {
        try {
            return ['status' => 'ok', 'result' => Encode::result(($operation->apply)(self::operands($fixture)))];
        } catch (Throwable $e) {
            return Encode::thrown($e);
        }
    }
}
