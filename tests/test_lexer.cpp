#include <type_traits>

#include "temppy/temppy.hpp"

int main() {
  using program = decltype("x=1 # ignored\ny=2"_s);
  using tokens = temppy::Tokenize<program>;

  static_assert(tokens::size == 8);

  using t0 = temppy::At_t<tokens, 0>;
  using t1 = temppy::At_t<tokens, 1>;
  using t2 = temppy::At_t<tokens, 2>;
  using t3 = temppy::At_t<tokens, 3>;
  using t4 = temppy::At_t<tokens, 4>;
  using t5 = temppy::At_t<tokens, 5>;
  using t6 = temppy::At_t<tokens, 6>;
  using t7 = temppy::At_t<tokens, 7>;

  static_assert(t0::type == temppy::TokenType::Identifier);
  static_assert(std::is_same_v<typename t0::data, temppy::String<'x'>>);
  static_assert(t1::type == temppy::TokenType::Assign);
  static_assert(t2::type == temppy::TokenType::Number && t2::int_data == 1);
  static_assert(t3::type == temppy::TokenType::Newline);
  static_assert(t4::type == temppy::TokenType::Identifier);
  static_assert(std::is_same_v<typename t4::data, temppy::String<'y'>>);
  static_assert(t5::type == temppy::TokenType::Assign);
  static_assert(t6::type == temppy::TokenType::Number && t6::int_data == 2);
  static_assert(t7::type == temppy::TokenType::Eof);

  return 0;
}
