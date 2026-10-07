# Special

Operations that do not belong to any of the other interface groups.

- **Namespace:** `Tensor\Special`

## Overview

`Special` holds the operations that neither the basic algebraic and trigonometric families (`Arithmetic`, `Comparable`, `Unary`, `Trigonometric`), aggregate reductions (`Reductions`), nor array-like access (`ArrayLike`) can describe.

`softmax()` is the only shape-preserving member whose output depends on a whole reduction axis rather than on one input element, so it is neither element-wise nor a reduction. `erf()` is an element-wise error-function transform grouped here so the error-function family stays together, distinct from the basic algebraic operations.

```php
interface Special
```

## Methods

### `softmax() : mixed`

Return the softmax of the tensor, i.e. the exponentials of its elements
normalized so that they sum to one.

For a `Matrix` this normalizes **each row independently**, so every row of the
result sums to `1.0`. To normalize each column instead, call `transpose()` on
either side:

```php
$normalized = $matrix->transpose()->softmax()->transpose();
```

For a `Vector` the whole vector is normalized as a single row, and an empty
vector normalizes to an empty vector.

The maximum of each row is subtracted before exponentiating, so no intermediate
can overflow regardless of the magnitude of the input, and a single-element row
normalizes to `1.0`. Note the degenerate case this implies: a matrix with a
single *column* has one-element rows, so it normalizes to all ones.

### `erf() : mixed`

Return the element-wise Gaussian error function.

A large input saturates to exactly `1.0` or `-1.0` rather than returning an
infinity.
