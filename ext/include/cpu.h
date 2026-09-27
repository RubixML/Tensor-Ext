#ifndef TENSOR_CPU_H
#define TENSOR_CPU_H

/**
 * One-time CPU feature detection.
 *
 * Feature bits are captured on the first call to any of the accessors below and
 * cached for the life of the process, so the cost of asking is a load and a
 * test. The dispatch hooks in cpu.c read these bits exactly once, from the
 * module initializer.
 */

int tensor_cpu_has_avx(void);
int tensor_cpu_has_avx2(void);
int tensor_cpu_has_avx512(void);

/* FMA3, which is a separate feature bit from AVX rather than a consequence of
 * it. Used only by the convolution kernels, which are the only ones with a
 * multiply-accumulate inner loop long enough to benefit from the fused form. */
int tensor_cpu_has_fma(void);

/* Install the widest set of kernels the CPU allows. Called once from MINIT. */
void tensor_cpu_init(void);

#endif
