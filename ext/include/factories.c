#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ext/spl/spl_exceptions.h>
#include "kernel/main.h"
#include "php_ext.h"
#include "kernel/buffer.h"
#include "kernel/exception.h"
#include "kernel/operators.h"
#include "include/buffer.h"

/* Include PHP's MT19937 public API. The header has moved between PHP versions:
 *   <= 8.1 : ext/standard/php_rand.h
 *   8.2-8.3: ext/random/php_mt_rand.h
 *   8.4+   : ext/random/php_random.h
 * If none resolves (very old / custom build), fall back to manual prototypes.
 * The symbols are exported by the PHP binary regardless (verified with nm). */
# if defined(__has_include)
#   if __has_include(<ext/random/php_random.h>)
#     include <ext/random/php_random.h>
#   elif __has_include(<ext/random/php_mt_rand.h>)
#     include <ext/random/php_mt_rand.h>
#   elif __has_include(<ext/standard/php_rand.h>)
#     include <ext/standard/php_rand.h>
#   else
PHPAPI uint32_t php_mt_rand(void);
PHPAPI zend_long php_mt_rand_range(zend_long min, zend_long max);
#   endif
# else
#   include <ext/standard/php_rand.h>
# endif

#ifndef PHP_MT_RAND_MAX
#  define PHP_MT_RAND_MAX ((zend_long) 0x7FFFFFFF) /* (1<<31) - 1 */
#endif

/**
 * Create a tensor filled with a specific value.
 */
void tensor_fill(zval * return_value, zval * value, zval * n)
{
	zend_long length = zephir_get_intval(n);
	double v = zephir_get_doubleval(value);
	zval c;

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

/**
 * Create a diagonal matrix from an array of elements.
 */
void tensor_diagonal(zval * return_value, zval * elements)
{
	zend_long length = (zend_long) zend_hash_num_elements(Z_ARRVAL_P(elements));

	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, length * length, &c) == FAILURE)) {
		return;
	}

	if (length > 0) {
		double * vc = zephir_buffer_doubles(&c);
		zval * value;
		zend_long i = 0;

		memset(vc, 0, (size_t) length * length * sizeof(double));

		/* Positional: the keys of the source array are discarded, the values
		 * are taken in iteration order and written down the diagonal. */
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(elements), value) {
			vc[i * (length + 1)] = zephir_get_doubleval(value);
			++i;
		} ZEND_HASH_FOREACH_END();
	}

	zval_ptr_dtor(&c);
}

/**
 * Create a tensor filled with random values uniformly distributed in the range [0, 1).
 */
void tensor_random_uniform_01(zval * return_value, zval * n)
{
	zend_long length = zephir_get_intval(n);
	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, length, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);
	zend_long i;
	zend_long max = PHP_MT_RAND_MAX;

	for (i = 0; i < length; ++i) {
		vc[i] = (double)((zend_long)(php_mt_rand() >> 1)) / (double) max;
	}

	zval_ptr_dtor(&c);
}
/**
 * Create a tensor filled with random values uniformly distributed in the range [-1, 1).
 */
void tensor_random_uniform_pm1(zval * return_value, zval * n)
{
	zend_long length = zephir_get_intval(n);
	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, length, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);
	zend_long i;
	zend_long max = PHP_MT_RAND_MAX;

	for (i = 0; i < length; ++i) {
		vc[i] = (double) php_mt_rand_range(-max, max) / (double) max;
	}

	zval_ptr_dtor(&c);
}
/**
 * Create a tensor filled with random values drawn from a standard Gaussian distribution (mean 0, variance 1).
 */
void tensor_random_gaussian(zval * return_value, zval * n)
{
	zend_long length = zephir_get_intval(n);
	zval c;

	if (UNEXPECTED(tensor_tensorbuffer_create(return_value, length, &c) == FAILURE)) {
		return;
	}

	double * vc = zephir_buffer_doubles(&c);
	zend_long written = 0;
	zend_long max = PHP_MT_RAND_MAX;
	const double TWO_PI = 6.28318530718;

	while (written < length) {
		uint32_t r1, r2;
		double u1, u2;

		do {
			r1 = php_mt_rand() >> 1;
			u1 = (double) r1 / (double) max;
		} while (UNEXPECTED(u1 <= 0.0));

		r2 = php_mt_rand() >> 1;
		u2 = (double) r2 / (double) max;

		double r = sqrt(-2.0 * log(u1));
		double phi = u2 * TWO_PI;

		vc[written++] = r * sin(phi);

		if (written < length)
			vc[written++] = r * cos(phi);
	}

	zval_ptr_dtor(&c);
}
