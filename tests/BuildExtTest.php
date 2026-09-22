<?php

namespace Tensor\Tests;

use RuntimeException;
use PHPUnit\Framework\TestCase;

/**
 * Exercises build-ext, the script composer compile runs between `zephir generate`
 * and `zephir compile`.
 *
 * It patches three things into the generated build files that Zephir does not
 * emit itself: OpenBLAS detection for Windows, the execinfo/musl handling for
 * Linux, and a build directory for every extra source tree. All three are keyed
 * off names derived from config.json's extension-name, and all three silently
 * stopped matching when this project renamed itself from `tensor` to
 * `tensor_ext` — Zephir began emitting PHP_TENSOR_EXT and TENSOR_EXT_SHARED_LIBADD
 * while build-ext kept looking for PHP_TENSOR and TENSOR_SHARED_LIBADD.
 *
 * Nobody noticed because CI builds through PIE from the committed ext/ and never
 * regenerates. The tests below run the script for real, against the generated
 * files, in a sandbox.
 *
 * @coversNothing
 */
class BuildExtTest extends TestCase
{
    protected string $sandbox = '';

    protected function setUp() : void
    {
        $sandbox = tempnam(sys_get_temp_dir(), 'buildext');

        if ($sandbox === false) {
            throw new RuntimeException('Cannot create a sandbox directory.');
        }

        unlink($sandbox);
        mkdir($sandbox . '/ext', 0o755, true);

        foreach (['build-ext', 'config.json'] as $file) {
            copy($this->root() . '/' . $file, $sandbox . '/' . $file);
        }

        foreach (['config.m4', 'config.w32'] as $file) {
            copy($this->root() . '/ext/' . $file, $sandbox . '/ext/' . $file);
        }

        $this->sandbox = $sandbox;
    }

    protected function tearDown() : void
    {
        if ($this->sandbox === '') {
            return;
        }

        foreach (['ext/config.m4', 'ext/config.w32', 'build-ext', 'config.json'] as $file) {
            if (is_file($this->sandbox . '/' . $file)) {
                unlink($this->sandbox . '/' . $file);
            }
        }

        rmdir($this->sandbox . '/ext');
        rmdir($this->sandbox);
    }

    /**
     * The regression that matters: before the anchors were derived from
     * config.json this exited 1 on the very first patch, taking composer compile
     * down with it.
     *
     * @test
     */
    public function buildExtSucceedsAgainstTheGeneratedConfig() : void
    {
        [$status, $output] = $this->runBuildExt();

        $this->assertSame(0, $status, "build-ext failed:\n" . $output);
    }

    /**
     * composer compile runs it on every build, so it has to be safe to re-run.
     *
     * @test
     */
    public function buildExtIsIdempotent() : void
    {
        $this->runBuildExt();

        $first = $this->artefacts();

        [$status, $output] = $this->runBuildExt();

        $this->assertSame(0, $status, "second run failed:\n" . $output);
        $this->assertSame($first, $this->artefacts(), 'build-ext is not idempotent.');
    }

    /**
     * @test
     */
    public function windowsOpenBlasPatchIsApplied() : void
    {
        $this->runBuildExt();

        $this->assertStringContainsString('CHECK_LIB("libopenblas.lib"', $this->read('ext/config.w32'));
    }

    /**
     * @test
     */
    public function execinfoPatchIsApplied() : void
    {
        $this->runBuildExt();

        $this->assertStringContainsString('backtrace_symbols', $this->read('ext/config.m4'));
    }

    /**
     * Zephir computes the build directories from the .zep.c files it generates
     * and never from extra-sources, so without this patch there is no object
     * directory for ext/include.
     *
     * @test
     */
    public function everyExtraSourceDirectoryGetsABuildDirectory() : void
    {
        $this->runBuildExt();

        $configM4 = $this->read('ext/config.m4');

        foreach ($this->extraSourceDirectories() as $directory) {
            $this->assertStringContainsString(
                'PHP_ADD_BUILD_DIR([$ext_builddir/' . $directory . '])',
                $configM4
            );
        }
    }

    /**
     * The anchors must follow config.json rather than be spelled out, or the
     * next rename breaks the build the same silent way.
     *
     * @test
     */
    public function anchorsAreNotHardCoded() : void
    {
        $buildExt = $this->read('build-ext', $this->root());

        $this->assertStringNotContainsString('PHP_SUBST(TENSOR_SHARED_LIBADD)', $buildExt);
        $this->assertStringNotContainsString('if (PHP_TENSOR != "no")', $buildExt);
    }

    /**
     * Every hand-written C file under ext/ must be in the generated source list.
     * Five files survived the rename as untracked orphans precisely because
     * nothing checked this.
     *
     * @test
     */
    public function noOrphanSourcesExistUnderExt() : void
    {
        $sources = $this->read('ext/config.m4', $this->root());

        $found = glob($this->root() . '/ext/*.c') ?: [];
        $found = array_merge($found, glob($this->root() . '/ext/include/*.c') ?: []);

        foreach ($found as $file) {
            $name = basename($file);

            $this->assertStringContainsString(
                $name,
                $sources,
                "ext/{$name} is not in config.m4's source list — it is compiled by nothing."
            );
        }
    }

    /**
     * @return array{0: int, 1: string}
     */
    private function runBuildExt() : array
    {
        $output = [];
        $status = 0;

        exec(
            escapeshellarg(PHP_BINARY) . ' ' . escapeshellarg($this->sandbox . '/build-ext') . ' 2>&1',
            $output,
            $status
        );

        return [$status, implode("\n", $output)];
    }

    /**
     * @return array<string, string>
     */
    private function artefacts() : array
    {
        return [
            'config.m4' => $this->read('ext/config.m4'),
            'config.w32' => $this->read('ext/config.w32'),
        ];
    }

    /**
     * @return list<string>
     */
    private function extraSourceDirectories() : array
    {
        /** @var array{extra-sources?: list<string>} $config */
        $config = json_decode($this->read('config.json', $this->root()), true);

        $directories = [];

        foreach ($config['extra-sources'] ?? [] as $source) {
            $directory = dirname($source);

            if ($directory !== '.' && !in_array($directory, $directories, true)) {
                $directories[] = $directory;
            }
        }

        return $directories;
    }

    private function read(string $file, ?string $base = null) : string
    {
        $contents = file_get_contents(($base ?? $this->sandbox) . '/' . $file);

        if ($contents === false) {
            throw new RuntimeException("Cannot read {$file}.");
        }

        return $contents;
    }

    private function root() : string
    {
        return dirname(__DIR__);
    }
}
