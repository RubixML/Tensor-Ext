namespace Tensor;

use Tensor\Exceptions\InvalidArgumentException;

/**
 * Settings
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
     * Return the CPU features the extension detected at startup, along with the
     * route the elementwise kernels actually took.
     *
     * The returned array has four keys: "avx", "avx2" and "avx512" are the
     * feature flags detected on the CPU, and "dispatch" is the route the
     * elementwise kernels were installed on: "avx512" when that ISA is usable,
     * otherwise "avx", and "scalar" when neither is.
     *
     * @return array
     */
    public static function cpuFeatures() -> array
    {
        return tensor_get_cpu_features();
    }
}
