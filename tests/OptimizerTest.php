<?php

namespace Tensor\Tests;

use Generator;
use ReflectionClass;
use Zephir\Optimizers\OptimizerAbstract;
use PHPUnit\Framework\TestCase;

/**
 * The optimizers are how a bare `tensor_*()` call in a .zep file becomes a
 * direct C call rather than a PHP function lookup. Zephir finds them purely by
 * convention — `optimizer-dirs` in config.json plus the PSR-4 mapping of
 * `Zephir\Optimizers\FunctionCall\` onto optimizers/ — so a misnamed or
 * misplaced class fails silently at compile time with a "nonexistent function"
 * warning rather than an error.
 *
 * They also sit outside phpstan.neon's paths, which cover only tests and
 * benchmarks. This is the only automated check they get.
 *
 * @coversNothing
 */
class OptimizerTest extends TestCase
{
    protected const NAMESPACE = 'Zephir\Optimizers\FunctionCall\\';

    /**
     * @test
     * @dataProvider optimizerProvider
     * @param string $shortName
     */
    public function optimizerIsDiscoverable(string $shortName) : void
    {
        $class = self::NAMESPACE . $shortName;

        $this->assertTrue(class_exists($class), "{$class} is not autoloadable.");
        $this->assertTrue((new ReflectionClass($class))->isSubclassOf(OptimizerAbstract::class));
    }

    /**
     * Zephir maps a snake_case call to a StudlyCase class name, so the file name
     * is load-bearing.
     *
     * @test
     * @dataProvider optimizerProvider
     * @param string $shortName
     */
    public function optimizerLivesInTheConfiguredDirectory(string $shortName) : void
    {
        $this->assertFileExists(dirname(__DIR__) . "/optimizers/{$shortName}.php");
    }

    /**
     * @test
     */
    public function everyOptimizerDirectoryEntryIsAnOptimizer() : void
    {
        $files = glob(dirname(__DIR__) . '/optimizers/*.php');

        $this->assertIsArray($files);
        $this->assertNotEmpty($files);

        foreach ($files as $file) {
            $class = self::NAMESPACE . basename($file, '.php');

            $this->assertTrue(class_exists($class), "{$file} does not declare {$class}.");
        }
    }

    /**
     * @return Generator<array{0: string}>
     */
    public function optimizerProvider() : Generator
    {
        foreach (['FromArray', 'Add', 'Multiply', 'Dot'] as $operation) {
            yield $operation => ["TensorBuffer{$operation}Optimizer"];
        }
    }
}
