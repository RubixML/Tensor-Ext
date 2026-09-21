<?php

namespace Tensor {

    /**
     * Static declarations of the extension classes for PHPStan.
     *
     * The PHP 8 reflection layer cannot read the default value of the kernel
     * `Buffer::__construct` / `Buffer::fromArray` `$type` argument
     * (`ReflectionParameter::isDefaultValueConstant()` throws "Failed to retrieve
     * the default value"), which makes PHPStan unable to reflect the class and any
     * class that type-hints it. Declare it statically so static analysis works.
     *
     * These files must stay in sync with `tensor/tensorbuffer.zep` and the kernel
     * `Buffer` class. They are registered via `scanFiles` in `phpstan.neon` and
     * are never analysed themselves.
     */
    final class Buffer implements \ArrayAccess, \Countable, \IteratorAggregate, \JsonSerializable
    {
        public const TYPE_DOUBLE = 1;

        public const TYPE_LONG = 2;

        public static function fromArray(array $values, int $type = self::TYPE_DOUBLE)
        {
        }

        public function __construct(int $size, int $type = self::TYPE_DOUBLE)
        {
        }

        public function toArray() : array
        {
        }

        public function type() : int
        {
        }

        public function count() : int
        {
        }

        public function fill(mixed $value) : void
        {
        }

        public function offsetExists(mixed $offset) : bool
        {
        }

        public function offsetGet(mixed $offset) : mixed
        {
        }

        public function offsetSet(mixed $offset, mixed $value) : void
        {
        }

        public function offsetUnset(mixed $offset) : void
        {
        }

        public function getIterator() : \Traversable
        {
        }

        public function jsonSerialize() : mixed
        {
        }

        public function __serialize() : array
        {
        }

        public function __unserialize(array $data) : void
        {
        }
    }

    class TensorBuffer
    {
        public function __construct(Buffer $buffer)
        {
        }

        public function asBuffer() : Buffer
        {
        }

        public function count() : int
        {
        }

        public function type() : int
        {
        }

        public function toArray() : array
        {
        }

        public function get(int $index) : mixed
        {
        }

        public function set(int $index, mixed $value) : void
        {
        }

        public function sort(bool $ascending = true) : void
        {
        }

        public function slice(int $offset, int $length) : TensorBuffer
        {
        }

        public function sum() : float
        {
        }

        public function product() : float
        {
        }

        public function min() : float
        {
        }

        public function max() : float
        {
        }

        public function argmin() : int
        {
        }

        public function argmax() : int
        {
        }

        public function sliceStrided(int $offset, int $length, int $stride) : TensorBuffer
        {
        }

        public function concat(array $buffers) : TensorBuffer
        {
        }

        public function split(int $chunkLength) : array
        {
        }

        public function repeat(int $times) : TensorBuffer
        {
        }
    }

}
