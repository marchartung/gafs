#include "parstd/vector.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <cstdint>

struct NeedInit {
  static std::atomic<int> num_init;

  NeedInit() { ++num_init; }
  NeedInit(const int val) : val(val) { ++num_init; }
  NeedInit(const NeedInit& in) : NeedInit(in.val) {}
  NeedInit(NeedInit&& in) { std::swap(in.val, val); }
  NeedInit& operator=(const NeedInit& in) {
    val = in.val;
    ++num_init;
    return *this;
  }
  NeedInit& operator=(NeedInit&& in) {
    std::swap(in.val, val);
    return *this;
  }

  ~NeedInit() {
    val = -1;
    --num_init;
  }

  bool is_default_init() const { return val == 42; }
  bool is_destructed() const { return val == -1; }

  int val = 42;
};
std::atomic<int> NeedInit::num_init = 0;

TEST(Vector, Construct) {
  NeedInit::num_init = 0;
  GpuVector<NeedInit> v1;
  EXPECT_EQ(v1.size(), 0);
  EXPECT_EQ(v1.capacity(), 0);
  EXPECT_TRUE(v1.empty());
  EXPECT_EQ(v1.data(), nullptr);
  EXPECT_EQ(NeedInit::num_init.load(), 0);

  NeedInit::num_init = 0;
  GpuVector<NeedInit> v2(5);
  EXPECT_EQ(v2.size(), 5);
  EXPECT_GE(v2.capacity(), 5);
  EXPECT_FALSE(v2.empty());
  EXPECT_EQ(NeedInit::num_init.load(), 5);

  NeedInit::num_init = 0;
  GpuVector<NeedInit> v3(2);
  ASSERT_EQ(v3.size(), 2);
  ASSERT_GE(v3.capacity(), 2);
  EXPECT_FALSE(v3.empty());
  EXPECT_EQ(NeedInit::num_init.load(), 2);
  EXPECT_TRUE(v3.data()[0].is_default_init());
  EXPECT_TRUE(v3.data()[1].is_default_init());

  NeedInit::num_init = 0;
  GpuVector<NeedInit> v4(1, NeedInit(7));
  ASSERT_EQ(v4.size(), 1);
  ASSERT_GE(v4.capacity(), 1);
  EXPECT_FALSE(v4.empty());
  EXPECT_EQ(v4.data()[0].val, 7);
  EXPECT_EQ(NeedInit::num_init.load(), 1);
}

TEST(Vector, Destruct) {
  NeedInit::num_init = 0;
  GpuVector<NeedInit>* pv = new GpuVector<NeedInit>(2);
  EXPECT_EQ(NeedInit::num_init.load(), 2);

  delete pv;
  EXPECT_EQ(NeedInit::num_init.load(), 0);
}

TEST(Vector, Reserve) {
  NeedInit::num_init = 0;
  NeedInit a(5);

  a = NeedInit(3);
  EXPECT_EQ(NeedInit::num_init.load(), 1);
  NeedInit::num_init = 0;
  GpuVector<NeedInit> v;
  v.reserve(5);
  EXPECT_GE(v.capacity(), 5);
  EXPECT_NE(v.data(), nullptr);
  EXPECT_EQ(NeedInit::num_init.load(), 0);
}

TEST(Vector, Clear) {
  NeedInit::num_init = 0;
  GpuVector<NeedInit> v(5);
  EXPECT_EQ(NeedInit::num_init.load(), 5);
  v.clear();
  EXPECT_EQ(NeedInit::num_init.load(), 0);
}

TEST(Vector, Resize) {
  NeedInit::num_init = 0;
  GpuVector<NeedInit> v;
  v.resize(2);
  EXPECT_GE(v.capacity(), 2);
  ASSERT_EQ(v.size(), 2);
  EXPECT_EQ(NeedInit::num_init.load(), 2);
  EXPECT_TRUE(v.data()[0].is_default_init());
  EXPECT_TRUE(v.data()[1].is_default_init());

  v.resize(1);
  ASSERT_EQ(v.size(), 1);
  EXPECT_GE(v.capacity(), 2);
  EXPECT_TRUE(v.data()[0].is_default_init());

  v.resize(2, NeedInit(13));
  ASSERT_EQ(v.size(), 2);
  EXPECT_GE(v.capacity(), 2);
  EXPECT_TRUE(v.data()[0].is_default_init());
  EXPECT_EQ(v.data()[1].val, 13);

  v.resize(10000);
  EXPECT_EQ(v.size(), 10000);
  EXPECT_GE(v.capacity(), 10000);
  EXPECT_EQ(NeedInit::num_init.load(), 10000);
}

TEST(Vector, Insert) {
  std::vector<NeedInit> base = {NeedInit(0), NeedInit(1), NeedInit(2)};

  NeedInit::num_init = 0;
  GpuVector<NeedInit> v;
  v.insert(v.end(), base.begin(), base.end());
  ASSERT_EQ(v.size(), base.size());
  EXPECT_EQ(NeedInit::num_init.load(), 3);
  for (size_t i = 0; i < base.size(); ++i) {
    EXPECT_EQ(i, v[i].val);
  }

  v.insert(v.end(), v.begin(), v.end());
  ASSERT_EQ(v.size(), 2 * base.size());
  EXPECT_EQ(NeedInit::num_init.load(), 2 * base.size());
  for (size_t i = 0; i < base.size(); ++i) {
    EXPECT_EQ(i, v[i].val);
    EXPECT_EQ(i, v[i + base.size()].val);
  }

  base[0] = 3;
  base[1] = 4;
  base[2] = 5;
  v.insert(v.begin() + base.size(), base.begin(), base.end());

  ASSERT_EQ(v.size(), 3 * base.size());
  EXPECT_EQ(NeedInit::num_init.load(), 3 * base.size());
  for (size_t i = 0; i < base.size(); ++i) {
    EXPECT_EQ(i, v[i].val);
    EXPECT_EQ(i + 3, v[i + base.size()].val);
    EXPECT_EQ(i, v[i + 2 * base.size()].val);
  }
}

TEST(Vector, Erase) {
  NeedInit::num_init = 0;
  GpuVector<NeedInit> v(3);
  v[0].val = 1;
  v[1].val = 2;
  v[2].val = 3;
  v.erase(v.begin() + 1, v.begin() + 2);
  ASSERT_EQ(v.size(), 2);
  EXPECT_EQ(NeedInit::num_init.load(), 2);
  EXPECT_EQ(v[0].val, 1);
  EXPECT_EQ(v[1].val, 3);
}
