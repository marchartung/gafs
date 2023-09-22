#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <ostream>

#include "macros.hpp"

template <typename T, size_t N>
class Array {
 public:
  using value_type = T;

  template <size_t I, typename Functor>
  DEVICE static void ForeachI(Functor f) {
    if constexpr (I < N) {
      f(I);
      ForeachI<I + 1>(f);
    }
  }

  Array() = default;
  DEVICE Array(const T &val) {
    ForeachI<0>([&](const size_t i) { elements[i] = val; });
  }

  DEVICE Array(const T x, const T y, const T z) {
    static_assert(N == 3);
    elements[0] = x;
    elements[1] = y;
    elements[2] = z;
  }

  Array(const std::array<T, N> &list) {
    ForeachI<0>([&](const size_t i) { elements[i] = list[i]; });
  }

  DEVICE Array<T, N> &operator=(const std::array<T, N> &list) {
    ForeachI<0>([&](const size_t i) { elements[i] = list[i]; });
    return *this;
  }

  DEVICE const T &operator[](const size_t i) const { return elements[i]; }
  DEVICE T &operator[](const size_t i) { return elements[i]; }
  DEVICE constexpr size_t size() const { return N; }

  DEVICE const T *data() const { return elements; }
  DEVICE T *data() { return elements; }

  DEVICE Array<T, N> &operator+=(const Array<T, N> &in) {
    ForeachI<0>([&](const size_t i) { elements[i] += in[i]; });
    return *this;
  }
  DEVICE Array<T, N> &operator-=(const Array<T, N> &in) {
    ForeachI<0>([&](const size_t i) { elements[i] -= in[i]; });
    return *this;
  }

  DEVICE Array<T, N> &operator+=(const T &in) {
    ForeachI<0>([&](const size_t i) { elements[i] += in; });
    return *this;
  }
  DEVICE Array<T, N> &operator-=(const T &in) {
    ForeachI<0>([&](const size_t i) { elements[i] -= in; });
    return *this;
  }
  DEVICE Array<T, N> &operator*=(const T &in) {
    ForeachI<0>([&](const size_t i) { elements[i] *= in; });
    return *this;
  }
  DEVICE Array<T, N> &operator/=(const T &in) {
    ForeachI<0>([&](const size_t i) { elements[i] /= in; });
    return *this;
  }

  DEVICE Array<T, N> operator-() const {
    Array<T, N> res;
    ForeachI<0>([&](const size_t i) { res[i] = -elements[i]; });
    return res;
  }

  DEVICE friend Array<T, N> operator+(const Array<T, N> &a,
                                      const Array<T, N> &b) {
    Array<T, N> res = a;
    return res += b;
  }
  DEVICE friend Array<T, N> operator-(const Array<T, N> &a,
                                      const Array<T, N> &b) {
    Array<T, N> res = a;
    return res -= b;
  }
  DEVICE friend T operator*(const Array<T, N> &a, const Array<T, N> &b) {
    T sum = a[0] * b[0];
    Array<T, N>::ForeachI<1>([&](const size_t i) { sum += a[i] * b[i]; });
    return sum;
  }

  DEVICE friend Array<T, N> operator+(const Array<T, N> &a, const T &b) {
    Array<T, N> res = a;
    return res += b;
  }
  DEVICE friend Array<T, N> operator-(const Array<T, N> &a, const T &b) {
    Array<T, N> res = a;
    return res -= b;
  }
  DEVICE friend Array<T, N> operator*(const Array<T, N> &a, const T &b) {
    Array<T, N> res = a;
    return res *= b;
  }
  DEVICE friend Array<T, N> operator/(const Array<T, N> &a, const T &b) {
    Array<T, N> res = a;
    return res /= b;
  }

  DEVICE friend Array<T, N> operator+(const T &b, const Array<T, N> &a) {
    return a + b;
  }
  DEVICE friend Array<T, N> operator-(const T &b, const Array<T, N> &a) {
    return -a + b;
  }
  DEVICE friend Array<T, N> operator*(const T &b, const Array<T, N> &a) {
    return a * b;
  }

 private:
  T elements[N];
};

template <typename T, size_t N>
std::ostream &operator<<(std::ostream &ous, const Array<T, N> &a) {
  ous << "[";
  for (size_t i = 0; i < a.size(); ++i) {
    ous << a[i] << ", ";
  }
  ous << "]";
  return ous;
}

template <typename T, size_t N>
DEVICE T Length(const Array<T, N> &v) {
  return std::sqrt(v * v);
}

template <typename T, size_t N>
DEVICE Array<T, N> Abs(const Array<T, N> &v) {
  Array<T, N> res;
  Array<T, N>::ForeachI<0>([&](const size_t i) { res[i] = std::abs(v[i]); });
  return res;
}

template <typename To, typename T, size_t N>
DEVICE Array<To, N> Cast(const Array<T, N> &in) {
  Array<To, N> res;
  Array<T, N>::template ForeachI<0>(
      [&](const size_t i) { res[i] = static_cast<To>(in[i]); });
  return res;
}

template <typename T, size_t N>
DEVICE Array<T, N> fmod(const Array<T, N> &v, const T div) {
  Array<T, N> res;
  Array<T, N>::template ForeachI<0>(
      [&](const size_t i) { res[i] = std::fmod(v[i], div); });
  return res;
}

template <size_t N, typename T>
using VectorNT = Array<T, N>;

template <typename T>
using Vector3T = VectorNT<3, T>;

using Pointf = Array<float, 3>;
using Pointd = Array<double, 3>;

#ifdef GPU_ENABLED

#include <thrust/device_vector.h>
#include <thrust/host_vector.h>

template <typename T>
using GpuVector = thrust::device_vector<T>;
template <typename T>
using CpuVector = thrust::host_vector<T>;

#else
#include <vector>

template <typename T>
using GpuVector = std::vector<T>;
template <typename T>
using CpuVector = std::vector<T>;

#endif
