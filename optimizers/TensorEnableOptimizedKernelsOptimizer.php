<?php

namespace Zephir\Optimizers\FunctionCall;

use Zephir\Call;
use Zephir\CompilationContext;
use Zephir\CompiledExpression;
use Zephir\HeadersManager;
use Zephir\Exception\CompilerException;
use Zephir\Optimizers\OptimizerAbstract;

class TensorEnableOptimizedKernelsOptimizer extends OptimizerAbstract
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
            'include/settings',
            HeadersManager::POSITION_LAST
        );

        $symbol = $context->backend->getVariableCode($symbolVariable);

        $context->codePrinter->output(
            "tensor_enable_optimized_kernels($symbol);"
        );

        return new CompiledExpression(
            'variable',
            $symbolVariable->getRealName(),
            $expression
        );
    }
}
