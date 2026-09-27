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
     * Return the CPU features the extension detected at startup.
     *
     * The returned array has four keys: "avx", "avx2", "avx512" and "fma",
     * each a flag reporting whether that instruction set is available on the
     * CPU. "fma" reports FMA3, which is a separate feature from "avx" rather
     * than a consequence of it: a CPU can have 256-bit AVX and no fused
     * multiply-add, and the two decide different kernel routes.
     *
     * @return array
     */
    public static function cpuFeatures() -> array
    {
        return tensor_get_cpu_features();
    }
}
