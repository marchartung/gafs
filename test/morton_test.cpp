#include "algo/morton.hpp"

#include <gtest/gtest.h>

#include "algo/morton_octree.hpp"
#include "algo/morton_points.hpp"
#include "utils/types.hpp"

TEST(AlgoMorton, Create64) {
  Vector3T<uint32_t> v{12, 13, 14};
  Morton64 m(v);
  auto vb = m.coords();
  EXPECT_EQ(v[0], vb[0]);
  EXPECT_EQ(v[1], vb[1]);
  EXPECT_EQ(v[2], vb[2]);
}

TEST(AlgoMorton, Compare64) {
  Vector3T<uint32_t> v1{0, 0, 0};
  Vector3T<uint32_t> v2{12, 13, 14};
  Vector3T<uint32_t> v3{12, 13, 14};
  Morton64 m1(v1);
  Morton64 m2(v2);
  Morton64 m3(v3);
  EXPECT_TRUE(m1 < m2);
  EXPECT_TRUE(m3 == m2);
}

TEST(AlgoMorton, Create32) {
  Vector3T<uint16_t> v{12, 13, 14};
  Morton32 m(v);
  auto vb = m.coords();
  EXPECT_EQ(v[0], vb[0]);
  EXPECT_EQ(v[1], vb[1]);
  EXPECT_EQ(v[2], vb[2]);
}

TEST(AlgoMorton, Compare32) {
  Vector3T<uint16_t> v1{0, 0, 0};
  Vector3T<uint16_t> v2{12, 13, 14};
  Vector3T<uint16_t> v3{12, 13, 14};
  Morton32 m1(v1);
  Morton32 m2(v2);
  Morton32 m3(v3);
  EXPECT_TRUE(m1 < m2);
  EXPECT_TRUE(m3 == m2);
}

TEST(AlgoMorton, MortonPoints32) {
  using morton_type = Morton32;
  constexpr double e = 1.e-7;
  const std::vector<Pointd> points = {
      Pointd(4., 1., 140.),    Pointd(0.1, 0.2, 0.3),
      Pointd(646., 2., 0.),    Pointd(0.5, 0.4, 0.3),
      Pointd(344., 3., 124.),  Pointd(0.6, 0.7, 0.43545543),
      Pointd(577., 4., 124.),  Pointd(0.6644, 0.34234, 0.5431),
      Pointd(765., 5., 3.),    Pointd(0., 0.67685, 0.354536),
      Pointd(455., 50., 1.),   Pointd(0.574334, 0.7645, 0.75777),
      Pointd(44., 100., 234.), Pointd(0.4444, 0.55555, 0.777777),
      Pointd(1., 1000., 1.),   Pointd(0.324433243423, 0.2243, 0.334)};
  auto [index_map_gpu, mp_gpu] = MortonPoints<morton_type>::Create(1., points);

  ASSERT_EQ(index_map_gpu.size(), points.size());
  ASSERT_EQ(mp_gpu.size(), points.size());

  CpuVector<uint32_t> index_map = index_map_gpu;

  std::vector<bool> once(index_map.size(), false);
  for (size_t i = 0; i < index_map.size(); ++i) {
    ASSERT_FALSE(once[index_map[i]]) << i << " failed";
    once[index_map[i]] = true;
  }

  CpuVector<Pointd> rpoints = mp_gpu.pointds();
  ASSERT_EQ(rpoints.size(), points.size());
  for (size_t i = 0; i < points.size(); ++i) {
    const size_t old_idx = index_map[i];
    ASSERT_NEAR(rpoints[i][0], points[old_idx][0], e)
        << i << " to " << old_idx << " failed";
    ASSERT_NEAR(rpoints[i][1], points[old_idx][1], e)
        << i << " to " << old_idx << " failed";
    ASSERT_NEAR(rpoints[i][2], points[old_idx][2], e)
        << i << " to " << old_idx << " failed";
  }
}

TEST(AlgoMorton, MortonPoints64) {
  using morton_type = Morton64;
  constexpr double e = 1.e-7;
  const std::vector<Pointd> points = {
      Pointd(4., 1., 121240.),     Pointd(0.1, 0.2, 0.3),
      Pointd(65346., 2., 0.),      Pointd(0.5, 0.4, 0.3),
      Pointd(3444., 3., 124.),     Pointd(0.6, 0.7, 0.43545543),
      Pointd(432577., 4., 12424.), Pointd(0.6644, 0.34234, 0.5431),
      Pointd(756665., 5., 3.),     Pointd(0., 0.67685, 0.354536),
      Pointd(45665., 50., 1.),     Pointd(0.574334, 0.7645, 0.75777),
      Pointd(44., 100., 234.),     Pointd(0.4444, 0.55555, 0.777777),
      Pointd(1., 10000., 1.),      Pointd(0.324433243423, 0.2243, 0.334)};
  auto [index_map_gpu, mp_gpu] = MortonPoints<morton_type>::Create(1., points);

  ASSERT_EQ(index_map_gpu.size(), points.size());
  ASSERT_EQ(mp_gpu.size(), points.size());

  CpuVector<uint32_t> index_map = index_map_gpu;

  std::vector<bool> once(index_map.size(), false);
  for (size_t i = 0; i < index_map.size(); ++i) {
    ASSERT_FALSE(once[index_map[i]]) << i << " failed";
    once[index_map[i]] = true;
  }

  CpuVector<Pointd> rpoints = mp_gpu.pointds();
  ASSERT_EQ(rpoints.size(), points.size());
  for (size_t i = 0; i < points.size(); ++i) {
    const size_t old_idx = index_map[i];
    ASSERT_NEAR(rpoints[i][0], points[old_idx][0], e)
        << i << " to " << old_idx << " failed";
    ASSERT_NEAR(rpoints[i][1], points[old_idx][1], e)
        << i << " to " << old_idx << " failed";
    ASSERT_NEAR(rpoints[i][2], points[old_idx][2], e)
        << i << " to " << old_idx << " failed";
  }
}

TEST(MortonOctree, test_create32) {
  using morton_type = Morton32;
  const double cell_size = 1.;
  const std::vector<Pointd> init_points = {
      Pointd(2., 1., 1.),    Pointd(1., 1., 1.),
      Pointd(1., 1, 2),      Pointd(1., 2., 1.),
      Pointd(1, 2, 2),       Pointd(2, 1, 1.),
      Pointd(2., 1., 2),     Pointd(0., 0., 0.),
      Pointd(1., 0., 0.),    Pointd(1., 1, 0),
      Pointd(1., 0., 1.),    Pointd(1, 1, 1),
      Pointd(0, 1, 1.),      Pointd(2., 1., 2),
      Pointd(4., 1., 120.),  Pointd(0.1, 0.2, 0.3),
      Pointd(646., 2., 0.),  Pointd(0.5, 0.4, 0.3),
      Pointd(34., 3., 124.), Pointd(0.6, 0.7, 0.43545543),
      Pointd(47., 4., 424.), Pointd(0.6644, 0.34234, 0.5431),
      Pointd(65., 5., 3.),   Pointd(0., 0.67685, 0.354536),
      Pointd(456., 50., 1.), Pointd(0.574334, 0.7645, 0.75777),
      Pointd(44., 10., 34.), Pointd(0.4444, 0.55555, 0.777777),
      Pointd(1., 10., 1.),   Pointd(0.324433243423, 0.2243, 0.334)};

  const auto [index_map, mp] =
      MortonPoints<morton_type>::Create(cell_size, init_points);
  const CpuVector<Pointd> points = mp.pointds();

  MortonOctree<morton_type> oct(mp.cell_mortons());
  for (const Pointd& p : points) {
    auto c = Cast<morton_type::Coord>(p / cell_size);
    morton_type m(c);
    const size_t cid = oct[m];
    ASSERT_LT(cid, mp.num_cells()) << c << " not found";
    const auto bm = mp.cell_mortons()[cid];
    ASSERT_EQ(bm.value(), m.value());
    const auto bc = mp.cell_mortons()[cid].coords();
    ASSERT_EQ(c[0], bc[0]);
    ASSERT_EQ(c[1], bc[1]);
    ASSERT_EQ(c[2], bc[2]);
  }
}

TEST(MortonOctree, test_create64) {
  using morton_type = Morton64;
  const double cell_size = 1.;
  const CpuVector<Pointd> init_points = {
      Pointd(2., 1., 1.),    Pointd(1., 1., 1.),
      Pointd(1., 1, 2),      Pointd(1., 2., 1.),
      Pointd(1, 2, 2),       Pointd(2, 1, 1.),
      Pointd(2., 1., 2),     Pointd(0., 0., 0.),
      Pointd(1., 0., 0.),    Pointd(1., 1, 0),
      Pointd(1., 0., 1.),    Pointd(1, 1, 1),
      Pointd(0, 1, 1.),      Pointd(2., 1., 2),
      Pointd(4., 1., 120.),  Pointd(0.1, 0.2, 0.3),
      Pointd(646., 2., 0.),  Pointd(0.5, 0.4, 0.3),
      Pointd(34., 3., 124.), Pointd(0.6, 0.7, 0.43545543),
      Pointd(47., 4., 424.), Pointd(0.6644, 0.34234, 0.5431),
      Pointd(65., 5., 3.),   Pointd(0., 0.67685, 0.354536),
      Pointd(456., 50., 1.), Pointd(0.574334, 0.7645, 0.75777),
      Pointd(44., 10., 34.), Pointd(0.4444, 0.55555, 0.777777),
      Pointd(1., 10., 1.),   Pointd(0.324433243423, 0.2243, 0.334)};

  const auto [index_map, mp] =
      MortonPoints<morton_type>::Create(cell_size, init_points);
  const CpuVector<Pointd> points = mp.pointds();

  MortonOctree<morton_type> oct(mp.cell_mortons());
  for (const Pointd& p : points) {
    auto c = Cast<morton_type::Coord>(p / cell_size);
    morton_type m(c);
    const size_t cid = oct[m];
    ASSERT_EQ(
        cid, MortonOctree<morton_type>::At(m, oct.nodes().data(), oct.depth()));
    ASSERT_LT(cid, mp.num_cells()) << c << " not found";
    const auto bm = mp.cell_mortons()[cid];
    ASSERT_EQ(bm.value(), m.value());
    const auto bc = mp.cell_mortons()[cid].coords();
    ASSERT_EQ(c[0], bc[0]);
    ASSERT_EQ(c[1], bc[1]);
    ASSERT_EQ(c[2], bc[2]);
  }
}