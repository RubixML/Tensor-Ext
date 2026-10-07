# Trigonometric

Element-wise trigonometric functions.

- **Namespace:** `Tensor\Trigonometric`

## Overview

`Trigonometric` defines element-wise trigonometric operations. Each method returns a new tensor of the same shape as `$this`. Angles are expressed in radians unless converting to/from degrees.

```php
interface Trigonometric
```

## Methods

### `sin() : mixed`

Return the sine of the tensor.

### `asin() : mixed`

Compute the arc sine of the tensor.

### `cos() : mixed`

Return the cosine of the tensor.

### `acos() : mixed`

Compute the arc cosine of the tensor.

### `tan() : mixed`

Return the tangent of the tensor.

### `atan() : mixed`

Compute the arc tangent of the tensor.

### `rad2deg() : mixed`

Convert angles from radians to degrees.

### `deg2rad() : mixed`

Convert angles from degrees to radians.

### `tanh() : mixed`

Return the element-wise hyperbolic tangent.

A large input saturates to exactly `1.0` or `-1.0` rather than returning an
infinity.

### `sinh() : mixed`

Return the element-wise hyperbolic sine.

### `cosh() : mixed`

Return the element-wise hyperbolic cosine.

The result is always positive, and `cosh(0) == 1.0`. For a large input it grows
as `exp(|x|) / 2`, so it can overflow to an infinity for sufficiently large
arguments.
