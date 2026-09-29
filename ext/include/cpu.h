#ifndef TENSOR_CPU_H
#define TENSOR_CPU_H

/**
 * One-time CPU feature detection.
 *
 * Feature bits are captured on the first call to any of the accessors below and
 * cached for the life of the process, so the cost of asking is a load and a
 * test. The dispatch hooks read these bits from the module initializer, and
 * again from tensor_enable_optimized_kernels() in include/settings.c, but
 * neither ever pays for the detection twice.
 */

int tensor_cpu_has_avx(void);
int tensor_cpu_has_avx2(void);
int tensor_cpu_has_avx512(void);

/* FMA3, which is a separate feature bit from AVX rather than a consequence of
 * it. */
int tensor_cpu_has_fma(void);

/* Install the widest set of kernels the CPU allows. Called once from MINIT,
 * and again whenever the user asks to re-enable the optimized kernels: the
 * feature bits are cached, so the repeat call is just the route-pointer
 * stores. Because every kernel pointer starts out on its baseline variant
 * and each init is a plain assignment, this is idempotent and can only ever
 * upgrade a route. */
void tensor_cpu_init(void);

#endif
