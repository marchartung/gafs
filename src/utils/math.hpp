#pragma once

#include <cmath>

namespace math {

template <typename T>
constexpr T pi() {
  return 3.14159265359;
}

template <size_t Exponent, typename T>
auto tpow(const T& x) {
  if constexpr (Exponent == 0)
    return 1;
  else if constexpr (Exponent == 1)
    return x;
  else if constexpr (Exponent == 2)
    return x * x;
  else if constexpr (Exponent == 3)
    return x * x * x;
  else {
    const auto half = tpow<Exponent / 2>(x);
    const auto half2 = half * half;
    if constexpr (Exponent % 2 == 0)
      return half2;
    else
      return half2 * x;
  }
}

}  // namespace math