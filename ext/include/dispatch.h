#ifndef TENSOR_DISPATCH_H
#define TENSOR_DISPATCH_H

#include <Zend/zend.h>

/**
 * Runtime CPU kernel dispatch.
 *
 * The extension is compiled with a plain -O3 and no ISA flags, so the baseline
 * code generation for x86-64 tops out at the SSE2 baseline: two doubles per
 * vector. AVX widens that to four doubles and AVX-512 to eight. A global
 * -mavx (or -mavx512f) is not an option -- it would let the compiler emit the
 * wider instructions anywhere in the extension and turn the load of a CPU that
 * does not support them into a SIGILL -- so the wider kernels instead carry a
 * per-function target attribute, which confines the wider instructions to
 * functions that are only ever reached through a pointer that cpu.c installs
 * when the CPU has been found to support the ISA.
 *
 * Each kernel is therefore compiled three times from one identical C body: once
 * as the plain baseline, once under TENSOR_TARGET_AVX, and once under
 * TENSOR_TARGET_AVX512. The copies cannot drift semantically because the
 * compiler picks the instruction selection, not us.
 *
 * There is deliberately no AVX2 tier. These kernels are double-precision
 * elementwise, and AVX2 offers the same 256-bit width as AVX for doubles --
 * its new operations are 256-bit integer, gather and permute instructions that
 * this code never uses. A second 256-bit route would be code-sized for no
 * measurable speedup, so AVX2 is detected and reported for diagnostics only.
 *
 * No intrinsic is used anywhere in this file or in the kernels it serves; the
 * target attribute alone is enough for the compiler to widen every loop we
 * dispatch, which keeps <immintrin.h> -- and the portability caveats that come
 * with it -- out of the build entirely.
 *
 * Five sets of kernels are dispatched: the arithmetic kernels in
 * include/arithmetic.c, the elementwise comparison kernels in
 * include/comparison.c, the elementwise unary kernels in include/unary.c, linear
 * algebra kernels in include/linear_algebra.c, and the convolution kernels in
 * include/signal_processing.c.
 *
 * Per-element libm calls -- pow, fmod, and exp, log, sin and the rest of the
 * transcendental unary operations -- are scalar no matter which ISA is
 * enabled, because the register only widens the call, not the work inside it.
 * These are left alone deliberately, not overlooked.
 */

/* Signatures shared by the dispatched kernels. `_scalar` operations take the
 * same shape as the plain binary ones: the scalar arrives as a zval, so
 * tensor_add_scalar is a tensor_binary_fn just like tensor_add. */
typedef void (*tensor_binary_fn)(zval * return_value, zval * a, zval * b);
typedef void (*tensor_dim_fn)(zval * return_value, zval * a, zval * b, zval * n);

/* The unary signatures. A plain unary op carries only its input; clipLower and
 * clipUpper take one bound and clip takes two, so each arity gets its own. */
typedef void (*tensor_unary_fn)(zval * return_value, zval * a);
typedef void (*tensor_unary_bound_fn)(zval * return_value, zval * a, zval * bound);
typedef void (*tensor_unary_clip_fn)(zval * return_value, zval * a, zval * lo, zval * hi);

/* The convolution signatures. Both carry the stride and the padding, and the 2D
 * kernel also carries the four extents the Zephir layer already knows, so that
 * the shape of the result can be validated against the buffers without a second
 * trip through the object. */
typedef void (*tensor_convolve_1d_fn)(zval * return_value, zval * a, zval * b, zval * stride, zval * padding);
typedef void (*tensor_convolve_2d_fn)(zval * return_value, zval * a, zval * b, zval * stride, zval * padding, zval * ma, zval * na, zval * mb, zval * nb);

/* True when the target attribute below expands to a real ISA override. */
#if (defined(__x86_64__) || defined(__i386__)) && \
	(defined(__GNUC__) || defined(__clang__))
#	define TENSOR_X86_DISPATCH 1
#	define TENSOR_TARGET_AVX __attribute__((target("avx")))
#	define TENSOR_TARGET_AVX512 __attribute__((target("avx512f")))
/* The FMA routes are separate from the AVX and AVX-512 ones above, and neither
 * implies the other: a CPU can have 256-bit AVX and no FMA3 (Sandy Bridge, Ivy
 * Bridge), and `target("avx512f")` on its own leaves GCC emitting a packed
 * multiply and add rather than a fused one. */
#	define TENSOR_TARGET_FMA __attribute__((target("avx,fma")))
#	define TENSOR_TARGET_AVX512_FMA __attribute__((target("avx512f,fma")))
#else
#	define TENSOR_TARGET_AVX
#	define TENSOR_TARGET_AVX512
#	define TENSOR_TARGET_FMA
#	define TENSOR_TARGET_AVX512_FMA
#endif

void tensor_arithmetic_dispatch_sse_init(void);
void tensor_arithmetic_dispatch_avx_init(void);
void tensor_arithmetic_dispatch_avx512_init(void);

void tensor_comparison_dispatch_sse_init(void);
void tensor_comparison_dispatch_avx_init(void);
void tensor_comparison_dispatch_avx512_init(void);

void tensor_unary_dispatch_sse_init(void);
void tensor_unary_dispatch_avx_init(void);
void tensor_unary_dispatch_avx512_init(void);

void tensor_linear_algebra_dispatch_sse_init(void);
void tensor_linear_algebra_dispatch_avx_init(void);
void tensor_linear_algebra_dispatch_fma_init(void);
void tensor_linear_algebra_dispatch_avx512_init(void);

void tensor_signal_processing_dispatch_sse_init(void);
void tensor_signal_processing_dispatch_avx_init(void);
void tensor_signal_processing_dispatch_fma_init(void);
void tensor_signal_processing_dispatch_avx512_init(void);

#endif
