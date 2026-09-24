#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <stdlib.h>
#include <ext/spl/spl_exceptions.h>
#include "kernel/main.h"
#include "php_ext.h"
#include "kernel/buffer.h"
#include "kernel/exception.h"
#include "kernel/operators.h"
#include "include/buffer.h"
#include "include/reductions.h"

#ifdef ZEPHIR_BUFFER_ENABLED

/**
 * Resolve the underlying raw buffer for the reduction operations, accepting
 * either a `TensorBuffer` decorator (returns a reference to its wrapped
 * buffer) or a raw `Buffer` object as-is (returns a reference to it). The
 * caller owns the reference written to `buf` and must release it with
 * `zval_ptr_dtor()`.
 *
 * @return 1 on success, 0 on failure (throwing).
 */
static int tensor_resolve_underlying_buffer(zval * obj, zval * buf)
{
	if (Z_TYPE_P(obj) == IS_OBJECT && Z_OBJCE_P(obj) == tensor_tensorbuffer_ce) {
		zval rv;
		zval *prop;

		ZVAL_UNDEF(&rv);

		prop = zend_read_property(tensor_tensorbuffer_ce, Z_OBJ_P(obj), "buffer", sizeof("buffer") - 1, 1, &rv);

		if (prop != NULL && prop != &rv) {
			ZVAL_COPY(buf, prop);
		} else {
			ZVAL_UNDEF(buf);
		}

		zval_ptr_dtor(&rv);
	} else {
		ZVAL_COPY(buf, obj);
	}

	if (UNEXPECTED(!zephir_is_buffer(buf))) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer or TensorBuffer object."));
		zval_ptr_dtor(buf);
		ZVAL_UNDEF(buf);
		return 0;
	}

	return 1;
}

/* The tensor_reduce_* operations reduce a flat buffer viewed as `groups`
 * contiguous chunks of `length` elements, mirroring TensorBuffer::split(). A
 * single group (groups == 1) reduces the whole buffer to a scalar, which is
 * exactly the Vector and ColumnVector case, while multiple groups yield the
 * per-row Matrix case. `mode` selects the reduction applied to each chunk. */

typedef enum {
	TENSOR_REDUCE_SUM = 0,
	TENSOR_REDUCE_PRODUCT,
	TENSOR_REDUCE_MIN,
	TENSOR_REDUCE_MAX,
	TENSOR_REDUCE_ARGMIN,
	TENSOR_REDUCE_ARGMAX
} tensor_reduce_mode;

/* Reduce a single contiguous run of `len` elements down to a scalar. For the
 * sum and product modes a zero-length run yields the identity, mirroring the
 * previous whole-buffer behaviour on empty buffers. The arg- modes write the
 * index of the extreme into `*index`; the min/max- modes do too, which is
 * ignored by their callers. */
static double tensor_group_reduce(const uint8_t kind, const void * data, const zend_long len, const int mode, zend_long * index)
{
	zend_long i;
	zend_long best_index = 0;

	if (mode == TENSOR_REDUCE_SUM) {
		double acc = 0.0;
		double u0 = 0.0, u1 = 0.0, u2 = 0.0, u3 = 0.0;
		double u4 = 0.0, u5 = 0.0, u6 = 0.0, u7 = 0.0;

		/* Accumulate into eight independent partial sums so the floating
		 * point dependency chain no longer serializes the iterations, and
		 * merge the partials in a small tree on the way out. The pairwise
		 * merge also keeps the accumulated rounding error a fraction of the
		 * naive serial sum. */
		i = 0;

		if (kind == ZEPHIR_BUFFER_LONG) {
			const zend_long * ptr = (const zend_long *) data;

			for (; i + 7 < len; i += 8) {
				u0 += (double) ptr[i];
				u1 += (double) ptr[i + 1];
				u2 += (double) ptr[i + 2];
				u3 += (double) ptr[i + 3];
				u4 += (double) ptr[i + 4];
				u5 += (double) ptr[i + 5];
				u6 += (double) ptr[i + 6];
				u7 += (double) ptr[i + 7];
			}

			for (; i < len; ++i) {
				acc += (double) ptr[i];
			}
		} else {
			const double * ptr = (const double *) data;

			for (; i + 7 < len; i += 8) {
				u0 += ptr[i];
				u1 += ptr[i + 1];
				u2 += ptr[i + 2];
				u3 += ptr[i + 3];
				u4 += ptr[i + 4];
				u5 += ptr[i + 5];
				u6 += ptr[i + 6];
				u7 += ptr[i + 7];
			}

			for (; i < len; ++i) {
				acc += ptr[i];
			}
		}

		return ((u0 + u1) + (u2 + u3)) + ((u4 + u5) + (u6 + u7)) + acc;
	}

	if (mode == TENSOR_REDUCE_PRODUCT) {
		double acc = 1.0;
		double u0 = 1.0, u1 = 1.0, u2 = 1.0, u3 = 1.0;
		double u4 = 1.0, u5 = 1.0, u6 = 1.0, u7 = 1.0;

		i = 0;

		if (kind == ZEPHIR_BUFFER_LONG) {
			const zend_long * ptr = (const zend_long *) data;

			for (; i + 7 < len; i += 8) {
				u0 *= (double) ptr[i];
				u1 *= (double) ptr[i + 1];
				u2 *= (double) ptr[i + 2];
				u3 *= (double) ptr[i + 3];
				u4 *= (double) ptr[i + 4];
				u5 *= (double) ptr[i + 5];
				u6 *= (double) ptr[i + 6];
				u7 *= (double) ptr[i + 7];
			}

			for (; i < len; ++i) {
				acc *= (double) ptr[i];
			}
		} else {
			const double * ptr = (const double *) data;

			for (; i + 7 < len; i += 8) {
				u0 *= ptr[i];
				u1 *= ptr[i + 1];
				u2 *= ptr[i + 2];
				u3 *= ptr[i + 3];
				u4 *= ptr[i + 4];
				u5 *= ptr[i + 5];
				u6 *= ptr[i + 6];
				u7 *= ptr[i + 7];
			}

			for (; i < len; ++i) {
				acc *= ptr[i];
			}
		}

		return ((u0 * u1) * (u2 * u3)) * ((u4 * u5) * (u6 * u7)) * acc;
	}

	int find_min = mode == TENSOR_REDUCE_MIN || mode == TENSOR_REDUCE_ARGMIN;

	if (kind == ZEPHIR_BUFFER_LONG) {
		const zend_long * ptr = (const zend_long *) data;
		zend_long best = ptr[0];

		for (i = 1; i < len; ++i) {
			if (find_min ? ptr[i] < best : ptr[i] > best) {
				best = ptr[i];
				best_index = i;
			}
		}

		if (index != NULL) {
			*index = best_index;
		}

		return (double) best;
	} else {
		const double * ptr = (const double *) data;
		double best = ptr[0];

		for (i = 1; i < len; ++i) {
			if (find_min ? ptr[i] < best : ptr[i] > best) {
				best = ptr[i];
				best_index = i;
			}
		}

		if (index != NULL) {
			*index = best_index;
		}

		return best;
	}
}

/* Shared implementation backing all tensor_reduce_* operations. Unwraps the
 * underlying buffer and validates it against a `groups` x `length` logical
 * shape before writing one reduced value per group into a new TensorBuffer. */
static void tensor_reduce_apply(zval * return_value, zval * obj, zval * groups_zval, zval * length_zval, int mode)
{
	zval buffer;
	uint8_t kind;
	zend_long total = 0, groupsHat = 0, lengthHat = 0, i;

	if (!tensor_resolve_underlying_buffer(obj, &buffer)) {
		return;
	}

	kind = zephir_buffer_kind(&buffer);
	total = zephir_buffer_len(&buffer);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		zval_ptr_dtor(&buffer);
		return;
	}

	groupsHat = zephir_get_intval(groups_zval);
	lengthHat = zephir_get_intval(length_zval);

	if (UNEXPECTED(groupsHat < 1)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Number of groups must be greater than 0."));
		zval_ptr_dtor(&buffer);
		return;
	}

	if (UNEXPECTED(lengthHat < 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Group length must be non-negative."));
		zval_ptr_dtor(&buffer);
		return;
	}

	if (lengthHat > 0) {
		if (UNEXPECTED(total % lengthHat != 0 || groupsHat != total / lengthHat)) {
			zephir_throw_exception_string(spl_ce_LengthException,
				SL("Matrix and row dimensions must agree."));
			zval_ptr_dtor(&buffer);
			return;
		}
	} else if (UNEXPECTED(total != 0)) {
		zephir_throw_exception_string(spl_ce_LengthException,
			SL("Group length must not be zero for a non-empty buffer."));
		zval_ptr_dtor(&buffer);
		return;
	}

	/* Extrema need at least one element in every group. */
	if (UNEXPECTED(lengthHat == 0 && (mode == TENSOR_REDUCE_MIN || mode == TENSOR_REDUCE_MAX
		|| mode == TENSOR_REDUCE_ARGMIN || mode == TENSOR_REDUCE_ARGMAX))) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Cannot compute the reduction of an empty group."));
		zval_ptr_dtor(&buffer);
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, groupsHat, &c) == FAILURE)) {
		zval_ptr_dtor(&buffer);
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	if (kind == ZEPHIR_BUFFER_LONG) {
		const zend_long * ptr = zephir_buffer_longs(&buffer);

		for (i = 0; i < groupsHat; ++i) {
			zend_long index = 0;

			double value = tensor_group_reduce(kind, ptr + i * lengthHat, lengthHat, mode, &index);

			vc[i] = (mode == TENSOR_REDUCE_ARGMIN || mode == TENSOR_REDUCE_ARGMAX)
				? (double) index : value;
		}
	} else {
		const double * ptr = zephir_buffer_doubles(&buffer);

		for (i = 0; i < groupsHat; ++i) {
			zend_long index = 0;
			
			double value = tensor_group_reduce(kind, ptr + i * lengthHat, lengthHat, mode, &index);

			vc[i] = (mode == TENSOR_REDUCE_ARGMIN || mode == TENSOR_REDUCE_ARGMAX)
				? (double) index : value;
		}
	}

	zval_ptr_dtor(&buffer);
	zval_ptr_dtor(&c);
}


/**
 * Row-wise operations work directly on the flat row-major buffer that backs a
 * Matrix (m * n doubles) instead of materializing per-row Buffers. This
 * helper unwraps the buffer and validates that it can be divided into whole
 * rows of length n, mirroring the semantics of `TensorBuffer::split()`. The
 * derived row count `*m` and the raw data pointer are written on success.
 *
 * @return 1 on success, 0 on failure (throwing).
 */
static int tensor_matrix_doubles(zval * obj, zval * n_zval, double ** ptr, zend_long * m, zend_long * n)
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

static int tensor_matrix_double_cmp(const void * a, const void * b)
{
	double da = *(const double *) a;
	double db = *(const double *) b;

	return (da > db) - (da < db);
}

/**
 * Return the sum of each group of the buffer as a TensorBuffer. A single
 * group covers the whole-buffer (Vector / ColumnVector) case.
 *
 * @param return_value
 * @param obj
 * @param groups
 * @param length
 */
void tensor_reduce_sum(zval * return_value, zval * obj, zval * groups, zval * length)
{
	tensor_reduce_apply(return_value, obj, groups, length, TENSOR_REDUCE_SUM);
}

/**
 * Return the product of each group of the buffer as a TensorBuffer. A single
 * group covers the whole-buffer (Vector / ColumnVector) case.
 *
 * @param return_value
 * @param obj
 * @param groups
 * @param length
 */
void tensor_reduce_product(zval * return_value, zval * obj, zval * groups, zval * length)
{
	tensor_reduce_apply(return_value, obj, groups, length, TENSOR_REDUCE_PRODUCT);
}

/**
 * Return the minimum of each group of the buffer as a TensorBuffer. A single
 * group covers the whole-buffer (Vector / ColumnVector) case.
 *
 * @param return_value
 * @param obj
 * @param groups
 * @param length
 */
void tensor_reduce_min(zval * return_value, zval * obj, zval * groups, zval * length)
{
	tensor_reduce_apply(return_value, obj, groups, length, TENSOR_REDUCE_MIN);
}

/**
 * Return the maximum of each group of the buffer as a TensorBuffer. A single
 * group covers the whole-buffer (Vector / ColumnVector) case.
 *
 * @param return_value
 * @param obj
 * @param groups
 * @param length
 */
void tensor_reduce_max(zval * return_value, zval * obj, zval * groups, zval * length)
{
	tensor_reduce_apply(return_value, obj, groups, length, TENSOR_REDUCE_MAX);
}

/**
 * Return the index of the minimum of each group of the buffer as a
 * TensorBuffer. A single group covers the whole-buffer (Vector /
 * ColumnVector) case.
 *
 * @param return_value
 * @param obj
 * @param groups
 * @param length
 */
void tensor_reduce_argmin(zval * return_value, zval * obj, zval * groups, zval * length)
{
	tensor_reduce_apply(return_value, obj, groups, length, TENSOR_REDUCE_ARGMIN);
}

/**
 * Return the index of the maximum of each group of the buffer as a
 * TensorBuffer. A single group covers the whole-buffer (Vector /
 * ColumnVector) case.
 *
 * @param return_value
 * @param obj
 * @param groups
 * @param length
 */
void tensor_reduce_argmax(zval * return_value, zval * obj, zval * groups, zval * length)
{
	tensor_reduce_apply(return_value, obj, groups, length, TENSOR_REDUCE_ARGMAX);
}

/**
 * Return the median of each row of a matrix as a column buffer, or of a
 * vector as a single element. The buffer itself is never modified; the rows
 * are copied once and sorted. A Vector is simply a 1 x n matrix.
 *
 * @param return_value
 * @param obj
 * @param n
 */
void tensor_median(zval * return_value, zval * obj, zval * n)
{
	double * va = NULL;
	zend_long m = 0, n_hat = 0;
	zend_long i;

	if (UNEXPECTED(!tensor_matrix_doubles(obj, n, &va, &m, &n_hat))) {
		return;
	}

	double * copy = NULL;

	if (UNEXPECTED(m > 0 && n_hat > 0)) {
		copy = safe_emalloc((size_t) m * (size_t) n_hat, sizeof(double), 0);

		memcpy(copy, va, (size_t) m * (size_t) n_hat * sizeof(double));
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, m, &c) == FAILURE)) {
		efree(copy);

		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	zend_long mid = n_hat / 2;
	int odd = n_hat % 2 == 1;

	for (i = 0; i < m; ++i) {
		double * row = copy + i * n_hat;

		qsort(row, (size_t) n_hat, sizeof(double), tensor_matrix_double_cmp);

		if (odd) {
			vc[i] = row[mid];
		} else {
			vc[i] = (row[mid - 1] + row[mid]) / 2.0;
		}
	}

	efree(copy);

	zval_ptr_dtor(&c);
}

/**
 * Return the q'th quantile of each row of a matrix as a column buffer, or of
 * a vector as a single element. The buffer itself is never modified; the rows
 * are copied once and sorted. A Vector is simply a 1 x n matrix.
 *
 * @param return_value
 * @param obj
 * @param n
 * @param q
 */
void tensor_quantile(zval * return_value, zval * obj, zval * n, zval * q)
{
	double * va = NULL;
	zend_long m = 0, n_hat = 0;
	zend_long i;

	if (UNEXPECTED(!tensor_matrix_doubles(obj, n, &va, &m, &n_hat))) {
		return;
	}

	double * copy = NULL;

	if (UNEXPECTED(m > 0 && n_hat > 0)) {
		copy = safe_emalloc((size_t) m * (size_t) n_hat, sizeof(double), 0);

		memcpy(copy, va, (size_t) m * (size_t) n_hat * sizeof(double));
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, m, &c) == FAILURE)) {
		efree(copy);

		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	double q_hat = zephir_get_doubleval(q);

	double x = q_hat * (n_hat - 1) + 1;

	zend_long x_hat = (zend_long) x;
	double remainder = x - (double) x_hat;

	for (i = 0; i < m; ++i) {
		double * row = copy + i * n_hat;
		double value;

		qsort(row, (size_t) n_hat, sizeof(double), tensor_matrix_double_cmp);

		if (x_hat >= n_hat) {
			value = row[n_hat - 1];
		} else {
			double t = row[x_hat - 1];

			value = t + remainder * (row[x_hat] - t);
		}

		vc[i] = value;
	}

	efree(copy);

	zval_ptr_dtor(&c);
}

/**
 * Return a new matrix buffer with rows and columns repeated the given number
 * of times. Result element (r, c) is element (r % m, c % n) of the input,
 * yielding (m * (times_m + 1)) rows and (n * (times_n + 1)) columns.
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
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Times must be non-negative."));
		return;
	}

	zend_long rows = m * (t_m + 1);
	zend_long cols = n_hat * (t_n + 1);

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

#endif