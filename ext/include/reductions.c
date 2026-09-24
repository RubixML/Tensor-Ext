#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <stdlib.h>
#include <ext/spl/spl_exceptions.h>
#include "kernel/main.h"
#include "kernel/buffer.h"
#include "kernel/exception.h"
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

static double tensor_buffer_sum_values(const uint8_t kind, const void * data, const zend_long len)
{
	double acc = 0.0;
	zend_long i;

	if (kind == ZEPHIR_BUFFER_LONG) {
		const zend_long * ptr = (const zend_long *) data;

		for (i = 0; i < len; ++i) {
			acc += (double) ptr[i];
		}
	} else {
		const double * ptr = (const double *) data;

		for (i = 0; i < len; ++i) {
			acc += ptr[i];
		}
	}

	return acc;
}

static double tensor_buffer_product_values(const uint8_t kind, const void * data, const zend_long len)
{
	double acc = 1.0;
	zend_long i;

	if (kind == ZEPHIR_BUFFER_LONG) {
		const zend_long * ptr = (const zend_long *) data;

		for (i = 0; i < len; ++i) {
			acc *= (double) ptr[i];
		}
	} else {
		const double * ptr = (const double *) data;

		for (i = 0; i < len; ++i) {
			acc *= ptr[i];
		}
	}

	return acc;
}

static void tensor_buffer_extreme(const uint8_t kind, const void * data, const zend_long len, const int find_min, double * value, zend_long * index)
{
	zend_long i;

	if (kind == ZEPHIR_BUFFER_LONG) {
		const zend_long * ptr = (const zend_long *) data;
		zend_long best = ptr[0];

		for (i = 1; i < len; ++i) {
			if (find_min ? ptr[i] < best : ptr[i] > best) {
				best = ptr[i];
				*index = i;
			}
		}

		if (value != NULL) {
			*value = (double) best;
		}
	} else {
		const double * ptr = (const double *) data;
		double best = ptr[0];

		for (i = 1; i < len; ++i) {
			if (find_min ? ptr[i] < best : ptr[i] > best) {
				best = ptr[i];
				*index = i;
			}
		}

		if (value != NULL) {
			*value = best;
		}
	}
}

/**
 * Return the sum of the elements of a Buffer.
 *
 * @param return_value
 * @param obj
 */
void tensor_buffer_sum(zval * return_value, zval * obj)
{
	zval buffer;
	uint8_t kind;
	zend_long len;

	if (!tensor_resolve_underlying_buffer(obj, &buffer)) {
		return;
	}

	kind = zephir_buffer_kind(&buffer);
	len = zephir_buffer_len(&buffer);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		zval_ptr_dtor(&buffer);
		return;
	}

	if (len == 0) {
		RETVAL_DOUBLE(0.0);
		zval_ptr_dtor(&buffer);
		return;
	}

	RETVAL_DOUBLE(tensor_buffer_sum_values(kind, kind == ZEPHIR_BUFFER_LONG
		? (const void *) zephir_buffer_longs(&buffer) : (const void *) zephir_buffer_doubles(&buffer), len));
	zval_ptr_dtor(&buffer);
}

/**
 * Return the product of the elements of a Buffer.
 *
 * @param return_value
 * @param obj
 */
void tensor_buffer_product(zval * return_value, zval * obj)
{
	zval buffer;
	uint8_t kind;
	zend_long len;

	if (!tensor_resolve_underlying_buffer(obj, &buffer)) {
		return;
	}

	kind = zephir_buffer_kind(&buffer);
	len = zephir_buffer_len(&buffer);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		zval_ptr_dtor(&buffer);
		return;
	}

	if (len == 0) {
		RETVAL_DOUBLE(1.0);
		zval_ptr_dtor(&buffer);
		return;
	}

	RETVAL_DOUBLE(tensor_buffer_product_values(kind, kind == ZEPHIR_BUFFER_LONG
		? (const void *) zephir_buffer_longs(&buffer) : (const void *) zephir_buffer_doubles(&buffer), len));
	zval_ptr_dtor(&buffer);
}

/**
 * Return the minimum element of a Buffer.
 *
 * @param return_value
 * @param obj
 */
void tensor_buffer_min(zval * return_value, zval * obj)
{
	zval buffer;
	uint8_t kind;
	zend_long len;
	double value = 0.0;
	zend_long index = 0;

	if (!tensor_resolve_underlying_buffer(obj, &buffer)) {
		return;
	}

	kind = zephir_buffer_kind(&buffer);
	len = zephir_buffer_len(&buffer);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		zval_ptr_dtor(&buffer);
		return;
	}

	if (UNEXPECTED(len == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Cannot compute the minimum of an empty buffer."));
		zval_ptr_dtor(&buffer);
		return;
	}

	tensor_buffer_extreme(kind, kind == ZEPHIR_BUFFER_LONG
		? (const void *) zephir_buffer_longs(&buffer) : (const void *) zephir_buffer_doubles(&buffer), len, 1, &value, &index);

	zval_ptr_dtor(&buffer);

	RETVAL_DOUBLE(value);
}

/**
 * Return the maximum element of a Buffer.
 *
 * @param return_value
 * @param obj
 */
void tensor_buffer_max(zval * return_value, zval * obj)
{
	zval buffer;
	uint8_t kind;
	zend_long len;
	double value = 0.0;
	zend_long index = 0;

	if (!tensor_resolve_underlying_buffer(obj, &buffer)) {
		return;
	}

	kind = zephir_buffer_kind(&buffer);
	len = zephir_buffer_len(&buffer);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		zval_ptr_dtor(&buffer);
		return;
	}

	if (UNEXPECTED(len == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Cannot compute the maximum of an empty buffer."));
		zval_ptr_dtor(&buffer);
		return;
	}

	tensor_buffer_extreme(kind, kind == ZEPHIR_BUFFER_LONG
		? (const void *) zephir_buffer_longs(&buffer) : (const void *) zephir_buffer_doubles(&buffer), len, 0, &value, &index);

	zval_ptr_dtor(&buffer);

	RETVAL_DOUBLE(value);
}

/**
 * Return the index of the minimum element of a Buffer.
 *
 * @param return_value
 * @param obj
 */
void tensor_buffer_argmin(zval * return_value, zval * obj)
{
	zval buffer;
	uint8_t kind;
	zend_long len;
	zend_long index = 0;

	if (!tensor_resolve_underlying_buffer(obj, &buffer)) {
		return;
	}

	kind = zephir_buffer_kind(&buffer);
	len = zephir_buffer_len(&buffer);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		zval_ptr_dtor(&buffer);
		return;
	}

	if (UNEXPECTED(len == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Cannot compute the argmin of an empty buffer."));
		zval_ptr_dtor(&buffer);
		return;
	}

	tensor_buffer_extreme(kind, kind == ZEPHIR_BUFFER_LONG
		? (const void *) zephir_buffer_longs(&buffer) : (const void *) zephir_buffer_doubles(&buffer), len, 1, NULL, &index);

	zval_ptr_dtor(&buffer);

	RETVAL_LONG(index);
}

/**
 * Return the index of the maximum element of a Buffer.
 *
 * @param return_value
 * @param obj
 */
void tensor_buffer_argmax(zval * return_value, zval * obj)
{
	zval buffer;
	uint8_t kind;
	zend_long len;
	zend_long index = 0;

	if (!tensor_resolve_underlying_buffer(obj, &buffer)) {
		return;
	}

	kind = zephir_buffer_kind(&buffer);
	len = zephir_buffer_len(&buffer);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		zval_ptr_dtor(&buffer);
		return;
	}

	if (UNEXPECTED(len == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Cannot compute the argmax of an empty buffer."));
		zval_ptr_dtor(&buffer);
		return;
	}

	tensor_buffer_extreme(kind, kind == ZEPHIR_BUFFER_LONG
		? (const void *) zephir_buffer_longs(&buffer) : (const void *) zephir_buffer_doubles(&buffer), len, 0, NULL, &index);

	zval_ptr_dtor(&buffer);

	RETVAL_LONG(index);
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
 * Return the sum of each row of a matrix as a column buffer.
 *
 * @param return_value
 * @param obj
 * @param n
 */
void tensor_matrix_sum(zval * return_value, zval * obj, zval * n)
{
	double * va = NULL;
	zend_long m = 0, n_hat = 0;
	zend_long i, j;

	if (UNEXPECTED(!tensor_matrix_doubles(obj, n, &va, &m, &n_hat))) {
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, m, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < m; ++i) {
		const double * row = va + i * n_hat;
		double acc = 0.0;

		for (j = 0; j < n_hat; ++j) {
			acc += row[j];
		}

		vc[i] = acc;
	}

	zval_ptr_dtor(&c);
}

/**
 * Return the product of the elements of each row of a matrix as a column
 * buffer.
 *
 * @param return_value
 * @param obj
 * @param n
 */
void tensor_matrix_product(zval * return_value, zval * obj, zval * n)
{
	double * va = NULL;
	zend_long m = 0, n_hat = 0;
	zend_long i, j;

	if (UNEXPECTED(!tensor_matrix_doubles(obj, n, &va, &m, &n_hat))) {
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, m, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < m; ++i) {
		const double * row = va + i * n_hat;
		double acc = 1.0;

		for (j = 0; j < n_hat; ++j) {
			acc *= row[j];
		}

		vc[i] = acc;
	}

	zval_ptr_dtor(&c);
}

/**
 * Return the minimum of each row of a matrix as a column buffer.
 *
 * @param return_value
 * @param obj
 * @param n
 */
void tensor_matrix_min(zval * return_value, zval * obj, zval * n)
{
	double * va = NULL;
	zend_long m = 0, n_hat = 0;
	zend_long i, j;

	if (UNEXPECTED(!tensor_matrix_doubles(obj, n, &va, &m, &n_hat))) {
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, m, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < m; ++i) {
		const double * row = va + i * n_hat;
		double best = row[0];

		for (j = 1; j < n_hat; ++j) {
			if (row[j] < best) {
				best = row[j];
			}
		}

		vc[i] = best;
	}

	zval_ptr_dtor(&c);
}

/**
 * Return the maximum of each row of a matrix as a column buffer.
 *
 * @param return_value
 * @param obj
 * @param n
 */
void tensor_matrix_max(zval * return_value, zval * obj, zval * n)
{
	double * va = NULL;
	zend_long m = 0, n_hat = 0;
	zend_long i, j;

	if (UNEXPECTED(!tensor_matrix_doubles(obj, n, &va, &m, &n_hat))) {
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, m, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < m; ++i) {
		const double * row = va + i * n_hat;
		double best = row[0];

		for (j = 1; j < n_hat; ++j) {
			if (row[j] > best) {
				best = row[j];
			}
		}

		vc[i] = best;
	}

	zval_ptr_dtor(&c);
}

/**
 * Return the index of the minimum of each row of a matrix as a column buffer.
 *
 * @param return_value
 * @param obj
 * @param n
 */
void tensor_matrix_argmin(zval * return_value, zval * obj, zval * n)
{
	double * va = NULL;
	zend_long m = 0, n_hat = 0;
	zend_long i, j;

	if (UNEXPECTED(!tensor_matrix_doubles(obj, n, &va, &m, &n_hat))) {
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, m, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < m; ++i) {
		const double * row = va + i * n_hat;
		double best = row[0];
		zend_long index = 0;

		for (j = 1; j < n_hat; ++j) {
			if (row[j] < best) {
				best = row[j];
				index = j;
			}
		}

		vc[i] = (double) index;
	}

	zval_ptr_dtor(&c);
}

/**
 * Return the index of the maximum of each row of a matrix as a column buffer.
 *
 * @param return_value
 * @param obj
 * @param n
 */
void tensor_matrix_argmax(zval * return_value, zval * obj, zval * n)
{
	double * va = NULL;
	zend_long m = 0, n_hat = 0;
	zend_long i, j;

	if (UNEXPECTED(!tensor_matrix_doubles(obj, n, &va, &m, &n_hat))) {
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, m, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < m; ++i) {
		const double * row = va + i * n_hat;
		double best = row[0];
		zend_long index = 0;

		for (j = 1; j < n_hat; ++j) {
			if (row[j] > best) {
				best = row[j];
				index = j;
			}
		}

		vc[i] = (double) index;
	}

	zval_ptr_dtor(&c);
}

/**
 * Return the median of each row of a matrix as a column buffer. The matrix
 * buffer itself is never modified; the rows are copied once and sorted.
 *
 * @param return_value
 * @param obj
 * @param n
 */
void tensor_matrix_median(zval * return_value, zval * obj, zval * n)
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
 * Return the q'th quantile of each row of a matrix as a column buffer. The
 * matrix buffer itself is never modified; the rows are copied once and sorted.
 *
 * @param return_value
 * @param obj
 * @param n
 * @param q
 */
void tensor_matrix_quantile(zval * return_value, zval * obj, zval * n, zval * q)
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