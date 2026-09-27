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
 * Allocate a fresh `Tensor\TensorBuffer` of `n` elements, every one of which
 * is set to `value`. Returns the buffer in return_value.
 *
 * @param return_value
 * @param value
 * @param n
 */
void tensor_fill(zval * return_value, zval * value, zval * n)
{
	zend_long length = zephir_get_intval(n);
	double v = zephir_get_doubleval(value);
	zval c;

	if (UNEXPECTED(length < 1)) {
		zephir_throw_exception_string(spl_ce_InvalidArgumentException,
			SL("N must be greater than 0."));
		return;
	}

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, length, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);

	zend_long i;

	for (i = 0; i < length; ++i) {
		vc[i] = v;
	}

	zval_ptr_dtor(&c);
}
