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
#include <execution>
#include <functional>
#include <vector>

#ifdef OMP_ENABLED
#include <omp.h>
#endif

template <typename T>
struct IsStdVector : public std::false_type {};
template <typename T>
struct IsStdVector<std::vector<T>> : public std::true_type {};

template <typename... Args>
struct ContainsStdVector {
  static constexpr bool value =
      (IsStdVector<std::remove_reference_t<Args>>::value || ...);
};

template <typename T>
decltype(auto) ConvertToPointerStd(T&& t) {
  if constexpr (std::is_arithmetic_v<std::remove_reference_t<T>>) {
    return t;
  } else {
    return t.data();
  }
}

template <typename Functor, typename... Args>
std::enable_if_t<ContainsStdVector<Args...>::value, void> ForEachIndex(
    size_t n, Functor device_func, Args&&... args) {
#ifdef OMP_ENABLED
#pragma omp parallel for schedule(static) num_threads(omp_get_max_threads())
  for (size_t i = 0; i < n; ++i) {
    device_func(i, ConvertToPointerStd(args)...);
  }
#else
  for (size_t i = 0; i < n; ++i) {
    device_func(i, ConvertToPointerStd(args)...);
  }
#endif
}

#ifdef GPU_ENABLED

#include <hip/hip_runtime_api.h>
#include <thrust/device_vector.h>
#include <thrust/reduce.h>
#include <thrust/sequence.h>

template <typename S, typename T, typename Functor>
S Reduce(const thrust::device_vector<T>& data, const S init, Functor f) {
  return thrust::reduce(std_exec_policy() data.begin(), data.end(), init, f);
}

template <typename T>
void Iota(thrust::device_vector<T>& data, const T init) {
  thrust::sequence(std_exec_policy() data.begin(), data.end(), init);
}

template <typename Functor, typename... Args>
__global__ void ForEachIndexGpuKernel2(Functor functor, const size_t n,
                                       Args... args) {
  const size_t start_i = blockIdx.x * blockDim.x + threadIdx.x;
  const size_t jump = blockDim.x * gridDim.x;
  for (size_t i = start_i; i < n; i += jump) {
    functor(i, args...);
  }
}

template <typename T>
decltype(auto) ConvertToPointerThrust(T&& t) {
  if constexpr (std::is_arithmetic_v<std::remove_reference_t<T>>) {
    return t;
  } else {
    return thrust::raw_pointer_cast(t.data());
  }
}

template <typename T>
struct IsThrustVector : public std::false_type {};
template <typename T>
struct IsThrustVector<thrust::device_vector<T>> : public std::true_type {};

template <typename... Args>
struct ContainsThrustVector {
  static constexpr bool value =
      (IsThrustVector<std::remove_reference_t<Args>>::value || ...);
};

template <typename Functor, typename... Args>
std::enable_if_t<ContainsThrustVector<Args...>::value, void> ForEachIndex(
    size_t n, Functor device_func, Args&&... args) {
  constexpr size_t num_procs = 60;
  constexpr size_t num_threads = 256;
  hipLaunchKernelGGL(ForEachIndexGpuKernel2<decltype(device_func)>,
                     dim3(num_procs), dim3(num_threads), 0, 0, device_func, n,
                     ConvertToPointerThrust(args)...);
}

#endif  // GPU_ENABLED