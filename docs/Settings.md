# Settings

A static API for global runtime settings: the OpenBLAS thread pool and the CPU feature detection the extension performed at startup.

- **Namespace:** `Tensor\Settings`

## Overview

`Settings` is a collection of stateless, static helpers. It has no instance state and cannot be constructed — every method is static.

`setNumThreads()` and `numThreads()` wrap OpenBLAS' `openblas_set_num_threads()` / `openblas_get_num_threads()` so the number of CPU threads OpenBLAS uses for its parallel kernels can be tuned and inspected at runtime. `cpuFeatures()` reports the CPU feature flags the extension detected at startup. `disableOptimizedKernels()` resets the dispatched C kernels to their scalar baseline variants and `enableOptimizedKernels()` puts the widest route the CPU supports back, giving callers a way to pin reproducible results across hosts without recompiling and then re-enable throughput without a restart.

## Methods

### `setNumThreads(int $threads) : void`

Set the number of CPU threads to use when multiprocessing.

- **Parameters:** `$threads` — the number of threads to use; must be greater than `0`
- **Throws:** `Tensor\Exceptions\InvalidArgumentException` if `$threads` is less than `1`

### `numThreads() : int`

Return the number of CPU threads currently in use for multiprocessing.

### `cpuFeatures() : array`

Return the CPU features the extension detected at startup.

The returned array has four keys:

| Key | Type | Meaning |
| --- | --- | --- |
| `avx` | `bool` | Whether the CPU exposes the AVX (256-bit) instruction set. |
| `avx2` | `bool` | Whether the CPU exposes the AVX2 instruction set. Reported for diagnostics only — the dispatched double-precision kernels gain no throughput from a second 256-bit route. |
| `avx512` | `bool` | Whether the CPU exposes the AVX-512F (512-bit) instruction set. |
| `fma` | `bool` | Whether the CPU exposes FMA3. Reported separately from `avx` because the two are independent features — some CPUs have 256-bit AVX with no fused multiply-add — and because the convolution kernels route on the pair rather than on AVX alone. |

### `disableOptimizedKernels() : void`

Reset the dispatched C kernels to their scalar baseline variants.

**Scope.** The elementwise arithmetic, comparison, unary, linear-algebra, and convolution kernels are each dispatched at MINIT onto one of three ISA-wide variants (AVX, FMA, AVX-512) based on CPU feature detection. Calling this method walks every one of those five families' route pointers back to their baseline (scalar) variants, so the same operations produce bit-identical results on any host — useful for reproducible testing, cross-host A/B benchmarks, or pinning a deterministic kernel route without recompiling.

**What this does not touch.** The OpenBLAS / LAPACKE-backed heavy linear-algebra routines (decompositions, matmul, SVD, Cholesky, LU) and per-element libm transcendental ops (`pow`, `fmod`, `exp`, `log`, `sin`, …) are unaffected — they are scalar either way, and the thread pool is controlled by `setNumThreads()`.

**Idempotence.** The call is cheap: it rewrites a handful of function-pointer slots to their file-scope initial values. Calling it multiple times has no further effect.

**Thread-safety.** This writes to the global kernel-route slots. Call from a single worker before any tensor op runs, if the application is multi-process (Swoole, FPM workers, etc.). Do not call from inside a concurrent worker.

### `enableOptimizedKernels() : void`

Route the dispatched C kernels back through their widest usable variants. Called automatically when the extension starts.

**On a CPU with no wide instruction set,** or off x86 entirely, every feature bit is clear and the routes stay on their baseline variants. That is the correct outcome, not a failed upgrade: the purpose of the feature detection is that unsupported instructions are never reachable.

**Idempotence.** Like the disable, calling it multiple times has no further effect — every route pointer starts out on its baseline variant and each init is a plain assignment, so replaying the selection can only ever re-install a route the CPU already supports.

**Thread-safety.** The same caveat as `disableOptimizedKernels()`: this writes the global kernel-route slots, so call it from a single worker before any tensor op runs.
