#pragma once

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

#include "execution.hpp"
#include "vector.hpp"

#ifndef GPU_ENABLED
#include <numeric>

namespace internal {
using std::reduce;
}
#else
#include <thrust/fill.h>

namespace internal {
using thrust::reduce;
}
#endif

template <typename T>
T Reduce(const GpuVector<T>& v) {
  return internal::reduce(std_exec_policy(), v.begin(), v.end());
}

template <typename T>
T Reduce(const GpuVector<T>& v, const T init) {
  return internal::reduce(std_exec_policy(), v.begin(), v.end(), init);
}

template <typename T, typename BinaryOp>
T Reduce(const GpuVector<T>& v, const T init, BinaryOp op) {
  return internal::reduce(std_exec_policy(), v.begin(), v.end(), init, op);
}