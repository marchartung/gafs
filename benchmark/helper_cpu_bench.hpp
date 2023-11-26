#pragma once

#include <tuple>
#include <vector>

#include "algo/morton.hpp"
#include "utils/random.hpp"
#include "utils/types.hpp"

template <typename T>
std::vector<T> RandomStdVector(const size_t n = 8'000'000, const T min_v = 0,
                               const T max_v = static_cast<T>(35314590)) {
  std::vector<T> res = Random(n, min_v, max_v);
  return res;
}

template <typename T>
std::vector<T> RandomDuplicateStdVector(
    const size_t n = 8'000'000, const size_t redundant_fac = 20,
    const T min_v = 0, const T max_v = static_cast<T>(35314590)) {
  std::vector<T> tmp = Random(n / redundant_fac, min_v, max_v);
  std::vector<T> values(tmp.size() * redundant_fac);
  for (size_t i = 0; i < tmp.size(); ++i) {
    for (size_t j = 0; j < redundant_fac; ++j) {
      values[i * redundant_fac + j] = tmp[i];
    }
  }
  return values;
}
template <size_t N, typename T>
std::vector<Array<T, N>> RandomStdVectorVectorNT(
    const size_t n = 8'000'000, const T min_v = 0,
    const T max_v = static_cast<T>(35314590)) {
  std::vector<Array<T, N>> res;
  for (size_t i = 0; i < N; ++i) {
    const auto rn = RandomStdVector<T>(n, min_v, max_v);
    res.resize(n);
    for (size_t j = 0; j < n; ++j) {
      res[j][i] = rn[j];
    }
  }
  return res;
}

inline std::tuple<double, std::vector<Vectord>> CreatePointCuboid(
    const size_t n = 8'000'000, const Vectord dims = Vectord(1.)) {
  const double f = std::cbrt(n / (dims[0] * dims[1] * dims[2]));
  const size_t xd = std::round(dims[0] * f), yd = std::round(dims[1] * f),
               zd = std::round(dims[2] * f);
  const double dr = (dims[0] / xd + dims[1] * yd + dims[2] * zd) / 3;
  const Vectord o(0.5 * dr);
  std::vector<Vectord> res;
  res.reserve(xd * yd * zd);
  for (size_t x = 0; x < xd; ++x)
    for (size_t y = 0; y < yd; ++y)
      for (size_t z = 0; z < zd; ++z) {
        res.push_back(o + Vectord(x * dr, y * dr, z * dr));
      }
  return std::make_tuple(dr, std::move(res));
}