<?php

namespace Tensor\Tests;

use ArrayAccess;
use Countable;
use Error;
use IteratorAggregate;
use JsonSerializable;
use ReflectionClass;
use Tensor\Buffer;
use PHPUnit\Framework\TestCase;

/**
 * Tensor\Buffer is the contiguous C numeric buffer the storage migration is
 * built on — a Zephir kernel class, registered at MINIT, opted into by
 * `kernel-classes.buffer` in config.json.
 *
 * Several of its behaviours are surprising enough that the tests below assert
 * them deliberately, as documentation of the hazard rather than approval of it.
 * A buffer compares equal to any other buffer; casting one to array yields an
 * empty array; and an element supports compound assignment but not increment.
 *
 * `@coversNothing` rather than `@covers \Tensor\Buffer`: the class is internal,
 * so there is no file for coverage to map it to, and `forceCoversAnnotation`
 * only insists that an annotation be present.
 *
 * @coversNothing
 */
class BufferTest extends TestCase
{
    /**
     * @test
     */
    public function bufferIsRegistered() : void
    {
        $this->assertTrue(class_exists(Buffer::class));

        $reflection = new ReflectionClass(Buffer::class);

        $this->assertTrue($reflection->isFinal());
        $this->assertTrue($reflection->isInternal());
    }

    /**
     * @test
     */
    public function elementTypesAreTheDocumentedConstants() : void
    {
        $this->assertSame(1, Buffer::TYPE_DOUBLE);
        $this->assertSame(2, Buffer::TYPE_LONG);
    }

    /**
     * @test
     */
    public function bufferImplementsTheExpectedInterfaces() : void
    {
        $buffer = new Buffer(1);

        $this->assertInstanceOf(ArrayAccess::class, $buffer);
        $this->assertInstanceOf(Countable::class, $buffer);
        $this->assertInstanceOf(IteratorAggregate::class, $buffer);
        $this->assertInstanceOf(JsonSerializable::class, $buffer);
    }

    /**
     * @test
     */
    public function constructionIsZeroFilled() : void
    {
        $buffer = new Buffer(3);

        $this->assertCount(3, $buffer);
        $this->assertSame([0.0, 0.0, 0.0], $buffer->toArray());
        $this->assertSame(Buffer::TYPE_DOUBLE, $buffer->type());
    }

    /**
     * @test
     */
    public function longBuffersAreZeroFilledWithIntegers() : void
    {
        $buffer = new Buffer(2, Buffer::TYPE_LONG);

        $this->assertSame([0, 0], $buffer->toArray());
        $this->assertSame(Buffer::TYPE_LONG, $buffer->type());
    }

    /**
     * @test
     */
    public function fromArrayRoundTrips() : void
    {
        $values = [1.5, -2.25, 0.0, 1.0e300];

        $this->assertSame($values, Buffer::fromArray($values)->toArray());
    }

    /**
     * A buffer is positional. Keys are discarded, which is what makes it a
     * sound backing store for a tensor and a poor one for anything else.
     *
     * @test
     */
    public function fromArrayDiscardsKeys() : void
    {
        $this->assertSame([1.0, 2.0], Buffer::fromArray([7 => 1.0, 'x' => 2.0])->toArray());
    }

    /**
     * The one accepted behaviour change of the whole migration: Vector::quick()
     * stores integers verbatim today, and a double buffer cannot. Asserted here
     * so the coercion is a decision on the record rather than a surprise.
     *
     * @test
     */
    public function integersAreCoercedToDoubles() : void
    {
        $values = Buffer::fromArray([1, 2, 3])->toArray();

        $this->assertSame([1.0, 2.0, 3.0], $values);

        foreach ($values as $value) {
            $this->assertIsFloat($value);
        }
    }

    /**
     * @test
     */
    public function fillOverwritesEveryElement() : void
    {
        $buffer = Buffer::fromArray([1.0, 2.0, 3.0]);
        $buffer->fill(9.5);

        $this->assertSame([9.5, 9.5, 9.5], $buffer->toArray());
    }

    /**
     * @test
     */
    public function elementsAreReadableAndWritableByOffset() : void
    {
        $buffer = Buffer::fromArray([1.0, 2.0]);

        $this->assertSame(1.0, $buffer[0]);

        $buffer[1] = 7.5;

        $this->assertSame(7.5, $buffer[1]);
        $this->assertTrue(isset($buffer[0]));
        $this->assertFalse(isset($buffer[2]));
    }

    /**
     * Unlike a fresh SplFixedArray, whose slots start null, every in-range
     * element of a numeric buffer is set — a zero is a value.
     *
     * @test
     */
    public function everyInRangeElementIsSetEvenWhenZero() : void
    {
        $buffer = new Buffer(2);

        $this->assertTrue(isset($buffer[0]));
        $this->assertTrue(isset($buffer[1]));
    }

    /**
     * @test
     */
    public function cloningCopiesTheElements() : void
    {
        $original = Buffer::fromArray([1.0, 2.0]);
        $copy = clone $original;

        $copy[0] = 99.0;

        $this->assertSame(1.0, $original[0]);
        $this->assertSame(99.0, $copy[0]);
    }

    /**
     * @test
     */
    public function bufferSerialisesToJsonAsAList() : void
    {
        $this->assertSame('[1.5,2.5]', json_encode(Buffer::fromArray([1.5, 2.5])));
    }

    /**
     * @test
     */
    public function bufferSurvivesSerialisation() : void
    {
        $buffer = Buffer::fromArray([1.5, -2.5, 3.0]);

        /** @var Buffer $restored */
        $restored = unserialize(serialize($buffer));

        $this->assertSame($buffer->toArray(), $restored->toArray());
        $this->assertSame($buffer->type(), $restored->type());
    }

    /**
     * @test
     */
    public function iterationYieldsTheElementsInOrder() : void
    {
        $this->assertSame([1.5, 2.5], iterator_to_array(Buffer::fromArray([1.5, 2.5])));
    }

    /**
     * TRAP: any two buffers compare equal under `==`, whatever they contain.
     *
     * There is no compare handler and no properties for the engine's default
     * object comparison to look at, so it finds nothing to disagree about.
     * Compare toArray() instead. Asserted here so the hazard is on the record.
     *
     * @test
     */
    public function anyTwoBuffersCompareEqual() : void
    {
        $this->assertTrue(Buffer::fromArray([1.0]) == Buffer::fromArray([9.9, 8.8]));
        $this->assertFalse(Buffer::fromArray([1.0]) === Buffer::fromArray([1.0]));

        // The supported way to ask the question:
        $this->assertNotSame(
            Buffer::fromArray([1.0])->toArray(),
            Buffer::fromArray([9.9, 8.8])->toArray()
        );
    }

    /**
     * TRAP: casting to array gives an empty array, because the elements are raw
     * C and there is no cast_object handler.
     *
     * @test
     */
    public function castingToArrayYieldsNothing() : void
    {
        $this->assertSame([], (array) Buffer::fromArray([1.0, 2.0]));
    }

    /**
     * TRAP: compound assignment works — it is a read followed by a write
     * through the handlers — but anything needing a reference to the element
     * does not, because a raw double is not a zval to point at.
     *
     * @test
     */
    public function compoundAssignmentWorks() : void
    {
        $buffer = Buffer::fromArray([1.0]);
        $buffer[0] += 5.0;

        $this->assertSame(6.0, $buffer[0]);
    }

    /**
     * TRAP: increment does not throw. It raises a notice and silently does
     * nothing, which is the more dangerous failure of the two — a loop built on
     * `++$buffer[$i]` runs to completion and leaves the data untouched.
     *
     * @test
     */
    public function incrementingAnElementRaisesANotice() : void
    {
        $buffer = Buffer::fromArray([1.0]);

        // An explicit handler rather than expectNotice(), which PHPUnit 10
        // removes, and which would also swallow the rest of the assertion.
        $raised = null;

        set_error_handler(static function (int $severity, string $message) use (&$raised) : bool {
            $raised = $message;

            return true;
        });

        ++$buffer[0];

        restore_error_handler();

        $this->assertIsString($raised);
        $this->assertStringContainsString('Indirect modification', $raised);
    }

    /**
     * @test
     */
    public function incrementingAnElementHasNoEffect() : void
    {
        $buffer = Buffer::fromArray([1.0]);

        // Silenced so the notice does not become an exception before the
        // interesting half of the behaviour can be observed.
        @++$buffer[0];

        $this->assertSame(1.0, $buffer[0]);
    }

    /**
     * @test
     */
    public function takingAReferenceToAnElementHasNoEffect() : void
    {
        $buffer = Buffer::fromArray([1.0]);

        $reference = @$buffer[0];
        $reference = 42.0;

        $this->assertSame(1.0, $buffer[0]);
    }

    /**
     * Asserted directly against PHP_VERSION_ID rather than by comparing to
     * SplFixedArray: its diagnostics moved three times between 8.0 and 8.5, so
     * it is an oracle that tells you about itself, not about this class.
     *
     * @test
     */
    public function outOfRangeReadsRaiseTheVersionAppropriateException() : void
    {
        $buffer = Buffer::fromArray([1.0]);

        $this->expectException(
            PHP_VERSION_ID >= 80400 ? \OutOfBoundsException::class : \RuntimeException::class
        );

        // Wrapped in an assertion so the read is an expression with a use; the
        // exception fires before the assertion is ever evaluated.
        $this->assertNotNull($buffer[5]);
    }

    /**
     * @test
     */
    public function appendingRaises() : void
    {
        $buffer = Buffer::fromArray([1.0]);

        $this->expectException(PHP_VERSION_ID >= 80100 ? Error::class : \RuntimeException::class);

        $buffer[] = 2.0;
    }

    /**
     * @test
     */
    public function anEmptyBufferIsLegal() : void
    {
        $buffer = new Buffer(0);

        $this->assertCount(0, $buffer);
        $this->assertSame([], $buffer->toArray());
    }
}
