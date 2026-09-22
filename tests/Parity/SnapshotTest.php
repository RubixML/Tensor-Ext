<?php

namespace Tensor\Tests\Parity;

use Generator;
use Throwable;
use Tensor\Tests\Support\Loader;
use PHPUnit\Framework\TestCase;

/**
 * The regression gate for the buffer migration.
 *
 * Every operation, on every parity fixture, must reproduce the transcript
 * recorded from the pre-migration build exactly — same bits, same shapes, same
 * element types, same exception messages. Floats are compared as their raw
 * IEEE-754 bytes, so "close enough" does not enter into it.
 *
 * A failure here is the whole point: it names the operation whose behaviour
 * moved. Re-recording the transcript to make it pass defeats the exercise, and
 * tools/parity-dump.php refuses to do so without an explicit override.
 *
 * @covers \Tensor\Tests\Parity\Transcript
 * @covers \Tensor\Tests\Parity\Compare
 * @covers \Tensor\Tests\Parity\Encode
 * @covers \Tensor\Tests\Parity\Operands
 */
class SnapshotTest extends TestCase
{
    /**
     * Operations whose result moved because a reduction changed association.
     *
     * Moving the inner product onto cblas_ddot replaces a sequential
     * accumulator with a blocked, vectorised one. Floating-point addition is
     * not associative, so the last couple of ulps move. Both answers are
     * correct; neither is more correct.
     *
     * This is a real change to returned values, so unlike the int-to-double
     * coercion it is not covered by a blanket rule. The set is enumerated, and
     * the tolerance is two orders of magnitude tighter than the drift actually
     * observed (about 5e-15 relative at worst), so a genuine regression in one
     * of these still fails.
     *
     * @var array<string, string>
     */
    public const REASSOCIATED = [
        'vector.dot' => 'cblas_ddot accumulates in blocks; the replaced loop was sequential',
        'vector.inner' => 'an alias of dot()',
        'columnVector.dot' => 'cblas_ddot accumulates in blocks; the replaced loop was sequential',
        'columnVector.inner' => 'an alias of dot()',
    ];

    /**
     * Relative tolerance applied to REASSOCIATED operations, and to those only.
     */
    protected const REASSOCIATED_TOLERANCE = 1.0e-12;

    /**
     * Operands are expensive to build — one of them is a matmul — and every
     * operation on a fixture needs the same ones.
     *
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
            Transcript::path(dirname(__DIR__) . '/fixtures/parity/snapshot', $fixture)
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
    public function operationReproducesTheSnapshot(string $fixture, string $key) : void
    {
        $expected = self::transcript($fixture)['operations'][$key] ?? null;

        $this->assertIsArray($expected, "{$fixture}/{$key} is absent from the transcript.");

        $actual = $this->execute($key, $fixture);

        if (array_key_exists($key, self::REASSOCIATED)) {
            $this->assertSame($expected['status'], $actual['status'], "{$fixture}/{$key} changed status.");

            $differences = $expected['status'] === 'ok'
                ? Compare::withinRelative($expected['result'], $actual['result'], self::REASSOCIATED_TOLERANCE)
                : [];

            $this->assertSame([], $differences, Compare::describe("{$fixture}/{$key}", $differences));

            return;
        }

        [$differences] = Compare::exactAllowingIntToDouble($expected, $actual);

        $this->assertSame([], $differences, Compare::describe("{$fixture}/{$key}", $differences));
    }

    /**
     * Every enumerated exception carries a reason, and names a real operation.
     *
     * @test
     */
    public function reassociationsAreJustified() : void
    {
        $keys = Operations::keys();

        foreach (self::REASSOCIATED as $key => $reason) {
            $this->assertContains($key, $keys, "{$key} is not an operation.");
            $this->assertNotSame('', trim($reason), "{$key} is excused without a reason.");
        }
    }

    /**
     * Pins which operations need the int-to-double allowance.
     *
     * Without this, the allowance would be a licence rather than a record: a
     * later change could start coercing somewhere new and nothing would say so.
     * The committed list is the reviewable artefact, and it shrinks to nothing
     * only if the migration is abandoned.
     *
     * @test
     * @dataProvider fixtureProvider
     * @param string $fixture
     */
    public function theCoercedSetIsExactlyAsRecorded(string $fixture) : void
    {
        $all = json_decode(
            (string) file_get_contents(dirname(__DIR__) . '/fixtures/parity/coerced.json'),
            true
        );

        $this->assertIsArray($all);
        $this->assertArrayHasKey($fixture, $all);

        // Per fixture, not global: vector.modVector throws on blob-100x4, where
        // an element truncates to integer zero, so it never reaches a result
        // whose type could coerce.
        $recorded = $all[$fixture];

        $coerced = [];

        foreach (Operations::keys() as $key) {
            $expected = self::transcript($fixture)['operations'][$key] ?? null;

            if (!is_array($expected)) {
                continue;
            }

            [, $used] = Compare::exactAllowingIntToDouble($expected, $this->execute($key, $fixture));

            if ($used) {
                $coerced[] = $key;
            }
        }

        sort($coerced);

        $this->assertSame($recorded, $coerced, 'The set of operations coercing int to double has changed.');
    }

    /**
     * @test
     * @dataProvider fixtureProvider
     * @param string $fixture
     */
    public function transcriptCoversEveryOperation(string $fixture) : void
    {
        $this->assertSame(
            Operations::keys(),
            array_keys(self::transcript($fixture)['operations'])
        );
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
     * @return Generator<array{0: string}>
     */
    public function fixtureProvider() : Generator
    {
        foreach (Loader::parityNames() as $fixture) {
            yield $fixture => [$fixture];
        }
    }

    /**
     * @param string $key
     * @param string $fixture
     * @return array<string, mixed>
     */
    private function execute(string $key, string $fixture) : array
    {
        $operation = Operations::all()[$key];

        try {
            return ['status' => 'ok', 'result' => Encode::result(($operation->apply)(self::operands($fixture)))];
        } catch (Throwable $e) {
            return Encode::thrown($e);
        }
    }
}
