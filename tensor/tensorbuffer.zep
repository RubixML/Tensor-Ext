namespace Tensor;

use Tensor\Exceptions\InvalidArgumentException;

/**
 * TensorBuffer
 *
 * A decorator that wraps the kernel Buffer class and provides additional
 * operations such as sorting and slicing.
 *
 * @category    Scientific Computing
 * @package     Rubix/Tensor
 * @author      Andrew DalPino
 */
class TensorBuffer
{
    /**
     * The underlying buffer being decorated.
     *
     * @var \Tensor\Buffer
     */
    protected buffer;

    /**
     * @param \Tensor\Buffer buffer
     */
    public function __construct(<Buffer> buffer)
    {
        let this->buffer = buffer;
    }

    /**
     * Return the underlying buffer.
     *
     * @return \Tensor\Buffer
     */
    public function asBuffer() -> <Buffer>
    {
        return this->buffer;
    }

    /**
     * Return the number of elements in the buffer.
     *
     * @return int
     */
    public function count() -> int
    {
        return this->buffer->count();
    }

    /**
     * Return the element type of the buffer.
     *
     * @return int
     */
    public function type() -> int
    {
        return this->buffer->type();
    }

    /**
     * Return the buffer as a PHP array.
     *
     * @return list<float>
     */
    public function toArray() -> array
    {
        return this->buffer->toArray();
    }

    /**
     * Return the element at the given index.
     *
     * @param int index
     * @return mixed
     */
    public function get(const int index)
    {
        return this->buffer[index];
    }

    /**
     * Set the element at the given index.
     *
     * @param int index
     * @param mixed value
     * @return void
     */
    public function set(const int index, const var value) -> void
    {
        let this->buffer[index] = value;
    }

    /**
     * Sort the buffer in place.
     *
     * @param bool ascending
     * @return void
     */
    public function sort(const bool ascending = true) -> void
    {
        var status = tensor_buffer_sort(this->buffer, ascending);
    }

    /**
     * Return a slice of the buffer as a new decorator.
     *
     * @param int offset
     * @param int length
     * @throws \Tensor\Exceptions\InvalidArgumentException
     * @return self
     */
    public function slice(const int offset, const int length) -> <TensorBuffer>
    {
        if unlikely offset < 0 || length < 0 || offset > this->buffer->count() - length {
            throw new InvalidArgumentException("Offset and length"
                . " must be within the bounds of the buffer.");
        }

        var b = tensor_buffer_slice(this->buffer, offset, length);

        return new TensorBuffer(<Buffer> b);
    }
}