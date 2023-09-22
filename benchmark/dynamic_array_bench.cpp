#include <benchmark/benchmark.h>
#include <omp.h>

#include <cstdlib>
#include <vector>

#include "algo/basic_algorithms.hpp"
#include "utils/dynamic_array.hpp"

volatile size_t num_dbl_elements = 1024 * 1024 * 128;

static void DynArr_CreateArray(benchmark::State& state) {
  for (auto _ : state) {
    DynamicArray<double> d(num_dbl_elements);
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
    DynamicArray<double> d(num_dbl_elements, 1.734);
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
    DynamicArray<double> d(num_dbl_elements);
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
    DynamicArray<double> d(num_dbl_elements, 1.734);
    benchmark::DoNotOptimize(d);
  }
}
BENCHMARK(DynArr_CreateArrayValue)->Unit(benchmark::kMillisecond);

static void SortSeq(benchmark::State& state) {
  std::vector<double> data(num_dbl_elements);
  std::srand(723445023);
  for (double& d : data) {
    d = std::rand();
  }
  for (auto _ : state) {
    std::vector<double> copy = data;
    std::sort(copy.begin(), copy.end());
    benchmark::DoNotOptimize(copy);
  }
}
BENCHMARK(SortSeq)->Unit(benchmark::kMillisecond);

static void SortPar(benchmark::State& state) {
  std::vector<double> data(state.range(0));
  std::srand(723445023);
  for (double& d : data) {
    d = std::rand();
  }
  for (auto _ : state) {
    std::vector<double> copy = data;
    Sort(copy);
    benchmark::DoNotOptimize(copy);
  }
}
BENCHMARK(SortPar)->Unit(benchmark::kMillisecond)->Range(8 << 20, 8 << 25);

static void SortParallelMergeBench(benchmark::State& state) {
  std::vector<double> data(state.range(0));
  std::srand(723445023);
  for (double& d : data) {
    d = std::rand();
  }
  for (auto _ : state) {
    std::vector<double> copy = data;
    SortParallelMerge(copy);
    benchmark::DoNotOptimize(copy);
  }
}
BENCHMARK(SortParallelMergeBench)
    ->Unit(benchmark::kMillisecond)
    ->Range(8 << 20, 8 << 25);