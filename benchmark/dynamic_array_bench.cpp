#include <benchmark/benchmark.h>
#include <omp.h>

#include <cstdlib>
#include <vector>

#include "parstd/internal/cpu_vector.hpp"

volatile size_t num_dbl_elements = 1024 * 1024 * 128;

static void DynArr_CreateArray(benchmark::State& state) {
  for (auto _ : state) {
    CpuStdVector<double> d(num_dbl_elements);
    benchmark::DoNotOptimize(d);
  }
}
BENCHMARK(DynArr_CreateArray)->Unit(benchmark::kMillisecond);

static void DynArr_CreateStdVector(benchmark::State& state) {
  for (auto _ : state) {
    std::vector<double> d(num_dbl_elements);
    benchmark::DoNotOptimize(d);
  }
}
BENCHMARK(DynArr_CreateStdVector)->Unit(benchmark::kMillisecond);

static void DynArr_CreateArrayValue(benchmark::State& state) {
  for (auto _ : state) {
    CpuStdVector<double> d(num_dbl_elements, 1.734);
    benchmark::DoNotOptimize(d);
  }
}
BENCHMARK(DynArr_CreateArrayValue)->Unit(benchmark::kMillisecond);

static void DynArr_CreateStdVectorValue(benchmark::State& state) {
  for (auto _ : state) {
    std::vector<double> d(num_dbl_elements, 1.734);
    benchmark::DoNotOptimize(d);
  }
}
BENCHMARK(DynArr_CreateStdVectorValue)->Unit(benchmark::kMillisecond);

static void DynArr_CreateArrayOmpValue(benchmark::State& state) {
  for (auto _ : state) {
    CpuStdVector<double> d(num_dbl_elements);
#pragma omp parallel for schedule(static, 4096)
    for (size_t i = 0; i < d.size(); ++i) {
      d[i] = 1.734;
    }
    benchmark::DoNotOptimize(d);
  }
}
BENCHMARK(DynArr_CreateArrayOmpValue)->Unit(benchmark::kMillisecond);

static void DynArr_ResizeArray(benchmark::State& state) {
  for (auto _ : state) {
    CpuStdVector<double> d(num_dbl_elements, 1.734);
    benchmark::DoNotOptimize(d);
  }
}
BENCHMARK(DynArr_CreateArrayValue)->Unit(benchmark::kMillisecond);
