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

#pragma once

#include <atomic>
#include <optional>
#include <tuple>
#include <vector>

#include "algo/morton_octree.hpp"
#include "coords.hpp"
#include "parstd/parstd.hpp"
#include "utils/types.hpp"

template <typename T>
std::vector<T> ApplyIndexMap(const std::vector<SizeT>& idx_map,
                             std::vector<T> v) {
  std::vector<T> res(idx_map.size());
#pragma omp for schedule(static)
  for (size_t i = 0; i < idx_map.size(); ++i) {
    res[i] = v[idx_map[i]];
  }
  return res;
}

class PointCellListD {
  using OctreeType = MortonOctree<Morton64>;

  static Coords GetCellListOffset(const double cell_size,
                                  const std::vector<Vectord>& points) {
    const Vectord min_p =
        Reduce(points, Vectord(std::numeric_limits<double>::max()),
               [](const Vectord a, const Vectord b) { return Min(a, b); });
    return Coords(cell_size, -min_p) + 1;
  }

 public:
  static std::tuple<std::vector<SizeT>, PointCellListD> Create(
      const double cell_size, const std::vector<Vectord>& points) {
    if (points.size() == 0)
      return std::tuple<std::vector<SizeT>, PointCellListD>();

    const Coords offset = GetCellListOffset(cell_size, points);
    std::vector<MortIdx<Morton64>> mort_ids(points.size());
#pragma omp for schedule(static)
    for (SizeT i = 0; i < points.size(); ++i) {
      mort_ids[i] = {Morton64(Coords(cell_size, points[i]) + offset), i};
    }
    Sort(mort_ids);
    std::vector<SizeT> index_map(points.size());
    std::vector<Vectord> sorted_points(points.size());
#pragma omp for schedule(static)
    for (SizeT i = 0; i < sorted_points.size(); ++i) {
      sorted_points[i] = points[mort_ids[i].idx];
      index_map[i] = mort_ids[i].idx;
      mort_ids[i].idx = i;
    }
    Unique(mort_ids);
    std::vector<SizeT> cell_starts(mort_ids.size() + 1);

    std::vector<Morton64> mortons(mort_ids.size());
#pragma omp for schedule(static)
    for (SizeT i = 0; i < mort_ids.size(); ++i) {
      cell_starts[i] = mort_ids[i].idx;
      mortons[i] = mort_ids[i].morton;
    }
    cell_starts.back() = points.size();
    OctreeType octree(std::move(mortons));

    PointCellListD cell_list(cell_size, offset, std::move(sorted_points),
                             std::move(cell_starts), std::move(octree));

    return std::make_tuple(std::move(index_map), std::move(cell_list));
  }

  static constexpr SizeT InvalidCellId() { return OctreeType::Invalid(); }

  PointCellListD() = default;

  std::vector<SizeT> Update() {
    std::vector<SizeT> res;
    std::tie(res, *this) = Create(cell_size_, std::move(points_));
    return res;
  }

  double cell_size() const { return cell_size_; }

  const Vectord& point(const SizeT point_id) const { return points_[point_id]; }
  Vectord& point(const SizeT point_id) { return points_[point_id]; }
  SizeT num_points() const { return points_.size(); }

  const Vectord& operator[](const SizeT point_id) const {
    return point(point_id);
  }
  Vectord& operator[](const SizeT point_id) { return point(point_id); }

  operator const std::vector<Vectord>&() const { return points_; }

  SizeT size() const { return num_points(); }

  SizeT num_cells() const {
    return cell_starts_.size() + ((cell_starts_.empty()) ? 0 : -1);
  }

  SizeT cell_id(const Coords c) const { return octree_[Morton64(c + offset_)]; }

  Coords point_coords(const SizeT point_id) const {
    return Coords(cell_size_, points_[point_id]);
  }

  Coords cell_coords(const SizeT cell_id) const {
    return point_coords(cell_start(cell_id));
  }

  const SizeT cell_start(const SizeT cell_id) const {
    return cell_starts_[cell_id];
  }
  const SizeT cell_end(const SizeT cell_id) const {
    return cell_starts_[cell_id + 1];
  }

 private:
  PointCellListD(const double cell_size, const Coords offset,
                 std::vector<Vectord> points, std::vector<SizeT> cell_starts,
                 OctreeType octree)
      : cell_size_(cell_size),
        offset_(offset),
        points_(std::move(points)),
        cell_starts_(std::move(cell_starts)),
        octree_(std::move(octree)) {}

  double cell_size_ = std::numeric_limits<double>::max();

  Coords offset_ = Coords(0);
  std::vector<Vectord> points_;
  std::vector<SizeT> cell_starts_;
  OctreeType octree_;
};