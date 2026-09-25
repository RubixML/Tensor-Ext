namespace Tensor;

interface Tensor extends ArrayLike, Arithmetic, Comparable, Unary, Trigonometric, Statistical, Reductions
{   
    const EPSILON = 0.00000001;
}
