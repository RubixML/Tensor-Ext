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
#include "include/reductions.h"
#include "include/shape.h"

/* The lane count is the number of rows carried through the three passes at
 * once, and it is derived from the row length rather than fixed, because the
 * useful value depends on the shape. A wide row is self-sufficient -- the
 * exponentials inside it are independent and the sum helper already unrolls
 * eight ways -- so one at a time is all it needs, and more would only push the
 * block past the cache and make the normalization re-read from memory. A narrow
 * row has almost no work to interleave, so several are kept in flight and the
 * per-row overhead is amortized across them.
 *
 * TENSOR_SOFTMAX_WORKING_SET doubles is the L1 budget a lane block is sized
 * against and TENSOR_SOFTMAX_LANES caps it so the per-row state stays in
 * registers. */
#define TENSOR_SOFTMAX_WORKING_SET 2048
#define TENSOR_SOFTMAX_LANES 2

/**
 * Softmax over the rows of a matrix or vector.
 *
 * A single kernel replaces the maximum, subtract, exponential, sum, divide
 * sequence. Three sweeps over a block of rows. The first takes the per-row
 * maximum so the exponentials cannot overflow, the second exponentiates and
 * sums, the third divides. The block is sized so that a block's input and
 * output both stay resident across all three.
 */
void tensor_softmax(zval * return_value, zval * a, zval * n)
{
	double * va = NULL;
	zend_long m = 0, cols = 0, i, j, i0, tile, lanes;
	
	zend_long cols_hat = zephir_get_intval(n);

	if (cols_hat < 1) {
		cols_hat = 1;
	}

	if (UNEXPECTED(!tensor_matrix_doubles_len(a, cols_hat, &va, &m, &cols))) {
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
	double maxima[TENSOR_SOFTMAX_LANES];
	double sums[TENSOR_SOFTMAX_LANES];

	lanes = TENSOR_SOFTMAX_WORKING_SET / cols;

	if (UNEXPECTED(lanes > TENSOR_SOFTMAX_LANES)) {
		lanes = TENSOR_SOFTMAX_LANES;
	} else if (UNEXPECTED(lanes < 1)) {
		lanes = 1;
	}

	for (i0 = 0; i0 < m; i0 += lanes) {
		tile = m - i0 < lanes ? m - i0 : lanes;

		/* Pass 1, the per-row maximum. */
		for (i = 0; i < tile; ++i) {
			const double * restrict src = input + (i0 + i) * cols;
			double best = src[0];

			for (j = 1; j < cols; ++j) {
				double v = src[j];

				best = v > best ? v : best;
			}

			maxima[i] = best;
		}

		/* Pass 2, the exponential and its sum. The subtraction is stable: at
		 * least one element per row is exactly exp(0) = 1.0, so every value in
		 * the sum is in (0, 1] and the sum cannot underflow.
		 *
		 * The sum is left to tensor_sum_doubles rather than accumulated here.
		 * That helper is the same one the sum reduction runs, so the total is
		 * bit-for-bit the total a subtract/maximum/exp/sum/divide composition
		 * produces, which is what lets the kernel stand in for that sequence
		 * without a tolerance. Its eight partial sums also keep the row's
		 * dependency chain from serializing the exponentials. */
		for (i = 0; i < tile; ++i) {
			const double * restrict src = input + (i0 + i) * cols;
			double * restrict dst = vc + (i0 + i) * cols;
			const double shift = maxima[i];

			for (j = 0; j < cols; ++j) {
				dst[j] = exp(src[j] - shift);
			}

			sums[i] = tensor_sum_doubles(dst, cols);
		}

		/* Pass 3, the normalization. Contiguous along the row, so unlike the
		 * strided form the column-wise version needed this is the shape the
		 * division vectorizes in. A true division rather than a multiplication
		 * by a cached reciprocal, so the result matches what a divide() by the
		 * same total produces bit for bit. */
		for (i = 0; i < tile; ++i) {
			double * restrict dst = vc + (i0 + i) * cols;
			const double total = sums[i];

			for (j = 0; j < cols; ++j) {
				dst[j] /= total;
			}
		}
	}

	zval_ptr_dtor(&c);
}
