# Getting Started

Tensor is a PHP extension that provides objects for scientific computing, backed by OpenBLAS / LAPACKE for native speed.

## Installation

Install the Tensor extension via [PIE](https://github.com/php/pie):

```sh
$ pie install rubix/tensor_ext
```

## Requirements

- [PHP](https://php.net) 8.1 or above
- The PHP development package (source code and tooling)
- A C compiler such as [GCC](https://gcc.gnu.org/) or [Clang](https://clang.llvm.org/)
- A Fortran compiler such as [GFortran](https://gcc.gnu.org/wiki/GFortran)
- [OpenBLAS](https://www.openblas.net/) development package
- [LAPACKE](https://www.netlib.org/lapack/lapacke.html) C interface to [LAPACK](http://www.netlib.org/lapack/)
- [re2c](https://re2c.org/) 0.13.6 or later
- [GNU make](https://www.gnu.org/software/make/) 3.81 or later
- [autoconf](https://www.gnu.org/software/autoconf/autoconf.html) 2.31 or later
- [automake](https://www.gnu.org/software/automake/) 1.14 or later
- Ubuntu build-essentials

## Manually Compiling the Extension

Clone the repository locally using [Git](https://git-scm.com/):

```sh
$ git clone https://github.com/RubixML/Tensor-Ext
```

Make sure you have all the necessary build tools installed such as a C compiler and make tools. For example, on an Ubuntu linux system you can enter the following on the command line to install the necessary dependencies.

```sh
$ sudo apt-get install make gcc gfortran php-dev libopenblas-dev liblapacke-dev re2c build-essential
```

Then, change into the `ext` directory from the project root and run the following commands from the terminal. See [this guide](https://www.php.net/manual/en/install.pecl.phpize.php) for more information on compiling PHP extensions with PHPize.

```sh
$ cd ./ext
$ phpize
$ ./configure
$ make
$ sudo make install
```

Finally, add the following line to your `php.ini` configuration to install the extension.

```
extension=tensor.so
```

To confirm that the extension is loaded in PHP, you can run the following command.

```sh
php -m | grep tensor
```

### Tip for Compiling on MacOS

To avoid some errors on Mac devices using homebrew, don't forget to add the following environment variables.

```sh
export LDFLAGS="-L$(brew --prefix openblas)/lib -L$(brew --prefix pcre2)/lib -L$(brew --prefix gcc)/lib/gcc/current"
export CPPFLAGS="-I$(brew --prefix openblas)/include -I$(brew --prefix pcre2)/include -I$(brew --prefix gcc)/include"
export PKG_CONFIG_PATH="$(brew --prefix openblas)/lib/pkgconfig:$(brew --prefix pcre2)/lib/pkgconfig:$(brew --prefix gcc)/lib/pkgconfig"
export PATH="$(brew --prefix gcc)/bin:$PATH"
export FC=$(brew --prefix gcc)/bin/gfortran
```

## Your First Script

Make sure the extension is loaded (see [Installation](#installation)). Then create a simple example that constructs a matrix, performs a matrix multiplication, computes some statistics, and reduces it to row echelon form.

```php
<?php

use Tensor\Matrix;
use Tensor\Vector;

// Build a 2 x 3 matrix.
$a = Matrix::fromArray([
    [1.0, 2.0, 3.0],
    [4.0, 5.0, 6.0],
]);

// Build a 3 x 2 matrix.
$b = Matrix::fromArray([
    [7.0, 8.0],
    [9.0, 10.0],
    [11.0, 12.0],
]);

// Matrix-matrix product -> 2 x 2 matrix.
$c = $a->matmul($b);

var_export($c->asArray());
// [[58.0, 64.0], [139.0, 154.0]]

// Element-wise operations work against scalars, vectors, and matrices.
$d = $a->multiplyScalar(2.0)->add(1.0);

var_export($d->asArray());
// [[3.0, 5.0, 7.0], [9.0, 11.0, 13.0]]

// Methods are chainable, immutable, and reuse underlying arrays when possible.
var_export($a->sqrt()->round(2)->asArray());
// [[1.0, 1.41, 1.73], [2.0, 2.24, 2.45]]

// Compute the mean of each row (a ColumnVector).
$means = $a->mean();

var_export($means->asArray());
// [2.0, 5.0]

// Compute the reduced row echelon form (RREF).
$rref = $a->rref()->a();

var_export($rref->asArray());
// [[1, 0, -1], [0.0, 1.0, 2.0]]

// Vectors are 1-dimensional tensors.
$v = Vector::range(1.0, 4.0);

echo $v->dot(Vector::ones(4)) . PHP_EOL;
// 10

var_export($v->l2Norm());
// 5.477225575051661
```

## Next Steps

- Browse the [API reference](index.md) to learn about every class and interface.
