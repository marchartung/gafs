// MIT License

// Copyright (c) 2023 Marc Hartung

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "verlet_neighbors.hpp"

#include <algorithm>
#include <iterator>

void VerletNeighborsD::Update(const bool recompute_saved_neighbors,
                              const PointCellListD& point_list) {
  if (recompute_saved_neighbors) {
    saved_.Update(point_list);
  } else {
    SetActiveNeighbors(point_list, point_list);
  }
}

void VerletNeighborsD::Update(const bool recompute_saved_neighbors,
                              const PointCellListD& src_list,
                              const PointCellListD& trg_list) {
  if (recompute_saved_neighbors) {
    saved_.Update(src_list, trg_list);
  } else {
    SetActiveNeighbors(src_list, trg_list);
  }
}

void VerletNeighborsD::SetActiveNeighbors(const PointCellListD& src_list,
                                          const PointCellListD& trg_list) {
  num_active_.resize(src_list.size());
  const double dist2 = math::tpow<2>(src_list.cell_size());
#pragma omp parallel for schedule(guided)
  for (SizeT i = 0; i < saved_.size(); ++i) {
    const Vectord p = src_list[i];
    auto rg = saved_[i];
    const auto it_end = std::partition(
        rg.begin(), rg.end(), [p, dist2, &trg_list](const SizeT ni) {
          return Distance(p, trg_list[ni]) <= dist2;
        });
    num_active_[i] = std::distance(rg.begin(), it_end);
  }
}