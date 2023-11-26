#include <benchmark/benchmark.h>

#include <cstdint>

#include "algo/morton_octree.hpp"
#include "algo/morton_points.hpp"
#include "helper_cpu_bench.hpp"
#include "parstd/parstd.hpp"

static void Morton64PointsCreate(benchmark::State& state) {
  auto [dr, init_points] = CreatePointCuboid(8'000'000, 1.0);
  const GpuVector<Vectord> points = init_points;
  for (auto _ : state) {
    auto [index_map, morton_points] =
        MortonPoints<Morton64>::Create(2. * 1.2 * dr, points);
    benchmark::DoNotOptimize(morton_points);
  }
}
BENCHMARK(Morton64PointsCreate)->Unit(benchmark::kMillisecond);

static void Morton32PointsCreate(benchmark::State& state) {
  auto [dr, init_points] = CreatePointCuboid(8'000'000, 1.0);
  const GpuVector<Vectord> points = init_points;
  for (auto _ : state) {
    auto [index_map, morton_points] =
        MortonPoints<Morton32>::Create(2. * 1.2 * dr, points);
    benchmark::DoNotOptimize(morton_points);
  }
}
BENCHMARK(Morton32PointsCreate)->Unit(benchmark::kMillisecond);

static void Morton64OctreeCreate(benchmark::State& state) {
  auto [dr, init_points] = CreatePointCuboid(8'000'000, 1.0);
  auto [index_map, morton_points] =
      MortonPoints<Morton64>::Create(2. * 1.2 * dr, init_points);
  for (auto _ : state) {
    MortonOctree<Morton64> octree(morton_points.cell_mortons());
    benchmark::DoNotOptimize(octree);
  }
}
BENCHMARK(Morton64OctreeCreate)->Unit(benchmark::kMillisecond);

static void Morton32OctreeCreate(benchmark::State& state) {
  auto [dr, init_points] = CreatePointCuboid(8'000'000, 1.0);
  auto [index_map, morton_points] =
      MortonPoints<Morton32>::Create(2. * 1.2 * dr, init_points);
  for (auto _ : state) {
    MortonOctree<Morton32> octree(morton_points.cell_mortons());
    benchmark::DoNotOptimize(octree);
  }
}
BENCHMARK(Morton32OctreeCreate)->Unit(benchmark::kMillisecond);