
#ifdef HAVE_CONFIG_H
#include "../../ext_config.h"
#endif

#include <php.h>
#include "../../php_ext.h"
#include "../../ext.h"

#include <Zend/zend_operators.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/fcall.h"
#include "kernel/operators.h"
#include "kernel/exception.h"
#include "kernel/concat.h"
#include "kernel/array.h"
#include "kernel/object.h"
#include "include/linear_algebra.h"


/**
 * Eigen
 *
 * The Eigendecompositon or (Spectral decomposition) is a matrix factorization resulting in a
 * matrix of eigenvectors and a corresponding vector of eigenvalues.
 *
 * For non-symmetric real matrices the eigenvalues come in complex conjugate pairs.
 * This class returns the real and imaginary parts as parallel `Vector` objects, see
 * `eigenvalues()` and `eigenvaluesImaginary()`. For a complex conjugate pair,
 * the two corresponding eigenvector columns are the real and imaginary parts
 * of a single complex eigenvector.
 *
 * For symmetric matrices (`symmetric === true`), all eigenvalues are real and
 * `eigenvaluesImaginary()` returns a zero-filled vector matching the size.
 *
 * @category    Scientific Computing
 * @package     Rubix/Tensor
 * @author      Andrew DalPino
 */
ZEPHIR_INIT_CLASS(Tensor_Decompositions_Eigen)
{
	ZEPHIR_REGISTER_CLASS(Tensor\\Decompositions, Eigen, tensor, decompositions_eigen, tensor_decompositions_eigen_method_entry, 0);

	/**
	 * The computed eigenvalues (real parts).
	 *
	 * @var \Tensor\Vector
	 */
	zend_declare_property_null(tensor_decompositions_eigen_ce, SL("eigenvalues"), ZEND_ACC_PROTECTED);
	/**
	 * The eigenvectors of the eigendecomposition.
	 *
	 * @var \Tensor\Matrix
	 */
	zend_declare_property_null(tensor_decompositions_eigen_ce, SL("eigenvectors"), ZEND_ACC_PROTECTED);
	/**
	 * The imaginary parts of the computed eigenvalues, in the same order as
	 * `eigenvalues`. Zero-filled when the input is symmetric.
	 *
	 * @var \Tensor\Vector
	 */
	zend_declare_property_null(tensor_decompositions_eigen_ce, SL("eigenvaluesImaginary"), ZEND_ACC_PROTECTED);
	return SUCCESS;
}

/**
 * Factory method to decompose a matrix.
 *
 * @param \Tensor\Matrix a
 * @param bool symmetric
 * @throws \Tensor\Exceptions\InvalidArgumentException
 * @throws \Tensor\Exceptions\RuntimeException
 * @return self
 */
PHP_METHOD(Tensor_Decompositions_Eigen, decompose)
{
	zephir_method_globals *ZEPHIR_METHOD_GLOBALS_PTR = NULL;
	zend_long ZEPHIR_LAST_CALL_STATUS;
	zend_bool symmetric;
	zval *a, a_sub, *symmetric_param = NULL, _0, result, eigenvalues, _8, eigenvaluesImaginary, _9, eigenvectors, _10, _11, _12, _13, _1$$3, _2$$3, _3$$3, _4$$4, _5$$4, _6$$5, _7$$5;

	ZVAL_UNDEF(&a_sub);
	ZVAL_UNDEF(&_0);
	ZVAL_UNDEF(&result);
	ZVAL_UNDEF(&eigenvalues);
	ZVAL_UNDEF(&_8);
	ZVAL_UNDEF(&eigenvaluesImaginary);
	ZVAL_UNDEF(&_9);
	ZVAL_UNDEF(&eigenvectors);
	ZVAL_UNDEF(&_10);
	ZVAL_UNDEF(&_11);
	ZVAL_UNDEF(&_12);
	ZVAL_UNDEF(&_13);
	ZVAL_UNDEF(&_1$$3);
	ZVAL_UNDEF(&_2$$3);
	ZVAL_UNDEF(&_3$$3);
	ZVAL_UNDEF(&_4$$4);
	ZVAL_UNDEF(&_5$$4);
	ZVAL_UNDEF(&_6$$5);
	ZVAL_UNDEF(&_7$$5);
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJECT_OF_CLASS(a, tensor_matrix_ce)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(symmetric)
	ZEND_PARSE_PARAMETERS_END();
	ZEPHIR_METHOD_GLOBALS_PTR = pecalloc(1, sizeof(zephir_method_globals), 0);
	zephir_memory_grow_stack(ZEPHIR_METHOD_GLOBALS_PTR, __func__);
	zephir_fetch_params(1, 1, 1, &a, &symmetric_param);
	if (!symmetric_param) {
		symmetric = 0;
	} else {
		}
	ZEPHIR_CALL_METHOD(&_0, a, "isSquare", NULL, 0);
	zephir_check_call_status();
	if (UNEXPECTED(!zephir_is_true(&_0))) {
		ZEPHIR_INIT_VAR(&_1$$3);
		object_init_ex(&_1$$3, tensor_exceptions_invalidargumentexception_ce);
		ZEPHIR_CALL_METHOD(&_2$$3, a, "shapeString", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_INIT_VAR(&_3$$3);
		ZEPHIR_CONCAT_SSVS(&_3$$3, "Matrix must be", " square, ", &_2$$3, " given.");
		ZEPHIR_CALL_METHOD(NULL, &_1$$3, "__construct", NULL, 2, &_3$$3);
		zephir_check_call_status();
		zephir_throw_exception_debug(&_1$$3, "tensor/decompositions/eigen.zep", 64);
		ZEPHIR_MM_RESTORE();
		return;
	}
	if (symmetric) {
		ZEPHIR_INIT_VAR(&result);
		ZEPHIR_CALL_METHOD(&_4$$4, a, "buffer", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_CALL_METHOD(&_5$$4, a, "n", NULL, 0);
		zephir_check_call_status();
		tensor_eig_symmetric(&result, &_4$$4, &_5$$4);
	} else {
		ZEPHIR_INIT_NVAR(&result);
		ZEPHIR_CALL_METHOD(&_6$$5, a, "buffer", NULL, 0);
		zephir_check_call_status();
		ZEPHIR_CALL_METHOD(&_7$$5, a, "n", NULL, 0);
		zephir_check_call_status();
		tensor_eig(&result, &_6$$5, &_7$$5);
	}
	if (Z_TYPE_P(&result) == IS_NULL) {
		ZEPHIR_THROW_EXCEPTION_DEBUG_STR(tensor_exceptions_runtimeexception_ce, "Failed to decompose matrix.", "tensor/decompositions/eigen.zep", 76);
		return;
	}
	zephir_memory_observe(&_8);
	zephir_array_fetch_long(&_8, &result, 0, PH_NOISY, "tensor/decompositions/eigen.zep", 79);
	ZEPHIR_CALL_CE_STATIC(&eigenvalues, tensor_vector_ce, "fromBuffer", NULL, 0, &_8);
	zephir_check_call_status();
	zephir_memory_observe(&_9);
	zephir_array_fetch_long(&_9, &result, 1, PH_NOISY, "tensor/decompositions/eigen.zep", 80);
	ZEPHIR_CALL_CE_STATIC(&eigenvaluesImaginary, tensor_vector_ce, "fromBuffer", NULL, 0, &_9);
	zephir_check_call_status();
	zephir_memory_observe(&_11);
	zephir_array_fetch_long(&_11, &result, 2, PH_NOISY, "tensor/decompositions/eigen.zep", 81);
	ZEPHIR_CALL_METHOD(&_12, a, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&_13, a, "n", NULL, 0);
	zephir_check_call_status();
	ZEPHIR_CALL_CE_STATIC(&_10, tensor_matrix_ce, "fromBuffer", NULL, 0, &_11, &_12, &_13);
	zephir_check_call_status();
	ZEPHIR_CALL_METHOD(&eigenvectors, &_10, "transpose", NULL, 0);
	zephir_check_call_status();
	object_init_ex(return_value, tensor_decompositions_eigen_ce);
	ZEPHIR_CALL_METHOD(NULL, return_value, "__construct", NULL, 13, &eigenvalues, &eigenvectors, &eigenvaluesImaginary);
	zephir_check_call_status();
	RETURN_MM();
}

/**
 * @param \Tensor\Vector eigenvalues
 * @param \Tensor\Matrix eigenvectors
 * @param \Tensor\Vector eigenvaluesImaginary
 */
PHP_METHOD(Tensor_Decompositions_Eigen, __construct)
{
	zval *eigenvalues, eigenvalues_sub, *eigenvectors, eigenvectors_sub, *eigenvaluesImaginary, eigenvaluesImaginary_sub;
	zval *this_ptr = getThis();

	ZVAL_UNDEF(&eigenvalues_sub);
	ZVAL_UNDEF(&eigenvectors_sub);
	ZVAL_UNDEF(&eigenvaluesImaginary_sub);
	static zend_string *_zephir_prop_0 = NULL;
	static zend_string *_zephir_prop_1 = NULL;
	static zend_string *_zephir_prop_2 = NULL;
	if (UNEXPECTED(!_zephir_prop_0)) {
		_zephir_prop_0 = zend_string_init("eigenvalues", 11, 1);
	}
	if (UNEXPECTED(!_zephir_prop_1)) {
		_zephir_prop_1 = zend_string_init("eigenvectors", 12, 1);
	}
	if (UNEXPECTED(!_zephir_prop_2)) {
		_zephir_prop_2 = zend_string_init("eigenvaluesImaginary", 20, 1);
	}

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_OBJECT_OF_CLASS(eigenvalues, tensor_vector_ce)
		Z_PARAM_OBJECT_OF_CLASS(eigenvectors, tensor_matrix_ce)
		Z_PARAM_OBJECT_OF_CLASS(eigenvaluesImaginary, tensor_vector_ce)
	ZEND_PARSE_PARAMETERS_END();
	zephir_fetch_params_without_memory_grow(3, 0, &eigenvalues, &eigenvectors, &eigenvaluesImaginary);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_0, 6, eigenvalues);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_1, 7, eigenvectors);
	zephir_update_property_zval_cached(this_ptr, _zephir_prop_2, 8, eigenvaluesImaginary);
}

/**
 * Return the eigenvalues.
 *
 * @return \Tensor\Vector
 */
PHP_METHOD(Tensor_Decompositions_Eigen, eigenvalues)
{

	RETURN_MEMBER(getThis(), "eigenvalues");
}

/**
 * Return the eigenvectors. For a complex conjugate eigenvalue pair the
 * two corresponding columns are the real and imaginary parts of a single
 * complex eigenvector.
 *
 * @return \Tensor\Matrix
 */
PHP_METHOD(Tensor_Decompositions_Eigen, eigenvectors)
{

	RETURN_MEMBER(getThis(), "eigenvectors");
}

/**
 * Return the imaginary parts of the eigenvalues, in the same order as
 * `eigenvalues()`. The i'th complex eigenvalue is
 * `eigenvalues()[i] + i * eigenvaluesImaginary()[i]`. Zero-filled for
 * symmetric inputs.
 *
 * @return \Tensor\Vector
 */
PHP_METHOD(Tensor_Decompositions_Eigen, eigenvaluesImaginary)
{

	RETURN_MEMBER(getThis(), "eigenvaluesImaginary");
}

