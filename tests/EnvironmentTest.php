<?php

namespace Tensor\Tests;

use ReflectionClass;
use Tensor\Settings;
use Tensor\Vector;
use PHPUnit\Framework\TestCase;

/**
 * Guards the assumptions every other test in this suite silently depends on.
 *
 * The parity harness compares the compiled extension against a recorded
 * transcript and against the pure-PHP rubix/tensor twin. Both of those declare
 * the same class names, so a suite run that forgot to load the extension would
 * exercise the twin and report green. These assertions make that impossible.
 *
 * @coversNothing
 */
class EnvironmentTest extends TestCase
{
    /**
     * @test
     */
    public function extensionIsLoaded() : void
    {
        $this->assertTrue(
            extension_loaded('tensor_ext'),
            'Run phpunit with -d extension=ext/modules/tensor_ext.so; the suite is meaningless without it.'
        );
    }

    /**
     * The extension registers Tensor\* at MINIT, so composer's autoloader never
     * fires for them. If this fails, the pure-PHP twin won the race.
     *
     * @test
     */
    public function tensorClassesComeFromTheExtension() : void
    {
        $this->assertTrue((new ReflectionClass(Vector::class))->isInternal());
    }

    /**
     * config.json pins OpenBLAS to one thread in the module initializer. Every
     * recorded transcript assumes it, because dgemm's reduction order — and so
     * its last ulps — varies with thread count.
     *
     * @test
     */
    public function openBlasThreadsDefaultToOne() : void
    {
        $this->assertSame(1, Settings::numThreads());
    }
}
