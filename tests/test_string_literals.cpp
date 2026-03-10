#include <type_traits>

#include "temppy/temppy.hpp"

static constexpr auto fixed = "ab=12"_fs;

int main() {
  using raw_string_type = decltype("ab=12"_s);
  using expected = temppy::String<'a', 'b', '=', '1', '2'>;
  using converted = temppy::ToString_t<fixed>;

  static_assert(std::is_same_v<raw_string_type, expected>);
  static_assert(std::is_same_v<converted, expected>);
  return 0;
}
