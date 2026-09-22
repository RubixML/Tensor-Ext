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
     * Build a buffer by concatenating an array of buffers into a single
     * contiguous buffer.
     *
     * @param \Tensor\TensorBuffer[] buffers
     * @return self
     */
    public static function fromBuffers(const array buffers) -> <TensorBuffer>
    {
        int rows = count(buffers);

        if unlikely rows < 1 {
            var zero = tensor_buffer_from_array([]);
            return new self(<Buffer> zero);
        }

        if unlikely rows == 1 {
            return new self(buffers[0]->asBuffer());
        }

        var b = buffers[0]->concat(array_slice(buffers, 1));

        return new self(b->asBuffer());
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

    /**
     * Map a function over the elements in the buffer and return a new decorator.
     *
     * @internal
     *
     * @param callable callback
     * @return self
     */
    public function map(const var callback) -> <TensorBuffer>
    {
        var value;

        array b = [];

        var a = this->buffer->toArray();

        for value in a {
            let b[] = {callback}(value);
        }

        var buffer = tensor_buffer_from_array(b);

        return new TensorBuffer(<Buffer> buffer);
    }

    /**
     * Return the sum of the elements in the buffer.
     *
     * @return float
     */
    public function sum() -> float
    {
        return tensor_buffer_sum(this->buffer);
    }

    /**
     * Return the product of the elements in the buffer.
     *
     * @return float
     */
    public function product() -> float
    {
        return tensor_buffer_product(this->buffer);
    }

    /**
     * Return the minimum element in the buffer.
     *
     * @return float
     */
    public function min() -> float
    {
        return tensor_buffer_min(this->buffer);
    }

    /**
     * Return the maximum element in the buffer.
     *
     * @return float
     */
    public function max() -> float
    {
        return tensor_buffer_max(this->buffer);
    }

    /**
     * Return the index of the minimum element in the buffer.
     *
     * @return int
     */
    public function argmin() -> int
    {
        return tensor_buffer_argmin(this->buffer);
    }

    /**
     * Return the index of the maximum element in the buffer.
     *
     * @return int
     */
    public function argmax() -> int
    {
        return tensor_buffer_argmax(this->buffer);
    }

    /**
     * Return a slice of the buffer with a given stride as a new decorator.
     *
     * @param int offset
     * @param int length
     * @param int stride
     * @throws \Tensor\Exceptions\InvalidArgumentException
     * @return self
     */
    public function sliceStrided(const int offset, const int length, const int stride) -> <TensorBuffer>
    {
        if unlikely offset < 0 || length < 0 || stride < 1
            || (length > 0 && offset > this->buffer->count() - (length - 1) * stride - 1) {
            throw new InvalidArgumentException("Offset, length, and"
                . " stride must be within the bounds of the buffer.");
        }

        var b = tensor_buffer_slice_strided(this->buffer, offset, length, stride);

        return new TensorBuffer(<Buffer> b);
    }

    /**
     * Return a new decorator wrapping a new buffer containing the elements of
     * this buffer concatenated with the given buffers.
     *
     * @param \Tensor\TensorBuffer[] buffers
     * @return self
     */
    public function concat(const array buffers) -> <TensorBuffer>
    {
        var buffer;

        array unwrapped = [];

        for buffer in buffers {
            let unwrapped[] = buffer->asBuffer();
        }

        var b = tensor_buffer_concat(this->buffer, unwrapped);

        return new TensorBuffer(<Buffer> b);
    }

    /**
     * Return an array of new decorators each wrapping a chunk of this buffer
     * of the given length.
     *
     * @param int chunkLength
     * @throws \Tensor\Exceptions\InvalidArgumentException
     * @return \Tensor\TensorBuffer[]
     */
    public function split(const int chunkLength) -> array
    {
        if unlikely chunkLength < 1 {
            throw new InvalidArgumentException("Chunk length must be"
                . " greater than 0, " . strval(chunkLength) . " given.");
        }

        var buffer;

        var buffers = tensor_buffer_split(this->buffer, chunkLength);

        array tensorBuffers = [];

        for buffer in buffers {
            let tensorBuffers[] = new TensorBuffer(<Buffer> buffer);
        }

        return tensorBuffers;
    }

    /**
     * Return a new decorator wrapping a new buffer with the elements of this
     * buffer repeated the given number of times.
     *
     * @param int times
     * @throws \Tensor\Exceptions\InvalidArgumentException
     * @return self
     */
    public function repeat(const int times) -> <TensorBuffer>
    {
        if unlikely times < 1 {
            throw new InvalidArgumentException("Times must be"
                . " greater than 0, " . strval(times) . " given.");
        }

        var b = tensor_buffer_repeat(this->buffer, times);

        return new TensorBuffer(<Buffer> b);
    }
}