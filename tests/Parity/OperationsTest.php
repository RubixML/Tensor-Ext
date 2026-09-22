<?php

namespace Tensor\Tests\Parity;

use Generator;
use ReflectionClass;
use ReflectionMethod;
use PHPUnit\Framework\TestCase;

/**
 * @covers \Tensor\Tests\Parity\Operations
 * @covers \Tensor\Tests\Parity\Operation
 * @covers \Tensor\Tests\Parity\Encode
 */
class OperationsTest extends TestCase
{
    /**
     * Probing every operation on every parity fixture is what makes the
     * migration safe, so the count is pinned: adding or removing a probe has to
     * be a deliberate edit to this number, not a silent side effect.
     */
    protected const EXPECTED_OPERATIONS = 464;

    /**
     * @var array<string, class-string>
     */
    protected const SUBJECTS = [
        'vector' => 'Tensor\Vector',
        'columnVector' => 'Tensor\ColumnVector',
        'matrix' => 'Tensor\Matrix',
    ];

    /**
     * @test
     */
    public function operationCountIsPinned() : void
    {
        $this->assertCount(self::EXPECTED_OPERATIONS, Operations::all());
    }

    /**
     * Transcripts are keyed by operation and written in key order, so a change
     * to one operation is a one-line diff rather than a reshuffle.
     *
     * @test
     */
    public function keysAreUniqueAndSorted() : void
    {
        $keys = Operations::keys();
        $sorted = $keys;
        sort($sorted);

        $this->assertSame($sorted, $keys);
        $this->assertCount(count(array_unique($keys)), $keys);
    }

    /**
     * @test
     */
    public function keysFollowTheNamingScheme() : void
    {
        foreach (Operations::keys() as $key) {
            $this->assertMatchesRegularExpression(
                '/^(vector|columnVector|matrix)\.[a-z][A-Za-z0-9]*(\.[a-zA-Z0-9]+)?$/',
                $key
            );
        }
    }

    /**
     * @test
     */
    public function everyOperationNamesAKnownSubject() : void
    {
        foreach (Operations::all() as $operation) {
            $this->assertArrayHasKey($operation->subject, self::SUBJECTS);
            $this->assertSame(self::SUBJECTS[$operation->subject], $operation->subjectClass());
        }
    }

    /**
     * The completeness guarantee. Every public method of every tensor class is
     * either probed or listed in EXCLUDED with a stated reason. Adding a method
     * to a tensor class without doing one or the other fails here.
     *
     * @test
     * @dataProvider subjectProvider
     *
     * @param class-string $class
     * @param string $subject
     */
    public function everyPublicMethodIsProbedOrExcluded(string $subject, string $class) : void
    {
        $probed = [];

        foreach (Operations::all() as $operation) {
            if ($operation->subject === $subject) {
                $probed[$operation->method()] = true;
            }
        }

        $missing = array_diff(
            $this->publicMethods($class),
            array_keys($probed),
            array_keys(Operations::EXCLUDED),
            Operations::excludedMethodsFor($subject)
        );

        $this->assertSame([], array_values($missing), sprintf(
            '%s has unprobed public methods: %s. Add a probe, or an EXCLUDED entry with a reason.',
            $class,
            implode(', ', $missing)
        ));
    }

    /**
     * The mirror of the above: a probe naming a method that no longer exists
     * would silently record a "threw" transcript forever.
     *
     * @test
     * @dataProvider subjectProvider
     *
     * @param class-string $class
     * @param string $subject
     */
    public function everyProbeNamesARealMethod(string $subject, string $class) : void
    {
        $methods = $this->publicMethods($class);

        foreach (Operations::all() as $operation) {
            if ($operation->subject !== $subject) {
                continue;
            }

            $this->assertContains($operation->method(), $methods, "{$operation->key} names no method on {$class}.");
        }
    }

    /**
     * @test
     */
    public function exclusionsCarryAReason() : void
    {
        $this->assertNotEmpty(Operations::EXCLUDED);

        foreach (Operations::EXCLUDED as $method => $reason) {
            $this->assertNotSame('', trim($reason), "{$method} is excluded without a reason.");
        }

        foreach (Operations::EXCLUDED_OPS as $key => $reason) {
            $this->assertNotSame('', trim($reason), "{$key} is excluded without a reason.");
        }
    }

    /**
     * Pinned because it decides which results keep their values and which are
     * compared by digest alone. Moving it silently would change what the
     * transcripts actually prove.
     *
     * @test
     */
    public function fullFidelityLimitIsPinned() : void
    {
        $this->assertSame(256, Encode::FULL_FIDELITY_LIMIT);
        $this->assertSame(8, Encode::EDGE);
    }

    /**
     * @test
     */
    public function floatsRoundTripExactly() : void
    {
        foreach ([0.0, -0.0, 1.0, -1.0, 0.1, M_PI, PHP_FLOAT_EPSILON, PHP_FLOAT_MAX, PHP_FLOAT_MIN] as $value) {
            $this->assertSame($value, Encode::decodeF64(Encode::f64($value)));
        }

        $this->assertSame('8000000000000000', Encode::f64(-0.0));
        $this->assertSame('NAN', Encode::f64(NAN));
        $this->assertSame('INF', Encode::f64(INF));
        $this->assertSame('-INF', Encode::f64(-INF));
    }

    /**
     * @return Generator<array{0: string, 1: class-string}>
     */
    public function subjectProvider() : Generator
    {
        foreach (self::SUBJECTS as $subject => $class) {
            yield $subject => [$subject, $class];
        }
    }

    /**
     * @param class-string $class
     * @return list<string>
     */
    private function publicMethods(string $class) : array
    {
        $names = [];

        foreach ((new ReflectionClass($class))->getMethods(ReflectionMethod::IS_PUBLIC) as $method) {
            if ($method->isConstructor() || $method->isDestructor()) {
                continue;
            }

            $names[] = $method->getName();
        }

        return $names;
    }
}
