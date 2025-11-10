# temppy

A header-only, template metaprogramming “Python‑esque” interpreter that runs entirely at compile time.

## Highlights

- C++23, zero runtime parsing; results computed at compile time
- Tiny language with precedence, parentheses, variables, and assignments

## Quick Start

Prereqs:

- A C++23 compiler (GCC 13+ or Clang 16+ recommended)

```bash
make run-examples
```

```cpp
#include "temppy/temppy.hpp"
#include <iostream>

int main() {
    using program = temppy::String<'2','+','3','*','4'>; // "2+3*4"
    using result  = temppy::Run<program>;

    static_assert(result::value::value == 14);
    std::cout << result::value::value << "\n"; // prints 14
}
```

Looking up variables from the final environment:

```cpp
using x_value = temppy::Lookup_t<typename result::env, temppy::String<'x'>>;
static_assert(x_value::value == 5);
```
