<?php

namespace Zephir\Optimizers\FunctionCall;

use Zephir\Call;
use Zephir\CompilationContext;
use Zephir\CompiledExpression;
use Zephir\HeadersManager;
use Zephir\Exception\CompilerException;
use Zephir\Optimizers\OptimizerAbstract;

class TensorBufferAppendColumnsOptimizer extends OptimizerAbstract
{
    /**
     * @param mixed[] $expression
     * @param Call $call
     * @param CompilationContext $context
     * @throws CompilerException
     * @return CompiledExpression|bool
     */
    public function optimize(array $expression, Call $call, CompilationContext $context)
    {
        if (!isset($expression['parameters'])) {
            return false;
        }

        if (count($expression['parameters']) !== 4) {
            throw new CompilerException(
                'Buffer AppendColumns accepts exactly 4 arguments, ' . count($expression['parameters']) . ' given.',
                $expression
            );
        }

        $call->processExpectedReturn($context);

        $symbolVariable = $call->getSymbolVariable();

        if (empty($symbolVariable)) {
            throw new CompilerException('Missing symbol variable.');
        }

        if ($symbolVariable->getType() !== 'variable') {
            throw new CompilerException(
                'Return value must only be assigned to a dynamic variable.',
                $expression
            );
        }

        if ($call->mustInitSymbolVariable()) {
            $symbolVariable->initVariant($context);
        }

        $context->headersManager->add(
            'include/buffer_matrix',
            HeadersManager::POSITION_LAST
        );

        $resolvedParams = $call->getResolvedParams(
            $expression['parameters'],
            $context,
            $expression
        );

        $symbol = $context->backend->getVariableCode($symbolVariable);

        $context->codePrinter->output(
            "tensor_buffer_append_columns($symbol, {$resolvedParams[0]}, {$resolvedParams[1]}, {$resolvedParams[2]}, {$resolvedParams[3]});"
        );

        return new CompiledExpression(
            'variable',
            $symbolVariable->getRealName(),
            $expression
        );
    }
}
