# Reductions

Aggregate reduction operations.

- **Namespace:** `Tensor\Reductions`

> **Note:** the `Reductions` *interface* shares its name with the `Tensor\Reductions` *namespace* that holds the [REF](../reductions/ref.md) and [RREF](../reductions/rref.md) classes. The two are distinct: `Tensor\Reductions` is the interface, `Tensor\Reductions\REF` and `Tensor\Reductions\RREF` are the row echelon form classes.

## Overview

`Reductions` defines aggregate operations that collapse a tensor along a single axis.

```php
interface Reductions
```

- For `Vector`, reductions return a `float` scalar.
- For `Matrix`, the reductions operate per-row and return a `ColumnVector`.

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

### `mean() : mixed`

Return the mean of the tensor.

### `variance($mean = null) : mixed`

Compute the variance of the tensor.

- **Parameters:** `$mean` — an optional pre-computed mean to avoid recomputation. If omitted, the mean is computed internally.

### `median() : mixed`

Return the median of the tensor.

### `quantile(float $q) : mixed`

Return the q'th quantile of the tensor.

- **Parameters:** `$q` — the quantile in the range `[0, 1]`.
- **Throws:** `Tensor\Exceptions\InvalidArgumentException` if `$q` is outside `[0, 1]`.
