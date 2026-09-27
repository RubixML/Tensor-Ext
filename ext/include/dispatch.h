#ifndef TENSOR_DISPATCH_H
#define TENSOR_DISPATCH_H

#include <Zend/zend.h>

/**
 * Runtime CPU dispatch for the elementwise kernels.
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
 * One kernel needs a little help beyond the target attribute. The compiler
 * will not vectorize sqrt() because the libm function may set errno, and a
 * target attribute does not lift that restriction, so the build carries
 * -fno-math-errno (see extra-cflags in config.json). Nothing in the extension
 * ever reads errno, and vsqrtpd returns the same double sqrtsd does, so the
 * routes remain bit-for-bit identical; it is worth checking that stays true if
 * the flag is ever dropped.
 *
 * No intrinsic is used anywhere in this file or in the kernels it serves; the
 * target attribute alone is enough for the compiler to widen every loop we
 * dispatch, which keeps <immintrin.h> -- and the portability caveats that come
 * with it -- out of the build entirely.
 *
 * Three sets of kernels are dispatched: the arithmetic kernels in
 * include/arithmetic.c, the elementwise comparison kernels in
 * include/comparison.c, and the elementwise unary kernels in include/unary.c.
 * The remaining kernels in those files keep a single baseline route, because
 * they are per-element libm calls.
 *
 * Per-element libm calls -- pow, fmod, and exp, log, sin and the rest of the
 * transcendental unary operations -- are scalar no matter which ISA is
 * enabled, because the register only widens the call, not the work inside it.
 * These are left alone deliberately, not overlooked. Closing that gap needs a
 * vector math library (SLEEF being the only portable one; the glibc _ZGV*_
 * symbols are x86-only) rather than a wider route, so it is a dependency
 * decision and not a dispatch one.
 *
 * floor and ceil were in that group until they were measured. They are not a
 * scalar library call in any meaningful sense -- each is a single exact
 * operation -- but SSE2 has no rounding instruction, so the baseline route had
 * to inline libm's floor() as a branchy sequence that the compiler then refused
 * to vectorize, which cost about 1.2 ns/element against 0.57 for abs. Under
 * TENSOR_TARGET_AVX the same line is one vroundsd and the branch goes away, so
 * include/unary.c now dispatches them. Two lessons generalise. First, before
 * concluding that a kernel cannot be dispatched, check whether the obstacle is
 * the operation or the ISA baseline. Second, dispatching a kernel does not
 * guarantee it widens: floor still processes one double per instruction because
 * GCC will not select the packed vrndscalepd, even under `#pragma omp simd`.
 * The distantly related case is that AVX2 is reported for diagnostics only, for
 * the opposite reason -- it is available and useful but adds nothing for double
 * precision.
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

/* True when the target attribute below expands to a real ISA override. */
#if (defined(__x86_64__) || defined(__i386__)) && \
	(defined(__GNUC__) || defined(__clang__))
#	define TENSOR_X86_DISPATCH 1
#	define TENSOR_TARGET_AVX __attribute__((target("avx")))
#	define TENSOR_TARGET_AVX512 __attribute__((target("avx512f")))
#else
#	define TENSOR_TARGET_AVX
#	define TENSOR_TARGET_AVX512
#endif

/* Installed once from the module initializer in config.json. The hooks are
 * idempotent and only ever upgrade a kernel from its baseline to its AVX or
 * AVX-512 variant, so an extension whose initializer never ran still computes
 * the right answers, just without the wider vectors. */
void tensor_arithmetic_dispatch_avx_init(void);
void tensor_unary_dispatch_avx_init(void);
void tensor_comparison_dispatch_avx_init(void);
void tensor_arithmetic_dispatch_avx512_init(void);
void tensor_unary_dispatch_avx512_init(void);
void tensor_comparison_dispatch_avx512_init(void);

#endif
