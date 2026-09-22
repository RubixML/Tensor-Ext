# Change Log

- 4.0.0
    - `Vector`, `ColumnVector` and `Matrix` now store their elements in a contiguous C buffer
      (`Tensor\Buffer`) rather than PHP arrays. Rank-1 arithmetic is 7-8x faster and the inner
      product 25x; storage for a million elements falls from 16.0 MB to 7.6 MB. A matrix is one
      flat row-major buffer, so an element-wise operation is a single call over m * n rather than
      one call per row: chained matrix workloads run 3-9x faster and `transpose()` 363x, having
      previously rebuilt the whole matrix once per column.
    - `matmul()` and `inverse()` now hand their operands straight to BLAS and LAPACK with no
      packing. With the marshalling gone, `matmul` also finally threads: 3.1x on four threads at
      n = 512, against 2.1x before.
    - Fixed `Matrix::pseudoinverse()` reading out of bounds for every matrix with more rows than
      columns. Results were silently wrong for small matrices and entirely non-finite past roughly
      200 rows, and depended on unrelated heap state in between.
    - Fixed `ColumnVector::modMatrix()` and `divideMatrix()` aborting the process (SIGABRT) when
      the operation raised. They now throw, catchably.
    - Fixed `composer compile`, which had been failing since the extension was renamed to
      `tensor_ext`: `build-ext` still looked for the anchors Zephir emits under the old name, so
      none of its three patches applied and it exited 1 on the first of them.
    - Four tests (`pseudoinverse`, `eig`, `eigSymmetric`, `svd`) carried `@requires extension
      tensor` and had therefore been skipped, not run, since the rename.
    - **Behaviour change.** Comparison and modulo operations on rank-1 tensors now return floats
      where they returned integers; a buffer of doubles cannot hold an integer. Values are
      unaffected.
    - **Behaviour change.** `Vector::dot()` and `inner()` now use `cblas_ddot`, whose blocked
      accumulation moves the last couple of ulps relative to the previous sequential loop.
    - **Behaviour change.** `mod` no longer emits an "implicit conversion from float" deprecation
      per element.
    - OpenBLAS still defaults to a single thread, now on re-measured grounds. Threading one process
      is a 3.3x win on `matmul(512)`, but four concurrent workers at four threads each run 1.94x
      slower per operation and deliver half the throughput, because OpenBLAS's pthread pool
      busy-waits between parallel regions. `Tensor\Settings::setNumThreads()` remains the opt-in for
      workloads that own the machine; `tools/concurrency-probe.php` measures it.
    - Major version because comparison and modulo results changed type. See `docs/buffers.md`.

- 3.1.1
    - Just triggering the first Packagist release

- 3.1.0
    - Implemented the singular value decomposition (SVD) in the extension
    - Implemented the Moore-Penrose pseudoinverse in the extension via SVD
    - Implemented the eigendecomposition in the extension

- 3.0.11
    - Update to Zephir 1.5.0
    - Fixed RREF/REF pivot float tolerance
    - RREF now zeroes the tail if below EPSILON
    - `rank()`, `fullRank()`, and `inverse()` are now consistent
    - Matrix::inverse() now throws a RuntimeException on singular input
    - Matrix::inverse() now throws an InvalidArgumentException on non-square input
    - Fix Poisson distribution when lambda is zero
    - Added square check to inverse()
    - Cholesky decompose() now throws a RuntimeException on non-positive-definite input
    - Added the SingularMatrix exception
    - The extension now reduces singular matrices to REF and RREF instead of throwing

- 3.0.10
    - Update to Zephir 0.23, adds support for PHP 8.5
    - Fixed Matrix and Vector build() validation
    - Fixed bug in Matrix row variance
    - Fixed LU decomposition pivot selection, singular matrices now throw a RuntimeException
    - Fixed undefined array key in Vector and Matrix quantile at q=1.0 or with a single element
    - Fix extension rank() operation
    - Fixed reduce() callback argument order inconsistency between Matrix and Vector
    - Fix covariance() summation axis
    - Fixed Vector notEqualMatrix throwing InvalidArgumentException instead of DimensionalityMismatch
    - Fixed memory leak in the extension on the failure path of inverse, pseudoinverse

- 3.0.7
    - Upgrade to Zephir version 0.19
    - Drop support for PHP 7.4 in extension

- 3.0.6
    - Upgrade to Zephir version 0.18

- 3.0.5
    - Upgraded Zephir dependency to 0.17

- 3.0.4
    - Fixed bug in Vector less equal operation

- 3.0.3
    - Added PHP 8.2 extension compatibility

- 3.0.2
    - Added PHP 8.1 extension compatibility

- 3.0.1
    - Add positive semi-definite typehinting

- 3.0.0
    - No changes

- 3.0.0-beta
    - Add support for PHP 8.0 in the extension
    - Drop extension support for PHP 7.2 and 7.3
    - Tensors only compute floating point operations
    - Remove dependency on JAMA library
    - Remove previously deprecated items
    - Matrix returns vector on row access
    - Added Algebraic and Special function interfaces

- 2.2.3
    - Remove dependency on pthreads

-2.2.2
    - Compensate for PSR-4 issues

- 2.2.1
    - Optimize 2D convolution operation
    - Fix and optimize vector convolve 1D operation

- 2.2.0
    - Implemented Singular Value Decomposition (SVD)
    - Implemented matrix (Moore-Penrose) pseudoinverse
    - Tensor objects now throw namespaced exceptions
    - Optimized Reduced Row Echelon Form (RREF)
    - Optimize eigendecomposition for symmetric matrices

- 2.1.4
    - Implemented Eigen decomposition in extension
    - Optimize row echelon form (REF) of matrix
    - Optimize Cholesky decomposition
    - Fix memory leak in matrix inverse
    - Optimize LU decomposition

- 2.1.3
    - Use BLAS DGEMM for matrix multiplication
    - Optimize matrix inverse

- 2.1.2
    - Optimize scalar arithmetic and comparisons

- 2.1.1
    - Zephir redux
    - Blanket extension optimizations
    - Added array arithmetic and comparison functions

- 2.1.0
    - Extension now compiles directly from C source code
    - Remove Zephir extension code
    - Update instructions for extension compilation using PHPize

- 2.0.5
    - Percentile method changed to quantile
    - Fixed Vector linspace precision

- 2.0.4
    - Individual Arithmetic and Comparison methods now public
    - Matrix reductions have their own namespace
    - Optimized Matrix structural operations
    - Added argmin and argmax custom optimizers
    - Added matrix/sub matrix insertion

- 2.0.3
    - Matrix decomp methods now return decomp objects
    - Optimized matrix statistical operations
    - Optimized vector outer product
    - Optimized Matrix flatten
    - Cannot instantiate null/empty tensors

- 2.0.2
    - Optimized random number generation

- 2.0.1
    - Added benchmarks
    - Matrix stacking now row or column-wise
    - Changed method signature and behavior of repeat matrix

- 2.0.0
    - Implemented the library as an extension using Zephir lang
    - Changed namespace from Rubix\Tensor to \Tensor
    - Matrix decompositions are now a separate abstraction
    - Removed Dimensionality Mismatch exception
    - Added inverse trigonometric methods to Trigonometric interface
    - Added Array-Like interface
    - Removed Column/Row exclude methods from Matrix
    - Changed method signature of matrix determinant
    - Added return sub matrix
    - Added positive definite and semi-definite methods to Matrix
    - Added is symmetric method to Matrix
    - Added Statistical and Trigonometric interfaces
    - Added Arithmetic and Comparable interfaces
    - Added log1p and expm1 methods to the Arithmetic interface
    - Added matrix full rank method
    - Added generate random Poisson distribution
    - Added Cholesky decomposition
    - Variance now takes an optional mean argument

- 1.0.4
    - Added transpose to Tensor API
    - Reduced memory footprint of matmul operation
    - Removed magic getters
    - Added shape string method to Tensor API
    - Improved error messages for matrix dimensionality mismatch

- 1.0.3
    - Added clip upper and lower bounds
    - Added isSquare method to Matrix
    - Added vector late static binding

- 1.0.2
    - Added ref using row elimination method
    - Added universal comparison methods to tensor API
    - Added convolve operation to Vector and Matrix

- 1.0.1
    - Added Column Vector
    - Implemented Eigenvalue decomposition
    - Added solve system of linear equations
    - Integration with 3rd party JAMA library
    - Fixed variance covariance calculation
    - Added percentile calculation for Vector and Matrix

- 1.0.0
    - Implemented LU decomposition
    - Added vector projection
    - Implemented Matrix inverse
    - Implemented Row Echelon and Reduced Row Echelon decomposition
    - Added Matrix determinant
    - Added Matrix rank
    - Added Matrix/Vector products
    - Implemented universal element-wise tensor operations
    - Added statistical functions
    - Added trigonometric functions
    - Added exponential and logarithmic functions
    - Added tensor factories

