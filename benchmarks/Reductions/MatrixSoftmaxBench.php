<?php

namespace Tensor\Benchmarks\Reductions;

use Tensor\Matrix;
use Generator;

/**
 * Softmax as a fused kernel, against the transpose/maximum/subtract/exp/sum/
 * clip/divide/transpose composition it replaces. The composition is a fair
 * stand-in for what the operation used to cost: it is the sequence the
 * downstream call sites spelled out, and the kernel is a drop-in for it.
 *
 * The sizes are the shapes a classifier actually produces -- a handful of
 * classes over many samples, and the reverse -- plus a square. The square case
 * is included deliberately because it is the one where the kernel's block width
 * has the least room to work and the win is smallest.
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
        $z = $this->a->transpose();
        $z = $z->subtractColumnVector($z->max())->exp();
        $total = $z->sum()->clipLower(1e-8);

        $z->divide($total)->transpose();
    }
}
