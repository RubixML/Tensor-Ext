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
#include "include/shape.h"
#include "../tensor/exceptions/invalidargumentexception.zep.h"

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

/* Sum a contiguous run of `len` doubles. Elements are accumulated into eight
 * independent partial sums so the floating point dependency chain no longer
 * serializes the iterations, and the partials are merged in a small tree on the
 * way out. The pairwise merge keeps the accumulated rounding error a fraction of
 * the naive serial sum. */
double tensor_sum_doubles(const double * data, zend_long len)
{
	double u0 = 0.0, u1 = 0.0, u2 = 0.0, u3 = 0.0;
	double u4 = 0.0, u5 = 0.0, u6 = 0.0, u7 = 0.0;
	double extra = 0.0;

	zend_long i;

	for (i = 0; i + 7 < len; i += 8) {
		u0 += data[i];
		u1 += data[i + 1];
		u2 += data[i + 2];
		u3 += data[i + 3];
		u4 += data[i + 4];
		u5 += data[i + 5];
		u6 += data[i + 6];
		u7 += data[i + 7];
	}

	for (; i < len; ++i) {
		extra += data[i];
	}

	return ((u0 + u1) + (u2 + u3)) + ((u4 + u5) + (u6 + u7)) + extra;
}

/* Product of a contiguous run of `len` doubles. Elements are multiplied into
 * eight independent partial products so the floating point dependency chain no
 * longer serializes the iterations, and the partials are merged in a small tree
 * on the way out.
 *
 * The lanes start at 1.0 rather than 0.0 so a zero-length run multiplies out to
 * 1.0, the multiplicative identity tensor_reduce_product returns for a
 * zero-width group. */
double tensor_product_doubles(const double * data, zend_long len)
{
	double u0 = 1.0, u1 = 1.0, u2 = 1.0, u3 = 1.0;
	double u4 = 1.0, u5 = 1.0, u6 = 1.0, u7 = 1.0;
	double extra = 1.0;

	zend_long i;

	for (i = 0; i + 7 < len; i += 8) {
		u0 *= data[i];
		u1 *= data[i + 1];
		u2 *= data[i + 2];
		u3 *= data[i + 3];
		u4 *= data[i + 4];
		u5 *= data[i + 5];
		u6 *= data[i + 6];
		u7 *= data[i + 7];
	}

	for (; i < len; ++i) {
		extra *= data[i];
	}

	return ((u0 * u1) * (u2 * u3)) * ((u4 * u5) * (u6 * u7)) * extra;
}

/* Reduce a single contiguous run of `len` elements down to a scalar. For the
 * sum and product modes a zero-length run yields the identity, mirroring the
 * previous whole-buffer behaviour on empty buffers. The arg- modes write the
 * index of the extreme into `*index`; the min/max- modes do too, which is
 * ignored by their callers. */
static double tensor_group_reduce(const void * data, const zend_long len, const int mode, zend_long * index)
{
	zend_long i;
	zend_long best_index = 0;

	if (mode == TENSOR_REDUCE_SUM) {
		return tensor_sum_doubles((const double *) data, len);
	}

	if (mode == TENSOR_REDUCE_PRODUCT) {
		return tensor_product_doubles((const double *) data, len);
	}

	int find_min = mode == TENSOR_REDUCE_MIN || mode == TENSOR_REDUCE_ARGMIN;
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

/* Shared implementation backing all tensor_reduce_* operations. Unwraps the
 * underlying buffer and validates it against a `groups` x `length` logical
 * shape before writing one reduced value per group into a new TensorBuffer.
 *
 * Only a double buffer is accepted, matching every other numeric operation in
 * the extension, which all reach their data through tensor_tensorbuffer_doubles()
 * and reject a non-double buffer. The check has to happen here rather than
 * letting the group reducer fall through to its double path: zend_long and
 * double are both eight bytes, so an integer buffer would be silently
 * reinterpreted as IEEE-754 data instead of faulting. */
static void tensor_reduce_apply(zval * return_value, zval * obj, zval * groups_zval, zval * length_zval, int mode)
{
	zval buffer;
	uint8_t kind;
	zend_long groups = 0, length = 0, i;

	if (!tensor_resolve_underlying_buffer(obj, &buffer)) {
		return;
	}

	kind = zephir_buffer_kind(&buffer);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must be a Buffer object."));
		zval_ptr_dtor(&buffer);
		return;
	}

	if (UNEXPECTED(kind != ZEPHIR_BUFFER_DOUBLE)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Argument must wrap a buffer of type double."));
		zval_ptr_dtor(&buffer);
		return;
	}

	groups = zephir_get_intval(groups_zval);
	length = zephir_get_intval(length_zval);

	if (UNEXPECTED(groups < 1)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Number of groups must be greater than 0."));
		zval_ptr_dtor(&buffer);
		return;
	}

	/* Extrema need at least one element in every group. The dimensions are not
	 * rechecked here: the Zephir callers pass either Vector's n, which is set
	 * from a->count(), or Matrix's m and n, which __construct has already
	 * matched against that same count. */
	if (UNEXPECTED(length == 0 && (mode == TENSOR_REDUCE_MIN || mode == TENSOR_REDUCE_MAX
		|| mode == TENSOR_REDUCE_ARGMIN || mode == TENSOR_REDUCE_ARGMAX))) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Cannot compute the reduction of an empty group."));
		zval_ptr_dtor(&buffer);
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, groups, &c) == FAILURE)) {
		zval_ptr_dtor(&buffer);
		return;
	}

	double * vc = zephir_buffer_doubles(&c);
	const double * ptr = zephir_buffer_doubles(&buffer);

	for (i = 0; i < groups; ++i) {
		zend_long index = 0;

		double value = tensor_group_reduce(ptr + i * length, length, mode, &index);

		vc[i] = (mode == TENSOR_REDUCE_ARGMIN || mode == TENSOR_REDUCE_ARGMAX)
			? (double) index : value;
	}

	zval_ptr_dtor(&buffer);
	zval_ptr_dtor(&c);
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

#endif