namespace Tensor;

interface Special
{
     /**
      * Return the softmax of the tensor i.e. each column normalized to sum to 1.
      *
      * @return mixed
      */
     public function softmax();
}
