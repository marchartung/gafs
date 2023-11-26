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
#include <algorithm>

namespace internal {
using std::merge;
}
#else
#include <thrust/merge.h>

namespace internal {
using thrust::merge;
}
#endif

template <typename T>
void Merge(const GpuVector<T>& v1, const GpuVector<T>& v2,
           GpuVector<T>& v_out) {
  internal::merge(std_exec_policy(), v1.begin(), v1.end(), v2.begin(), v2.end(),
                  v_out.begin());
}

template <typename T>
GpuVector<T> Merge(const GpuVector<T>& v1, const GpuVector<T>& v2) {
  GpuVector<T> res(v1.size() + v2.size());
  internal::merge(std_exec_policy(), v1.begin(), v1.end(), v2.begin(), v2.end(),
                  res.begin());
  return res;
}

template <typename T, typename Comp>
void Merge(const GpuVector<T>& v1, const GpuVector<T>& v2, GpuVector<T>& v_out,
           Comp comp) {
  internal::merge(std_exec_policy(), v1.begin(), v1.end(), v2.begin(), v2.end(),
                  v_out.begin(), comp);
}
template <typename T, typename Comp>
GpuVector<T> Merge(const GpuVector<T>& v1, const GpuVector<T>& v2, Comp comp) {
  GpuVector<T> res(v1.size() + v2.size());
  internal::merge(std_exec_policy(), v1.begin(), v1.end(), v2.begin(), v2.end(),
                  res.begin(), comp);
  return res;
}
