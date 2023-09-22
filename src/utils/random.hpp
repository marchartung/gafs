#pragma once

#include <random>
#include <type_traits>
#include <vector>

template<typename T>
std::vector<T>
Random(size_t n, const T min, const T max)
{
  using Distribution = std::conditional_t<std::is_integral_v<T>,
                                          std::uniform_int_distribution<T>,
                                          std::uniform_real_distribution<T>>;

  std::random_device rnd_device;
  std::mt19937 mersenne_engine{ rnd_device() }; // Generates random integers
  Distribution dist{ min, max };

  std::vector<T> res(n);
  for (size_t i = 0; i < n; ++i) {
    res[i] = dist(mersenne_engine);
  }
  return res;
}