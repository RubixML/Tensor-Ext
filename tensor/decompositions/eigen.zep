namespace Tensor\Decompositions;

use Tensor\Matrix;
use Tensor\Vector;
use Tensor\ColumnVector;
use Tensor\Exceptions\InvalidArgumentException;
use Tensor\Exceptions\RuntimeException;

/**
 * Eigen
 *
 * The Eigendecompositon or (Spectral decomposition) is a matrix factorization resulting in a
 * matrix of eigenvectors and a corresponding array of eigenvalues.
 *
 * For non-symmetric real matrices the eigenvalues come in complex conjugate pairs.
 * This class returns the real and imaginary parts as parallel lists, see
 * `eigenvalues()` and `eigenvaluesImaginary()`. For a complex conjugate pair,
 * the two corresponding eigenvector columns are the real and imaginary parts
 * of a single complex eigenvector.
 *
 * For symmetric matrices (`symmetric === true`), all eigenvalues are real and
 * `eigenvaluesImaginary()` returns a zero-filled list matching the size.
 *
 * @category    Scientific Computing
 * @package     Rubix/Tensor
 * @author      Andrew DalPino
 */
class Eigen
{
    /**
     * The computed eigenvalues (real parts).
     *
     * @var array<float>
     */
    protected eigenvalues;

    /**
     * The imaginary parts of the computed eigenvalues, in the same order as
     * `eigenvalues`. Zero-filled when the input is symmetric.
     *
     * @var array<float>
     */
    protected eigenvaluesImaginary;

    /**
     * The eigenvectors of the eigendecomposition.
     *
     * @var \Tensor\Matrix
     */
    protected eigenvectors;

    /**
     * Factory method to decompose a matrix.
     *
     * @param \Tensor\Matrix a
     * @param bool symmetric
     * @throws \Tensor\Exceptions\InvalidArgumentException
     * @throws \Tensor\Exceptions\RuntimeException
     * @return self
     */
    public static function decompose(const <Matrix> a, const bool symmetric = false) -> <Eigen>
    {
        if unlikely !a->isSquare() {
            throw new InvalidArgumentException("Matrix must be"
                . " square, " . $a->shapeString() . " given.");
        }

        var result;

        if symmetric {
            let result = tensor_eig_symmetric(a->buffer(), a->n());
        } else {
            let result = tensor_eig(a->buffer(), a->n());
        }

        if is_null(result) {
            throw new RuntimeException("Failed to decompose matrix.");
        }

        array eig = [];

        let eig = (array) result;

        var eigenvalues = (array) eig[0];
        var eigenvaluesImaginary = (array) eig[1];
        var eigenvectors = Matrix::fromBuffer(eig[2], a->n(), a->n())->transpose();

        return new self(eigenvalues, eigenvectors, eigenvaluesImaginary);
    }

    /**
     * @param array<float> eigenvalues
     * @param \Tensor\Matrix eigenvectors
     * @param array<float> eigenvaluesImaginary
     */
    public function __construct(const array eigenvalues, const <Matrix> eigenvectors, const array eigenvaluesImaginary = [])
    {
        let this->eigenvalues = eigenvalues;
        let this->eigenvectors = eigenvectors;
        let this->eigenvaluesImaginary = eigenvaluesImaginary;
    }

    /**
     * Return the eigenvalues.
     *
     * @return array<float>
     */
    public function eigenvalues() -> array
    {
        return this->eigenvalues;
    }

    /**
     * Return the imaginary parts of the eigenvalues, in the same order as
     * `eigenvalues()`. The i'th complex eigenvalue is
     * `eigenvalues()[i] + i * eigenvaluesImaginary()[i]`. Zero-filled for
     * symmetric inputs.
     *
     * @return array<float>
     */
    public function eigenvaluesImaginary() -> array
    {
        return this->eigenvaluesImaginary;
    }

    /**
     * Return the eigenvectors. For a complex conjugate eigenvalue pair the
     * two corresponding columns are the real and imaginary parts of a single
     * complex eigenvector.
     *
     * @return \Tensor\Matrix
     */
    public function eigenvectors() -> <Matrix>
    {
        return this->eigenvectors;
    }
}
