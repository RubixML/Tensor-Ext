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
 */
void tensor_convolve_2d(zval * return_value, zval * a, zval * b, zval * stride)
{
    unsigned int i, j, k, l;
    int x, y;
    double sigma;
    zval * row;
    zval rowC, c;

    zend_array * aa = Z_ARR_P(a);
    zend_array * ab = Z_ARR_P(b);

    unsigned int s = zephir_get_intval(stride);

    unsigned int ma = zend_array_count(aa);
    unsigned int na = zend_array_count(Z_ARR_P(zend_hash_index_find(aa, 0)));
    unsigned int mb = zend_array_count(ab);
    unsigned int nb = zend_array_count(Z_ARR_P(zend_hash_index_find(ab, 0)));

    double * va = emalloc(ma * na * sizeof(double));
    double * vb = emalloc(mb * nb * sizeof(double));

    for (i = 0; i < ma; ++i) {
        row = zend_hash_index_find(aa, i);

        for (j = 0; j < na; ++j) {
            va[i * na + j] = zephir_get_doubleval(zend_hash_index_find(Z_ARR_P(row), j));
        }
    }

    for (i = 0; i < mb; ++i) {
        row = zend_hash_index_find(ab, i);

        for (j = 0; j < nb; ++j) {
            vb[i * nb + j] = zephir_get_doubleval(zend_hash_index_find(Z_ARR_P(row), j));
        }
    }

    unsigned int p = mb / 2;
    unsigned int q = nb / 2;

    array_init_size(&c, (ma + s - 1) / s);

    for (i = 0; i < ma; i += s) {
        array_init_size(&rowC, (na + s - 1) / s);

        for (j = 0; j < na; j += s) {   
            sigma = 0.0;

            for (k = 0; k < mb; ++k) {
                x = i + p - k;

                if (x < 0 || x >= ma) {
                    continue;
                }

                for (l = 0; l < nb; ++l) {
                    y = j + q - l;

                    if (y >= 0 && y < na) {
                        sigma += va[x * na + y] * vb[k * nb + l];
                    }
                }
            }

            add_next_index_double(&rowC, sigma);
        }

        add_next_index_zval(&c, &rowC);
    }

    RETVAL_ARR(Z_ARR(c));

    efree(va);
    efree(vb);
}
