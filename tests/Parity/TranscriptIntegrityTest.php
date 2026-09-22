<?php

namespace Tensor\Tests\Parity;

use Generator;
use RuntimeException;
use Tensor\Tests\Support\Loader;
use PHPUnit\Framework\TestCase;

/**
 * Keeps the recorded transcript from drifting.
 *
 * Nothing in this migration may be committed, so git cannot act as the guard.
 * Four independent things have to agree instead: the commit named in each
 * header, the sha256 recorded in SNAPSHOT.lock, the toolchain the transcript
 * was produced on, and the refusal built into tools/parity-dump.php. Any one of
 * them failing means the snapshot is no longer a photograph of the baseline,
 * and SnapshotTest's verdict is worthless.
 *
 * The environment is checked before any values are, so running on a different
 * PHP or a different BLAS reports that plainly instead of drowning the reader
 * in a diff of several hundred thousand numbers.
 *
 * @covers \Tensor\Tests\Parity\Transcript
 */
class TranscriptIntegrityTest extends TestCase
{
    /**
     * The commit this baseline photographs. A literal, on purpose: a transcript
     * re-recorded against a later build carries a different commit or a
     * different ext_sha256, and overriding that has to mean editing a test.
     */
    protected const SOURCE_COMMIT = 'b197cbe1635f321a0a4bb1a24c6667849fb5b1cf';

    /**
     * Recorded from a build produced by plain phpize/configure/make over the
     * committed ext/. Once composer compile has run, this string changes, which
     * is exactly how a contaminated baseline announces itself.
     */
    protected const ZEPHIR = 'none (built from the committed ext/)';

    protected const DIRECTORY = 'snapshot';

    /**
     * Reports, rather than fails, when the transcript was recorded elsewhere.
     *
     * The snapshot compares raw IEEE-754 bytes, and libm and BLAS are entitled
     * to differ between builds, so a transcript recorded on one toolchain is
     * not automatically a gate on another. It happens to hold across the whole
     * supported range -- 8.1 through 8.5 reproduce the 8.3 recording exactly --
     * but that is a measured fact, not a guarantee.
     *
     * So this skips with the diagnostic instead of failing. The real gate is
     * SnapshotTest, which does run on every version and does fail loudly; this
     * exists so that a reader looking at such a failure on a different
     * toolchain knows to suspect the toolchain first.
     *
     * @test
     * @dataProvider fixtureProvider
     * @param string $fixture
     */
    public function transcriptWasRecordedOnThisToolchain(string $fixture) : void
    {
        $recorded = $this->header($fixture)['env'];
        $current = Transcript::environment();

        if ($recorded !== $current) {
            $this->markTestSkipped(sprintf(
                '%s was recorded on %s; this is %s. SnapshotTest still runs, and a failure there'
                . ' may be toolchain drift rather than a behaviour change.',
                $fixture,
                (string) json_encode($recorded),
                (string) json_encode($current)
            ));
        }

        $this->assertSame($current, $recorded);
    }

    /**
     * @test
     * @dataProvider fixtureProvider
     * @param string $fixture
     */
    public function transcriptNamesTheBaselineCommit(string $fixture) : void
    {
        $this->assertSame(self::SOURCE_COMMIT, $this->header($fixture)['source_commit']);
    }

    /**
     * @test
     * @dataProvider fixtureProvider
     * @param string $fixture
     */
    public function transcriptPredatesTheCompilerSwitch(string $fixture) : void
    {
        $this->assertSame(self::ZEPHIR, $this->header($fixture)['zephir']);
    }

    /**
     * dgemm's reduction order, and so its last ulps, varies with thread count.
     *
     * @test
     * @dataProvider fixtureProvider
     * @param string $fixture
     */
    public function transcriptWasRecordedSingleThreaded(string $fixture) : void
    {
        $this->assertSame(1, $this->header($fixture)['threads']);
    }

    /**
     * @test
     * @dataProvider fixtureProvider
     * @param string $fixture
     */
    public function transcriptRecordsEveryOperation(string $fixture) : void
    {
        $this->assertSame(count(Operations::all()), $this->header($fixture)['operations']);
    }

    /**
     * Any departure from the pinned commit has to be stated in the transcript
     * itself, not left to a commit message the reader does not have.
     *
     * @test
     * @dataProvider fixtureProvider
     * @param string $fixture
     */
    public function deviationsFromTheBaselineAreDeclared(string $fixture) : void
    {
        $patches = $this->header($fixture)['patches'];

        $this->assertIsArray($patches);

        foreach ($patches as $patch) {
            $this->assertIsString($patch);
            $this->assertNotSame('', trim($patch));
        }
    }

    /**
     * @test
     */
    public function everyTranscriptIsLocked() : void
    {
        $lock = $this->lock();
        $directory = $this->directory();

        $files = glob($directory . '/*.json');

        $this->assertIsArray($files);
        $this->assertNotEmpty($files);

        foreach ($files as $file) {
            $name = self::DIRECTORY . '/' . basename($file);

            $this->assertArrayHasKey($name, $lock, "{$name} is not in SNAPSHOT.lock.");
            $this->assertSame(
                $lock[$name],
                'sha256:' . hash_file('sha256', $file),
                "{$name} does not match SNAPSHOT.lock. Either it was edited, or it was re-recorded"
                . ' without updating the lock — both of which need a deliberate decision.'
            );
        }

        $this->assertCount(count($files), $lock, 'SNAPSHOT.lock names a transcript that is not present.');
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
     * @param string $fixture
     * @return array<string, mixed>
     */
    private function header(string $fixture) : array
    {
        return Transcript::read(Transcript::path($this->directory(), $fixture))['header'];
    }

    private function directory() : string
    {
        return dirname(__DIR__) . '/fixtures/parity/' . self::DIRECTORY;
    }

    /**
     * @throws RuntimeException
     * @return array<string, string>
     */
    private function lock() : array
    {
        $raw = file_get_contents(dirname(__DIR__) . '/fixtures/parity/SNAPSHOT.lock');

        if ($raw === false) {
            throw new RuntimeException('SNAPSHOT.lock is missing.');
        }

        $decoded = json_decode($raw, true);

        if (!is_array($decoded)) {
            throw new RuntimeException('SNAPSHOT.lock is malformed.');
        }

        /** @var array<string, string> $decoded */
        return $decoded;
    }
}
