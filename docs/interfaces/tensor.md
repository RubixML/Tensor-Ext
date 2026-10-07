# Tensor

Interface implemented by all tensor objects.

- **Namespace:** `Tensor\Tensor`

## Overview

`Tensor` is the top-level marker interface for every tensor type in the extension. It has no methods of its own; it composes the entire tensor API by extending the capable sub-interfaces below. See [the API reference](../index.md) for the full surface.

```php
interface Tensor extends ArrayLike, Arithmetic, Comparable,
    Unary, Trigonometric, Reductions, Special
```

## Inherited Method Groups

| Interface | Methods |
| --- | --- |
| [ArrayLike](arraylike.md) | `shape`, `shapeString`, `size`, `map`, `reduce`, `asArray` |
| [Arithmetic](arithmetic.md) | `multiply`, `divide`, `add`, `subtract`, `pow`, `mod` |
| [Comparable](comparable.md) | `equal`, `notEqual`, `greater`, `greaterEqual`, `less`, `lessEqual` |
| [Unary](unary.md) | `abs`, `square`, `sqrt`, `rsqrt`, `exp`, `expm1`, `log`, `log1p`, `sigmoid`, `softplus`, `round`, `floor`, `ceil`, `sign`, `negate`, `clip`, `clipLower`, `clipUpper` |
| [Trigonometric](trigonometric.md) | `sin`, `asin`, `cos`, `acos`, `tan`, `atan`, `tanh`, `sinh`, `cosh`, `rad2deg`, `deg2rad` |
| [Reductions](reductions.md) | `sum`, `product`, `min`, `max`, `argmin`, `argmax`, `mean`, `variance`, `median`, `quantile` |
| [Special](special.md) | `softmax`, `erf` |

Every group is homogeneous in what it does to the tensor's shape: `Arithmetic`,
`Comparable`, `Unary`, and `Trigonometric` are element-wise and preserve it,
`Reductions` collapse it, and `ArrayLike` is about access rather than arithmetic.
`Special` is the escape hatch for operations that fit no other group, such as shape-preserving cross-axis operations like `softmax()` and the element-wise error-function family `erf()`.

Additionally, because `ArrayLike` extends `ArrayAccess`, `IteratorAggregate`, and `Countable`, every tensor is array-accessible, iterable, and countable.

## Implementations

- [`Tensor\Vector`](../Vector.md)
- [`Tensor\ColumnVector`](../ColumnVector.md)
- [`Tensor\Matrix`](../Matrix.md)