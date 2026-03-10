#include "temppy/temppy.hpp"

int main() {
  using program = decltype("total=0\n"
                           "for i in range(5):\n"
                           "  total=total+i\n"
                           "  if i:\n"
                           "    total=total+10\n"
                           "if total:\n"
                           "  total=total+1\n"
                           "total"_s);

  using result = temppy::Run<program>;
  using total = temppy::Lookup_t<typename result::env,
                                 temppy::String<'t', 'o', 't', 'a', 'l'>>;
  using i = temppy::Lookup_t<typename result::env, temppy::String<'i'>>;

  static_assert(total::value == 51);
  static_assert(i::value == 4);
  static_assert(result::value::value == 51);
  return 0;
}
