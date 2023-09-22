#pragma once

#include <vector>

#include "algo/morton.hpp"
#include "helper_cpu_bench.hpp"
#include "utils/types.hpp"

template <typename T>
GpuVector<T> RandomGpuThrustVector(const size_t n = 8'000'000,
                                   const T min_v = 0,
                                   const T max_v = static_cast<T>(35314590)) {
  auto v = RandomStdVector<T>(n, min_v, max_v);
  thrust::host_vector<T> hthrust(v.begin(), v.end());
  v.clear();
  GpuVector<T> dthrust(hthrust);
  return dthrust;
}

template <typename T>
GpuVector<T> RandomDuplicateGpuThrustVector(
    const size_t n = 8'000'000, const size_t redundant_fac = 20,
    const T min_v = 0, const T max_v = static_cast<T>(35314590)) {
  auto v = RandomDuplicateStdVector<T>(n, redundant_fac, min_v, max_v);
  thrust::host_vector<T> hthrust(v.begin(), v.end());
  v.clear();
  GpuVector<T> dthrust(hthrust);
  return dthrust;
}

template <size_t N, typename T>
GpuVector<VectorNT<N, T>> RandomThrustVectorVectorNT(
    const size_t n = 8'000'000, const T min_v = 0,
    const T max_v = static_cast<T>(35314590)) {
  auto v = RandomStdVectorVectorNT<N, T>(n, min_v, max_v);
  thrust::host_vector<VectorNT<N, T>> hthrust(v.begin(), v.end());
  v = std::vector<VectorNT<N, T>>(0);
  GpuVector<VectorNT<N, T>> dthrust(hthrust);
  hthrust = thrust::host_vector<VectorNT<N, T>>(0);

  return dthrust;
}
#endif