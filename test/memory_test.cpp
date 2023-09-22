
#include "utils/memory.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>

TEST(Memory, CpuAlloc_Alloc_Free) {
  CpuPointer<double> mem;
  ASSERT_NO_THROW(mem = CpuAlloc::Allocate<double>(20));
  std::fill(mem + 0, mem + 20, 3.14);
  for (size_t i = 0; i < 20; ++i) {
    ASSERT_EQ(mem[i], 3.14);
  }
  ASSERT_NO_THROW(CpuAlloc::Free(mem));
  EXPECT_EQ(mem, nullptr);
}

TEST(Memory, CpuAlloc_Page_Bytes) {
  EXPECT_EQ(CpuAlloc::NumBytesPageAligned<double>(1), 4096);
  EXPECT_EQ(CpuAlloc::NumBytesPageAligned<int>(4096 / sizeof(int)), 4096);

  using DummyType = std::tuple<double, int, std::array<char, 13>>;
  EXPECT_EQ(CpuAlloc::NumBytesPageAligned<DummyType>(4096 / sizeof(DummyType)),
            4096);
}