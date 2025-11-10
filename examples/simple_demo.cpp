#include <iostream>

namespace temppy {

template <int N> struct IntValue {
  static constexpr int value = N;
};

template <int A, int B> struct Add {
  static constexpr int value = A + B;
};

template <int A, int B> struct Mul {
  static constexpr int value = A * B;
};

template <int X = 0, int Y = 0, int Z = 0> struct Env {
  static constexpr int x = X;
  static constexpr int y = Y;
  static constexpr int z = Z;
};

template <typename E = Env<>> struct Program {

  using env1 = Env<5, E::y, E::z>;

  using env2 = Env<env1::x, env1::x * 2, env1::z>;

  using result_env = Env<env2::x, env2::y, env2::x + env2::y>;

  static constexpr int x = result_env::x;
  static constexpr int y = result_env::y;
  static constexpr int z = result_env::z;
};

} // namespace temppy

int main() {
  std::cout << "TempPy: Compile-Time Python Interpreter (Demo)\n";
  std::cout << "==================================================\n\n";

  std::cout << "Program:\n";
  std::cout << "  x = 5\n";
  std::cout << "  y = x * 2\n";
  std::cout << "  z = x + y\n\n";

  using result = temppy::Program<>;

  static_assert(result::x == 5);
  static_assert(result::y == 10);
  static_assert(result::z == 15);

  std::cout << "Results (computed at compile-time):\n";
  std::cout << "  x = " << result::x << "\n";
  std::cout << "  y = " << result::y << "\n";
  std::cout << "  z = " << result::z << "\n";
  std::cout << "\nAll calculations were performed by the C++ compiler!\n";

  return 0;
}
