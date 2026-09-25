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

/* The route the elementwise kernels actually took: "avx" or "scalar". */
const char * tensor_cpu_dispatch_name(void);

/* Install the AVX kernels where the CPU allows it. Called once from MINIT. */
void tensor_cpu_init(void);

#endif
