# TensorBuffer

A decorator that wraps the kernel `Buffer` class and adds higher-level operations such as sorting and slicing.

- **Namespace:** `Tensor\TensorBuffer`

## Overview

The Zephir kernel `Buffer` (`Tensor\Buffer`) is an ordinary refcounted object that stores its elements in one contiguous C array of `double` or `zend_long`. It exposes a minimal PHP API — construction, `fromArray()` / `toArray()`, `count()`, `type()`, `fill()`, and array access — but no sorting or slicing.

`TensorBuffer` composes a `Buffer` and delegates to it, adding operations that cannot be expressed through the minimal API. Those operations are implemented in C over the buffer's raw element pointers (`qsort`, `memcpy`), so no intermediate PHP array is materialised.

`sort()` mutates the wrapped buffer in place; `slice()` returns a **new** `TensorBuffer` wrapping a new buffer, leaving the source untouched.

## Constructors

### `__construct(Buffer $buffer)`

Wrap an existing buffer.

- **Parameters:** `$buffer` — the `Tensor\Buffer` to decorate

## Methods

### `asBuffer() : Buffer`

Return the underlying buffer being decorated.

### `count() : int`

Return the number of elements in the buffer.

### `type() : int`

Return the element type of the buffer — `Buffer::TYPE_DOUBLE` or `Buffer::TYPE_LONG`.

### `toArray() : array`

Return the buffer as a PHP array.

### `get(int $index) : mixed`

Return the element at the given index.

- **Throws:** `Tensor\Exceptions\RuntimeException` (or `OutOfBoundsException` on PHP 8.4+) if the index is out of range

### `set(int $index, mixed $value) : void`

Set the element at the given index, cast to the buffer's element type.

- **Throws:** `Tensor\Exceptions\RuntimeException` (or `OutOfBoundsException` on PHP 8.4+) if the index is out of range

### `sort(bool $ascending = true) : void`

Sort the buffer **in place**.

- **Parameters:** `$ascending` — sort ascending when `true`, descending when `false` (default `true`)

### `slice(int $offset, int $length) : TensorBuffer`

Return a new decorator wrapping a new buffer of `$length` elements copied from `buffer[$offset .. $offset + $length - 1]`. The source buffer is left unchanged.

- **Throws:** `Tensor\Exceptions\InvalidArgumentException` if `$offset` or `$length` is negative, or the requested range exceeds the buffer size

## Notes

- `sort()` and `slice()` are implemented in C (`ext/include/buffer.c`) and route through this class's optimizer calls, operating directly on the buffer's raw pointer.
- Both element kinds (`TYPE_DOUBLE` and `TYPE_LONG`) are supported; `slice()` preserves the source kind.