#include "temppy/temppy.hpp"

int main() {
  using program = decltype("(2 + 3 * 10) - 4 * 2"_s);
  using result = temppy::Run<program>;
  static_assert(result::value::value == 24,
                "(2 + 3 * 10) - 4 * 2 should equal 24");
  return result::value::value;
}
