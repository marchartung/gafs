#include "neighbor/point_cell_list.hpp"

#include <gtest/gtest.h>

std::vector<Vectord> TestPoints(const double dr = 0.1) {
  const Vectord off(-dr * 4.), d1(1.e-10, -1.e-10, dr), d2(-1.e-10, dr, 1.e-10);
  std::vector<Vectord> res;
  for (size_t i = 0; i < 91; ++i)
    for (size_t j = 0; j < 87; ++j) {
      res.push_back(off + i * d1 + j * d2);
    }
  return res;
}

TEST(PointCellList, CreateEmpty) {
  std::vector<Vectord> points;
  auto [idx_map, point_cells] = PointCellListD::Create(0.1, points);
  EXPECT_EQ(idx_map.size(), 0);
  EXPECT_EQ(point_cells.size(), 0);
  EXPECT_EQ(point_cells.num_points(), 0);
  EXPECT_EQ(point_cells.num_cells(), 0);
}

TEST(PointCellList, Create) {
  const double dr = 0.1;
  std::vector<Vectord> points = TestPoints(dr);
  auto [idx_map, point_cells] = PointCellListD::Create(dr, points);
  EXPECT_EQ(idx_map.size(), points.size());
  EXPECT_EQ(point_cells.size(), points.size());
  EXPECT_EQ(point_cells.num_points(), points.size());
  EXPECT_GT(point_cells.num_cells(), 0);

  for (size_t i = 0; i < points.size(); ++i) {
    const Vectord o = points[idx_map[i]];
    const Vectord m = point_cells[i];
    ASSERT_EQ(o[0], m[0]);
    ASSERT_EQ(o[1], m[1]);
    ASSERT_EQ(o[2], m[2]);

    const auto c1 = point_cells.point_coords(i);
    const SizeT cell_id = point_cells.cell_id(c1);
    const auto c2 = point_cells.cell_coords(cell_id);
    ASSERT_EQ(c1[0], c2[0]);
    ASSERT_EQ(c1[1], c2[1]);
    ASSERT_EQ(c1[2], c2[2]);
  }
}