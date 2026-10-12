namespace Tensor;

use Tensor\Exceptions\InvalidArgumentException;

/**
 * TensorBuffer
 *
 * A decorator that wraps the kernel Buffer class and provides structural
 * operations such as sorting, slicing, splitting, concatenating, and
 * repeating.
 *
 * @internal
 *
 * @category    Scientific Computing
 * @package     Rubix/Tensor
 * @author      Andrew DalPino
 */
class TensorBuffer
{
    const TYPE_DOUBLE = 1;

    const TYPE_LONG = 2;

    /**
     * The underlying buffer being decorated.
     *
     * @var \Tensor\Buffer
     */
    protected buffer;

    /**
     * The element offset of a view into the underlying buffer.
     *
     * @var int
     */
    protected offset;

    /**
     * The logical shape of a view, or null for a plain dense buffer.
     *
     * @var int[]|null
     */
    protected shape;

    /**
     * The stride of each shape dimension, or null for a plain dense buffer.
     *
     * @var int[]|null
     */
    protected strides;

    /**
     * A dense copy of the view gathered for kernels that require contiguous
     * storage, cached for the lifetime of the view.
     *
     * @var \Tensor\Buffer|null
     */
    protected denseData;

    /**
     * The in-place write epoch the cached dense copy was gathered against.
     *
     * @var int|null
     */
    protected denseEpoch;

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
            return tensor_buffer_from_array([]);
        }

        if unlikely rows == 1 {
            return buffers[0];
        }

        return buffers[0]->concat(array_slice(buffers, 1));
    }

    /**
     * @param \Tensor\Buffer buffer
     */
    public function __construct(<Buffer> buffer)
    {
        let this->buffer = buffer;
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
     * Return the element at the given index.
     *
     * @param int index
     * @return mixed
     */
    public function get(const int index)
    {
        if this->shape === null {
            return this->buffer[index];
        }

        var value = tensor_tensorbuffer_get(this->buffer, this->offset, this->shape, this->strides, index);

        return value;
    }

    /**
     * Set the element at the given index. Views are read-only and reject the
     * write.
     *
     * @param int index
     * @param mixed value
     * @return void
     */
    public function set(const int index, const var value) -> void
    {
        var status = tensor_tensorbuffer_set(this->buffer, this->offset, this->shape, this->strides, index, value);
    }

    /**
     * Sort the buffer in place. Views are read-only and reject the sort.
     *
     * @param bool ascending
     * @return void
     */
    public function sort(const bool ascending = true) -> void
    {
        if unlikely this->shape !== null {
            throw new InvalidArgumentException("Cannot sort a read-only view.");
        }

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
        if unlikely offset < 0 || length < 0 || offset > this->count() - length {
            throw new InvalidArgumentException("Offset and length"
                . " must be within the bounds of the buffer.");
        }

        if this->type() !== TensorBuffer::TYPE_DOUBLE {
            var b = tensor_buffer_slice(this->buffer, offset, length);

            return new TensorBuffer(<Buffer> b);
        }

        var view = tensor_buffer_slice_view(this->buffer, this->offset, this->shape, this->strides, offset, length);

        return view;
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
        int limit = 0;
        int a = 0;
        int acc = 0;
        int product = 0;
        bool invalid = offset < 0 || length < 0 || stride < 1;

        if likely !invalid && length > 0 {
            let limit = this->count() - 1 - offset;

            let invalid = limit < 0 || (length > 1 && stride > limit);

            if likely !invalid {
                let a = length - 1;
                
                let acc = stride;

                while a > 0 {
                    if (a & 1) {
                        let product += acc;

                        if product > limit {
                            let invalid = true;

                            break;
                        }
                    }

                    let a = a >> 1;

                    if a > 0 {
                        if acc > (limit >> 1) {
                            let invalid = true;

                            break;
                        }

                        let acc = acc << 1;
                    }
                }
            }
        }

        if unlikely invalid {
            throw new InvalidArgumentException("Offset, length, and"
                . " stride must be within the bounds of the buffer.");
        }

        if this->type() !== TensorBuffer::TYPE_DOUBLE {
            var b = tensor_buffer_slice_strided(this->buffer, offset, length, stride);

            return new TensorBuffer(<Buffer> b);
        }

        var view = tensor_buffer_slice_strided_view(this->buffer, this->offset, this->shape, this->strides, offset, length, stride);

        return view;
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

        var self = this->asBuffer();

        var b = tensor_buffer_concat(self, unwrapped);

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

        if this->type() !== TensorBuffer::TYPE_DOUBLE {
            var buffers = tensor_buffer_split(this->buffer, chunkLength);

            array tensorBuffers = [];

            for buffer in buffers {
                let tensorBuffers[] = new TensorBuffer(<Buffer> buffer);
            }

            return tensorBuffers;
        }

        var views = tensor_buffer_split_view(this->buffer, this->offset, this->shape, this->strides, chunkLength);

        return views;
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

        var self = this->asBuffer();

        var b = tensor_buffer_repeat(self, times);

        return new TensorBuffer(<Buffer> b);
    }

     /**
     * Return the underlying buffer.
     *
     * @return \Tensor\Buffer
     */
    public function asBuffer() -> <Buffer>
    {
        if this->shape === null {
            return this->buffer;
        }

        var b = tensor_buffer_view_materialize(this->buffer, this->offset, this->shape, this->strides);

        return b;
    }

    /**
     * Return the number of elements in the buffer.
     *
     * @return int
     */
    public function count() -> int
    {
        if this->shape === null {
            return this->buffer->count();
        }

        return tensor_tensorbuffer_count(this->buffer, this->shape);
    }

    /**
     * Return the buffer as a PHP array.
     *
     * @return list<float>
     */
    public function toArray() -> array
    {
        if this->shape === null {
            return this->buffer->toArray();
        }

        return tensor_tensorbuffer_to_array(this->buffer, this->offset, this->shape, this->strides);
    }
}