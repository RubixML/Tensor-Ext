# TensorBuffer

A decorator that wraps the kernel `Buffer` class and adds higher-level operations such as sorting and slicing.

- **Namespace:** `Tensor\TensorBuffer`

## Overview

The Zephir kernel `Buffer` (`Tensor\Buffer`) is an ordinary refcounted object that stores its elements in one contiguous C array of `double` or `zend_long`. It exposes a minimal PHP API — construction, `fromArray()` / `toArray()`, `count()`, `type()`, `fill()`, and array access — but no sorting or slicing.

`TensorBuffer` composes a `Buffer` and delegates to it, adding operations that cannot be expressed through the minimal API. Those operations are implemented in C over the buffer's raw element pointers (`qsort`, `memcpy`), so no intermediate PHP array is materialised.

`sort()` mutates the wrapped buffer in place. `slice()`, `sliceStrided()`, and `split()` on a **double** buffer return an O(1) **view**: a decorator that keeps the source buffer plus an element `offset`, a logical `shape`, and a matching `strides` array, without copying any elements. Views are read-only — `set()` and `sort()` reject them — and `asBuffer()` materialises a fresh dense `Buffer` on demand. Kernels consume a non-contiguous view natively, either by walking its strides in place or, where the underlying C library requires a contiguous block, by materialising a transient work copy for the duration of the call; nothing is cached, so a write through `set()` or `sort()` is observed immediately. On long buffers the operations keep their existing copy semantics.

## Constructors

### `__construct(Buffer $buffer)`

Wrap an existing buffer.

- **Parameters:** `$buffer` — the `Tensor\Buffer` to decorate

## Methods

### `asBuffer() : Buffer`

Return a dense `Buffer` holding the decorator's elements. For a plain buffer this is the wrapped buffer itself; for a view it is a freshly materialised copy in row-major order.

### `count() : int`

Return the number of elements in the buffer. For a view this is the product of its `shape`, not the (possibly larger) wrapped buffer.

### `type() : int`

Return the element type of the buffer — `Buffer::TYPE_DOUBLE` or `Buffer::TYPE_LONG`.

### `toArray() : array`

Return the buffer as a PHP array. For a view the elements are gathered in logical row-major order.

### `get(int $index) : mixed`

Return the element at the given index.

- **Throws:** `Tensor\Exceptions\RuntimeException` (or `OutOfBoundsException` on PHP 8.4+) if the index is out of range

### `set(int $index, mixed $value) : void`

Set the element at the given index, cast to the buffer's element type.

- **Throws:** `Tensor\Exceptions\InvalidArgumentException` if the decorator is a read-only view

### `sort(bool $ascending = true) : void`

Sort the buffer **in place**.

- **Parameters:** `$ascending` — sort ascending when `true`, descending when `false` (default `true`)
- **Throws:** `Tensor\Exceptions\InvalidArgumentException` if the decorator is a read-only view

### `slice(int $offset, int $length) : TensorBuffer`

Return a new decorator of `$length` elements over `buffer[$offset .. $offset + $length - 1]`. On a **double** buffer the result is an O(1) read-only view into the same storage; on a **long** buffer it is a copy. The source buffer is left unchanged.

- **Throws:** `Tensor\Exceptions\InvalidArgumentException` if `$offset` or `$length` is negative, or the requested range exceeds the buffer size

### `sliceStrided(int $offset, int $length, int $stride) : TensorBuffer`

Return a new decorator of `$length` elements gathered at `$stride` intervals from `buffer[$offset]` — i.e. `buffer[$offset + i * $stride]` for `i` in `0..length - 1`. On a **double** buffer the result is an O(1) read-only view into the same storage; on a **long** buffer it is a copy. Supports gathering rows, columns, and diagonals from a flat matrix layout.

- **Throws:** `Tensor\Exceptions\InvalidArgumentException` if `$offset` or `$length` is negative, `$stride` is less than 1, or the requested range exceeds the buffer size

### `concat(TensorBuffer[] $buffers) : TensorBuffer`

Return a new decorator wrapping a new buffer containing a copy of this buffer followed by the contents of each buffer in `$buffers`, in order. The source buffers are left unchanged. Passing an empty list returns a copy of this buffer.

- **Throws:** `InvalidArgumentException` if an element of `$buffers` is not a `TensorBuffer` of the same element type

### `split(int $chunkLength) : TensorBuffer[]`

Return an array of new decorators splitting this buffer into consecutive chunks of up to `$chunkLength` elements each. The final chunk may be shorter. On a **double** buffer each chunk is an O(1) read-only view; on a **long** buffer each is a copy. The source buffer is left unchanged.

- **Throws:** `Tensor\Exceptions\InvalidArgumentException` if `$chunkLength` is less than 1

### `repeat(int $times) : TensorBuffer`

Return a new decorator wrapping a new buffer containing the elements of this buffer repeated `$times` times. The source buffer is left unchanged.

- **Throws:** `Tensor\Exceptions\InvalidArgumentException` if `$times` is less than 1

## Notes

- `sort()`, `slice()`, `sliceStrided()`, `concat()`, `split()`, and `repeat()` are implemented in C (`ext/include/buffer.c`) and route through this class's optimizer calls, operating directly on the buffer's raw pointer.
- A view is a `TensorBuffer` carrying integer `offset`, `shape`, and `strides` properties; a decorator without them is a plain dense buffer and is treated as `offset = 0`, `shape = [length]`, `strides = [1]`.
- `Matrix::transpose()` now returns such a view shape `[n, m]`, stride `[1, n]`, and `Matrix::rowAsVector()` / `columnAsVector()` / `diagonalAsVector()` compose slice and strided views over it.
- The reduction kernels (`sum`, `product`, `min`, `max`, `argmin`, `argmax`) read a non-contiguous view through its `offset` and `strides` in place (`ext/include/reductions.c`), so reducing a transposed matrix does not trigger any copy.
- Both element kinds (`TYPE_DOUBLE` and `TYPE_LONG`) are supported; `slice()`, `sliceStrided()`, `concat()`, `split()`, and `repeat()` preserve the source kind.