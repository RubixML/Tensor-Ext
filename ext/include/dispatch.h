#ifndef TENSOR_DISPATCH_H
#define TENSOR_DISPATCH_H

#include <Zend/zend.h>

/**
 * Runtime CPU dispatch for the elementwise kernels.
 *
 * The extension is compiled with a plain -O3 and no ISA flags, so the baseline
 * code generation for x86-64 tops out at the SSE2 baseline: two doubles per
 * vector. AVX widens that to four. A global -mavx is not an option -- it would
 * let the compiler emit AVX anywhere in the extension and turn the load of a
 * non-AVX CPU into a SIGILL -- so the AVX kernels instead carry a per-function
 * target attribute, which confines the wider instructions to functions that are
 * only ever reached through a pointer that cpu.c installs when the CPU has been
 * found to support AVX.
 *
 * Each kernel is therefore compiled twice from one identical C body: once as
 * the plain baseline and once under TENSOR_TARGET_AVX. The two cannot drift
 * semantically because the compiler picks the instruction selection, not us.
 *
 * No intrinsic is used anywhere in this file or in the kernels it serves; the
 * target attribute alone is enough for the compiler to widen these loops, which
 * keeps <immintrin.h> -- and the portability caveats that come with it -- out of
 * the build entirely.
 *
 * Scope is the arithmetic kernels in include/arithmetic.c only. The unary
 * kernels in include/unary.c are deliberately *not* dispatched.
 */

/* Signatures shared by the dispatched kernels. `_scalar` operations take the
 * same shape as the plain binary ones: the scalar arrives as a zval, so
 * tensor_add_scalar is a tensor_binary_fn just like tensor_add. */
typedef void (*tensor_binary_fn)(zval * return_value, zval * a, zval * b);
typedef void (*tensor_dim_fn)(zval * return_value, zval * a, zval * b, zval * n);

/* True when the target attribute below expands to a real ISA override. */
#if (defined(__x86_64__) || defined(__i386__)) && \
	(defined(__GNUC__) || defined(__clang__))
#	define TENSOR_X86_DISPATCH 1
#	define TENSOR_TARGET_AVX __attribute__((target("avx")))
#else
#	define TENSOR_TARGET_AVX
#endif

/* Installed once from the module initializer in config.json. The hook is
 * idempotent and only ever upgrades a kernel from its baseline to its AVX
 * variant, so an extension whose initializer never ran still computes the right
 * answers, just without the wider vectors. */
void tensor_arithmetic_dispatch_avx_init(void);

#endif
