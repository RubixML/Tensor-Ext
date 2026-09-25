namespace Tensor;

use Tensor\Exceptions\InvalidArgumentException;

/**
 * Settings
 *
 * @internal
 *
 * @category    Scientific Computing
 * @package     Rubix/Tensor
 * @author      Andrew DalPino
 */
class Settings
{
    /**
     * Set the number of CPU threads to use when multiprocessing.
     *
     * @param int threads
     * @return void
     */
    public static function setNumThreads(const int threads) -> void
    {
        if unlikely threads < 1 {
            throw new InvalidArgumentException("The number of threads"
                . " must be greater than 0, " . strval(threads) . " given.");
        }

        var status = tensor_set_num_threads(threads);
    }

    /**
     * Return the number of CPU threads used for multiprocessing.
     *
     * @return int
     */
    public static function numThreads() -> int
    {
        return tensor_get_num_threads();
    }

    /**
     * Return the CPU features the extension detected at startup, along with
     * the route the elementwise kernels actually took.
     *
     * The feature flags describe the CPU. Only "avx" is acted on: the
     * elementwise add, subtract, multiply and divide kernels (and their
     * scalar, column and row forms) are compiled a second time with a 256-bit
     * target attribute and are reached through a function pointer that is
     * switched over once during module initialization. On a CPU without AVX
     * the pointer keeps pointing at the baseline, narrower kernels and
     * "dispatch" reads "scalar".
     *
     * "avx2" is reported for diagnostics only. No kernel is compiled for AVX2,
     * so that the dispatched routes stay bit-for-bit identical to the baseline
     * ones.
     *
     * @return array
     */
    public static function cpuFeatures() -> array
    {
        return tensor_get_cpu_features();
    }
}
