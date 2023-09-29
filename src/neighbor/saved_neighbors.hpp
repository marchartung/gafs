#pragma once

#include "point_cell_list.hpp"
#include "utils/math.hpp"
#include "utils/span.hpp"
#include "utils/types.hpp"

class SavedNeighborsD {
 public:
  SavedNeighborsD() = default;

  SavedNeighborsD(const PointCellListD& point_list) {
    RecomputeNeighbors(point_list);
  }

  Span<const SizeT> neighbors(const SizeT idx) const {
    return Span<const SizeT>(neighbors_[idx].data(), num_active_[idx]);
  }

  void Update(const PointCellListD& point_list) {
    if (point_list.UpdateVersion() != point_list_update_version_) {
      RecomputeNeighbors(point_list);
    } else {
      SetActiveNeighbors(point_list);
    }
  }

 private:
  void SetActiveNeighbors(const PointCellListD& point_list) {
    num_active_.resize(point_list.size());
    const double dist2 = math::tpow<2>(point_list.cell_size());
#pragma omp parallel for schedule(guided)
    for (SizeT i = 0; i < point_list.size(); ++i) {
      SizeT j = 0, end = neighbors_[i].size();
      while (j != end) {
        if (math::tpow<2>(point_list[i] - point_list[neighbors_[i][j]]) >=
            dist2) {
          --end;
          std::swap(neighbors_[i][j], neighbors_[i][end]);
        } else {
          ++j;
        }
      }
      num_active_[i] = end;
    }
  }

  void RecomputeNeighbors(const PointCellListD& point_list) {
    neighbors_.resize(point_list.size());
#pragma omp parallel for schedule(static)
    for (SizeT i = 0; i < point_list.size(); ++i) {
      neighbors_[i].clear();
    }
    const double dist2 =
        math::tpow<2>(point_list.cell_size() * point_list.cell_factor());
#pragma omp parallel for schedule(guided)
    for (SizeT ci = 0; ci < point_list.num_cells(); ++ci) {
      SizeT nncells = 0;
      std::array<SizeT, 27> cell_ids_;
      const Coords own_coords = point_list.cell_coords(ci);
      for (const Coords d : Coords::NeighborCoords()) {
        const SizeT nci = point_list.cell_id(own_coords + d);
        if (nci != PointCellListD::InvalidCellId()) {
          cell_ids_[nncells++] = nci;
        }
      }
      const SizeT own_start = point_list.cell_start(ci),
                  own_end = point_list.cell_end(ci);
      for (SizeT* cur_cell = cell_ids_.data();
           cur_cell != cell_ids_.data() + nncells; ++cur_cell) {
        for (size_t npi = point_list.cell_start(*cur_cell);
             npi < point_list.cell_end(*cur_cell); ++npi) {
          const Vectord neigh_pos = point_list[npi];
          for (size_t pi = own_start; pi < own_end; ++pi) {
            if (pi != npi &&
                math::tpow<2>(point_list[pi] - neigh_pos) < dist2) {
              neighbors_[pi].push_back(npi);
            }
          }
        }
      }
    }

    SetActiveNeighbors(point_list);
    point_list_update_version_ = point_list.UpdateVersion();
  }

  size_t point_list_update_version_ = 0;
  std::vector<std::vector<SizeT>> neighbors_;
  std::vector<SizeT> num_active_;
};