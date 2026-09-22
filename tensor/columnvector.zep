namespace Tensor;

use Tensor\Vector;
use Tensor\Exceptions\DimensionalityMismatch;

/**
 * Column Vector
 *
 * A one dimensional (rank 1) tensor with integer and/or floating point elements.
 *
 * @category    Scientific Computing
 * @package     Rubix/Tensor
 * @author      Andrew DalPino
 */
class ColumnVector extends Vector
{
    /**
     * Factory method to build a new vector from an array.
     *
     * @param (int|float)[] a
     * @return self
     */
    public static function build(const array a = [])
    {
        return new self(a, true);
    }
 
    /**
     * Build a vector foregoing any validation for quicker instantiation.
     *
     * @param (int|float)[] a
     * @return self
     */
    public static function quick(const array a = [])
    {
        return new self(a, false);
    }

    /**
     * Return the number of rows in the vector.
     *
     * @return int
     */
    public function m() -> int
    {
        return this->n;
    }

    /**
     * Return the number of columns in the vector.
     *
     * @return int
     */
    public function n() -> int
    {
        return 1;
    }

    /**
     * Transpose the vector i.e. rotate it.
     *
     * @return mixed
     */
    public function transpose()
    {
        return Vector::quick(this->asArray());
    }

    /**
     * Compute the vector-matrix product of this vector and matrix a.
     *
     * @param \Tensor\Matrix b
     * @return \Tensor\Matrix
     */
    public function matmul(const <Matrix> b) -> <Matrix>
    {
        return this->asColumnMatrix()->matmul(b);
    }

    /**
     * Multiply this column vector with a matrix.
     *
     * @param \Tensor\Matrix b
     * @throws \Tensor\Exceptions\DimensionalityMismatch
     * @return \Tensor\Matrix
     */
    public function multiplyMatrix(const <Matrix> b) -> <Matrix>
    {
        if unlikely this->n !== b->m() {
            throw new DimensionalityMismatch("Vector A expects "
                . (string) this->n . " rows but Matrix B has "
                . (string) b->m() . ".");
        }

        return Matrix::quick(tensor_buffer_broadcast_rows(this->a, b->asArray(), "multiply"));
    }

    /**
     * Divide this column vector with a matrix.
     *
     * @param \Tensor\Matrix b
     * @throws \Tensor\Exceptions\DimensionalityMismatch
     * @return \Tensor\Matrix
     */
    public function divideMatrix(const <Matrix> b) -> <Matrix>
    {
        if unlikely this->n !== b->m() {
            throw new DimensionalityMismatch("Vector A expects "
                . (string) this->n . " rows but Matrix B has "
                . (string) b->m() . ".");
        }

        return Matrix::quick(tensor_buffer_broadcast_rows(this->a, b->asArray(), "divide"));
    }

    /**
     * Add this column vector to a matrix.
     *
     * @param \Tensor\Matrix b
     * @throws \Tensor\Exceptions\DimensionalityMismatch
     * @return \Tensor\Matrix
     */
    public function addMatrix(const <Matrix> b) -> <Matrix>
    {
        if unlikely this->n !== b->m() {
            throw new DimensionalityMismatch("Vector A expects "
                . (string) this->n . " rows but Matrix B has "
                . (string) b->m() . ".");
        }

        return Matrix::quick(tensor_buffer_broadcast_rows(this->a, b->asArray(), "add"));
    }

    /**
     * Subtract a matrix from this column vector.
     *
     * @param \Tensor\Matrix b
     * @throws \Tensor\Exceptions\DimensionalityMismatch
     * @return \Tensor\Matrix
     */
    public function subtractMatrix(const <Matrix> b) -> <Matrix>
    {
        if unlikely this->n !== b->m() {
            throw new DimensionalityMismatch("Vector A expects "
                . (string) this->n . " rows but Matrix B has "
                . (string) b->m() . ".");
        }

        return Matrix::quick(tensor_buffer_broadcast_rows(this->a, b->asArray(), "subtract"));
    }

    /**
     * Raise this column vector to the power of a matrix.
     *
     * @param \Tensor\Matrix b
     * @throws \Tensor\Exceptions\DimensionalityMismatch
     * @return \Tensor\Matrix
     */
    public function powMatrix(const <Matrix> b) -> <Matrix>
    {
        if unlikely this->n !== b->m() {
            throw new DimensionalityMismatch("Vector A expects "
                . (string) this->n . " rows but Matrix B has "
                . (string) b->m() . ".");
        }

        return Matrix::quick(tensor_buffer_broadcast_rows(this->a, b->asArray(), "pow"));
    }
 
    /**
     * Mod this column vector with a matrix.
     *
     * @param \Tensor\Matrix b
     * @throws \Tensor\Exceptions\DimensionalityMismatch
     * @return \Tensor\Matrix
     */
    public function modMatrix(const <Matrix> b) -> <Matrix>
    {
        if unlikely this->n !== b->m() {
            throw new DimensionalityMismatch("Vector A expects "
                . (string) this->n . " rows but Matrix B has "
                . (string) b->m() . ".");
        }

        return Matrix::quick(tensor_buffer_broadcast_rows(this->a, b->asArray(), "mod"));
    }

    /**
     * Return the element-wise equality comparison of this column vector and a matrix.
     *
     * @param \Tensor\Matrix b
     * @throws \Tensor\Exceptions\DimensionalityMismatch
     * @return \Tensor\Matrix
     */
    public function equalMatrix(const <Matrix> b) -> <Matrix>
    {
        if unlikely this->n !== b->m() {
            throw new DimensionalityMismatch("Vector A expects "
                . (string) this->n . " rows but Matrix B has "
                . (string) b->m() . ".");
        }

        return Matrix::quick(tensor_buffer_broadcast_rows(this->a, b->asArray(), "equal"));
    }

    /**
     * Return the element-wise not equal comparison of this column vector and a matrix.
     *
     * @param \Tensor\Matrix b
     * @throws \Tensor\Exceptions\DimensionalityMismatch
     * @return \Tensor\Matrix
     */
    public function notEqualMatrix(const <Matrix> b) -> <Matrix>
    {
        if unlikely this->n !== b->m() {
            throw new DimensionalityMismatch("Vector A expects "
                . (string) this->n . " rows but Matrix B has "
                . (string) b->m() . ".");
        }

        return Matrix::quick(tensor_buffer_broadcast_rows(this->a, b->asArray(), "notEqual"));
    }

    /**
     * Return the element-wise greater than comparison of this column vector and a matrix.
     *
     * @param \Tensor\Matrix b
     * @throws \Tensor\Exceptions\DimensionalityMismatch
     * @return \Tensor\Matrix
     */
    public function greaterMatrix(const <Matrix> b) -> <Matrix>
    {
        if unlikely this->n !== b->m() {
            throw new DimensionalityMismatch("Vector A expects "
                . (string) this->n . " rows but Matrix B has "
                . (string) b->m() . ".");
        }

        return Matrix::quick(tensor_buffer_broadcast_rows(this->a, b->asArray(), "greater"));
    }

    /**
     * Return the element-wise greater than or equal to comparison of this column vector and a matrix.
     *
     * @param \Tensor\Matrix b
     * @throws \Tensor\Exceptions\DimensionalityMismatch
     * @return \Tensor\Matrix
     */
    public function greaterEqualMatrix(const <Matrix> b) -> <Matrix>
    {
        if unlikely this->n !== b->m() {
            throw new DimensionalityMismatch("Vector A expects "
                . (string) this->n . " rows but Matrix B has "
                . (string) b->m() . ".");
        }

        return Matrix::quick(tensor_buffer_broadcast_rows(this->a, b->asArray(), "greaterEqual"));
    }

    /**
     * Return the element-wise less than comparison of this column vector and a matrix.
     *
     * @param \Tensor\Matrix b
     * @throws \Tensor\Exceptions\DimensionalityMismatch
     * @return \Tensor\Matrix
     */
    public function lessMatrix(const <Matrix> b) -> <Matrix>
    {
        if unlikely this->n !== b->m() {
            throw new DimensionalityMismatch("Vector A expects "
                . (string) this->n . " rows but Matrix B has "
                . (string) b->m() . ".");
        }

        return Matrix::quick(tensor_buffer_broadcast_rows(this->a, b->asArray(), "less"));
    }

    /**
     * Return the element-wise less than or equal to comparison of this column vector and a matrix.
     *
     * @param \Tensor\Matrix b
     * @throws \Tensor\Exceptions\DimensionalityMismatch
     * @return \Tensor\Matrix
     */
    public function lessEqualMatrix(const <Matrix> b) -> <Matrix>
    {
        if unlikely this->n !== b->m() {
            throw new DimensionalityMismatch("Vector A expects "
                . (string) this->n . " rows but Matrix B has "
                . (string) b->m() . ".");
        }

        return Matrix::quick(tensor_buffer_broadcast_rows(this->a, b->asArray(), "lessEqual"));
    }
}