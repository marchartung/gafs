#include "algo/basic_algorithms.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>

#include "utils/types.hpp"

TEST(BasicAlgorithms, SortUint32) {
  std::vector<uint32_t> vals = {9, 4, 1, 3, 6, 7, 46, 45235, 3,
                                1, 1, 1, 1, 1, 2, 0,  99};
  CpuVector<uint32_t> cvals = vals;
  GpuVector<uint32_t> gvals = vals;

  Sort(gvals);
  cvals = gvals;

  std::sort(vals.begin(), vals.end());
  ASSERT_EQ(vals.size(), cvals.size());
  ASSERT_EQ(vals.size(), gvals.size());

  for (size_t i = 0; i < vals.size(); ++i) {
    ASSERT_EQ(vals[i], cvals[i]);
  }
}

TEST(BasicAlgorithms, SortStruct) {
  struct TS {
    uint64_t sort_val;
    uint32_t dummy;
  };
  std::vector<TS> vals = {
      {9, 3},   {7, 22},    {3, 324},  {1, 44},
      {9, 123}, {734, 324}, {9222, 3}, {1112437, 1},
  };
  CpuVector<TS> cvals = vals;
  GpuVector<TS> gvals = vals;

  Sort(gvals, [](const TS a, const TS b) { return a.sort_val < b.sort_val; });
  cvals = gvals;

  std::sort(vals.begin(), vals.end(),
            [](const TS a, const TS b) { return a.sort_val < b.sort_val; });
  ASSERT_EQ(vals.size(), cvals.size());
  ASSERT_EQ(vals.size(), gvals.size());

  for (size_t i = 0; i < vals.size(); ++i) {
    ASSERT_EQ(vals[i].sort_val, cvals[i].sort_val);
    ASSERT_EQ(vals[i].dummy, cvals[i].dummy);
  }
}

TEST(BasicAlgorithms, UniqueUint32) {
  std::vector<uint32_t> vals = {1,  2,    3,          4,          5,         6,
                                7,  8,    33,         33,         34,        35,
                                36, 4355, 2345245342, 2345245343, 2345245343};
  CpuVector<uint32_t> cvals = vals;
  GpuVector<uint32_t> gvals = vals;

  Unique(gvals);
  cvals = gvals;

  auto it = std::unique(vals.begin(), vals.end());
  vals.erase(it, vals.end());
  ASSERT_EQ(vals.size(), cvals.size());
  ASSERT_EQ(vals.size(), gvals.size());

  for (size_t i = 0; i < vals.size(); ++i) {
    ASSERT_EQ(vals[i], cvals[i]);
  }
}

TEST(BasicAlgorithms, UniqueStruct) {
  struct TS {
    uint64_t sort_val;
    uint32_t dummy;
  };
  std::vector<TS> vals = {{9, 3},   {10, 22},     {13, 324},
                          {13, 44}, {13, 123},    {734, 324},
                          {734, 3}, {1112437, 1}, {1112437, 1}};
  CpuVector<TS> cvals = vals;
  GpuVector<TS> gvals = vals;

  Unique(gvals,
         [](const TS a, const TS b) { return a.sort_val == b.sort_val; });
  cvals = gvals;

  auto it = std::unique(vals.begin(), vals.end(), [](const TS a, const TS b) {
    return a.sort_val == b.sort_val;
  });
  vals.erase(it, vals.end());
  ASSERT_EQ(vals.size(), cvals.size());
  ASSERT_EQ(vals.size(), gvals.size());

  for (size_t i = 0; i < vals.size(); ++i) {
    ASSERT_EQ(vals[i].sort_val, cvals[i].sort_val);
    ASSERT_EQ(vals[i].dummy, cvals[i].dummy);
  }
}

TEST(BasicAlgorithms, ForEachIndexOneArg) {
  const uint32_t fac = 2;
  std::vector<uint32_t> vals = {1,  2,    3,          4,          5,         6,
                                7,  8,    33,         33,         34,        35,
                                36, 4355, 2345245342, 2345245343, 2345245343};
  CpuVector<uint32_t> cvals = vals;
  GpuVector<uint32_t> gvals = vals;

  ForEachIndex(
      vals.size(),
      [fac](const size_t i, uint32_t* v, uint32_t t) {
        v[i] *= fac;
        v[i] += t;
      },
      gvals, 0);

  cvals = gvals;
  ASSERT_EQ(vals.size(), cvals.size());
  ASSERT_EQ(vals.size(), gvals.size());

  for (size_t i = 0; i < vals.size(); ++i) {
    ASSERT_EQ(fac * vals[i], cvals[i]);
  }
}

TEST(BasicAlgorithms, SortParallelMerge) {
  std::vector<uint32_t> vals(64);
  std::iota(vals.begin(), vals.end(), 0);
  std::reverse(vals.begin(), vals.end());

  SortParallelMerge(vals);
  for (const auto v : vals) std::cout << v << " ";
  std::cout << std::endl;
  for (size_t i = 0; i < vals.size() - 1; ++i) {
    ASSERT_LE(vals[i], vals[i + 1]);
  }
}