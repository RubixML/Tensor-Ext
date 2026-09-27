# Installation

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

## Installation

Make sure you have all the necessary build tools installed such as a C compiler and make tools. For example, on an Ubuntu linux system you can enter the following on the command line to install the necessary dependencies.

```sh
$ sudo apt-get install make gcc gfortran php-dev libopenblas-dev liblapacke-dev re2c build-essential
```

Install the Tensor extension via [PIE](https://github.com/php/pie):

```sh
$ pie install rubix/tensor_ext
```

### Tips for MacOS

Install the build dependencies with [Homebrew](https://brew.sh) (alongside a PHP build, e.g. `brew install php`):

```sh
$ brew install gcc openblas lapack re2c make autoconf automake
```

Several of these are keg-only and not on the default build paths, so export the following before compiling:

```sh
export LDFLAGS="-L$(brew --prefix openblas)/lib -L$(brew --prefix lapack)/lib -L$(brew --prefix pcre2)/lib -L$(brew --prefix gcc)/lib/gcc/current"
export CPPFLAGS="-I$(brew --prefix openblas)/include -I$(brew --prefix lapack)/include -I$(brew --prefix pcre2)/include -I$(brew --prefix gcc)/include"
export PKG_CONFIG_PATH="$(brew --prefix openblas)/lib/pkgconfig:$(brew --prefix lapack)/lib/pkgconfig:$(brew --prefix pcre2)/lib/pkgconfig:$(brew --prefix gcc)/lib/pkgconfig"
export PATH="$(brew --prefix gcc)/bin:$PATH"
export FC=$(brew --prefix gcc)/bin/gfortran
```

## Manually Compiling the Extension

Clone the repository locally using [Git](https://git-scm.com/):

```sh
$ git clone https://github.com/RubixML/Tensor-Ext
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
