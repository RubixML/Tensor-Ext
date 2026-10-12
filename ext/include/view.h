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

/* Resolve any kernel operand -- a `Tensor\TensorBuffer` decorator or a raw
 * `Tensor\Buffer` -- to a stride view. A raw Buffer is a plain 1-D view of its
 * whole allocation. Returns 1 on success and 0 on failure (throwing).
 * Implemented in buffer.c. */
int tensor_view_resolve_arg(zval * obj, tensor_double_view * v);

/* Materialise the elements of `obj` into a contiguous run when the resolved
 * view is not already contiguous. On success writes a pointer to the first
 * element to `*ptr` and the element count to `*len`. A contiguous operand is
 * borrowed (`*owned` stays NULL and the pointer aliases existing storage); a
 * non-contiguous one is allocated into `*owned`, which the caller must release
 * with efree(). Returns 1 on success and 0 on failure (throwing).
 * Implemented in buffer.c. */
int tensor_view_doubles(zval * obj, const double ** ptr, zend_long * len, double ** owned);

/* Copy the logical elements of `v` in row-major order into `dst`, which must
 * hold `v->len` doubles. Implemented in buffer.c. */
void tensor_view_copy_to(const tensor_double_view * v, double * dst);

/* Copy logical row `row` of `v` -- `tensor_view_row_length(v)` doubles -- into
 * `dst`. Implemented in buffer.c. */
void tensor_view_copy_row(const tensor_double_view * v, zend_long row, double * dst);

/* Classify `obj` as a `rows` x `cols` matrix operand for BLAS/LAPACK, using the
 * caller-supplied logical dimensions (a plain dense buffer carries no shape of
 * its own). On success writes the storage pointer to `*ptr`, the leading
 * dimension to `*ld` and either 0 (stored as-is, row-major `rows` x `cols`) or
 * 1 (logical matrix is the transpose of the stored one) to `*trans`. A
 * contiguous or pure-transpose 2-D view is borrowed; any other layout is
 * materialised into `*owned` (a full `rows*cols` row-major copy the caller must
 * efree). Returns 1 on success and 0 on failure (throwing). Implemented in
 * buffer.c. */
int tensor_matrix_blas(zval * obj, zend_long rows, zend_long cols,
	const double ** ptr, zend_long * ld, int * trans, double ** owned);

/* Classify `obj` as a vector operand for BLAS. On success writes the storage
 * pointer, element count and element stride to `*ptr`/`*len`/`*inc`. A 1-D view
 * is borrowed with its own stride; a contiguous higher-rank view is borrowed
 * with stride 1; any other layout is materialised into `*owned`, which the
 * caller must efree. Returns 1 on success and 0 on failure (throwing).
 * Implemented in buffer.c. */
int tensor_vector_blas(zval * obj, const double ** ptr, zend_long * len, zend_long * inc, double ** owned);

/* Return the largest absolute value among the elements of `v`. Implemented in
 * buffer.c. */
double tensor_view_abs_max(const tensor_double_view * v);

/* A cursor over a resolved view that walks the logical elements in row-major
 * order. The base offset is advanced incrementally as the per-dimension
 * indices roll over, so a full traversal costs one add and one compare per
 * element with no modulo decode. */
typedef struct _tensor_view_reader {
	const double * base;
	zend_long off;
	zend_long shape[TENSOR_VIEW_MAX_DIMS];
	zend_long strides[TENSOR_VIEW_MAX_DIMS];
	int ndim;
	zend_long idx[TENSOR_VIEW_MAX_DIMS];
} tensor_view_reader;

static zend_always_inline void tensor_view_reader_init(tensor_view_reader * r, const tensor_double_view * v)
{
	r->base = v->base;
	r->off = v->offset;
	r->ndim = v->ndim;

	for (int d = 0; d < v->ndim; ++d) {
		r->shape[d] = v->shape[d];
		r->strides[d] = v->strides[d];
		r->idx[d] = 0;
	}
}

static zend_always_inline double tensor_view_reader_value(const tensor_view_reader * r)
{
	return r->base[r->off];
}

static zend_always_inline void tensor_view_reader_advance(tensor_view_reader * r)
{
	for (int d = r->ndim - 1; d >= 0; --d) {
		if (++r->idx[d] < r->shape[d]) {
			r->off += r->strides[d];
			return;
		}

		r->idx[d] = 0;
		r->off -= (r->shape[d] - 1) * r->strides[d];
	}
}

#endif