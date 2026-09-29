namespace Tensor;

interface Tensor extends ArrayLike, Arithmetic, Comparable, Unary, Trigonometric, Reductions, Special
{   
    const EPSILON = 0.00000001;
}
