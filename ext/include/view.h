#ifndef TENSOR_VIEW_H
#define TENSOR_VIEW_H

#include <Zend/zend.h>

/* Maximum rank of a stride view. Matrix transposes and their rows/columns are
 * 2-D at most; the headroom exists for a future ndarray. */
#define TENSOR_VIEW_MAX_DIMS 4

/* A resolve view over a flat `double *` buffer that carries a logical shape and
 * the matching element strides. A plain dense buffer resolves to a 1-D view of
 * its whole allocation (`shape[0] = len`, `strides[0] = 1`). */
typedef struct _tensor_double_view {
	double * base;
	zend_long offset;
	zend_long len;
	int ndim;
	zend_long shape[TENSOR_VIEW_MAX_DIMS];
	zend_long strides[TENSOR_VIEW_MAX_DIMS];
	int contiguous;
} tensor_double_view;

/* Read the element at logical index `i`. `i` is decoded from the last
 * dimension backwards so the strides of a row-major view are walked in the
 * order they were laid out. */
static zend_always_inline double tensor_double_view_get(const tensor_double_view * v, zend_long i)
{
	if (v->contiguous) {
		return v->base[v->offset + i];
	}

	zend_long rem = i;
	zend_long off = v->offset;
	int d;

	for (d = v->ndim - 1; d >= 0; --d) {
		zend_long dim = v->shape[d];

		if (dim > 0) {
			off += (rem % dim) * v->strides[d];
			rem /= dim;
		}
	}

	return v->base[off];
}

/* The size of the trailing dimensions (row length): 1 for a 1-D view, and the
 * product of dims 1..ndim-1 otherwise. */
static zend_always_inline zend_long tensor_view_row_length(const tensor_double_view * v)
{
	zend_long c = 1;

	for (int d = 1; d < v->ndim; ++d) {
		c *= v->shape[d];
	}

	return c;
}

/* Resolve a `Tensor\TensorBuffer` into a stride view, reading its
 * `buffer`/`offset`/`shape`/`strides` properties. Returns 1 on success and 0
 * on failure (throwing). Implemented in buffer.c. */
int tensor_view_resolve_object(zval * obj, tensor_double_view * v);

/* The same, from the raw property values (a non-array `shape` resolves to a
 * plain 1-D view of the wrapped buffer). Returns 1 on success and 0 on
 * failure (throwing). Implemented in buffer.c. */
int tensor_view_resolve(zval * buffer, zval * offset, zval * shape, zval * strides, tensor_double_view * v);

#endif