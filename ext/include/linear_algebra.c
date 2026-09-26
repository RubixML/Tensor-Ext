#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <math.h>
#include <ext/spl/spl_exceptions.h>
#include <cblas.h>
#include <lapacke.h>
#include "kernel/operators.h"
#include "php_ext.h"
#include "kernel/buffer.h"
#include "include/buffer.h"

/**
 * Matrix-matrix multiplication i.e. linear transformation of matrices A and B.
 * 
 * @param return_value
 * @param a
 * @param b
 * @param m
 * @param p
 * @param n
 */
void tensor_matmul(zval * return_value, zval * a, zval * b, zval * m, zval * p, zval * n)
{
    zend_long i;
    zend_long ma = zephir_get_intval(m);
    zend_long pa = zephir_get_intval(p);
    zend_long nb = zephir_get_intval(n);
    zend_long na = 0, nbb = 0;
    int ok_a = 0, ok_b = 0;

    double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);
    double * vb = tensor_tensorbuffer_doubles(b, &nbb, &ok_b);

    if (UNEXPECTED(!ok_a || !ok_b)) {
        return;
    }

    if (UNEXPECTED(na != ma * pa || nbb != pa * nb)) {
        zephir_throw_exception_string(spl_ce_LengthException,
            SL("Input buffers must match the given dimensions."));
        return;
    }

    zval c;

    if (UNEXPECTED(tensor_tensorbuffer_zeros(return_value, ma * nb, &c) == FAILURE)) {
        return;
    }

    double * vc = zephir_buffer_doubles(&c);

    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, ma, nb, pa, 1.0, va, pa, vb, nb, 0.0, vc, nb);

    zval_ptr_dtor(&c);
}

/**
 * Matrix-vector product i.e. the dot product of matrix A and vector B.
 *
 * @param return_value
 * @param a
 * @param b
 * @param m
 * @param p
 */
void tensor_matrix_dot(zval * return_value, zval * a, zval * b, zval * m, zval * p)
{
    zend_long ma = zephir_get_intval(m);
    zend_long pc = zephir_get_intval(p);
    zend_long na = 0, nb = 0;
    int ok_a = 0, ok_b = 0;

    double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);
    double * vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);

    if (UNEXPECTED(!ok_a || !ok_b)) {
        return;
    }

    if (UNEXPECTED(na != ma * pc || nb != pc)) {
        zephir_throw_exception_string(spl_ce_LengthException,
            SL("Input buffers must match the given dimensions."));
        return;
    }

    zval c;

    if (UNEXPECTED(tensor_tensorbuffer_zeros(return_value, ma, &c) == FAILURE)) {
        return;
    }

    double * vc = zephir_buffer_doubles(&c);

    cblas_dgemv(CblasRowMajor, CblasNoTrans, (blasint) ma, (blasint) pc, 1.0, va, (blasint) pc, vb, 1, 0.0, vc, 1);

    zval_ptr_dtor(&c);
}

/**
 * Dot product between vectors A and B.
 * 
 * @param return_value
 * @param a
 * @param b
 */
void tensor_dot(zval * return_value, zval * a, zval * b)
{
	zend_long na = 0, nb = 0;
	int ok_a = 0, ok_b = 0;

	double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);
	double * vb = tensor_tensorbuffer_doubles(b, &nb, &ok_b);

	if (UNEXPECTED(!ok_a || !ok_b)) {
		return;
	}

	if (UNEXPECTED(na != nb)) {
		zephir_throw_exception_string(spl_ce_LengthException,
			SL("Input buffers must be the same length."));
		return;
	}

	RETVAL_DOUBLE(cblas_ddot((blasint) na, va, 1, vb, 1));
}

/**
 * Return the multiplicative inverse of a square matrix A.
 *
 * @param return_value
 * @param a
 * @param n
 */
void tensor_inverse(zval * return_value, zval * a, zval * n)
{
    zend_long i;
    zend_long nn = zephir_get_intval(n);
    zend_long na = 0;
    int ok_a = 0;

    double * va = tensor_tensorbuffer_doubles(a, &na, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    if (UNEXPECTED(na != nn * nn)) {
        zephir_throw_exception_string(spl_ce_LengthException,
            SL("Input buffer must match the given dimensions."));
        return;
    }

    double * w = emalloc(na * sizeof(double));
    int * pivots = emalloc(nn * sizeof(int));

    for (i = 0; i < na; ++i) {
        w[i] = va[i];
    }

    lapack_int status;

    status = LAPACKE_dgetrf(LAPACK_ROW_MAJOR, nn, nn, w, nn, pivots);

    if (status != 0) {
        efree(w);
        efree(pivots);

        RETURN_NULL();
    }

    status = LAPACKE_dgetri(LAPACK_ROW_MAJOR, nn, w, nn, pivots);

    if (status != 0) {
        efree(w);
        efree(pivots);

        RETURN_NULL();
    }

    zval c;

    if (UNEXPECTED(tensor_tensorbuffer_create_uninit(return_value, na, &c) == FAILURE)) {
        efree(w);
        efree(pivots);

        return;
    }

    double * vc = zephir_buffer_doubles(&c);

    for (i = 0; i < na; ++i) {
        vc[i] = w[i];
    }

    zval_ptr_dtor(&c);

    efree(w);
    efree(pivots);
}

/**
 * Return the (Moore-Penrose) pseudoinverse of a general matrix A.
 * 
 * @param return_value
 * @param a
 * @param m
 * @param n
 */
void tensor_pseudoinverse(zval * return_value, zval * a, zval * m, zval * n)
{
    zend_long i;
    zend_long ma = zephir_get_intval(m);
    zend_long na = zephir_get_intval(n);
    zend_long nbuf = 0;
    int ok_a = 0;

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    if (UNEXPECTED(nbuf != ma * na)) {
        zephir_throw_exception_string(spl_ce_LengthException,
            SL("Input buffer must match the given dimensions."));
        return;
    }

    unsigned int k = MIN(ma, na);

    double * w = emalloc(nbuf * sizeof(double));
    double * vu = emalloc(ma * ma * sizeof(double));
    double * vs = emalloc(k * sizeof(double));
    double * vvt = emalloc(na * na * sizeof(double));
    double * vb = emalloc(na * ma * sizeof(double));

    for (i = 0; i < nbuf; ++i) {
        w[i] = va[i];
    }

    lapack_int status = LAPACKE_dgesdd(LAPACK_ROW_MAJOR, 'A', ma, na, w, na, vs, vu, ma, vvt, na);

    if (status != 0) {
        efree(w);
        efree(vu);
        efree(vs);
        efree(vvt);
        efree(vb);

        RETURN_NULL();
    }

    for (i = 0; i < k; ++i) {
        cblas_dscal(ma, 1.0 / vs[i], &vu[i], ma);
    }

    cblas_dgemm(CblasRowMajor, CblasTrans, CblasTrans, na, ma, ma, 1.0, vvt, na, vu, ma, 0.0, vb, ma);

    zval c;

    if (UNEXPECTED(tensor_tensorbuffer_create_uninit(return_value, na * ma, &c) == FAILURE)) {
        efree(w);
        efree(vu);
        efree(vs);
        efree(vvt);
        efree(vb);

        return;
    }

    double * vc = zephir_buffer_doubles(&c);

    for (i = 0; i < na * ma; ++i) {
        vc[i] = vb[i];
    }

    zval_ptr_dtor(&c);

    efree(w);
    efree(vu);
    efree(vs);
    efree(vvt);
    efree(vb);
}

/**
 * Reduce a (possibly singular) row echelon matrix stored in `w` in place,
 * mirroring the pure-PHP row reduction path. Pivot rows are not normalized so
 * the output matches the non-singular (LAPACK dgetrf) path. Returns the
 * number of row swaps performed.
 *
 * @param w
 * @param m
 * @param n
 * @return long
 */
static long tensor_ref_singular(double * w, unsigned int m, unsigned int n)
{
    unsigned int i, j;

    double epsilon = 0.00000001;
    double pivot, scale, tmp;
    unsigned int r = 0;
    unsigned int c = 0;
    long swaps = 0;

    while (r < m && c < n) {
        double * pivotRow = w + r * n;

        if (fabs(pivotRow[c]) < epsilon) {
            for (i = r + 1; i < m; ++i) {
                if (fabs(w[i * n + c]) >= epsilon) {
                    for (j = 0; j < n; ++j) {
                        tmp = pivotRow[j];
                        pivotRow[j] = w[i * n + j];
                        w[i * n + j] = tmp;
                    }

                    ++swaps;

                    break;
                }
            }
        }

        if (fabs(pivotRow[c]) < epsilon) {
            ++c;

            continue;
        }

        pivot = pivotRow[c];

        for (i = r + 1; i < m; ++i) {
            scale = w[i * n + c] / pivot;

            if (fabs(scale) >= epsilon) {
                for (j = 0; j < n; ++j) {
                    w[i * n + j] -= scale * pivotRow[j];
                }
            }
        }

        ++r;
        ++c;
    }

    return swaps;
}

/**
 * Bring a matrix in row-echelon form on a scratch buffer in place, mirroring
 * the pure-PHP forward elimination path. Returns the number of row swaps and
 * sets `*status` to the LAPACK result (negative on failure).
 */
static long tensor_ref_step(double * w, const double * orig, unsigned int m, unsigned int n, lapack_int * status);

/**
 * Compute the row echelon form (REF) of matrix A, reading A out of a
 * TensorBuffer, and return a tuple with the reduced matrix (as a TensorBuffer)
 * and the number of row swaps performed.
 *
 * @param return_value
 * @param a
 * @param m
 * @param n
 */
void tensor_ref(zval * return_value, zval * a, zval * m, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;
    unsigned int i;

    unsigned int ma = (unsigned int) zephir_get_intval(m);
    unsigned int na = (unsigned int) zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    if (UNEXPECTED(nbuf != (zend_long) ma * na)) {
        zephir_throw_exception_string(spl_ce_LengthException,
            SL("Input buffer must match the given dimensions."));
        return;
    }

    double * w = emalloc(ma * na * sizeof(double));

    for (i = 0; i < ma * na; ++i) {
        w[i] = va[i];
    }

    lapack_int status;

    long swaps = tensor_ref_step(w, va, ma, na, &status);

    if (status < 0) {
        efree(w);

        RETURN_NULL();
    }

    zval matrix, buf;

    if (UNEXPECTED(tensor_tensorbuffer_create_uninit(&matrix, (zend_long) ma * na, &buf) == FAILURE)) {
        efree(w);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&buf);

        for (i = 0; i < (zend_ulong)(ma * na); ++i) {
            vc[i] = w[i];
        }
    }

    zval_ptr_dtor(&buf);

    zval tuple;

    array_init_size(&tuple, 2);

    add_next_index_zval(&tuple, &matrix);
    add_next_index_long(&tuple, swaps);

    RETVAL_ARR(Z_ARR(tuple));

    efree(w);
}

/**
 * Compute the Cholesky decomposition of matrix A (read from a TensorBuffer)
 * and return the lower triangular matrix as a TensorBuffer.
 *
 * @param return_value
 * @param a
 * @param n
 */
void tensor_cholesky(zval * return_value, zval * a, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;
    unsigned int i, j;

    unsigned int na = (unsigned int) zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    if (UNEXPECTED(nbuf != (zend_long) na * na)) {
        zephir_throw_exception_string(spl_ce_LengthException,
            SL("Input buffer must match the given dimensions."));
        return;
    }

    double * w = emalloc(na * na * sizeof(double));

    for (i = 0; i < na * na; ++i) {
        w[i] = va[i];
    }

    lapack_int status = LAPACKE_dpotrf(LAPACK_ROW_MAJOR, 'L', na, w, na);

    if (status != 0) {
        efree(w);

        RETURN_NULL();
    }

    /* zero the upper triangle so the result is a proper lower triangular matrix */
    for (i = 0; i < na; ++i) {
        for (j = i + 1; j < na; ++j) {
            w[i * na + j] = 0.0;
        }
    }

    zval l, buf;

    if (UNEXPECTED(tensor_tensorbuffer_create_uninit(&l, (zend_long) na * na, &buf) == FAILURE)) {
        efree(w);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&buf);

        for (i = 0; i < (zend_ulong)(na * na); ++i) {
            vc[i] = w[i];
        }
    }

    zval_ptr_dtor(&buf);

    *return_value = l;

    efree(w);
}

/**
 * Compute the LU factorization of matrix A (read from a TensorBuffer) and
 * return a tuple with the lower, upper, and permutation matrices (each as a
 * TensorBuffer).
 *
 * @param return_value
 * @param a
 * @param n
 */
void tensor_lu(zval * return_value, zval * a, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;
    unsigned int i, j;

    unsigned int na = (unsigned int) zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    if (UNEXPECTED(nbuf != (zend_long) na * na)) {
        zephir_throw_exception_string(spl_ce_LengthException,
            SL("Input buffer must match the given dimensions."));
        return;
    }

    unsigned int * perm;
    double * va_ = emalloc(na * na * sizeof(double));
    int * pivots = emalloc(na * sizeof(int));

    for (i = 0; i < na * na; ++i) {
        va_[i] = va[i];
    }

    lapack_int status = LAPACKE_dgetrf(LAPACK_ROW_MAJOR, na, na, va_, na, pivots);

    if (status != 0) {
        efree(va_);
        efree(pivots);

        RETURN_NULL();
    }

    double * lbuf = emalloc(na * na * sizeof(double));
    double * ubuf = emalloc(na * na * sizeof(double));
    double * pbuf = emalloc(na * na * sizeof(double));

    for (i = 0; i < na; ++i) {
        for (j = 0; j < i; ++j) {
            lbuf[i * na + j] = va_[i * na + j];
        }

        lbuf[i * na + i] = 1.0;

        for (j = i + 1; j < na; ++j) {
            lbuf[i * na + j] = 0.0;
        }

        for (j = 0; j < i; ++j) {
            ubuf[i * na + j] = 0.0;
        }

        for (j = i; j < na; ++j) {
            ubuf[i * na + j] = va_[i * na + j];
        }
    }

    perm = emalloc(na * sizeof(unsigned int));

    for (i = 0; i < na; ++i) {
        perm[i] = i;
    }

    for (i = 0; i < na; ++i) {
        unsigned int r = (unsigned int)(pivots[i] - 1);

        if (r != i) {
            unsigned int t = perm[i];

            perm[i] = perm[r];
            perm[r] = t;
        }
    }

    for (i = 0; i < na; ++i) {
        for (j = 0; j < na; ++j) {
            pbuf[i * na + j] = (j == perm[i]) ? 1.0 : 0.0;
        }
    }

    zval l, u, p, tuple;
    zval bufL, bufU, bufP;

    if (UNEXPECTED(tensor_tensorbuffer_create_uninit(&l, (zend_long) na * na, &bufL) == FAILURE)) {
        efree(perm);
        efree(lbuf);
        efree(ubuf);
        efree(pbuf);
        efree(va_);
        efree(pivots);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&bufL);

        for (i = 0; i < (zend_ulong)(na * na); ++i) {
            vc[i] = lbuf[i];
        }
    }

    zval_ptr_dtor(&bufL);

    if (UNEXPECTED(tensor_tensorbuffer_create_uninit(&u, (zend_long) na * na, &bufU) == FAILURE)) {
        zval_ptr_dtor(&l);
        
        efree(perm);
        efree(lbuf);
        efree(ubuf);
        efree(pbuf);
        efree(va_);
        efree(pivots);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&bufU);

        for (i = 0; i < (zend_ulong)(na * na); ++i) {
            vc[i] = ubuf[i];
        }
    }

    zval_ptr_dtor(&bufU);

    if (UNEXPECTED(tensor_tensorbuffer_create_uninit(&p, (zend_long) na * na, &bufP) == FAILURE)) {
        zval_ptr_dtor(&l);
        zval_ptr_dtor(&u);

        efree(perm);
        efree(lbuf);
        efree(ubuf);
        efree(pbuf);
        efree(va_);
        efree(pivots);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&bufP);

        for (i = 0; i < (zend_ulong)(na * na); ++i) {
            vc[i] = pbuf[i];
        }
    }

    zval_ptr_dtor(&bufP);

    array_init_size(&tuple, 3);

    add_next_index_zval(&tuple, &l);
    add_next_index_zval(&tuple, &u);
    add_next_index_zval(&tuple, &p);

    RETVAL_ARR(Z_ARR(tuple));

    efree(perm);
    efree(lbuf);
    efree(ubuf);
    efree(pbuf);
    efree(va_);
    efree(pivots);
}

/**
 * Compute the eigendecomposition of a general matrix A and return the eigenvalues and eigenvectors in a tuple.
 * 
 * @param return_value
 * @param a
 */
void tensor_eig(zval * return_value, zval * a, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;
    unsigned int i;

    unsigned int na = (unsigned int) zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    if (UNEXPECTED(nbuf != (zend_long) na * na)) {
        zephir_throw_exception_string(spl_ce_LengthException,
            SL("Input buffer must match the given dimensions."));
        return;
    }

    double * w = emalloc(na * na * sizeof(double));
    double * wr = emalloc(na * sizeof(double));
    double * wi = emalloc(na * sizeof(double));
    double * vr = emalloc(na * na * sizeof(double));

    for (i = 0; i < na * na; ++i) {
        w[i] = va[i];
    }

    lapack_int status = LAPACKE_dgeev(LAPACK_ROW_MAJOR, 'N', 'V', na, w, na, wr, wi, NULL, na, vr, na);

    if (status != 0) {
        efree(w);
        efree(wr);
        efree(wi);
        efree(vr);

        RETURN_NULL();
    }

    zval eigenvalues;

    array_init_size(&eigenvalues, na);

    for (i = 0; i < na; ++i) {
        add_next_index_double(&eigenvalues, wr[i]);
    }

    zval eigenvectors, buf;

    if (UNEXPECTED(tensor_tensorbuffer_create_uninit(&eigenvectors, (zend_long) na * na, &buf) == FAILURE)) {
        zval_ptr_dtor(&eigenvalues);

        efree(w);
        efree(wr);
        efree(wi);
        efree(vr);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&buf);

        for (i = 0; i < (zend_ulong)(na * na); ++i) {
            vc[i] = vr[i];
        }
    }

    zval_ptr_dtor(&buf);

    zval tuple;

    array_init_size(&tuple, 2);

    add_next_index_zval(&tuple, &eigenvalues);
    add_next_index_zval(&tuple, &eigenvectors);

    RETVAL_ARR(Z_ARR(tuple));

    efree(w);
    efree(wr);
    efree(wi);
    efree(vr);
}

/**
 * Compute the eigendecomposition of a symmetric matrix A and return the eigenvalues and eigenvectors in a tuple.
 * 
 * @param return_value
 * @param a
 */
void tensor_eig_symmetric(zval * return_value, zval * a, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;
    unsigned int i;

    unsigned int na = (unsigned int) zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    if (UNEXPECTED(nbuf != (zend_long) na * na)) {
        zephir_throw_exception_string(spl_ce_LengthException,
            SL("Input buffer must match the given dimensions."));
        return;
    }

    double * w = emalloc(na * na * sizeof(double));
    double * wr = emalloc(na * sizeof(double));

    for (i = 0; i < na * na; ++i) {
        w[i] = va[i];
    }

    lapack_int status = LAPACKE_dsyev(LAPACK_ROW_MAJOR, 'V', 'U', na, w, na, wr);

    if (status != 0) {
        efree(w);
        efree(wr);

        RETURN_NULL();
    }

    zval eigenvalues;

    array_init_size(&eigenvalues, na);

    for (i = 0; i < na; ++i) {
        add_next_index_double(&eigenvalues, wr[i]);
    }

    zval eigenvectors, buf;

    if (UNEXPECTED(tensor_tensorbuffer_create_uninit(&eigenvectors, (zend_long) na * na, &buf) == FAILURE)) {
        zval_ptr_dtor(&eigenvalues);

        efree(w);
        efree(wr);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&buf);

        for (i = 0; i < (zend_ulong)(na * na); ++i) {
            vc[i] = w[i];
        }
    }

    zval_ptr_dtor(&buf);

    zval tuple;

    array_init_size(&tuple, 2);

    add_next_index_zval(&tuple, &eigenvalues);
    add_next_index_zval(&tuple, &eigenvectors);

    RETVAL_ARR(Z_ARR(tuple));

    efree(w);
    efree(wr);
}

/**
 * Compute the singular value decomposition of a matrix A and return the singular values and unitary matrices U and VT in a tuple.
 * 
 * @param return_value
 * @param a
 */
void tensor_svd(zval * return_value, zval * a, zval * m, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;
    unsigned int i;

    unsigned int ma = (unsigned int) zephir_get_intval(m);
    unsigned int na = (unsigned int) zephir_get_intval(n);
    unsigned int k = MIN(ma, na);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    if (UNEXPECTED(nbuf != (zend_long) ma * na)) {
        zephir_throw_exception_string(spl_ce_LengthException,
            SL("Input buffer must match the given dimensions."));
        return;
    }

    double * w = emalloc(ma * na * sizeof(double));
    double * vu = emalloc(ma * ma * sizeof(double));
    double * vs = emalloc(k * sizeof(double));
    double * vvt = emalloc(na * na * sizeof(double));

    for (i = 0; i < ma * na; ++i) {
        w[i] = va[i];
    }

    lapack_int status = LAPACKE_dgesdd(LAPACK_ROW_MAJOR, 'A', ma, na, w, na, vs, vu, ma, vvt, na);

    if (status != 0) {
        efree(w);
        efree(vu);
        efree(vs);
        efree(vvt);

        RETURN_NULL();
    }

    zval u, bufU;

    if (UNEXPECTED(tensor_tensorbuffer_create_uninit(&u, (zend_long) ma * ma, &bufU) == FAILURE)) {
        efree(w);
        efree(vu);
        efree(vs);
        efree(vvt);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&bufU);

        for (i = 0; i < (zend_ulong)(ma * ma); ++i) {
            vc[i] = vu[i];
        }
    }

    zval_ptr_dtor(&bufU);

    zval s;

    array_init_size(&s, k);

    for (i = 0; i < k; ++i) {
        add_next_index_double(&s, vs[i]);
    }

    zval vt, bufVt;

    if (UNEXPECTED(tensor_tensorbuffer_create_uninit(&vt, (zend_long) na * na, &bufVt) == FAILURE)) {
        zval_ptr_dtor(&u);
        zval_ptr_dtor(&s);

        efree(w);
        efree(vu);
        efree(vs);
        efree(vvt);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&bufVt);

        for (i = 0; i < (zend_ulong)(na * na); ++i) {
            vc[i] = vvt[i];
        }
    }

    zval_ptr_dtor(&bufVt);

    zval tuple;

    array_init_size(&tuple, 3);

    add_next_index_zval(&tuple, &u);
    add_next_index_zval(&tuple, &s);
    add_next_index_zval(&tuple, &vt);

    RETVAL_ARR(Z_ARR(tuple));

    efree(w);
    efree(vu);
    efree(vs);
    efree(vvt);
}

/**
 * Run the forward elimination + singular row reduction used by both
 * `tensor_ref` and `tensor_rref`. On failure (LAPACK error) sets *status to a
 * large negative value. On success returns the number of row swaps.
 */
static long tensor_ref_step(double * w, const double * orig, unsigned int m, unsigned int n, lapack_int * status)
{
    unsigned int i, j;

    if (m == 0 || n == 0) {
        *status = 0;

        return 0;
    }

    int * pivots = emalloc(MIN(m, n) * sizeof(int));

    *status = LAPACKE_dgetrf(LAPACK_ROW_MAJOR, m, n, w, n, pivots);

    long swaps = 0;

    if (*status > 0) {
        /* Singular: `dgetrf` left `w` partially eliminated. The pure-PHP REF
         * fallback must operate on the original (unmodified) matrix to match
         * the previous behaviour, so restore `w` from `orig` first. */
        for (i = 0; i < m * n; ++i) {
            w[i] = orig[i];
        }

        swaps = tensor_ref_singular(w, m, n);
    } else if (*status != 0) {
        efree(pivots);

        return 0;
    } else {
        for (i = 0; i < MIN(m, n); ++i) {
            if (i + 1 != (unsigned int) pivots[i]) {
                ++swaps;
            }
        }

        /* `dgetrf` returns the `L + U` factor in `w`. Extract the upper
         * triangular `U` (row-echelon form) by clearing the strictly-lower
         * triangle, matching the original `tensor_ref` output shape. */
        for (i = 0; i < m; ++i) {
            unsigned int lim = i < n ? i : n;

            for (j = 0; j < lim; ++j) {
                w[i * n + j] = 0.0;
            }
        }
    }

    efree(pivots);

    return swaps;
}

/**
 * Compute the reduced row echelon form (RREF) of matrix A (read from a
 * TensorBuffer). The matrix is first brought to row-echelon form via LAPACK
 * `dgetrf` (with a singular-matrix fallback), and then Gauss-Jordan
 * normalization and elimination are applied to produce unit pivots, matching
 * the pure-PHP `Rref::reduce` path.
 *
 * @param return_value
 * @param a
 * @param m
 * @param n
 */
void tensor_rref(zval * return_value, zval * a, zval * m, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;
    unsigned int i, j;

    double epsilon = 0.00000001;

    unsigned int ma = (unsigned int) zephir_get_intval(m);
    unsigned int na = (unsigned int) zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    if (UNEXPECTED(nbuf != (zend_long) ma * na)) {
        zephir_throw_exception_string(spl_ce_LengthException,
            SL("Input buffer must match the given dimensions."));
        return;
    }

    double * w = emalloc(ma * na * sizeof(double));

    for (i = 0; i < ma * na; ++i) {
        w[i] = va[i];
    }

    lapack_int status;

    (void) tensor_ref_step(w, va, ma, na, &status);

    if (status < 0) {
        efree(w);

        RETURN_NULL();
    }

    /* Normalize each pivot to 1 and eliminate the entries above it, matching
     * the pure-PHP `Rref::reduce` path. */
    unsigned int r = 0, c = 0;

    while (r < ma && c < na) {
        double pivot = w[r * na + c];

        if (fabs(pivot) < epsilon) {
            int found = 0;

            for (i = c; i < na; ++i) {
                if (fabs(w[r * na + i]) >= epsilon) {
                    found = 1;
                    break;
                }
            }

            if (!found) {
                for (j = c; j < na; ++j) {
                    w[r * na + j] = 0.0;
                }

                ++r;

                continue;
            }

            ++c;

            continue;
        }

        if (pivot != 1.0) {
            for (j = 0; j < na; ++j) {
                w[r * na + j] /= pivot;
            }
        }

        for (i = 0; i < r; ++i) {
            double scale = w[i * na + c];

            if (fabs(scale) >= epsilon) {
                for (j = 0; j < na; ++j) {
                    w[i * na + j] -= scale * w[r * na + j];
                }
            }
        }

        ++r;
        ++c;
    }

    zval result, buf;

    if (UNEXPECTED(tensor_tensorbuffer_create_uninit(&result, (zend_long) ma * na, &buf) == FAILURE)) {
        efree(w);

        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&buf);

        for (i = 0; i < (zend_ulong)(ma * na); ++i) {
            vc[i] = w[i];
        }
    }

    zval_ptr_dtor(&buf);

    *return_value = result;

    efree(w);
}

/**
 * Return the rank of an already-reduced matrix stored in a TensorBuffer, i.e.
 * the number of rows containing at least one non-zero element.
 *
 * @param return_value
 * @param a
 * @param m
 * @param n
 */
void tensor_rank(zval * return_value, zval * a, zval * m, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;
    unsigned int i, j;

    double epsilon = 0.00000001;

    unsigned int ma = (unsigned int) zephir_get_intval(m);
    unsigned int na = (unsigned int) zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    if (UNEXPECTED(nbuf != (zend_long) ma * na)) {
        zephir_throw_exception_string(spl_ce_LengthException,
            SL("Input buffer must match the given dimensions."));
        return;
    }

    unsigned int rank = 0;

    for (i = 0; i < ma; ++i) {
        for (j = 0; j < na; ++j) {
            if (fabs(va[i * na + j]) >= epsilon) {
                ++rank;

                break;
            }
        }
    }

    RETVAL_LONG((zend_long) rank);
}

/**
 * Return whether the square matrix A (read from a TensorBuffer) is symmetric
 * with respect to a strict element-wise equality (matching PHP `!=`).
 *
 * @param return_value
 * @param a
 * @param n
 */
void tensor_is_symmetric(zval * return_value, zval * a, zval * n)
{
    zend_long nbuf = 0;
    int ok_a = 0;

    unsigned int i, j;

    unsigned int na = (unsigned int) zephir_get_intval(n);

    double * va = tensor_tensorbuffer_doubles(a, &nbuf, &ok_a);

    if (UNEXPECTED(!ok_a)) {
        return;
    }

    if (UNEXPECTED(nbuf != (zend_long) na * na)) {
        zephir_throw_exception_string(spl_ce_LengthException,
            SL("Input buffer must be square."));
        return;
    }

    if (na < 2) {
        RETVAL_TRUE;

        return;
    }

    for (i = 0; i < na - 1; ++i) {
        for (j = i + 1; j < na; ++j) {
            if (va[i * na + j] != va[j * na + i]) {
                RETVAL_FALSE;

                return;
            }
        }
    }

    RETVAL_TRUE;
}

/**
 * Compute the outer product of two vectors (read from TensorBuffers) and
 * return the resulting matrix as a TensorBuffer.
 *
 * @param return_value
 * @param a
 * @param b
 * @param na
 * @param nb
 */
void tensor_outer(zval * return_value, zval * a, zval * b, zval * na, zval * nb)
{
    zend_long nbufa = 0, nbufb = 0;
    int ok_a = 0, ok_b = 0;

    unsigned int naHat = (unsigned int) zephir_get_intval(na);
    unsigned int nbHat = (unsigned int) zephir_get_intval(nb);

    double * va = tensor_tensorbuffer_doubles(a, &nbufa, &ok_a);
    double * vb = tensor_tensorbuffer_doubles(b, &nbufb, &ok_b);

    if (UNEXPECTED(!ok_a || !ok_b)) {
        return;
    }

    if (UNEXPECTED(nbufa != (zend_long) naHat || nbufb != (zend_long) nbHat)) {
        zephir_throw_exception_string(spl_ce_LengthException,
            SL("Buffer lengths must match the given dimensions."));
        return;
    }

    zval product, buf;

    if (UNEXPECTED(tensor_tensorbuffer_create_uninit(&product, (zend_long) naHat * nbHat, &buf) == FAILURE)) {
        return;
    }

    {
        double * vc = (double *) zephir_buffer_doubles(&buf);
        unsigned int i, j;

        for (i = 0; i < naHat; ++i) {
            for (j = 0; j < nbHat; ++j) {
                vc[i * nbHat + j] = va[i] * vb[j];
            }
        }
    }

    zval_ptr_dtor(&buf);

    *return_value = product;
}
