#include "../include/temppy/temppy.hpp"

static constexpr temppy::fixed_string<21> program_str{"(2 + 3 * 10) - 4 * 2"};

int main() {
  using program = temppy::ToString_t<program_str>;
  using result = temppy::Run<program>;
  static_assert(result::value::value == 24,
                "(2 + 3 * 10) - 4 * 2 should equal 24");
  return result::value::value;
}
