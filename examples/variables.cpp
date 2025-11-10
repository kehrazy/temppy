#include "../include/temppy/temppy.hpp"
#include <iostream>

template <typename Env, typename VarName> constexpr int lookup_var() {
  return temppy::Lookup_t<Env, VarName>::value;
}

int main() {
  {
    using program = temppy::String<'x', '=', '4', '2'>;
    using result = temppy::Run<program>;
    using x_value = temppy::Lookup_t<typename result::env, temppy::String<'x'>>;
    static_assert(x_value::value == 42, "x should equal 42");
    std::cout << "x = 42\n";
    std::cout << "x -> " << x_value::value << "\n\n";
  }

  {
    using program =
        temppy::String<'x', '=', '5', '\n', 'y', '=', 'x', '*', '2'>;
    using result = temppy::Run<program>;
    using x_value = temppy::Lookup_t<typename result::env, temppy::String<'x'>>;
    using y_value = temppy::Lookup_t<typename result::env, temppy::String<'y'>>;
    static_assert(x_value::value == 5, "x should equal 5");
    static_assert(y_value::value == 10, "y should equal 10");
    std::cout << "x = 5\n";
    std::cout << "y = x * 2\n";
    std::cout << "x -> " << x_value::value << "\n";
    std::cout << "y -> " << y_value::value << "\n\n";
  }

  {
    using program =
        temppy::String<'a', '=', '1', '0', '\n', 'b', '=', '2', '0', '\n', 'c',
                       '=', 'a', '+', 'b', '*', '2', '\n', 'd', '=', '(', 'a',
                       '+', 'b', ')', '*', 'c'>;
    using result = temppy::Run<program>;
    using a_value = temppy::Lookup_t<typename result::env, temppy::String<'a'>>;
    using b_value = temppy::Lookup_t<typename result::env, temppy::String<'b'>>;
    using c_value = temppy::Lookup_t<typename result::env, temppy::String<'c'>>;
    using d_value = temppy::Lookup_t<typename result::env, temppy::String<'d'>>;

    static_assert(a_value::value == 10, "a should equal 10");
    static_assert(b_value::value == 20, "b should equal 20");
    static_assert(c_value::value == 50, "c should equal 50 (10 + 20 * 2)");
    static_assert(d_value::value == 1500,
                  "d should equal 1500 ((10 + 20) * 50)");

    std::cout << "a = 10\n";
    std::cout << "b = 20\n";
    std::cout << "c = a + b * 2\n";
    std::cout << "d = (a + b) * c\n\n";
    std::cout << "a -> " << a_value::value << "\n";
    std::cout << "b -> " << b_value::value << "\n";
    std::cout << "c -> " << c_value::value << " (10 + 20 * 2)\n";
    std::cout << "d -> " << d_value::value << " ((10 + 20) * 50)\n\n";
  }

  {
    using program = temppy::String<'x', '=', '1', '\n', 'x', '=', 'x', '+', '1',
                                   '\n', 'x', '=', 'x', '*', '2'>;
    using result = temppy::Run<program>;
    using x_value = temppy::Lookup_t<typename result::env, temppy::String<'x'>>;
    static_assert(x_value::value == 4, "x should equal 4 ((1 + 1) * 2)");

    std::cout << "x = 1\n";
    std::cout << "x = x + 1  # x is now 2\n";
    std::cout << "x = x * 2  # x is now 4\n";
    std::cout << "x -> " << x_value::value << "\n\n";
  }

  std::cout << "All compile-time assertions passed!\n";
  std::cout << "All variable computations were performed at compile time.\n";

  return 0;
}
