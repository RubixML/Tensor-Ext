#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <stdlib.h>
#include <string.h>
#include <Zend/zend_alloc_sizes.h>
#include <ext/spl/spl_exceptions.h>
#include "kernel/main.h"
#include "php_ext.h"
#include "kernel/buffer.h"
#include "kernel/exception.h"
#include "kernel/operators.h"
#include "include/buffer.h"
#include "include/view.h"
#include "../tensor/exceptions/invalidargumentexception.zep.h"

#ifdef ZEPHIR_BUFFER_ENABLED

zend_class_entry * tensor_buffer_ce;

/* Requests at or above this are individually mmap()ed by Zend MM, so they are
 * the ones worth recycling. Smaller allocations come from Zend MM's own run
 * caches and are left strictly alone. */
#define TENSOR_POOL_THRESHOLD ((size_t) ZEND_MM_CHUNK_SIZE)

/* Never pin more than this, so one outsized operation cannot hold an unbounded
 * amount of memory resident until the request ends. */
#define TENSOR_POOL_MAX_BYTES ((size_t) 256 * 1024 * 1024)

/* Number of blocks kept per request. A single layer of a neural network emits
 * two qualifying buffers back to back (the bias add, then the activation), and
 * with one slot the second one always misses because the first is still live. */
#ifndef TENSOR_POOL_SLOTS
#define TENSOR_POOL_SLOTS 2
#endif

/* Set between RSHUTDOWN and the next RINIT. A release arriving while sealed
 * belongs to a request that is already being torn down, and the slots are about
 * to be discarded, so it has to go straight back to efree(). */
static int tensor_pool_sealed = 1;

static struct {
	void * ptr;
	size_t bytes;
} tensor_pool[TENSOR_POOL_SLOTS];

/* NTS assumption, matching the feature cache in include/cpu.c. Under a ZTS
 * build this static would be shared by every thread, so the oversized buffer
 * cache is only correct for non-thread-safe builds. */

/**
 * Whether a block of `bytes` is large enough to be worth recycling and small
 * enough that keeping it resident is acceptable.
 */
static zend_always_inline int tensor_poolable(size_t bytes)
{
	return bytes >= TENSOR_POOL_THRESHOLD && bytes <= TENSOR_POOL_MAX_BYTES;
}

/**
 * Take a recycled block when the slot holds one at least as large as `bytes`,
 * otherwise allocate. A cached block larger than requested is fine: readers
 * only ever look at b->len elements, so a stale tail past that length is
 * indistinguishable from a fresh one.
 */
static void * tensor_pool_acquire(size_t bytes)
{
	if (UNEXPECTED(tensor_pool_sealed)) {
		return emalloc(bytes);
	}

	for (int i = 0; i < TENSOR_POOL_SLOTS; ++i) {
		if (tensor_pool[i].ptr != NULL && tensor_pool[i].bytes >= bytes) {
			void * ptr = tensor_pool[i].ptr;

			tensor_pool[i].ptr = NULL;
			tensor_pool[i].bytes = 0;

			return ptr;
		}
	}

	return emalloc(bytes);
}

/**
 * Hand a block back. It takes a free slot, or displaces the smallest slot when
 * that slot cannot serve as many future requests, and is efree()d when no slot
 * can take it.
 *
 * Displacing a smaller block is what keeps a small allocation from wedging the
 * cache shut. Leaving it in place would make the next larger release find every
 * slot occupied and efree() itself instead, and the undersized block would then
 * stay there for the rest of the request, silently disabling recycling.
 *
 * A block is a valid emalloc() allocation whether it was recycled or freshly
 * allocated, so no record of its origin is needed.
 */
static void tensor_pool_release(void * ptr, size_t bytes)
{
	if (UNEXPECTED(tensor_pool_sealed) || !tensor_poolable(bytes)) {
		efree(ptr);

		return;
	}

	int smallest = -1;

	for (int i = 0; i < TENSOR_POOL_SLOTS; ++i) {
		if (tensor_pool[i].ptr == NULL) {
			tensor_pool[i].ptr = ptr;
			tensor_pool[i].bytes = bytes;

			return;
		}

		if (tensor_pool[i].bytes < bytes
			&& (smallest < 0 || tensor_pool[i].bytes < tensor_pool[smallest].bytes)) {
			smallest = i;
		}
	}

	if (smallest >= 0) {
		efree(tensor_pool[smallest].ptr);
		tensor_pool[smallest].ptr = ptr;
		tensor_pool[smallest].bytes = bytes;

		return;
	}

	efree(ptr);
}

/* zephir_buffer_object_handlers is static to the generated kernel/buffer.c, so
 * the class entry cannot be used as a source: Zephir fills that static in and
 * assigns it per object from create_object(), but never publishes it as
 * ce->default_object_handlers, which is left as the stock std handlers with a
 * zero offset. Copying from there would silently misplace every field read.
 *
 * The object just created is therefore the only faithful source. create_object
 * has already installed the fully populated struct on it, and every Buffer
 * shares that one struct, so snapshotting it once and swapping free_obj is
 * enough: all of read_dimension, write_dimension, clone_obj, get_debug_info
 * and the rest are inherited unchanged. */
static zend_object_handlers tensor_pool_handlers;
static int tensor_pool_handlers_ready = 0;

static void tensor_buffer_pool_free_object(zend_object * object)
{
	zephir_buffer_object * b = zephir_buffer_fetch(object);

	if (b->data.raw) {
		void * raw = b->data.raw;
		b->data.raw = NULL;

		tensor_pool_release(raw, (size_t) b->len * sizeof(double));
	}

	zend_object_std_dtor(&b->std);
}

/**
 * Reset the cache at RINIT; see the `initializers.request` hook in config.json.
 *
 * A pointer left over from the previous request is dropped WITHOUT being
 * freed. That block came from the previous request's Zend MM heap, which has
 * already been reclaimed, so efree() on it would corrupt the current heap.
 */
void tensor_pool_activate(void)
{
	for (int i = 0; i < TENSOR_POOL_SLOTS; ++i) {
		tensor_pool[i].ptr = NULL;
		tensor_pool[i].bytes = 0;
	}

	tensor_pool_sealed = 0;
}

/**
 * Seal the cache at RSHUTDOWN; see the `destructors.request` hook in
 * config.json.
 *
 * Objects are force destroyed during request teardown, so releases can still
 * arrive after this point, and sealing sends them straight to efree(). The
 * cached block itself is deliberately left to Zend MM, which reclaims every
 * request allocation in zend_deactivate().
 */
void tensor_pool_seal(void)
{
	for (int i = 0; i < TENSOR_POOL_SLOTS; ++i) {
		tensor_pool[i].ptr = NULL;
		tensor_pool[i].bytes = 0;
	}

	tensor_pool_sealed = 1;
}

/**
 * Allocate a new `Tensor\TensorBuffer` wrapping a fresh double buffer of `len`
 * elements whose contents are NOT zeroed (see include/buffer.h).
 *
 * PHP's `ecalloc()` always memsets, even for freshly mmap()ed pages, so every
 * elementwise operation paid a full extra write pass over its output only to
 * overwrite it immediately. Callers using this variant MUST overwrite all
 * `len` elements before any of them is read.
 *
 * Teardown is unaffected: `zephir_buffer_free_object()` efree()s `data.raw`
 * regardless of how it was allocated, so there is no leak and no double free.
 *
 * Buffers at or above ZEND_MM_CHUNK_SIZE additionally recycle a single cached
 * block per request; see the oversized buffer cache above.
 */
int tensor_tensorbuffer_create(zval * ret, zend_long len, zval * buffer)
{
	/* `bytes` is a size_t, so a count between SIZE_MAX / sizeof(double) and
	 * ZEND_LONG_MAX wraps it to a small allocation while the caller's fill loop
	 * still writes `len` elements through it. Reject that here rather than in
	 * every caller: Matrix::fill() reaches it with len = m * n, which overflows
	 * nothing on the Zephir side. */
	if (UNEXPECTED(len < 0 || (uint64_t) len > (uint64_t) (SIZE_MAX / sizeof(double)))) {
		ZVAL_NULL(buffer);

		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("The requested buffer size is too large to allocate."));

		return FAILURE;
	}

	size_t bytes = (size_t) len * sizeof(double);

	object_init_ex(buffer, zephir_buffer_ce);

	zephir_buffer_object * b = ZEPHIR_BUFFER_P(buffer);

	b->len       = len;
	b->kind      = ZEPHIR_BUFFER_DOUBLE;
	b->data.raw  = NULL;

	if (len > 0) {
		if (tensor_poolable(bytes)) {
			if (!tensor_pool_handlers_ready) {
				memcpy(&tensor_pool_handlers, Z_OBJ_P(buffer)->handlers, sizeof(zend_object_handlers));
				tensor_pool_handlers.free_obj = tensor_buffer_pool_free_object;
				tensor_pool_handlers_ready = 1;
			}

			/* Installed for every oversized buffer, recycled or not. The
			 * point is to route the free into the slot, and a block that was
			 * freshly emalloc()ed still deserves the chance to be kept. */
			Z_OBJ_P(buffer)->handlers = &tensor_pool_handlers;

			b->data.raw = tensor_pool_acquire(bytes);
		} else {
			b->data.raw = emalloc(bytes);
		}
	}

	object_init_ex(ret, tensor_tensorbuffer_ce);

	/* zend_update_property() sets the engine's fake scope to the owner class
	 * so protected property writes from C pass the PHP 8.4 access check. */
	zend_update_property(tensor_tensorbuffer_ce, Z_OBJ_P(ret), "buffer", sizeof("buffer") - 1, buffer);

	if (UNEXPECTED(EG(exception))) {
		zval_ptr_dtor(ret);
		ZVAL_UNDEF(ret);
		zval_ptr_dtor(buffer);
		ZVAL_UNDEF(buffer);
		
		return FAILURE;
	}

	return SUCCESS;
}

/**
 * Allocate a new `Tensor\TensorBuffer` decorator wrapping a `Tensor\Buffer`
 * built from a PHP array of values, casting every element to a double (see
 * include/buffer.h).
 */
void tensor_buffer_from_array(zval * ret, zval * arr)
{
	zval buffer;

	if (UNEXPECTED(zephir_buffer_create_from_array(&buffer, arr, ZEPHIR_BUFFER_DOUBLE) == FAILURE)) {
		ZVAL_NULL(ret);
		return;
	}

	object_init_ex(ret, tensor_tensorbuffer_ce);

	zend_update_property(tensor_tensorbuffer_ce, Z_OBJ_P(ret), "buffer", sizeof("buffer") - 1, &buffer);

	if (UNEXPECTED(EG(exception))) {
		zval_ptr_dtor(ret);
		ZVAL_UNDEF(ret);
		zval_ptr_dtor(&buffer);
		ZVAL_UNDEF(&buffer);
		return;
	}

	zval_ptr_dtor(&buffer);
}

/* -------------------------------------------------------------------------
 * Structural views
 *
 * See include/view.h for the model. Everything here works on a resolved
 * `tensor_double_view`, which is the raw storage pointer plus the strides of a
 * TensorBuffer. A plain dense buffer resolves to a 1-D contiguous view of its
 * whole allocation, so kernels can keep calling tensor_tensorbuffer_doubles()
 * without knowing about strides.
 * ------------------------------------------------------------------------- */

/* Bumped by every in-place write to a Buffer. A view caches the dense copy it
 * gathered for the kernels that need one; the epoch lets it notice that the
 * source was rewritten behind its back and re-gather. */
static zend_long tensor_buffer_epoch = 0;

void tensor_buffer_bump_epoch(void)
{
	++tensor_buffer_epoch;
}

/**
 * Copy up to `max` integers out of a PHP array, discarding keys. Returns the
 * number of entries written; a NULL or non-array input yields 0.
 */
static int tensor_read_long_list(zval * arr, zend_long * out, int max)
{
	if (arr == NULL || Z_TYPE_P(arr) != IS_ARRAY) {
		return 0;
	}

	int n = 0;
	zval * el;

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(arr), el) {
		if (n >= max) {
			break;
		}

		out[n++] = zval_get_long(el);
	} ZEND_HASH_FOREACH_END();

	return n;
}

/**
 * Resolve a TensorBuffer's `buffer`/`offset`/`shape`/`strides` property values
 * into a stride view. Returns 1 on success and 0 on failure (throwing).
 */
int tensor_view_resolve(zval * buffer, zval * offset, zval * shape, zval * strides, tensor_double_view * v)
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

	int ndim = tensor_read_long_list(shape, v->shape, TENSOR_VIEW_MAX_DIMS);

	if (ndim <= 0) {
		v->ndim = 1;
		v->shape[0] = zephir_buffer_len(buffer);
		v->strides[0] = 1;
	} else {
		v->ndim = ndim;

		if (tensor_read_long_list(strides, v->strides, TENSOR_VIEW_MAX_DIMS) != ndim) {
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

/**
 * Resolve a `Tensor\TensorBuffer` object into a stride view. Returns 1 on
 * success and 0 on failure (throwing).
 */
int tensor_view_resolve_object(zval * obj, tensor_double_view * v)
{
	if (UNEXPECTED(Z_TYPE_P(obj) != IS_OBJECT || Z_OBJCE_P(obj) != tensor_tensorbuffer_ce)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Argument must be a Tensor\\TensorBuffer object."));
		return 0;
	}

	zval rv_buffer, rv_offset, rv_shape, rv_strides;

	ZVAL_UNDEF(&rv_buffer);
	ZVAL_UNDEF(&rv_offset);
	ZVAL_UNDEF(&rv_shape);
	ZVAL_UNDEF(&rv_strides);

	zval * buffer = zend_read_property(tensor_tensorbuffer_ce, Z_OBJ_P(obj), "buffer", sizeof("buffer") - 1, 1, &rv_buffer);
	zval * offset = zend_read_property(tensor_tensorbuffer_ce, Z_OBJ_P(obj), "offset", sizeof("offset") - 1, 1, &rv_offset);
	zval * shape  = zend_read_property(tensor_tensorbuffer_ce, Z_OBJ_P(obj), "shape", sizeof("shape") - 1, 1, &rv_shape);
	zval * strides = zend_read_property(tensor_tensorbuffer_ce, Z_OBJ_P(obj), "strides", sizeof("strides") - 1, 1, &rv_strides);

	int ok = tensor_view_resolve(buffer, offset, shape, strides, v);

	if (!Z_ISUNDEF(rv_buffer)) {
		zval_ptr_dtor(&rv_buffer);
	}

	if (!Z_ISUNDEF(rv_offset)) {
		zval_ptr_dtor(&rv_offset);
	}

	if (!Z_ISUNDEF(rv_shape)) {
		zval_ptr_dtor(&rv_shape);
	}

	if (!Z_ISUNDEF(rv_strides)) {
		zval_ptr_dtor(&rv_strides);
	}

	return ok;
}

/**
 * Build a view object over `buffer` (see include/buffer.h).
 */
int tensor_buffer_make_view(zval * ret, zval * buffer, zend_long offset, int ndim, const zend_long * shape, const zend_long * strides)
{
	object_init_ex(ret, tensor_tensorbuffer_ce);

	zend_update_property(tensor_tensorbuffer_ce, Z_OBJ_P(ret), "buffer", sizeof("buffer") - 1, buffer);

	zval value;

	ZVAL_LONG(&value, offset);

	zend_update_property(tensor_tensorbuffer_ce, Z_OBJ_P(ret), "offset", sizeof("offset") - 1, &value);

	zval shape_arr, strides_arr;

	array_init_size(&shape_arr, (zend_ulong) ndim);
	array_init_size(&strides_arr, (zend_ulong) ndim);

	for (int d = 0; d < ndim; ++d) {
		add_next_index_long(&shape_arr, shape[d]);
		add_next_index_long(&strides_arr, strides[d]);
	}

	zend_update_property(tensor_tensorbuffer_ce, Z_OBJ_P(ret), "shape", sizeof("shape") - 1, &shape_arr);
	zend_update_property(tensor_tensorbuffer_ce, Z_OBJ_P(ret), "strides", sizeof("strides") - 1, &strides_arr);

	zval_ptr_dtor(&shape_arr);
	zval_ptr_dtor(&strides_arr);

	if (UNEXPECTED(EG(exception))) {
		zval_ptr_dtor(ret);
		ZVAL_UNDEF(ret);

		return FAILURE;
	}

	return SUCCESS;
}

void tensor_tensorbuffer_count(zval * ret, zval * buffer, zval * shape)
{
	zend_long dims[TENSOR_VIEW_MAX_DIMS];
	int ndim = tensor_read_long_list(shape, dims, TENSOR_VIEW_MAX_DIMS);

	if (ndim <= 0) {
		ZVAL_LONG(ret, zephir_is_buffer(buffer) ? zephir_buffer_len(buffer) : 0);

		return;
	}

	zend_long len = 1;

	for (int d = 0; d < ndim; ++d) {
		len *= dims[d];
	}

	ZVAL_LONG(ret, len);
}

void tensor_tensorbuffer_to_array(zval * ret, zval * buffer, zval * offset, zval * shape, zval * strides)
{
	tensor_double_view v;

	if (UNEXPECTED(!tensor_view_resolve(buffer, offset, shape, strides, &v))) {
		return;
	}

	array_init_size(ret, (zend_ulong) v.len);

	for (zend_long i = 0; i < v.len; ++i) {
		add_next_index_double(ret, tensor_double_view_get(&v, i));
	}
}

void tensor_buffer_view_materialize(zval * ret, zval * buffer, zval * offset, zval * shape, zval * strides)
{
	tensor_double_view v;

	if (UNEXPECTED(!tensor_view_resolve(buffer, offset, shape, strides, &v))) {
		return;
	}

	if (UNEXPECTED(zephir_buffer_create(ret, v.len, ZEPHIR_BUFFER_DOUBLE) == FAILURE)) {
		return;
	}

	double * dst = zephir_buffer_doubles(ret);

	for (zend_long i = 0; i < v.len; ++i) {
		dst[i] = tensor_double_view_get(&v, i);
	}
}

void tensor_tensorbuffer_get(zval * ret, zval * buffer, zval * offset, zval * shape, zval * strides, zval * index)
{
	tensor_double_view v;

	if (UNEXPECTED(!tensor_view_resolve(buffer, offset, shape, strides, &v))) {
		return;
	}

	zend_long i = zephir_get_intval(index);

	if (UNEXPECTED(i < 0 || i >= v.len)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Index is out of bounds."));
		return;
	}

	ZVAL_DOUBLE(ret, tensor_double_view_get(&v, i));
}

void tensor_tensorbuffer_set(zval * ret, zval * buffer, zval * offset, zval * shape, zval * strides, zval * index, zval * value)
{
	if (shape != NULL && Z_TYPE_P(shape) == IS_ARRAY) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Cannot modify a read-only view."));
		return;
	}

	if (UNEXPECTED(!zephir_is_buffer(buffer))) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Argument is not wrapping a Buffer object."));
		return;
	}

	zend_long i = zephir_get_intval(index);
	zend_long len = zephir_buffer_len(buffer);

	if (UNEXPECTED(i < 0 || i >= len)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Index is out of bounds."));
		return;
	}

	if (zephir_buffer_kind(buffer) == ZEPHIR_BUFFER_LONG) {
		zephir_buffer_longs(buffer)[i] = zephir_get_intval(value);
	} else {
		zephir_buffer_doubles(buffer)[i] = zephir_get_doubleval(value);
	}

	tensor_buffer_bump_epoch();

	ZVAL_NULL(ret);
}

/* The flat base index of element `i` of a resolved view, i.e. the decode of
 * `tensor_double_view_get` without the final read. */
static zend_always_inline zend_long tensor_view_offset(const tensor_double_view * v, zend_long i)
{
	zend_long rem = i;
	zend_long off = v->offset;

	for (int d = v->ndim - 1; d >= 0; --d) {
		zend_long dim = v->shape[d];

		if (dim > 0) {
			off += (rem % dim) * v->strides[d];
			rem /= dim;
		}
	}

	return off;
}

/**
 * Build a 1-D view selecting flat indices `first + k * step` (k in [0, count))
 * of the resolved base view.
 *
 * When the selection is expressible as a single strided run over the base -- a
 * 1-D base, a contiguous range that stays within one row, or a step that
 * advances whole rows -- the result is a composed view sharing storage. When
 * the run cannot be folded into one stride (an arbitrary diagonal of a 2-D
 * view, say) the selected elements are gathered into a fresh dense Buffer and
 * the view is built over that, so the cost is bounded by `count` rather than
 * by the whole base.
 *
 * Returns 1 on success (writing `ret`) and 0 on failure (throwing).
 */
static int tensor_slice_view_1d(zval * ret, zval * buffer, const tensor_double_view * base,
	zend_long first, zend_long step, zend_long count)
{
	int expressible = 1;
	zend_long out_offset;
	zend_long out_stride;

	if (count == 0) {
		zend_long zero_shape[1] = { 0 };
		zend_long zero_strides[1] = { 1 };

		return tensor_buffer_make_view(ret, buffer, base->offset, 1, zero_shape, zero_strides) == SUCCESS;
	}

	if (base->ndim == 1) {
		out_offset = base->offset + first * base->strides[0];
		out_stride = step * base->strides[0];
	} else {
		zend_long c = tensor_view_row_length(base);

		if (step % c == 0) {
			/* The step advances whole rows, so the trailing coordinates stay
			 * fixed while the first dimension moves. */
			out_offset = tensor_view_offset(base, first);
			out_stride = (step / c) * base->strides[0];
		} else if (step == 1 && (base->contiguous || (first / c) == ((first + count - 1) / c))) {
			out_offset = tensor_view_offset(base, first);
			out_stride = base->strides[base->ndim - 1];
		} else {
			expressible = 0;
			out_offset = 0;
			out_stride = 1;
		}
	}

	if (expressible) {
		zend_long one_shape[1] = { count };
		zend_long one_strides[1] = { out_stride };

		return tensor_buffer_make_view(ret, buffer, out_offset, 1, one_shape, one_strides) == SUCCESS;
	}

	zval dense;

	if (UNEXPECTED(zephir_buffer_create(&dense, count, ZEPHIR_BUFFER_DOUBLE) == FAILURE)) {
		return 0;
	}

	double * dst = zephir_buffer_doubles(&dense);

	for (zend_long k = 0; k < count; ++k) {
		dst[k] = tensor_double_view_get(base, first + k * step);
	}

	zend_long one_shape[1] = { count };
	zend_long one_strides[1] = { 1 };

	int ok = tensor_buffer_make_view(ret, &dense, 0, 1, one_shape, one_strides) == SUCCESS;

	zval_ptr_dtor(&dense);

	return ok;
}

void tensor_buffer_slice_view(zval * ret, zval * buffer, zval * offset, zval * shape, zval * strides, zval * slice_offset, zval * slice_length)
{
	tensor_double_view base;

	if (UNEXPECTED(!tensor_view_resolve(buffer, offset, shape, strides, &base))) {
		return;
	}

	zend_long o = zephir_get_intval(slice_offset);
	zend_long len = zephir_get_intval(slice_length);

	tensor_slice_view_1d(ret, buffer, &base, o, 1, len);
}

void tensor_buffer_slice_strided_view(zval * ret, zval * buffer, zval * offset, zval * shape, zval * strides, zval * slice_offset, zval * slice_length, zval * slice_stride)
{
	tensor_double_view base;

	if (UNEXPECTED(!tensor_view_resolve(buffer, offset, shape, strides, &base))) {
		return;
	}

	zend_long o = zephir_get_intval(slice_offset);
	zend_long len = zephir_get_intval(slice_length);
	zend_long step = zephir_get_intval(slice_stride);

	tensor_slice_view_1d(ret, buffer, &base, o, step, len);
}

void tensor_buffer_split_view(zval * ret, zval * buffer, zval * offset, zval * shape, zval * strides, zval * chunk_length)
{
	tensor_double_view base;

	if (UNEXPECTED(!tensor_view_resolve(buffer, offset, shape, strides, &base))) {
		return;
	}

	zend_long chunk = zephir_get_intval(chunk_length);
	zend_long len = base.len;
	zend_long chunks;

	if (UNEXPECTED(len == 0)) {
		chunks = 0;
	} else if (UNEXPECTED(chunk > len)) {
		chunks = 1;
	} else {
		chunks = (len + chunk - 1) / chunk;
	}

	array_init_size(ret, (zend_ulong) chunks);

	for (zend_long i = 0; i < chunks; ++i) {
		zend_long start = i * chunk;
		zend_long chunk_len = len - start < chunk ? len - start : chunk;
		zval view;

		ZVAL_UNDEF(&view);

		if (UNEXPECTED(!tensor_slice_view_1d(&view, buffer, &base, start, 1, chunk_len))) {
			return;
		}

		add_next_index_zval(ret, &view);
	}
}

void tensor_buffer_transpose_view(zval * ret, zval * a, zend_long m, zend_long n)
{
	tensor_double_view v;

	if (UNEXPECTED(!tensor_view_resolve_object(a, &v))) {
		return;
	}

	zend_long row_stride, col_stride;

	if (v.ndim <= 1) {
		row_stride = n * v.strides[0];
		col_stride = v.strides[0];
	} else if (v.ndim == 2) {
		row_stride = v.strides[0];
		col_stride = v.strides[1];
	} else {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Transposing a view of more than two dimensions is not supported."));
		return;
	}

	zval rv, * buffer_prop;

	ZVAL_UNDEF(&rv);

	buffer_prop = zend_read_property(tensor_tensorbuffer_ce, Z_OBJ_P(a), "buffer", sizeof("buffer") - 1, 1, &rv);

	if (UNEXPECTED(buffer_prop == NULL || !zephir_is_buffer(buffer_prop))) {
		if (!Z_ISUNDEF(rv)) {
			zval_ptr_dtor(&rv);
		}

		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Argument is not wrapping a Buffer object."));
		return;
	}

	zend_long shape[2] = { n, m };
	zend_long strides[2] = { col_stride, row_stride };

	tensor_buffer_make_view(ret, buffer_prop, v.offset, 2, shape, strides);

	if (!Z_ISUNDEF(rv)) {
		zval_ptr_dtor(&rv);
	}
}

/**
 * Unwrap the double buffer hidden inside a `Tensor\TensorBuffer` object
 * (see include/buffer.h).
 *
 * A dense buffer (or a dense view) returns a pointer straight into the source.
 * A non-contiguous view is gathered into a dense Buffer that is cached on the
 * view and reused until the source is written through set()/sort().
 */
double * tensor_tensorbuffer_doubles(zval * obj, zend_long * len, int * success)
{
	zend_long dummy = 0;

	if (len == NULL) {
		len = &dummy;
	}

	if (success != NULL) {
		*success = 0;
	}

	*len = 0;

	tensor_double_view v;

	if (UNEXPECTED(!tensor_view_resolve_object(obj, &v))) {
		return NULL;
	}

	*len = v.len;

	if (v.contiguous) {
		if (success != NULL) {
			*success = 1;
		}

		return v.base + v.offset;
	}

	zval rv_epoch, rv_data;

	ZVAL_UNDEF(&rv_epoch);
	ZVAL_UNDEF(&rv_data);

	zval * epoch_prop = zend_read_property(tensor_tensorbuffer_ce, Z_OBJ_P(obj), "denseEpoch", sizeof("denseEpoch") - 1, 1, &rv_epoch);
	zval * data_prop = zend_read_property(tensor_tensorbuffer_ce, Z_OBJ_P(obj), "denseData", sizeof("denseData") - 1, 1, &rv_data);

	if (epoch_prop != NULL && Z_TYPE_P(epoch_prop) == IS_LONG
		&& Z_LVAL_P(epoch_prop) == tensor_buffer_epoch
		&& data_prop != NULL && zephir_is_buffer(data_prop)
		&& zephir_buffer_kind(data_prop) == ZEPHIR_BUFFER_DOUBLE
		&& zephir_buffer_len(data_prop) == v.len) {
		if (!Z_ISUNDEF(rv_epoch)) {
			zval_ptr_dtor(&rv_epoch);
		}

		if (!Z_ISUNDEF(rv_data)) {
			zval_ptr_dtor(&rv_data);
		}

		if (success != NULL) {
			*success = 1;
		}

		return zephir_buffer_doubles(data_prop);
	}

	if (!Z_ISUNDEF(rv_epoch)) {
		zval_ptr_dtor(&rv_epoch);
	}

	if (!Z_ISUNDEF(rv_data)) {
		zval_ptr_dtor(&rv_data);
	}

	zval gathered;

	if (UNEXPECTED(zephir_buffer_create(&gathered, v.len, ZEPHIR_BUFFER_DOUBLE) == FAILURE)) {
		return NULL;
	}

	double * dst = zephir_buffer_doubles(&gathered);

	for (zend_long i = 0; i < v.len; ++i) {
		dst[i] = tensor_double_view_get(&v, i);
	}

	zval epoch_value;

	ZVAL_LONG(&epoch_value, tensor_buffer_epoch);

	zend_update_property(tensor_tensorbuffer_ce, Z_OBJ_P(obj), "denseData", sizeof("denseData") - 1, &gathered);
	zend_update_property(tensor_tensorbuffer_ce, Z_OBJ_P(obj), "denseEpoch", sizeof("denseEpoch") - 1, &epoch_value);

	zval_ptr_dtor(&gathered);

	if (success != NULL) {
		*success = 1;
	}

	return dst;
}

static int tensor_double_cmp(const void * a, const void * b)
{
	double da = *(const double *) a;
	double db = *(const double *) b;

	return (da > db) - (da < db);
}

static int tensor_double_cmp_desc(const void * a, const void * b)
{
	return tensor_double_cmp(b, a);
}

static int tensor_long_cmp(const void * a, const void * b)
{
	zend_long la = *(const zend_long *) a;
	zend_long lb = *(const zend_long *) b;

	return (la > lb) - (la < lb);
}

static int tensor_long_cmp_desc(const void * a, const void * b)
{
	return tensor_long_cmp(b, a);
}

/**
 * Sort the elements of a Buffer in place.
 *
 * @param return_value
 * @param obj
 * @param ascending
 */
void tensor_buffer_sort(zval * return_value, zval * obj, zval * ascending)
{
	uint8_t kind = zephir_buffer_kind(obj);
	zend_long len = zephir_buffer_len(obj);
	int ascending_order = zend_is_true(ascending);

	if (UNEXPECTED(kind == 0 || len < 2)) {
		RETURN_NULL();
	}

	if (kind == ZEPHIR_BUFFER_LONG) {
		qsort((void *) zephir_buffer_longs(obj), (size_t) len, sizeof(zend_long),
			ascending_order ? tensor_long_cmp : tensor_long_cmp_desc);
	} else {
		qsort((void *) zephir_buffer_doubles(obj), (size_t) len, sizeof(double),
			ascending_order ? tensor_double_cmp : tensor_double_cmp_desc);
	}

	tensor_buffer_bump_epoch();

	RETURN_NULL();
}

/**
 * Return a new Buffer populated with a slice of the given buffer.
 *
 * @param return_value
 * @param obj
 * @param offset
 * @param length
 */
void tensor_buffer_slice(zval * return_value, zval * obj, zval * offset, zval * length)
{
	uint8_t kind = zephir_buffer_kind(obj);
	zend_long offsetHat = zephir_get_intval(offset);
	zend_long lengthHat = zephir_get_intval(length);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Argument must be a Buffer object."));
		return;
	}

	if (UNEXPECTED(zephir_buffer_create(return_value, lengthHat, kind) == FAILURE)) {
		return;
	}

	if (lengthHat == 0) {
		return;
	}

	if (kind == ZEPHIR_BUFFER_LONG) {
		memcpy(zephir_buffer_longs(return_value), zephir_buffer_longs(obj) + offsetHat,
			(size_t) lengthHat * sizeof(zend_long));
	} else {
		memcpy(zephir_buffer_doubles(return_value), zephir_buffer_doubles(obj) + offsetHat,
			(size_t) lengthHat * sizeof(double));
	}
}

/**
 * Return a new Buffer populated with a strided slice of the given buffer.
 *
 * @param return_value
 * @param obj
 * @param offset
 * @param length
 * @param stride
 */
void tensor_buffer_slice_strided(zval * return_value, zval * obj, zval * offset, zval * length, zval * stride)
{
	uint8_t kind = zephir_buffer_kind(obj);
	zend_long offsetHat = zephir_get_intval(offset);
	zend_long lengthHat = zephir_get_intval(length);
	zend_long strideHat = zephir_get_intval(stride);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Argument must be a Buffer object."));
		return;
	}

	if (UNEXPECTED(zephir_buffer_create(return_value, lengthHat, kind) == FAILURE)) {
		return;
	}

	if (lengthHat == 0) {
		return;
	}

	zend_long i;

	if (kind == ZEPHIR_BUFFER_LONG) {
		const zend_long * src = zephir_buffer_longs(obj);
		zend_long * dst = zephir_buffer_longs(return_value);
		zend_long index = offsetHat;

		for (i = 0; i < lengthHat; ++i) {
			dst[i] = src[index];

			if (i + 1 < lengthHat) {
				index += strideHat;
			}
		}
	} else {
		const double * src = zephir_buffer_doubles(obj);
		double * dst = zephir_buffer_doubles(return_value);
		zend_long index = offsetHat;

		for (i = 0; i < lengthHat; ++i) {
			dst[i] = src[index];

			if (i + 1 < lengthHat) {
				index += strideHat;
			}
		}
	}
}

/**
 * Return a new Buffer populated with a copy of this buffer concatenated with
 * the given buffers.
 *
 * @param return_value
 * @param obj
 * @param others
 */
void tensor_buffer_concat(zval * return_value, zval * obj, zval * others)
{
	uint8_t kind = zephir_buffer_kind(obj);
	zend_long len = zephir_buffer_len(obj);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Argument must be a Buffer object."));
		return;
	}

	HashTable * ht = Z_ARRVAL_P(others);

	zend_long total = len;
	zval * other;

	ZEND_HASH_FOREACH_VAL(ht, other) {
		if (UNEXPECTED(!zephir_is_buffer(other) || zephir_buffer_kind(other) != kind)) {
			zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
				SL("All buffers must be the same type as the given buffer."));
			return;
		}

		total += zephir_buffer_len(other);
	} ZEND_HASH_FOREACH_END();

	if (UNEXPECTED(zephir_buffer_create(return_value, total, kind) == FAILURE)) {
		return;
	}

	if (len > 0) {
		if (kind == ZEPHIR_BUFFER_LONG) {
			memcpy(zephir_buffer_longs(return_value), zephir_buffer_longs(obj),
				(size_t) len * sizeof(zend_long));
		} else {
			memcpy(zephir_buffer_doubles(return_value), zephir_buffer_doubles(obj),
				(size_t) len * sizeof(double));
		}
	}

	zend_long pos = len;

	ZEND_HASH_FOREACH_VAL(ht, other) {
		zend_long other_len = zephir_buffer_len(other);

		if (other_len > 0) {
			if (kind == ZEPHIR_BUFFER_LONG) {
				memcpy(zephir_buffer_longs(return_value) + pos, zephir_buffer_longs(other),
					(size_t) other_len * sizeof(zend_long));
			} else {
				memcpy(zephir_buffer_doubles(return_value) + pos, zephir_buffer_doubles(other),
					(size_t) other_len * sizeof(double));
			}
		}

		pos += other_len;
	} ZEND_HASH_FOREACH_END();
}

/**
 * Return a new array of Buffers splitting the given buffer into chunks of
 * the given length.
 *
 * @param return_value
 * @param obj
 * @param chunk_length
 */
void tensor_buffer_split(zval * return_value, zval * obj, zval * chunk_length)
{
	uint8_t kind = zephir_buffer_kind(obj);
	zend_long len = zephir_buffer_len(obj);
	zend_long chunkHat = zephir_get_intval(chunk_length);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Argument must be a Buffer object."));
		return;
	}

	zend_long chunks;

	if (UNEXPECTED(len == 0)) {
		chunks = 0;
	} else if (UNEXPECTED(chunkHat > len)) {
		/* The whole buffer fits in one chunk; avoids the overflow of
		 * len + chunkHat - 1 for a chunk length near LONG_MAX. */
		chunks = 1;
	} else {
		/* Both operands are at most 2 * len, so the sum cannot overflow. */
		chunks = (len + chunkHat - 1) / chunkHat;
	}

	zend_long i;

	array_init_size(return_value, (zend_ulong) chunks);

	for (i = 0; i < chunks; ++i) {
		zend_long start = i * chunkHat;
		zend_long chunk_len = len - start < chunkHat ? len - start : chunkHat;
		zval buffer;

		if (UNEXPECTED(zephir_buffer_create(&buffer, chunk_len, kind) == FAILURE)) {
			return;
		}

		if (chunk_len > 0) {
			if (kind == ZEPHIR_BUFFER_LONG) {
				memcpy(zephir_buffer_longs(&buffer), zephir_buffer_longs(obj) + start,
					(size_t) chunk_len * sizeof(zend_long));
			} else {
				memcpy(zephir_buffer_doubles(&buffer), zephir_buffer_doubles(obj) + start,
					(size_t) chunk_len * sizeof(double));
			}
		}

		add_next_index_zval(return_value, &buffer);
	}
}

/**
 * Return a new Buffer populated with the elements of the given buffer
 * repeated the given number of times.
 *
 * @param return_value
 * @param obj
 * @param times
 */
void tensor_buffer_repeat(zval * return_value, zval * obj, zval * times)
{
	uint8_t kind = zephir_buffer_kind(obj);
	zend_long len = zephir_buffer_len(obj);
	zend_long timesHat = zephir_get_intval(times);

	if (UNEXPECTED(kind == 0)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Argument must be a Buffer object."));
		return;
	}

	/* Reject before len * timesHat wraps; the product is then guaranteed to
	 * fit, so every i * len destination offset stays in bounds. */
	if (UNEXPECTED(len > 0 && timesHat > ZEND_LONG_MAX / len)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Repeat count must not overflow the buffer length."));
		return;
	}

	zend_long total = len * timesHat;
	zend_long i;

	if (UNEXPECTED(zephir_buffer_create(return_value, total, kind) == FAILURE)) {
		return;
	}

	if (len > 0) {
		for (i = 0; i < timesHat; ++i) {
			if (kind == ZEPHIR_BUFFER_LONG) {
				memcpy(zephir_buffer_longs(return_value) + i * len, zephir_buffer_longs(obj),
					(size_t) len * sizeof(zend_long));
			} else {
				memcpy(zephir_buffer_doubles(return_value) + i * len, zephir_buffer_doubles(obj),
					(size_t) len * sizeof(double));
			}
		}
	}
}

#endif