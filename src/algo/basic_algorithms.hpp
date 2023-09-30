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
#include <iostream>
#include <vector>  // FIXME

#include "utils/macros.hpp"

#ifdef OMP_ENABLED
#include <omp.h>
#endif

#ifdef TBB_ENABLED
inline constexpr auto& std_exec_policy() { return std::execution::par_unseq; }
#else
inline constexpr auto& std_exec_policy() { return std::execution::seq; }
#endif

template <typename T>
void Sort(std::vector<T>& data) {
  std::sort(std_exec_policy(), data.begin(), data.end());
}

template <typename T, typename Comp>
void Sort(std::vector<T>& data, Comp comp) {
  std::sort(std_exec_policy(), data.begin(), data.end(), comp);
}

template <typename T>
void SortParallelMerge(std::vector<T>& data) {
  constexpr size_t thread_fac = 8;
  if (data.size() < thread_fac * omp_get_max_threads()) {
    std::sort(data.begin(), data.end());
    return;
  }
  const size_t d = std::log2(thread_fac * omp_get_max_threads()),
               nt = std::pow(2, d), m = data.size() / nt;
  std::vector<std::vector<T>> recv_data(nt), work_data(nt);
#pragma omp parallel for schedule(static)
  for (size_t i = 0; i < recv_data.size(); ++i) {
    const size_t b = std::min(i * m, data.size()),
                 e = std::min((i + 1) * m, data.size());
    std::vector<T> tdata(data.begin() + b, data.begin() + e);
    std::sort(tdata.begin(), tdata.end());

    if (e < m) {
      std::fill_n(std::back_inserter(tdata), m - e,
                  std::numeric_limits<T>::max());
    }

    std::swap(tdata, recv_data[i]);
    work_data[i] = std::vector<T>(m);
  }

  for (size_t i = 0; i < d; ++i) {
#pragma omp parallel for schedule(static)
    for (size_t tid = 0; tid < recv_data.size(); ++tid) {
      const size_t nid = tid ^ (1 << i);
      std::vector<T> tdata = std::move(work_data[tid]);
      const T *a = recv_data[tid].data(), *b = recv_data[nid].data();
      if (tid & (1 << i)) {
        // high
        size_t k = m - 1, l = m - 1;
        for (size_t i = m; i > 0; --i) {
          const T va = a[k], vb = b[l];
          const bool sm = (va > vb);
          tdata[i - 1] = sm ? va : vb;
          k -= sm;
          l -= !sm;
        }
      } else {
        // low
        size_t k = 0, l = 0;
        for (size_t i = 0; i < m; ++i) {
          const T va = a[k], vb = b[l];
          const bool sm = (va < vb);
          tdata[i] = sm ? va : vb;
          k += sm;
          l += !sm;
        }
      }
      work_data[tid] = std::move(tdata);
    }

#pragma omp parallel for schedule(static)
    for (size_t tid = 0; tid < recv_data.size(); ++tid) {
      std::swap(work_data[tid], recv_data[tid]);
    }
  }
#pragma omp parallel for schedule(static)
  for (size_t tid = 0; tid < recv_data.size(); ++tid) {
    const size_t b = std::min(tid * m, data.size()),
                 e = std::min((tid + 1) * m, data.size());
    std::copy(recv_data[tid].begin(), recv_data[tid].begin() + (e - b),
              data.begin() + b);
  }
}

template <typename T>
void Unique(std::vector<T>& data) {
  auto it = std::unique(std_exec_policy(), data.begin(), data.end());
  data.erase(it, data.end());
}

template <typename T, typename Comp>
void Unique(std::vector<T>& data, Comp comp) {
  auto it = std::unique(std_exec_policy(), data.begin(), data.end(), comp);
  data.erase(it, data.end());
}

template <typename S, typename T, typename Functor>
S Reduce(const std::vector<T>& data, const S init, Functor f) {
  return std::reduce(std_exec_policy(), data.begin(), data.end(), init, f);
}

template <typename T>
void Iota(std::vector<T>& data, const T init) {
  std::iota(std_exec_policy(), data.begin(), data.end(), init);
}

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
#include <thrust/execution_policy.h>
#include <thrust/reduce.h>
#include <thrust/sequence.h>
#include <thrust/sort.h>
#include <thrust/unique.h>

template <typename T>
void Sort(thrust::device_vector<T>& data) {
  thrust::sort(thrust::device, data.begin(), data.end());
}

template <typename T, typename Comp>
void Sort(thrust::device_vector<T>& data, Comp comp) {
  thrust::sort(thrust::device, data.begin(), data.end(), comp);
}

template <typename T>
void Unique(thrust::device_vector<T>& data) {
  auto it = thrust::unique(thrust::device, data.begin(), data.end());
  data.erase(it, data.end());
}

template <typename T, typename Comp>
void Unique(thrust::device_vector<T>& data, Comp comp) {
  auto it = thrust::unique(thrust::device, data.begin(), data.end(), comp);
  data.erase(it, data.end());
}

template <typename S, typename T, typename Functor>
S Reduce(const thrust::device_vector<T>& data, const S init, Functor f) {
  return thrust::reduce(thrust::device, data.begin(), data.end(), init, f);
}

template <typename T>
void Iota(thrust::device_vector<T>& data, const T init) {
  thrust::sequence(thrust::device, data.begin(), data.end(), init);
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