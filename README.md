# temppy

A header-only, template metaprogramming "Python-esque" interpreter that runs entirely at compile time.

## Highlights

- C++23 compile-time lexing, parsing, and evaluation
- Arithmetic with precedence and parentheses
- Variables and multi-line assignments
- Unary `+` / unary `-`
- `#` comments
- String-literal program syntax via `"_s"`

## Build (CMake)

Prereqs:

- A C++23 compiler (GCC 13+ or Clang 16+ recommended)
- CMake 3.20+

```bash
cmake -S . -B build/cmake
cmake --build build/cmake
ctest --test-dir build/cmake --output-on-failure
```

## Build (Makefile)

```bash
make run-examples
make run-tests
```

## Quick Start

```cpp
#include "temppy/temppy.hpp"
#include <iostream>

int main() {
    using program = decltype(
        "x = -5\n"
        "# comments are supported\n"
        "y = x * -2\n"
        "y"_s);

    using result = temppy::Run<program>;

    static_assert(result::value::value == 10);
    std::cout << result::value::value << "\n";
}
```

Looking up variables from the final environment:

```cpp
using x_value = temppy::Lookup_t<typename result::env, temppy::String<'x'>>;
static_assert(x_value::value == -5);
```

## Project Layout

- `include/temppy/`: header-only library
- `examples/`: runnable examples (including `language_features.cpp`)
- `tests/`: compile-time/static-assert based smoke tests
