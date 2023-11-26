
#include "neighbor/saved_neighbors.hpp"

#include <gtest/gtest.h>

#include "preprocess/point_shapes.hpp"

TEST(SavedNeighbors, CellSelf) {
  const double cell_size = 0.1213;
  PointCellListD cell_list = std::get<1>(PointCellListD::Create(
      cell_size, PointDiscretize::Ellipsoid(cell_size / 2.4, 10. * cell_size,
                                            Vectord(0.))));

  SavedNeighborsD saved(cell_list);
  for (size_t i = 0; i < cell_list.size(); ++i) {
    const Vectord p = cell_list[i];
    for (size_t j = 0; j < cell_list.size(); ++j) {
      if (i == j) continue;
      const Vectord p2 = cell_list[j];
      if (Distance(p, p2) < cell_size) {
        ASSERT_TRUE(std::find(saved.neighbors(i).begin(),
                              saved.neighbors(i).end(),
                              j) != saved.neighbors(i).end())
            << "failed for " << i << " and  " << j;
      }
    }
    for (const auto j : saved.neighbors(i)) {
      ASSERT_LT(Distance(p, cell_list[j]), cell_size);
    }
  }
}