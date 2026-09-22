namespace Tensor;

use Tensor\Exceptions\DimensionalityMismatch;

/**
 * BufferVector
 *
 * A rank-1 tensor whose elements live in a contiguous C array -- a
 * <Ns>\Buffer, the kernel class added for zephir-lang/zephir#2721 -- instead
 * of a PHP array. A chain of operations therefore never materialises an
 * intermediate PHP array: the elements stay in C from `build()` to
 * `asArray()`.
 *
 * This is a measurement prototype, not a replacement for Vector. It carries
 * only the three operations the issue names as dominated by pack/unpack
 * overhead, so that the two representations can be benchmarked against each
 * other on identical work.
 *
 * Note that no method here names the Buffer class: the compiler has no
 * definition for a kernel class, so the buffer is produced by
 * tensor_buffer_from_array() and thereafter only ever passed along.
 */
class BufferVector
{
    /**
     * The elements of the vector, as a contiguous buffer of doubles.
     *
     * @var \Tensor\Buffer
     */
    protected a;

    /**
     * The number of elements in the vector.
     *
     * @var int
     */
    protected n;

    /**
     * Build a vector from a PHP array. This is the only point at which the
     * elements are converted.
     *
     * @param float[] a
     * @return self
     */
    public static function build(const array a = []) -> <BufferVector>
    {
        return new self(tensor_buffer_from_array(a));
    }

    /**
     * Wrap an existing buffer without copying it.
     *
     * @return self
     */
    public static function quick(var a) -> <BufferVector>
    {
        return new self(a);
    }

    public function __construct(var a)
    {
        let this->a = a;
        let this->n = count(a);
    }

    /**
     * Return the underlying buffer.
     */
    public function buffer()
    {
        return this->a;
    }

    /**
     * Return the number of elements in the vector.
     */
    public function n() -> int
    {
        return this->n;
    }

    /**
     * Materialise the elements as a PHP array. The whole point of the class is
     * that this is called once at the end, not once per operation.
     *
     * @return list<float>
     */
    public function asArray() -> array
    {
        return this->a->toArray();
    }

    /**
     * Elementwise addition.
     *
     * @throws \Tensor\Exceptions\DimensionalityMismatch
     * @return self
     */
    public function add(const <BufferVector> b) -> <BufferVector>
    {
        if unlikely b->n() !== this->n {
            throw new DimensionalityMismatch("Vector A requires "
                . (string) this->n . " elements but Vector B has "
                . (string) b->n() . ".");
        }

        return self::quick(tensor_buffer_add(this->a, b->buffer()));
    }

    /**
     * Elementwise product.
     *
     * @throws \Tensor\Exceptions\DimensionalityMismatch
     * @return self
     */
    public function multiply(const <BufferVector> b) -> <BufferVector>
    {
        if unlikely b->n() !== this->n {
            throw new DimensionalityMismatch("Vector A requires "
                . (string) this->n . " elements but Vector B has "
                . (string) b->n() . ".");
        }

        return self::quick(tensor_buffer_multiply(this->a, b->buffer()));
    }

    /**
     * Inner product.
     *
     * @throws \Tensor\Exceptions\DimensionalityMismatch
     * @return float
     */
    public function dot(const <BufferVector> b) -> float
    {
        if unlikely b->n() !== this->n {
            throw new DimensionalityMismatch("Vector A requires "
                . (string) this->n . " elements but Vector B has "
                . (string) b->n() . ".");
        }

        return tensor_buffer_dot(this->a, b->buffer());
    }
}
