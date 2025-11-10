#include "../include/temppy/temppy.hpp"
#include <iostream>

int main() {

  {
    std::cout << "Computing Fibonacci-like sequence:\n";
    using program =
        temppy::String<'a', '=', '0', '\n', 'b', '=', '1', '\n', 'c', '=', 'a',
                       '+', 'b', '\n', 'd', '=', 'b', '+', 'c', '\n', 'e', '=',
                       'c', '+', 'd', '\n', 'f', '=', 'd', '+', 'e'>;
    using result = temppy::Run<program>;
    using a = temppy::Lookup_t<typename result::env, temppy::String<'a'>>;
    using b = temppy::Lookup_t<typename result::env, temppy::String<'b'>>;
    using c = temppy::Lookup_t<typename result::env, temppy::String<'c'>>;
    using d = temppy::Lookup_t<typename result::env, temppy::String<'d'>>;
    using e = temppy::Lookup_t<typename result::env, temppy::String<'e'>>;
    using f = temppy::Lookup_t<typename result::env, temppy::String<'f'>>;

    static_assert(a::value == 0);
    static_assert(b::value == 1);
    static_assert(c::value == 1);
    static_assert(d::value == 2);
    static_assert(e::value == 3);
    static_assert(f::value == 5);

    std::cout << "a = 0 -> " << a::value << "\n";
    std::cout << "b = 1 -> " << b::value << "\n";
    std::cout << "c = a + b -> " << c::value << "\n";
    std::cout << "d = b + c -> " << d::value << "\n";
    std::cout << "e = c + d -> " << e::value << "\n";
    std::cout << "f = d + e -> " << f::value << "\n\n";
  }

  {
    std::cout << "Computing discriminant b^2 - 4*a*c where a=1, b=5, c=6:\n";
    using program = decltype("a=1\n"
                             "b=5\n"
                             "c=6\n"
                             "bsq=b*b\n"
                             "fourac=4*a*c\n"
                             "disc=bsq-fourac"_s);

    using result = temppy::Run<program>;
    using disc = temppy::Lookup_t<typename result::env,
                                  temppy::String<'d', 'i', 's', 'c'>>;

    static_assert(disc::value == 1, "discriminant should be 25 - 24 = 1");

    std::cout << "a = 1\n";
    std::cout << "b = 5\n";
    std::cout << "c = 6\n";
    std::cout << "bsq = b * b = 25\n";
    std::cout << "fourac = 4 * a * c = 24\n";
    std::cout << "disc = bsq - fourac = " << disc::value << "\n\n";
  }

  {
    std::cout << "Rectangle area and perimeter (width=12, height=8):\n";
    using program =
        temppy::String<'w', '=', '1', '2', '\n', 'h', '=', '8', '\n', 'a', 'r',
                       'e', 'a', '=', 'w', '*', 'h', '\n', 'p', 'e', 'r', 'i',
                       '=', '2', '*', '(', 'w', '+', 'h', ')'>;
    using result = temppy::Run<program>;
    using area = temppy::Lookup_t<typename result::env,
                                  temppy::String<'a', 'r', 'e', 'a'>>;
    using peri = temppy::Lookup_t<typename result::env,
                                  temppy::String<'p', 'e', 'r', 'i'>>;

    static_assert(area::value == 96, "area should be 96");
    static_assert(peri::value == 40, "perimeter should be 40");

    std::cout << "width = 12\n";
    std::cout << "height = 8\n";
    std::cout << "area = w * h = " << area::value << "\n";
    std::cout << "perimeter = 2 * (w + h) = " << peri::value << "\n\n";
  }

  return 0;
}
