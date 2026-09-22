#ifndef TENSOR_BUFFER_UNARY_H
#define TENSOR_BUFFER_UNARY_H

#include <Zend/zend.h>

/**
 * Element-wise unary maths over a <Ns>\Buffer.
 *
 * These replace seventeen array_map("<phpfunc>", ...) calls and five hand-rolled
 * Zephir loops. array_map with a function name is the worst case in the library:
 * a full PHP function-call frame per element, around which the arithmetic is
 * noise. The cheaper the maths, the worse the ratio -- which is why sqrt trailed
 * exp by more than a factor of two before this.
 *
 * The operation is selected by name rather than by an opcode so that the .zep
 * call sites stay readable and no opcode constants leak onto the public class
 * surface. The string is resolved once per call, never per element, and each
 * operation then runs its own tight loop so -O3 can vectorise it.
 */
void tensor_buffer_unary(zval * return_value, zval * a, zval * op);

/** Unary operations taking one parameter: log(base), round(precision), clipLower, clipUpper. */
void tensor_buffer_unary_arg(zval * return_value, zval * a, zval * op, zval * arg);

/** Clamp to [min, max]. Separate because it is the only one taking two. */
void tensor_buffer_clip(zval * return_value, zval * a, zval * min, zval * max);

/**
 * Reductions to a single double: sum, product, min, max.
 *
 * Summation is sequential, deliberately. A blocked or vectorised sum would
 * reassociate the additions and move the last ulps away from what array_sum()
 * returned, for a reduction that is already memory-bound.
 */
void tensor_buffer_reduce(zval * return_value, zval * a, zval * op);

#endif
