<?php

namespace Tensor\Benchmarks\Special;

use Tensor\Matrix;
use Generator;

/**
 * Softmax as a fused kernel, against the maximum/subtract/exp/sum/divide
 * composition it replaces. The composition is a fair stand-in for what the
 * operation used to cost: it is the sequence the downstream call sites spelled
 * out, and the kernel is a drop-in for it.
 *
 * The sizes are the shapes a classifier actually produces -- a handful of
 * classes over many samples, and the reverse -- plus a square. The square case
 * is included deliberately because it is the one where the kernel's block width
 * has the least room to work and the win is smallest. tall-narrow is the
 * opposite corner: rows only a few elements wide leave so little work per row
 * that the per-row overhead is the whole cost, so it is the shape where the
 * kernel has the least to win and is the honest worst case.
 *
 * @Groups({"Functions"})
 * @BeforeMethods({"setUp"})
 */
class MatrixSoftmaxBench
{
    /**
     * @var Matrix
     */
    protected $a;

    /**
     * @param array $params
     */
    public function setUp(array $params) : void
    {
        $this->a = Matrix::uniform(...$params['size'])->multiply(20.0);
    }

    /**
     * @return list<array{size: list<int>}>
     */
    public function sizes() : Generator
    {
        yield 'classes' => ['size' => [10, 10000]];
        yield 'classes-wide' => ['size' => [64, 65536]];
        yield 'flat' => ['size' => [4, 262144]];
        yield 'square' => ['size' => [1024, 1024]];
        yield 'tall' => ['size' => [4096, 4096]];
        yield 'tall-narrow' => ['size' => [262144, 4]];
    }

    /**
     * @Subject
     * @Iterations(5)
     * @ParamProviders({"sizes"})
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function benchFused() : void
    {
        $this->a->softmax();
    }

    /**
     * @Subject
     * @Iterations(5)
     * @ParamProviders({"sizes"})
     * @OutputTimeUnit("milliseconds", precision=3)
     */
    public function benchComposed() : void
    {
        $z = $this->a->subtractColumnVector($this->a->max())->exp();

        $z->divide($z->sum());
    }
}
