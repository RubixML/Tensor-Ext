# Eigen

The eigendecomposition (or spectral decomposition) of a matrix.

- **Namespace:** `Tensor\Decompositions\Eigen`

## Overview

The eigendecomposition is a matrix factorization resulting in a matrix of eigenvectors and a corresponding vector of eigenvalues.

> **Note:** For matrices with complex eigenvalues, only the real parts of the eigenvalues and eigenvectors are returned, mirroring the [extension](../getting-started.md). The eigenvector at row `i` of `eigenvectors()` corresponds to the eigenvalue at index `i` of `eigenvalues()`. Each eigenvector is normalized to unit length.

## Factory

### `Eigen::decompose(Matrix $a, bool $symmetric = false) : self`

Factory method to decompose a matrix.

- **Parameters:**
  - `$a` — the matrix to decompose
  - `$symmetric` — whether the matrix is known to be symmetric (default `false`), which selects a faster solver
- **Returns:** `Eigen`
- **Throws:** `Tensor\Exceptions\InvalidArgumentException` if the matrix is not square, `Tensor\Exceptions\RuntimeException` if the decomposition fails to converge

## Accessors

### `__construct(Vector $eigenvalues, Matrix $eigenvectors, Vector $eigenvaluesImaginary)`

Instantiate from eigenvalues, eigenvectors, and imaginary eigenvalues.

- **Parameters:** `$eigenvalues` — `Vector`, `$eigenvectors` — `Matrix`, `$eigenvaluesImaginary` — `Vector`

### `eigenvalues() : Vector`

Return the eigenvalues of the eigendecomposition.

- **Returns:** `Vector`

### `eigenvaluesImaginary() : Vector`

Return the imaginary parts of the eigenvalues, in the same order as `eigenvalues()`. The `i`'th complex eigenvalue is `eigenvalues()[i] + i * eigenvaluesImaginary()[i]`. Zero-filled for symmetric inputs.

- **Returns:** `Vector`

### `eigenvectors() : Matrix`

Return the eigenvectors of the eigendecomposition.

- **Returns:** `Matrix`