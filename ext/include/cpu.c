#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include "include/cpu.h"
#include "include/dispatch.h"

#if TENSOR_X86_DISPATCH

#include <cpuid.h>

/* CPUID leaf 1, ECX. */
#define TENSOR_CPUID_ECX_AVX      0x10000000u /* bit 28 */
#define TENSOR_CPUID_ECX_OSXSAVE  0x08000000u /* bit 27 */

/* XCR0 bits covering the SSE and AVX (YMM) register state. */
#define TENSOR_XCR0_SSE_AVX 0x6u

/* XCR0 bits covering the state AVX-512 needs saved on context switch: the
 * SSE and AVX bits above, plus the OPMASK (bit 5) and ZMM_Hi256 (bit 6) state
 * that AVX-512F introduces. All four must be set before trusting the leaf-7
 * AVX-512F bit. */
#define TENSOR_XCR0_SSE_AVX_AVX512 (0x6u | 1u << 5 | 1u << 6)

/* __builtin_cpu_supports() landed in GCC 4.8 and Clang 3.9. It is the better
 * of the two paths below because it performs the OSXSAVE and XGETBV checks
 * itself, so it never reports AVX on a kernel that has not enabled YMM state
 * saving. */
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
#	define TENSOR_HAVE_BUILTIN_CPU_SUPPORTS 1
#endif

/* Read XCR0. Encoded by hand rather than with the xgetbv mnemonic so that the
 * fallback does not depend on the assembler understanding it. */
static uint64_t tensor_xgetbv0(void)
{
	uint32_t lo, hi;

	__asm__ __volatile__(".byte 0x0f, 0x01, 0xd0" : "=a"(lo), "=d"(hi) : "c"(0));

	return ((uint64_t) hi << 32) | (uint64_t) lo;
}

#endif

typedef struct {
	int avx;
	int avx2;
	int avx512;
} tensor_cpu_features;

static tensor_cpu_features tensor_cpu_detected = { 0, 0, 0 };
static int tensor_cpu_detected_ready = 0;

/**
 * Sample the CPU feature bits once and cache them.
 *
 * On anything that is not x86 with a GCC-compatible compiler this leaves every
 * bit clear, which routes the kernels to their baseline variants.
 */
static void tensor_cpu_detect(void)
{
#if TENSOR_X86_DISPATCH

	unsigned int eax = 0, ebx = 0, ecx = 0, edx = 0;

	if (!__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
		return;
	}

#	ifdef TENSOR_HAVE_BUILTIN_CPU_SUPPORTS

	__builtin_cpu_init();

	/* NB: __builtin_cpu_supports() yields a nonzero bitmask rather than 1, so
	 * these must be tested for truth and not compared against 1. */
	tensor_cpu_detected.avx = __builtin_cpu_supports("avx") ? 1 : 0;
	tensor_cpu_detected.avx2 = __builtin_cpu_supports("avx2") ? 1 : 0;
	tensor_cpu_detected.avx512 = __builtin_cpu_supports("avx512f") ? 1 : 0;

#	else

	/* CPUID on its own is not sufficient evidence that AVX is usable. The CPU
	 * may advertise it while the kernel has never enabled saving of the YMM
	 * register state, in which case the first AVX instruction raises #UD. Both
	 * OSXSAVE and the XCR0 bits have to be checked before believing bit 28.
	 * The same reasoning extends to AVX-512, whose use also depends on the
	 * OPMASK and ZMM_Hi256 state bits. OSXSAVE is the gate for reading XCR0,
	 * so xgetbv() is only called inside its guard. */
	uint64_t xcr0 = 0;

	if (ecx & TENSOR_CPUID_ECX_OSXSAVE) {
		xcr0 = tensor_xgetbv0();

		if ((xcr0 & TENSOR_XCR0_SSE_AVX) == TENSOR_XCR0_SSE_AVX) {
			tensor_cpu_detected.avx = (ecx & TENSOR_CPUID_ECX_AVX) ? 1 : 0;
		}
	}

	if (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
		tensor_cpu_detected.avx2 = (ebx & (1u << 5)) ? 1 : 0;

		/* AVX-512F (leaf-7 EBX bit 16) needs, in addition to AVX being usable
		 * (checked above), the OPMASK and ZMM_Hi256 state bits set in XCR0.
		 * xcr0 is zero (and thus the mask test fails) when OSXSAVE was clear,
		 * so no extra guard is required. */
		if ((xcr0 & TENSOR_XCR0_SSE_AVX_AVX512) == TENSOR_XCR0_SSE_AVX_AVX512) {
			tensor_cpu_detected.avx512 = (ebx & (1u << 16)) ? 1 : 0;
		}
	}

#	endif

#endif
}

int tensor_cpu_has_avx(void)
{
	if (!tensor_cpu_detected_ready) {
		tensor_cpu_detect();

		tensor_cpu_detected_ready = 1;
	}

	return tensor_cpu_detected.avx;
}

int tensor_cpu_has_avx2(void)
{
	if (!tensor_cpu_detected_ready) {
		tensor_cpu_detect();

		tensor_cpu_detected_ready = 1;
	}

	return tensor_cpu_detected.avx2;
}

int tensor_cpu_has_avx512(void)
{
	if (!tensor_cpu_detected_ready) {
		tensor_cpu_detect();

		tensor_cpu_detected_ready = 1;
	}

	return tensor_cpu_detected.avx512;
}

/**
 * Route the elementwise kernels through their widest usable variant, giving
 * precedence to AVX-512 and falling back to AVX. Called once from the module
 * initializer; see the `initializers.module` entry in config.json.
 *
 * Only one set of hooks runs, so a route ends up pointing at exactly one
 * non-baseline variant. Every kernel pointer starts out pointing at its
 * baseline variant, so this function can only ever upgrade a route. An
 * extension whose initializer did not run still returns correct results, just
 * at the narrower width.
 */
void tensor_cpu_init(void)
{
	if (tensor_cpu_has_avx512()) {
		tensor_arithmetic_dispatch_avx512_init();
		tensor_comparison_dispatch_avx512_init();
		tensor_unary_dispatch_avx512_init();
	} else if (tensor_cpu_has_avx()) {
		tensor_arithmetic_dispatch_avx_init();
		tensor_comparison_dispatch_avx_init();
		tensor_unary_dispatch_avx_init();
	}
}
