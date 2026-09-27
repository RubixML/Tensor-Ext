#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <stdlib.h>
#include <string.h>
#include <ext/spl/spl_exceptions.h>
#include "kernel/main.h"
#include "php_ext.h"
#include "kernel/buffer.h"
#include "kernel/exception.h"
#include "kernel/operators.h"
#include "include/buffer.h"
#include "include/shape.h"
#include "../tensor/exceptions/invalidargumentexception.zep.h"

/**
 * Row-wise operations work directly on the flat row-major buffer that backs a
 * Matrix (m * n doubles) instead of materializing per-row Buffers. This
 * helper unwraps the buffer and validates that it can be divided into whole
 * rows of length n, mirroring the semantics of `TensorBuffer::split()`. The
 * derived row count `*m` and the raw data pointer are written on success.
 *
 * The row-wise statistical operations in reductions.c share this rather than
 * carrying their own copy.
 *
 * @return 1 on success, 0 on failure (throwing).
 */
int tensor_matrix_doubles(zval * obj, zval * n_zval, double ** ptr, zend_long * m, zend_long * n)
{
	zend_long total = 0;
	int ok = 0;

	double * va = tensor_tensorbuffer_doubles(obj, &total, &ok);

	if (UNEXPECTED(!ok)) {
		return 0;
	}

	zend_long n_hat = zephir_get_intval(n_zval);

	if (UNEXPECTED(n_hat < 1)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Chunk length must be greater than 0."));
		return 0;
	}

	if (UNEXPECTED(total % n_hat != 0)) {
		zephir_throw_exception_string(spl_ce_LengthException,
			SL("Matrix and row dimensions must agree."));
		return 0;
	}

	*ptr = va;
	*n = n_hat;
	*m = total / n_hat;

	return 1;
}

/**
 * Return the transpose of the matrix as a new matrix buffer, i.e. row i of the
 * output holds column i of the input. Eliminates the strided per-column slices
 * and concatenation used by the previous pure-Zephir path.
 *
 * @param return_value
 * @param a
 * @param m
 * @param n
 */
void tensor_matrix_transpose(zval * return_value, zval * a, zval * m, zval * n)
{
	zend_long total = 0;
	int ok = 0;

	double * va = tensor_tensorbuffer_doubles(a, &total, &ok);

	if (UNEXPECTED(!ok)) {
		return;
	}

	zend_long ma = zephir_get_intval(m);
	zend_long na = zephir_get_intval(n);

	if (UNEXPECTED(ma < 0 || na < 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Dimensions must be non-negative."));
		return;
	}

	if (UNEXPECTED(total != ma * na)) {
		zephir_throw_exception_string(spl_ce_LengthException,
			SL("Input buffer must match the given dimensions."));
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, total, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	if (total > 0) {
		zend_long i, j;

		for (i = 0; i < ma; ++i) {
			const double * src = va + i * na;
			double * dst = vc + i;

			for (j = 0; j < na; ++j) {
				dst[j * ma] = src[j];
			}
		}
	}

	zval_ptr_dtor(&c);
}

/**
 * Return a new matrix buffer with rows and columns repeated the given number
 * of times. Result element (r, c) is element (r % m, c % n) of the input,
 * yielding (m * (times_m + 1)) rows and (n * (times_n + 1)) columns.
 *
 * Throws an InvalidArgumentException when either time is negative, or when any
 * of the resulting counts would overflow a zend_long.
 *
 * @param return_value
 * @param obj
 * @param n
 * @param times_m
 * @param times_n
 */
void tensor_matrix_repeat(zval * return_value, zval * obj, zval * n, zval * times_m, zval * times_n)
{
	double * va = NULL;
	zend_long m = 0, n_hat = 0;
	zend_long t_m = 0, t_n = 0;

	if (UNEXPECTED(!tensor_matrix_doubles(obj, n, &va, &m, &n_hat))) {
		return;
	}

	t_m = zephir_get_intval(times_m);
	t_n = zephir_get_intval(times_n);

	if (UNEXPECTED(t_m < 0 || t_n < 0)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Times must be non-negative."));
		return;
	}

	/* Every count below is a zend_long, so reject the repeat counts that would
	 * wrap before any of them is computed. A wrapped count is not merely a
	 * wrong answer: rows * cols can fold back to a small or negative element
	 * count, the destination buffer is then under-allocated, and the copy loop
	 * below still strides by the unwrapped rows and cols, so it reads and
	 * writes far past the end of the allocation. Dividing instead of
	 * multiplying keeps the guards exact on the boundary value. */

	/* times + 1 */
	if (UNEXPECTED(t_m == ZEND_LONG_MAX || t_n == ZEND_LONG_MAX)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Repeat count must not overflow the matrix dimensions."));
		return;
	}

	/* m * (times_m + 1) and n * (times_n + 1) */
	if (UNEXPECTED((m > 0 && t_m > ZEND_LONG_MAX / m - 1) ||
		(n_hat > 0 && t_n > ZEND_LONG_MAX / n_hat - 1))) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Repeat count must not overflow the matrix dimensions."));
		return;
	}

	zend_long rows = m * (t_m + 1);
	zend_long cols = n_hat * (t_n + 1);

	/* rows * cols, the number of elements in the result */
	if (UNEXPECTED(rows > 0 && cols > ZEND_LONG_MAX / rows)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Repeat count must not overflow the matrix dimensions."));
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, rows * cols, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	zend_long i, k;

	for (i = 0; i < rows; ++i) {
		const double * src = va + (i % m) * n_hat;
		double * dst = vc + i * cols;

		for (k = 0; k <= t_n; ++k) {
			memcpy(dst + k * n_hat, src, (size_t) n_hat * sizeof(double));
		}
	}

	zval_ptr_dtor(&c);
}

/**
 * Return the matrix as an array of arrays, built directly from the flat
 * row-major buffer.
 *
 * @param return_value
 * @param obj
 * @param n
 */
void tensor_matrix_to_array(zval * return_value, zval * obj, zval * n)
{
	double * va = NULL;
	zend_long m = 0, n_hat = 0;
	zend_long i, j;

	if (UNEXPECTED(!tensor_matrix_doubles(obj, n, &va, &m, &n_hat))) {
		return;
	}

	array_init_size(return_value, (zend_ulong) m);

	for (i = 0; i < m; ++i) {
		const double * row = va + i * n_hat;
		zval row_value;

		array_init_size(&row_value, (zend_ulong) n_hat);

		for (j = 0; j < n_hat; ++j) {
			add_next_index_double(&row_value, row[j]);
		}

		add_next_index_zval(return_value, &row_value);
	}
}