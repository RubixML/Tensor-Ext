namespace Tensor;

interface Reductions
{    
    /**
     * Sum the tensor.
     *
     * @return mixed
     */
    public function sum();

    /**
     * Calculate the product of the tensor.
     *
     * @return mixed
     */
    public function product();

    /**
     * Return the minimum of the tensor.
     *
     * @return mixed
     */
    public function min();

    /**
     * Return the maximum of the tensor.
     *
     * @return mixed
     */
    public function max();

    /**
     * Return the index (or per-row indices) of the minimum of the tensor.
     *
     * @return mixed
     */
    public function argmin();

    /**
     * Return the index (or per-row indices) of the maximum of the tensor.
     *
     * @return mixed
     */
    public function argmax();
}
