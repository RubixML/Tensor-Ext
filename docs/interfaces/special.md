# Special

Reductions and clipping operations.

- **Namespace:** `Tensor\Special`

## Overview

`Special` defines aggregate reductions and interval-clipping operations.

```php
interface Special
```

- For `Vector`, reductions return a `float` scalar.
- For `Matrix`, the reductions `sum`, `product`, `min`, `max`, `argmin`, and `argmax` operate per-row and return a `ColumnVector`; clipping returns a new `Matrix` of the same shape.

## Methods

### `sum() : mixed`

Sum the tensor.

### `product() : mixed`

Calculate the product of the tensor.

### `min() : mixed`

Return the minimum of the tensor.

### `max() : mixed`

Return the maximum of the tensor.

### `argmin() : mixed`

Return the index of the minimum of the tensor. For a `Vector`, this is an `int`; for a `Matrix`, this is a `ColumnVector` of the per-row minimum indices. Ties resolve to the first occurrence.

### `argmax() : mixed`

Return the index of the maximum of the tensor. For a `Vector`, this is an `int`; for a `Matrix`, this is a `ColumnVector` of the per-row maximum indices. Ties resolve to the first occurrence.

### `clip(float $min, float $max) : mixed`

Clip the tensor to be between the given minimum and maximum.

- **Parameters:** `$min`, `$max` — the clip interval bounds.
- **Throws:** `Tensor\Exceptions\InvalidArgumentException` if `$min` is greater than `$max`.

### `clipLower(float $min) : mixed`

Clip the tensor to be lower bounded by a given minimum.

### `clipUpper(float $max) : mixed`

Clip the tensor to be upper bounded by a given maximum.