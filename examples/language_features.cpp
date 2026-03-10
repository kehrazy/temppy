#include "temppy/temppy.hpp"

#include <iostream>

int main() {
  using program = decltype("x = -5 # unary minus\n"
                           "y = +x * -2\n"
                           "# comment-only line\n"
                           "z = -(x + y) + 8\n"
                           "z"_s);

  using result = temppy::Run<program>;
  using x = temppy::Lookup_t<typename result::env, temppy::String<'x'>>;
  using y = temppy::Lookup_t<typename result::env, temppy::String<'y'>>;
  using z = temppy::Lookup_t<typename result::env, temppy::String<'z'>>;

  static_assert(x::value == -5, "x should equal -5");
  static_assert(y::value == 10, "y should equal 10");
  static_assert(z::value == 3, "z should equal 3");
  static_assert(result::value::value == 3,
                "final expression should evaluate to z");

  std::cout << "x = " << x::value << "\n";
  std::cout << "y = " << y::value << "\n";
  std::cout << "z = " << z::value << "\n";
  std::cout << "result = " << result::value::value << "\n";
  return 0;
}
