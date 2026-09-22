<?php

namespace Tensor\Tests\Parity;

use Closure;

/**
 * One probe in the parity transcript.
 *
 * A bare `callable` cannot say whether a result is comparable bit-for-bit,
 * whether a float tolerance applies, or whether the pure-PHP twin even
 * implements the method. This record can, and the two dumpers plus the two
 * comparators all read it, so the extension side and the oracle side cannot
 * drift apart in what they measure.
 */
final class Operation
{
    /**
     * @param Closure(Operands): mixed $apply
     * @param list<string> $tags
     * @param string $key
     * @param string $subject
     * @param float $tolerance
     * @param bool $deterministic
     */
    public function __construct(
        public readonly string $key,
        public readonly string $subject,
        public readonly Closure $apply,
        public readonly float $tolerance = 1e-9,
        public readonly bool $deterministic = true,
        public readonly array $tags = []
    ) {
    }

    /**
     * The class this operation probes, used to ask the pure-PHP twin whether it
     * implements the method before treating a difference as a failure.
     */
    public function subjectClass() : string
    {
        return match ($this->subject) {
            'matrix' => 'Tensor\Matrix',
            'columnVector' => 'Tensor\ColumnVector',
            default => 'Tensor\Vector',
        };
    }

    /**
     * `matrix.multiplyColumnVector` -> `multiplyColumnVector`;
     * `vector.map.sqrt` -> `map`.
     */
    public function method() : string
    {
        $parts = explode('.', $this->key);

        return $parts[1] ?? $this->key;
    }

    public function hasTag(string $tag) : bool
    {
        return in_array($tag, $this->tags, true);
    }
}
