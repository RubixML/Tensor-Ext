# Special

Operations that do not belong to any of the other interface groups.

- **Namespace:** `Tensor\Special`

## Overview

`Special` holds the operations that neither pointwise element-wise functions (`Arithmetic`, `Comparable`, `Unary`, `Trigonometric`), nor aggregate reductions (`Reductions`), nor array-like access (`ArrayLike`) can describe.

`softmax()` is the sole member: it preserves the shape of the tensor, but each element of the output depends on a whole reduction axis rather than on one input element, so it is neither element-wise nor a reduction.

```php
interface Special
```

## Methods

### `softmax() : mixed`

Return the softmax of the tensor, i.e. the exponentials of its elements
normalized so that they sum to one.

For a `Matrix` this normalizes **each column independently**, so the transpose of
the result sums to `1.0` down every column. To normalize each row instead, call
`transpose()` on either side:

```php
$normalized = $matrix->transpose()->softmax()->transpose();
```

For a `Vector` the whole vector is normalized as a single column.

The maximum of each column is subtracted before exponentiating, so no
intermediate can overflow regardless of the magnitude of the input, and a
single-element column normalizes to `1.0`.
