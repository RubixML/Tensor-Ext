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
 * The tensor_reduce_* operations reduce a buffer viewed as `groups` contiguous
 * chunks of `length` elements, mirroring TensorBuffer::split(). A single group
 * covers the whole-buffer (Vector / ColumnVector) case.
 *
 * Views are consumed in place: a non-contiguous TensorBuffer view (e.g. a
 * transposed Matrix) is iterated through its strides instead of materializing
 * a dense copy, so each element is read exactly once.
 */

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

#define TENSOR_REDUCE_MAX_DIMS 4

typedef struct _tensor_reduce_view {
	double * base;
	zend_long offset;
	zend_long len;
	int ndim;
	zend_long shape[TENSOR_REDUCE_MAX_DIMS];
	zend_long strides[TENSOR_REDUCE_MAX_DIMS];
	int contiguous;
} tensor_reduce_view;

static double tensor_group_reduce(const void * data, const zend_long len, const int mode, zend_long * index);

static zend_always_inline double tensor_reduce_get(const tensor_reduce_view * v, zend_long i)
{
	if (v->contiguous) {
		return v->base[v->offset + i];
	}

	zend_long rem = i;
	zend_long off = v->offset;

	for (int d = v->ndim - 1; d >= 0; --d) {
		zend_long dim = v->shape[d];

		if (dim > 0) {
			off += (rem % dim) * v->strides[d];
			rem /= dim;
		}
	}

	return v->base[off];
}

/* Decode a PHP array of integers into `out`; returns the element count, or 0
 * when the value is not an array. */
static int tensor_reduce_read_long_list(zval * arr, zend_long * out, int max)
{
	HashTable * ht;
	zval * el;
	int n = 0;

	if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY || (ht = Z_ARRVAL_P(arr)) == NULL) {
		return 0;
	}

	ZEND_HASH_FOREACH_VAL(ht, el) {
		if (n >= max) {
			break;
		}

		out[n++] = zval_get_long(el);
	} ZEND_HASH_FOREACH_END();

	return n;
}

/* Resolve a TensorBuffer's `buffer`/`offset`/`shape`/`strides` property values
 * into a stride view. Returns 1 on success and 0 on failure (throwing). */
static int tensor_reduce_view_resolve(zval * buffer, zval * offset, zval * shape, zval * strides, tensor_reduce_view * v)
{
	if (UNEXPECTED(!zephir_is_buffer(buffer))) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Argument is not wrapping a Buffer object."));
		return 0;
	}

	if (UNEXPECTED(zephir_buffer_kind(buffer) != ZEPHIR_BUFFER_DOUBLE)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Argument must wrap a buffer of type double."));
		return 0;
	}

	v->base = zephir_buffer_doubles(buffer);
	v->offset = offset != NULL ? zephir_get_intval(offset) : 0;

	int ndim = tensor_reduce_read_long_list(shape, v->shape, TENSOR_REDUCE_MAX_DIMS);

	if (ndim <= 0) {
		v->ndim = 1;
		v->shape[0] = zephir_buffer_len(buffer);
		v->strides[0] = 1;
	} else {
		v->ndim = ndim;

		if (tensor_reduce_read_long_list(strides, v->strides, TENSOR_REDUCE_MAX_DIMS) != ndim) {
			zend_long expected = 1;

			for (int d = ndim - 1; d >= 0; --d) {
				v->strides[d] = expected;
				expected *= v->shape[d];
			}
		}
	}

	zend_long len = 1;

	for (int d = 0; d < v->ndim; ++d) {
		len *= v->shape[d];
	}

	v->len = len;
	v->contiguous = 1;

	if (len > 1) {
		zend_long expected = 1;

		for (int d = v->ndim - 1; d >= 0; --d) {
			if (v->strides[d] != expected) {
				v->contiguous = 0;
				break;
			}

			expected *= v->shape[d];
		}
	}

	return 1;
}

/* Resolve `obj` to a stride view. Accepts either a TensorBuffer decorator
 * (view or dense) or a raw Buffer object. Returns 1 on success, 0 on failure
 * (throwing). */
static int tensor_reduce_resolve(zval * obj, tensor_reduce_view * v)
{
	if (Z_TYPE_P(obj) == IS_OBJECT && Z_OBJCE_P(obj) == tensor_tensorbuffer_ce) {
		zval rv_buffer, rv_offset, rv_shape, rv_strides;
		zval * buffer, * offset, * shape, * strides;
		int ok;

		ZVAL_UNDEF(&rv_buffer);
		ZVAL_UNDEF(&rv_offset);
		ZVAL_UNDEF(&rv_shape);
		ZVAL_UNDEF(&rv_strides);

		buffer = zend_read_property(tensor_tensorbuffer_ce, Z_OBJ_P(obj), "buffer", sizeof("buffer") - 1, 1, &rv_buffer);
		offset = zend_read_property(tensor_tensorbuffer_ce, Z_OBJ_P(obj), "offset", sizeof("offset") - 1, 1, &rv_offset);
		shape = zend_read_property(tensor_tensorbuffer_ce, Z_OBJ_P(obj), "shape", sizeof("shape") - 1, 1, &rv_shape);
		strides = zend_read_property(tensor_tensorbuffer_ce, Z_OBJ_P(obj), "strides", sizeof("strides") - 1, 1, &rv_strides);

		ok = tensor_reduce_view_resolve(buffer, offset, shape, strides, v);

		zval_ptr_dtor(&rv_buffer);
		zval_ptr_dtor(&rv_offset);
		zval_ptr_dtor(&rv_shape);
		zval_ptr_dtor(&rv_strides);

		return ok;
	}

	if (UNEXPECTED(!zephir_is_buffer(obj))) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Argument must be a Buffer or TensorBuffer object."));
		return 0;
	}

	if (UNEXPECTED(zephir_buffer_kind(obj) != ZEPHIR_BUFFER_DOUBLE)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Argument must wrap a buffer of type double."));
		return 0;
	}

	v->base = zephir_buffer_doubles(obj);
	v->offset = 0;
	v->len = zephir_buffer_len(obj);
	v->ndim = 1;
	v->shape[0] = v->len;
	v->strides[0] = 1;
	v->contiguous = 1;

	return 1;
}

/* Contiguous reduction of `len` elements spaced `stride` doubles apart. */
static double tensor_group_reduce_strided(const double * data, zend_long stride, zend_long len, const int mode, zend_long * index)
{
	zend_long i;
	zend_long best_index = 0;

	if (mode == TENSOR_REDUCE_SUM) {
		double sum = 0.0;

		for (i = 0; i < len; ++i) {
			sum += data[i * stride];
		}

		return sum;
	}

	if (mode == TENSOR_REDUCE_PRODUCT) {
		double prod = 1.0;

		for (i = 0; i < len; ++i) {
			prod *= data[i * stride];
		}

		return prod;
	}

	int find_min = mode == TENSOR_REDUCE_MIN || mode == TENSOR_REDUCE_ARGMIN;
	double best = data[0];

	for (i = 1; i < len; ++i) {
		if (find_min ? data[i * stride] < best : data[i * stride] > best) {
			best = data[i * stride];
			best_index = i;
		}
	}

	if (index != NULL) {
		*index = best_index;
	}

	return best;
}

/* Reduce the `length` elements of the logical flat view starting at `start`.
 * Non-contiguous layouts iterate through the view's strides directly (a 1-D
 * view at its element stride, a 2-D view row by row); anything else decodes
 * each element from the descriptor. */
static double tensor_group_reduce_view(const tensor_reduce_view * v, zend_long start, zend_long length, const int mode, zend_long * index)
{
	zend_long i;
	zend_long best_index = 0;

	if (v->contiguous) {
		return tensor_group_reduce((const void *) (v->base + v->offset + start), length, mode, index);
	}

	if (v->ndim == 1 && start == 0 && v->shape[0] == length) {
		return tensor_group_reduce_strided(v->base + v->offset, v->strides[0], length, mode, index);
	}

	if (v->ndim == 2 && v->shape[1] == length) {
		zend_long row = start / length;

		return tensor_group_reduce_strided(v->base + v->offset + row * v->strides[0], v->strides[1], length, mode, index);
	}

	if (mode == TENSOR_REDUCE_SUM) {
		double sum = 0.0;

		for (i = 0; i < length; ++i) {
			sum += tensor_reduce_get(v, start + i);
		}

		return sum;
	}

	if (mode == TENSOR_REDUCE_PRODUCT) {
		double prod = 1.0;

		for (i = 0; i < length; ++i) {
			prod *= tensor_reduce_get(v, start + i);
		}

		return prod;
	}

	int find_min = mode == TENSOR_REDUCE_MIN || mode == TENSOR_REDUCE_ARGMIN;
	double best = tensor_reduce_get(v, start);

	for (i = 1; i < length; ++i) {
		double val = tensor_reduce_get(v, start + i);

		if (find_min ? val < best : val > best) {
			best = val;
			best_index = i;
		}
	}

	if (index != NULL) {
		*index = best_index;
	}

	return best;
}
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

/* Shared implementation backing all tensor_reduce_* operations. Resolves the
 * underlying buffer (dense or view) and validates it against a `groups` x
 * `length` logical shape before writing one reduced value per group into a new
 * TensorBuffer (one for the whole buffer with groups = 1). */
static void tensor_reduce_apply(zval * return_value, zval * obj, zval * groups_zval, zval * length_zval, int mode)
{
	tensor_reduce_view v;
	zend_long groups = 0, length = 0, i;

	if (UNEXPECTED(!tensor_reduce_resolve(obj, &v))) {
		return;
	}

	groups = zephir_get_intval(groups_zval);
	length = zephir_get_intval(length_zval);

	if (UNEXPECTED(groups < 1)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Number of groups must be greater than 0."));
		return;
	}

	/* Extrema need at least one element in every group. */
	if (UNEXPECTED(length == 0 && (mode == TENSOR_REDUCE_MIN || mode == TENSOR_REDUCE_MAX
		|| mode == TENSOR_REDUCE_ARGMIN || mode == TENSOR_REDUCE_ARGMAX))) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Cannot compute the reduction of an empty group."));
		return;
	}

	if (UNEXPECTED(length < 0 || groups * length != v.len)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Number of groups and length must match the size of the buffer."));
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, groups, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	for (i = 0; i < groups; ++i) {
		zend_long index = 0;

		double value = tensor_group_reduce_view(&v, i * length, length, mode, &index);

		vc[i] = (mode == TENSOR_REDUCE_ARGMIN || mode == TENSOR_REDUCE_ARGMAX)
			? (double) index : value;
	}

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
	tensor_double_view v;
	zend_long m = 0, n_hat = 0;
	zend_long i;

	if (UNEXPECTED(!tensor_matrix_view(obj, zephir_get_intval(n), &v, &m, &n_hat))) {
		return;
	}

	double * copy = NULL;

	if (UNEXPECTED(m > 0 && n_hat > 0)) {
		copy = safe_emalloc((size_t) m * (size_t) n_hat, sizeof(double), 0);

		tensor_view_copy_to(&v, copy);
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
	tensor_double_view v;
	zend_long m = 0, n_hat = 0;
	zend_long i;

	if (UNEXPECTED(!tensor_matrix_view(obj, zephir_get_intval(n), &v, &m, &n_hat))) {
		return;
	}

	double * copy = NULL;

	if (UNEXPECTED(m > 0 && n_hat > 0)) {
		copy = safe_emalloc((size_t) m * (size_t) n_hat, sizeof(double), 0);

		tensor_view_copy_to(&v, copy);
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