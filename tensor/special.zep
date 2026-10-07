namespace Tensor;

interface Special
{
     /**
     * Return the element-wise logistic function i.e. 1 / (1 + exp(-x)).
     *
     * @return mixed
     */
    public function sigmoid();

    /**
     * Return the element-wise softplus i.e. log(1 + exp(x)).
     *
     * @return mixed
     */
    public function softplus();

     /**
      * Return the softmax of the tensor i.e. each row normalized to sum to 1.
      *
      * @return mixed
      */
     public function softmax();

     /**
      * Return the element-wise Gaussian error function.
      *
      * @return mixed
      */
     public function erf();
}
