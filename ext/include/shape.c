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

/**
 * Return the transpose of the matrix as a new matrix buffer, i.e. row i of the
 * output holds column i of the input. Eliminates the strided per-column slices
 * and concatenation used by the previous pure-Zephir path.
 *
 * @param return_value
 * @param a
 * @param m
 * @param n
 */
void tensor_matrix_transpose(zval * return_value, zval * a, zval * m, zval * n)
{
	zend_long total = 0;
	int ok = 0;

	double * va = tensor_tensorbuffer_doubles(a, &total, &ok);

	if (UNEXPECTED(!ok)) {
		return;
	}

	zend_long ma = zephir_get_intval(m);
	zend_long na = zephir_get_intval(n);

	if (UNEXPECTED(ma < 0 || na < 0)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("Dimensions must be non-negative."));
		return;
	}

	if (UNEXPECTED(total != ma * na)) {
		zephir_throw_exception_string(spl_ce_LengthException,
			SL("Input buffer must match the given dimensions."));
		return;
	}

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create_uninit(return_value, total, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	if (total > 0) {
		zend_long i, j;

		for (i = 0; i < ma; ++i) {
			const double * src = va + i * na;
			double * dst = vc + i;

			for (j = 0; j < na; ++j) {
				dst[j * ma] = src[j];
			}
		}
	}

	zval_ptr_dtor(&c);
}