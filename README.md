# nu_shell_calc

## About

A hacked-together C++ plugin for nushell that calculates math expressions, based on the Python example: https://github.com/nushell/contributor-book/blob/master/en/plugins.md#creating-a-plugin-in-python


Expressions are in the form of:

```
[id = ] <lhs> <op> <rhs>
```

 * ID assignment is optional
 * Valid identifiers are in the form of `[a-z]+`
 * Allowed operations: `-`, `+`, `*`, `/`
 * All arithmetic is done with `double`


Valid input list might be:

```
a = 26.4 + 2.3
b = 19.2 - 1.45
c = a + b
a / b
c + 102
```

## Building

All dependencies are gathered using conan.io

```
git clone git@github.com:lefticus/nu_plugin_calc
mkdir nu_plugin_calc-build
cd nu_plugin_calc-build
cmake -D CMAKE_BUILD_TYPE=Release ../nu_plugin_calc
make
```

## Usage within nushell


Make sure the binary `nu_plugin_calc` is available in your path.

```
cat filewithexpressions | calc
```

## To Do

 * Consider supporting scientific notation
 * Set up tests
 * Set up CI
 * Make logging optional on command line
 * Consider more advanced expressions
 * Stronger typed JSON-RPC API of some sort
 * Actually make the below build status items do things...


[![codecov](https://codecov.io/gh/lefticus/nu_shell_calc/branch/master/graph/badge.svg)](https://codecov.io/gh/lefticus/nu_shell_calc)

[![Build Status](https://travis-ci.org/lefticus/nu_shell_calc.svg?branch=master)](https://travis-ci.org/lefticus/nu_shell_calc)

