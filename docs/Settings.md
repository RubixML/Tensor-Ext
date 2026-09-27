# Settings

A static API for global runtime settings: the OpenBLAS thread pool and the CPU feature detection the extension performed at startup.

- **Namespace:** `Tensor\Settings`

## Overview

`Settings` is a collection of stateless, static helpers. It has no instance state and cannot be constructed — every method is static.

`setNumThreads()` and `numThreads()` wrap OpenBLAS' `openblas_set_num_threads()` / `openblas_get_num_threads()` so the number of CPU threads OpenBLAS uses for its parallel kernels can be tuned and inspected at runtime. `cpuFeatures()` reports the CPU feature flags the extension detected at startup together with the route the elementwise kernels actually took.

## Methods

### `setNumThreads(int $threads) : void`

Set the number of CPU threads to use when multiprocessing.

- **Parameters:** `$threads` — the number of threads to use; must be greater than `0`
- **Throws:** `Tensor\Exceptions\InvalidArgumentException` if `$threads` is less than `1`

### `numThreads() : int`

Return the number of CPU threads currently in use for multiprocessing.

### `cpuFeatures() : array`

Return the CPU features the extension detected at startup.

The returned array has three keys:

| Key | Type | Meaning |
| --- | --- | --- |
| `avx` | `bool` | Whether the CPU exposes the AVX (256-bit) instruction set. |
| `avx2` | `bool` | Whether the CPU exposes the AVX2 instruction set. Reported for diagnostics only — the dispatched double-precision kernels gain no throughput from a second 256-bit route. |
| `avx512` | `bool` | Whether the CPU exposes the AVX-512F (512-bit) instruction set. |
