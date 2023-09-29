#pragma once

#include <array>
#include <cmath>
#include <cstdint>

#include "array.hpp"
#include "macros.hpp"

using SizeT = uint32_t;

using Vectorf = Array<float, 3>;
using Vectord = Array<double, 3>;
using Vectori = Array<int32_t, 3>;

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
