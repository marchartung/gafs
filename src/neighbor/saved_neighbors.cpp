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

#include "saved_neighbors.hpp"

template <bool IsSameList>
void SavedNeighborsD::RecomputeNeighbors(const PointCellListD& src_list,
                                         const PointCellListD& trg_list) {
  if (src_list.cell_size() != trg_list.cell_size()) {
    throw std::runtime_error(
        "SavedNeighborsD: Cell lists have different sizes");
  }
  neighbors_.resize(src_list.size());
  if (trg_list.size() > 0 && src_list.size() > 0) {
#pragma omp parallel for schedule(static)
    for (SizeT i = 0; i < src_list.size(); ++i) {
      neighbors_[i].clear();
    }
    const double dist2 =
        math::tpow<2>(src_list.cell_size() * src_list.cell_factor());
#pragma omp parallel for schedule(guided)
    for (SizeT ci = 0; ci < src_list.num_cells(); ++ci) {
      SizeT nncells = 0;
      std::array<SizeT, 27> cell_ids_;
      const Coords own_coords = src_list.cell_coords(ci);
      for (const Coords d : Coords::NeighborCoords()) {
        const SizeT nci = trg_list.cell_id(own_coords + d);
        if (nci != PointCellListD::InvalidCellId()) {
          cell_ids_[nncells++] = nci;
        }
      }
      const SizeT own_start = src_list.cell_start(ci),
                  own_end = src_list.cell_end(ci);
      for (SizeT* cur_cell = cell_ids_.data();
           cur_cell != cell_ids_.data() + nncells; ++cur_cell) {
        for (size_t npi = trg_list.cell_start(*cur_cell);
             npi < trg_list.cell_end(*cur_cell); ++npi) {
          const Vectord neigh_pos = trg_list[npi];
          for (size_t pi = own_start; pi < own_end; ++pi) {
            if (IsSameList && pi == npi) continue;
            if (math::tpow<2>(src_list[pi] - neigh_pos) < dist2) {
              neighbors_[pi].push_back(npi);
            }
          }
        }
      }
    }
  }
  SetActiveNeighbors(src_list, trg_list);
  src_update_version_ = src_list.UpdateVersion();
  trg_update_version_ = trg_list.UpdateVersion();
}
template void SavedNeighborsD::RecomputeNeighbors<true>(const PointCellListD&,
                                                        const PointCellListD&);
template void SavedNeighborsD::RecomputeNeighbors<false>(const PointCellListD&,
                                                         const PointCellListD&);

void SavedNeighborsD::Update(const PointCellListD& point_list) {
  if (point_list.UpdateVersion() != src_update_version_) {
    RecomputeNeighbors<true>(point_list, point_list);
  } else {
    SetActiveNeighbors(point_list, point_list);
  }
}

void SavedNeighborsD::Update(const PointCellListD& src_list,
                             const PointCellListD& trg_list) {
  if (src_list.UpdateVersion() != src_update_version_ ||
      trg_list.UpdateVersion() != trg_update_version_) {
    RecomputeNeighbors<false>(src_list, trg_list);
  } else {
    SetActiveNeighbors(src_list, trg_list);
  }
}

void SavedNeighborsD::SetActiveNeighbors(const PointCellListD& src_list,
                                         const PointCellListD& trg_list) {
  num_active_.resize(src_list.size());
  const double dist2 = math::tpow<2>(src_list.cell_size());
#pragma omp parallel for schedule(guided)
  for (SizeT i = 0; i < src_list.size(); ++i) {
    SizeT j = 0, end = neighbors_[i].size();
    while (j != end) {
      if (math::tpow<2>(src_list[i] - trg_list[neighbors_[i][j]]) >= dist2) {
        --end;
        std::swap(neighbors_[i][j], neighbors_[i][end]);
      } else {
        ++j;
      }
    }
    num_active_[i] = end;
  }
}