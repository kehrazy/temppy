#include "temppy/temppy.hpp"

int main() {
  using program = decltype("x = -5 # unary minus on literal\n"
                           "y = +x * -2\n"
                           "# comment-only line\n"
                           "z = -(x + y) + 8\n"
                           "z"_s);

  using result = temppy::Run<program>;
  using x = temppy::Lookup_t<typename result::env, temppy::String<'x'>>;
  using y = temppy::Lookup_t<typename result::env, temppy::String<'y'>>;
  using z = temppy::Lookup_t<typename result::env, temppy::String<'z'>>;

  static_assert(x::value == -5);
  static_assert(y::value == 10);
  static_assert(z::value == 3);
  static_assert(result::value::value == 3);
  return 0;
}
