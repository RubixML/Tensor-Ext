<?php

namespace Tensor\Tests;

use Generator;
use Tensor\Tests\Support\Loader;
use PHPUnit\Framework\TestCase;

/**
 * @covers \Tensor\Tests\Support\Loader
 */
class FixtureLoaderTest extends TestCase
{
    /**
     * @test
     */
    public function namesAreComplete() : void
    {
        $this->assertCount(7, Loader::names());
    }

    /**
     * @test
     */
    public function parityNamesAreASubsetOfNames() : void
    {
        $this->assertSame([], array_diff(Loader::parityNames(), Loader::names()));
    }

    /**
     * @test
     * @dataProvider fixtureProvider
     * @param string $name
     */
    public function fixtureLoads(string $name) : void
    {
        $fixture = Loader::load($name);

        $this->assertNotEmpty($fixture['samples']);
        $this->assertIsArray($fixture['samples'][0]);
        $this->assertNotEmpty($fixture['samples'][0]);
    }

    /**
     * Rectangularity, numeric type and finiteness are enforced by the loader,
     * which throws rather than returning a half-valid structure.
     *
     * @test
     * @dataProvider fixtureProvider
     * @param string $name
     */
    public function fixtureIsRectangularAndFinite(string $name) : void
    {
        $samples = Loader::load($name)['samples'];
        $width = count($samples[0]);

        foreach ($samples as $row) {
            $this->assertCount($width, $row);

            foreach ($row as $value) {
                $this->assertTrue(is_finite($value));
            }
        }
    }

    /**
     * The manifest is what makes "re-running the harness produces identical
     * fixture bytes" checkable. If a regeneration drifts, this is what says so.
     *
     * @test
     * @dataProvider fixtureProvider
     * @param string $name
     */
    public function fixtureMatchesTheManifest(string $name) : void
    {
        $entry = Loader::manifest()[$name];
        $samples = Loader::load($name)['samples'];

        $this->assertCount($entry['rows'], $samples);
        $this->assertCount($entry['columns'], $samples[0]);
        $this->assertSame($entry['sha256'], hash_file('sha256', Loader::path($name)));
    }

    /**
     * @return Generator<array{0: string}>
     */
    public function fixtureProvider() : Generator
    {
        foreach (Loader::names() as $name) {
            yield $name => [$name];
        }
    }
}
