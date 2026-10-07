namespace Tensor;

interface Special
{
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

     /**
      * Return the element-wise scaled complementary error function,
      * i.e. exp(-x^2) * erfc(-x).
      *
      * @return mixed
      */
     public function cerf();
}
