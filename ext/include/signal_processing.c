#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <ext/spl/spl_exceptions.h>
#include "kernel/operators.h"
#include "kernel/buffer.h"
#include "include/buffer.h"

/**
 * 1D convolution between a vector A and B (kernel) with a given stride.
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

	double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);
	double * vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);

	if (UNEXPECTED(!ok_a || !ok_b)) {
		return;
	}

	zend_long s = zephir_get_intval(stride);

	zend_long nc = na + nb - 1;
	zend_long nout = (nc + s - 1) / s;

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, nout, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	zend_long idx = 0;

	for (i = 0; i < nc; i += s) {
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

    zend_long s = zephir_get_intval(stride);
    zend_long ma_ = zephir_get_intval(ma);
    zend_long na_ = zephir_get_intval(na);
    zend_long mb_ = zephir_get_intval(mb);
    zend_long nb_ = zephir_get_intval(nb);

    double * va = tensor_tensorbuffer_doubles(a, &nbufa, &ok_a);
    double * vb = tensor_tensorbuffer_doubles(b, &nbufb, &ok_b);

    if (UNEXPECTED(!ok_a || !ok_b)) {
        return;
    }

    if (UNEXPECTED(nbufa != ma_ * na_ || nbufb != mb_ * nb_)) {
        zephir_throw_exception_string(spl_ce_LengthException,
            SL("Input buffers must match the given dimensions."));
        return;
    }

    zend_long p = mb_ / 2;
    zend_long q = nb_ / 2;

    zend_long om = (ma_ + s - 1) / s;
    zend_long on = (na_ + s - 1) / s;

    zval c;

    if (UNEXPECTED(tensor_tensorbuffer_create(return_value, om * on, &c) == FAILURE)) {
        return;
    }

    double * vc = zephir_buffer_doubles(&c);

    zend_long idx = 0;

    for (i = 0; i < ma_; i += s) {
        for (j = 0; j < na_; j += s) {
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
