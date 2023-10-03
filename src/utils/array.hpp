// MIT License

// Copyright (c) 2023 Marc Hartung

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cmath>
#include <cstddef>
#include <ostream>

#include "macros.hpp"

template <typename T, size_t N>
class Array {
 public:
  using value_type = T;

  template <size_t I, typename Functor>
  DEVICE static constexpr void ForeachI(Functor f) {
    if constexpr (I < N) {
      f(I);
      ForeachI<I + 1>(f);
    }
  }

  Array() {}
  DEVICE constexpr Array(const T &val) {
    ForeachI<0>([&](const size_t i) { elements[i] = val; });
  }

  DEVICE constexpr Array(const T x, const T y, const T z) {
    static_assert(N == 3);
    elements[0] = x;
    elements[1] = y;
    elements[2] = z;
  }

  DEVICE constexpr Array(const std::array<T, N> &list) {
    ForeachI<0>([&](const size_t i) { elements[i] = list[i]; });
  }

  DEVICE constexpr Array<T, N> &operator=(const std::array<T, N> &list) {
    ForeachI<0>([&](const size_t i) { elements[i] = list[i]; });
    return *this;
  }

  DEVICE constexpr const T &operator[](const size_t i) const {
    return elements[i];
  }
  DEVICE constexpr T &operator[](const size_t i) { return elements[i]; }
  DEVICE constexpr size_t size() const { return N; }

  DEVICE constexpr const T *data() const { return elements; }
  DEVICE constexpr T *data() { return elements; }

  DEVICE constexpr Array<T, N> &operator+=(const Array<T, N> &in) {
    ForeachI<0>([&](const size_t i) { elements[i] += in[i]; });
    return *this;
  }
  DEVICE constexpr Array<T, N> &operator-=(const Array<T, N> &in) {
    ForeachI<0>([&](const size_t i) { elements[i] -= in[i]; });
    return *this;
  }

  DEVICE constexpr Array<T, N> &operator+=(const T &in) {
    ForeachI<0>([&](const size_t i) { elements[i] += in; });
    return *this;
  }
  DEVICE constexpr Array<T, N> &operator-=(const T &in) {
    ForeachI<0>([&](const size_t i) { elements[i] -= in; });
    return *this;
  }
  DEVICE constexpr Array<T, N> &operator*=(const T &in) {
    ForeachI<0>([&](const size_t i) { elements[i] *= in; });
    return *this;
  }
  DEVICE constexpr Array<T, N> &operator/=(const T &in) {
    ForeachI<0>([&](const size_t i) { elements[i] /= in; });
    return *this;
  }

  DEVICE constexpr Array<T, N> operator-() const {
    Array<T, N> res;
    ForeachI<0>([&](const size_t i) { res[i] = -elements[i]; });
    return res;
  }

  DEVICE friend constexpr Array<T, N> operator+(const Array<T, N> &a,
                                                const Array<T, N> &b) {
    Array<T, N> res = a;
    return res += b;
  }
  DEVICE friend constexpr Array<T, N> operator-(const Array<T, N> &a,
                                                const Array<T, N> &b) {
    Array<T, N> res = a;
    return res -= b;
  }
  DEVICE friend constexpr T operator*(const Array<T, N> &a,
                                      const Array<T, N> &b) {
    T sum = a[0] * b[0];
    Array<T, N>::ForeachI<1>([&](const size_t i) { sum += a[i] * b[i]; });
    return sum;
  }

  DEVICE friend constexpr Array<T, N> operator+(const Array<T, N> &a,
                                                const T &b) {
    Array<T, N> res = a;
    return res += b;
  }
  DEVICE friend constexpr Array<T, N> operator-(const Array<T, N> &a,
                                                const T &b) {
    Array<T, N> res = a;
    return res -= b;
  }
  DEVICE friend constexpr Array<T, N> operator*(const Array<T, N> &a,
                                                const T &b) {
    Array<T, N> res = a;
    return res *= b;
  }
  DEVICE friend constexpr Array<T, N> operator/(const Array<T, N> &a,
                                                const T &b) {
    Array<T, N> res = a;
    return res /= b;
  }

  DEVICE friend constexpr Array<T, N> operator+(const T &b,
                                                const Array<T, N> &a) {
    return a + b;
  }
  DEVICE friend constexpr Array<T, N> operator-(const T &b,
                                                const Array<T, N> &a) {
    return -a + b;
  }
  DEVICE friend constexpr Array<T, N> operator*(const T &b,
                                                const Array<T, N> &a) {
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
DEVICE Array<T, N> Min(const Array<T, N> &a, const Array<T, N> &b) {
  Array<T, N> res;
  for (size_t i = 0; i < N; ++i) {
    res[i] = std::min(a[i], b[i]);
  }
  return res;
}
template <typename T, size_t N>
DEVICE Array<T, N> Max(const Array<T, N> &a, const Array<T, N> &b) {
  Array<T, N> res;
  for (size_t i = 0; i < N; ++i) {
    res[i] = std::max(a[i], b[i]);
  }
  return res;
}

template <typename T, size_t N>
DEVICE T Length(const Array<T, N> &v) {
  return std::sqrt(v * v);
}

template <typename T, size_t N>
DEVICE T Distance(const Array<T, N> &a, const Array<T, N> &b) {
  return Length(a - b);
}

template <typename T>
DEVICE Array<T, 3> CrossProduct(const Array<T, 3> &u, const Array<T, 3> &v) {
  const T x = u[1] * v[2] - u[2] * v[1];
  const T y = u[2] * v[0] - u[0] * v[2];
  const T z = u[0] * v[1] - u[1] * v[0];
  return Array<T, 3>(x, y, z);
}

template <typename T>
DEVICE Array<T, 3> Normal(const Array<T, 3> &u, const Array<T, 3> &v) {
  const Array<T, 3> d = CrossProduct(u, v);
  return d / Length(d);
}

template <typename T, size_t N>
DEVICE constexpr Array<T, N> Abs(const Array<T, N> &v) {
  Array<T, N> res;
  Array<T, N>::ForeachI<0>([&](const size_t i) { res[i] = std::abs(v[i]); });
  return res;
}

template <typename To, typename T, size_t N>
DEVICE constexpr Array<To, N> Cast(const Array<T, N> &in) {
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