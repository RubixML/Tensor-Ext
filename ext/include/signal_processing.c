#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <ext/spl/spl_exceptions.h>
#include "kernel/operators.h"
#include "php_ext.h"
#include "kernel/buffer.h"
#include "include/buffer.h"
#include "tensor/exceptions/invalidargumentexception.zep.h"

/**
 * Divide n by s, rounding the result up.
 *
 * Written as `n / s + (n % s ? 1 : 0)` rather than the usual `(n + s - 1) / s`
 * on purpose: the latter overflows zend_long for a stride near ZEND_LONG_MAX
 * and yields a length that disagrees with the number of samples the convolve
 * loops actually emit, which previously led to a write through a NULL buffer
 * pointer. This form cannot overflow because s >= 1 is enforced by the caller.
 */
static zend_long tensor_conv_ceil_div(zend_long n, zend_long s)
{
	return n / s + (n % s ? 1 : 0);
}

/**
 * 1D convolution between a vector A and B (kernel) with a given stride.
 *
 * The result is a "full" convolution sampled every `stride` samples, so it
 * holds ceil((na + nb - 1) / stride) elements.
 *
 * @param return_value
 * @param a
 * @param b
 * @param stride
 */
void tensor_convolve_1d(zval * return_value, zval * a, zval * b, zval * stride)
{
	zend_long i, j;
	zend_long jmin, jmax;
	double sigma;
	zend_long na = 0, nb = 0;
	int ok_a = 0, ok_b = 0;

	/* The optimizers emit this call as a statement and then pass the result
	 * straight to a constructor, so every path out of here has to leave
	 * *return_value defined even when it is about to throw. */
	ZVAL_NULL(return_value);

	double * restrict va = tensor_tensorbuffer_doubles(a, &na, &ok_a);

	/* Checked one at a time: unwrapping the second buffer while the first
	 * failure is already pending would only do work that gets discarded. */
	if (UNEXPECTED(!ok_a)) {
		return;
	}

	double * restrict vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);

	if (UNEXPECTED(!ok_b)) {
		return;
	}

	zend_long s = zephir_get_intval(stride);

	if (UNEXPECTED(s < 1)) {
		zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
			SL("Stride must be at least 1."));
		return;
	}

	/* Two empty buffers convolve to nothing, so guard the full-convolution
	 * length against going negative before it reaches the allocation. */
	zend_long nc = na + nb - 1;
	zend_long nout = nc > 0 ? tensor_conv_ceil_div(nc, s) : 0;

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, nout, &c) == FAILURE)) {
		return;
	}

	double * restrict vc = zephir_buffer_doubles(&c);

	zend_long idx = 0;

	/* Bounded by nout as well as by nc so the loop can never write past the
	 * allocation, even if the length above is ever computed wrongly. */
	for (i = 0; i < nc && idx < nout; i += s) {
		jmin = i >= nb - 1 ? i - (nb - 1) : 0;
		jmax = i < na ? i : na - 1;

		sigma = 0.0;

		for (j = jmin; j <= jmax; ++j) {
			sigma += va[j] * vb[i - j];
		}

		vc[idx++] = sigma;
	}

	zval_ptr_dtor(&c);
}

/**
 * 2D convolution between a matrix A and B (kernel) with a given stride using the "same" method for zero padding.
 *
 * The result holds ceil(ma / stride) x ceil(na / stride) elements, i.e. the
 * "same" shape as the input, sub-sampled every `stride` elements.
 *
 * @param return_value
 * @param a
 * @param b
 * @param stride
 * @param ma
 * @param na
 * @param mb
 * @param nb
 */
void tensor_convolve_2d(zval * return_value, zval * a, zval * b, zval * stride, zval * ma, zval * na, zval * mb, zval * nb)
{
    zend_long i, j, k, l;
    zend_long x, y;
    double sigma;
    zend_long nbufa = 0, nbufb = 0;
    int ok_a = 0, ok_b = 0;

    /* See tensor_convolve_1d: *return_value must be defined on every path. */
    ZVAL_NULL(return_value);

    zend_long s = zephir_get_intval(stride);
    zend_long ma_ = zephir_get_intval(ma);
    zend_long na_ = zephir_get_intval(na);
    zend_long mb_ = zephir_get_intval(mb);
    zend_long nb_ = zephir_get_intval(nb);

    if (UNEXPECTED(s < 1)) {
        zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
            SL("Stride must be at least 1."));
        return;
    }

    double * restrict va = tensor_tensorbuffer_doubles(a, &nbufa, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    double * restrict vb = tensor_tensorbuffer_doubles(b, &nbufb, &ok_b);

    if (UNEXPECTED(!ok_b)) {
        return;
    }

    if (UNEXPECTED(ma_ < 0 || na_ < 0 || mb_ < 0 || nb_ < 0)) {
        zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
            SL("Dimensions must be non-negative."));
        return;
    }

    if (UNEXPECTED(nbufa != ma_ * na_ || nbufb != mb_ * nb_)) {
        zephir_throw_exception_string(tensor_exceptions_invalidargumentexception_ce,
            SL("Input buffers must match the given dimensions."));
        return;
    }

    /* "Same" padding anchors the kernel's centre sample on the output sample.
     * Using (n - 1) / 2 rather than n / 2 keeps even-sized kernels aligned the
     * way numpy and scipy's mode='same' align them. */
    zend_long p = mb_ > 0 ? (mb_ - 1) / 2 : 0;
    zend_long q = nb_ > 0 ? (nb_ - 1) / 2 : 0;

    zend_long om = ma_ > 0 ? tensor_conv_ceil_div(ma_, s) : 0;
    zend_long on = na_ > 0 ? tensor_conv_ceil_div(na_, s) : 0;

    /* om <= ma_ and on <= na_ for s >= 1, so this product cannot overflow and
     * is bounded by the input size. */
    zend_long nout = om * on;

    zval c;

    if (UNEXPECTED(tensor_tensorbuffer_create(return_value, nout, &c) == FAILURE)) {
        return;
    }

    double * restrict vc = zephir_buffer_doubles(&c);

    zend_long idx = 0;

    /* Bounded by nout as well as by the input extents so the loop can never
     * write past the allocation. */
    for (i = 0; i < ma_ && idx < nout; i += s) {
        for (j = 0; j < na_ && idx < nout; j += s) {
            sigma = 0.0;

            for (k = 0; k < mb_; ++k) {
                x = i + p - k;

                if (x < 0 || x >= ma_) {
                    continue;
                }

                for (l = 0; l < nb_; ++l) {
                    y = j + q - l;

                    if (y >= 0 && y < na_) {
                        sigma += va[x * na_ + y] * vb[k * nb_ + l];
                    }
                }
            }

            vc[idx++] = sigma;
        }
    }

    zval_ptr_dtor(&c);
}
