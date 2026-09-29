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

    /**
     * Reset the dispatched C kernels to their scalar baseline variants.
     *
     * The elementwise arithmetic, comparison, unary, linear algebra, and
     * convolution kernels each have a baseline (scalar) variant and a wide
     * SIMD variant, selected once at load time by the CPU feature bits; this
     * method walks every route back to its baseline variant for the rest of
     * the process. The OpenBLAS/LAPACKE-backed routines and the per-elem libm
     * transcendental ops are unaffected: they are scalar either way, and the
     * thread pool is controlled by `setNumThreads`.
     *
     * This is one half of a pair: `enableOptimizedKernels` puts the widest
     * route the CPU supports back in place. It is idempotent, and safe to call
     * multiple times.
     *
     * Call this from a single worker before any tensor op runs so the route
     * pointer resets do not race a concurrent kernel call.
     *
     * @return void
     */
    public static function disableOptimizedKernels() -> void
    {
        var status = tensor_disable_optimized_kernels();
    }

    /**
     * Route the dispatched C kernels back through their widest usable variants.
     *
     * The inverse of `disableOptimizedKernels`: the CPU feature bits were
     * sampled once and cached when the extension loaded, so this replays the
     * same selection the module initializer made, restoring the AVX, FMA, or
     * AVX-512 route as appropriate. On a CPU with no usable wide instruction
     * set the routes stay on their baseline variants, which is the correct
     * outcome rather than a failed upgrade.
     *
     * Idempotent, and safe to call multiple times. Call this from a single
     * worker before any tensor op runs, for the same reason as the disable.
     *
     * @return void
     */
    public static function enableOptimizedKernels() -> void
    {
        var status = tensor_enable_optimized_kernels();
    }
}
