# Parity cross-check oracle

`rubix/tensor` is the pure-PHP twin of this extension: same namespace, same class names, a
PSR-4 `Tensor\` mapped onto its own `src/`. With the extension loaded it never gets a look in,
because the classes already exist at MINIT. Without the extension it answers instead. That makes
it a free second opinion on what the extension ought to compute.

It is installed **here** rather than in the root `composer.json` on purpose. PHPStan's composer
source locator outranks its internal-class locator, so a root-level `rubix/tensor` would silently
redirect `composer analyze` from the extension's real signatures to the twin's `src/`, and the
level-8 run would stop checking what it is supposed to check.

## Install

```sh
composer --working-dir=tools/oracle install
```

`vendor/` here is gitignored.

## Use

```sh
php -n -d memory_limit=4G tools/oracle-dump.php --out=tests/fixtures/parity/oracle
```

The dumper exits non-zero if `tensor_ext` is loaded — the two cannot coexist in one process, and
an oracle recorded from the extension would be comparing the extension against itself.

## Status

This is a **cross-check**, not the gate. The twin is a different implementation and is not
guaranteed to be method-complete: any operation it lacks is recorded as `unsupported`, and the
set of those is pinned in `tests/fixtures/parity/oracle/unsupported.json` so it cannot quietly
grow. The hard gate is `tests/fixtures/parity/snapshot/`, which is exact.
