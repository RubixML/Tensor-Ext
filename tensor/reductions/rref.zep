namespace Tensor\Reductions;

use Tensor\Matrix;
use Tensor\Exceptions\InvalidArgumentException;
use Tensor\Exceptions\RuntimeException;

/**
 * RREF
 *
 * The reduced row echelon form (RREF) of a matrix.
 *
 * @category    Scientific Computing
 * @package     Rubix/Tensor
 * @author      Andrew DalPino
 */
class Rref
{
    /**
     * The reduced matrix in reduced row echelon form.
     *
     * @var \Tensor\Matrix
     */
    protected a;

    /**
     * Factory method to decompose a matrix.
     *
     * @param \Tensor\Matrix a
     * @return self
     */
    public static function reduce(const <Matrix> a) -> <Rref>
    {
        var result = tensor_rref(a->asTensorBuffer(), a->m(), a->n());

        if is_null(result) {
            throw new RuntimeException("Failed to reduce matrix.");
        }

        return new self(Matrix::fromTensorBuffer(result, a->m(), a->n()));
    }

    /**
     * @param \Tensor\Matrix a
     * @throws \InvalidArgumentException
     */
    public function __construct(const <Matrix> a)
    {
        let this->a = a;
    }

    /**
     * Return the reduced matrix in reduced row echelon form.
     *
     * @return \Tensor\Matrix
     */
    public function a() -> <Matrix>
    {
        return this->a;
    }
}
