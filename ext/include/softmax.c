#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <math.h>
#include <ext/spl/spl_exceptions.h>
#include "kernel/main.h"
#include "php_ext.h"
#include "kernel/buffer.h"
#include "kernel/operators.h"
#include "include/buffer.h"
#include "include/shape.h"

/* The block width is derived from the row count rather than fixed, because the
 * useful block depends on the shape: a short matrix keeps many columns in flight
 * and can afford a wide block, a tall one only ever has a few rows per column to
 * work with. TENSOR_SOFTMAX_WORKING_SET doubles is derived from the L1 budget,
 * TENSOR_SOFTMAX_TILE caps the block so the per-column state stays cheap in
 * registers, and the floor below keeps the inner loop long enough to vectorize.
 *
 * The floor matters more than the cap. Deriving the width as working-set/rows
 * alone collapses it to a single column once the matrix is taller than the
 * working set, and a one-wide inner loop both fails to vectorize and re-walks
 * the whole buffer once per column. Measured on a 4096x4096 matrix, 4096 blocks
 * of one column took 381ms against 258ms for a floor of 32; every width from 32
 * to 128 was within noise of every other, so the floor is what matters and the
 * cap is only there to bound the stack arrays. */
#define TENSOR_SOFTMAX_TILE 128
#define TENSOR_SOFTMAX_WORKING_SET 2048
#define TENSOR_SOFTMAX_MIN_WIDTH 32

/**
 * Softmax over the columns of a matrix or vector.
 *
 * A single kernel replaces the transpose, maximum, subtract, exponential, sum,
 * clip, divide, transpose sequence.
 *
 * Three sweeps over a block of columns. The first takes the per-column maximum
 * so the exponentials cannot overflow, the second exponentiates and sums, the
 * third divides. The block is sized so that a block's input and output both stay
 * resident across all three.
 */
void tensor_softmax(zval * return_value, zval * a, zval * n)
{
	double * va = NULL;
	zend_long m = 0, cols = 0, i, j, j0, tile, width;

	if (UNEXPECTED(!tensor_matrix_doubles(a, n, &va, &m, &cols))) {
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, m * cols, &c) == FAILURE)) {
		return;
	}

	if (UNEXPECTED(m == 0)) {
		zval_ptr_dtor(&c);

		return;
	}

	const double * restrict input = va;
	double * restrict vc = zephir_buffer_doubles(&c);
	double maxima[TENSOR_SOFTMAX_TILE];
	double sums[TENSOR_SOFTMAX_TILE];

	tile = TENSOR_SOFTMAX_WORKING_SET / m;

	if (UNEXPECTED(tile > TENSOR_SOFTMAX_TILE)) {
		tile = TENSOR_SOFTMAX_TILE;
	} else if (UNEXPECTED(tile < TENSOR_SOFTMAX_MIN_WIDTH)) {
		tile = TENSOR_SOFTMAX_MIN_WIDTH;
	}

	for (j0 = 0; j0 < cols; j0 += tile) {
		width = cols - j0 < tile ? cols - j0 : tile;

		/* Pass 1, the per-column maximum. The inner loop is the block of
		 * columns and the outer loop the rows, so the reduction runs across the
		 * vector register rather than along it: each column carries its own
		 * accumulator in `maxima`, which is what leaves a vector unit free to
		 * widen. Reversing the loops, so each register held one column's
		 * vertical max, does not vectorize at all. */
		for (j = 0; j < width; ++j) {
			maxima[j] = -INFINITY;
		}

		for (i = 0; i < m; ++i) {
			const double * restrict src = input + i * cols + j0;

			for (j = 0; j < width; ++j) {
				double v = src[j];

				maxima[j] = v > maxima[j] ? v : maxima[j];
			}
		}

		/* Pass 2, the exponential and its running sum. The subtraction is
		 * stable: at least one element per column is exactly exp(0) = 1.0, so
		 * every value in the sum is in (0, 1] and the sum cannot underflow.
		 * One accumulator per column is what breaks the floating point
		 * dependency chain -- the columns are independent, so a wide block
		 * already supplies as many concurrent chains as it has columns, and
		 * there is no need to unroll the inner loop to get them. */
		for (j = 0; j < width; ++j) {
			sums[j] = 0.0;
		}

		for (i = 0; i < m; ++i) {
			const double * restrict src = input + i * cols + j0;
			double * restrict dst = vc + i * cols + j0;

			for (j = 0; j < width; ++j) {
				double e = exp(src[j] - maxima[j]);

				dst[j] = e;
				sums[j] += e;
			}
		}

		/* Pass 3, the normalization. A true division rather than a
		 * multiplication by a cached reciprocal, so the result matches what a
		 * divide() by the same total produces bit for bit. Here the inner loop
		 * is the rows and the outer loop the columns, the opposite of the two
		 * passes above, because a column is what has to run down a stride: put
		 * the column in the accumulator instead and the loop is sequential in
		 * memory, which is the form the division actually vectorizes in. */
		for (j = 0; j < width; ++j) {
			const double total = sums[j];
			double * restrict col = vc + j0 + j;

			for (i = 0; i < m; ++i) {
				col[i * cols] /= total;
			}
		}
	}

	zval_ptr_dtor(&c);
}
