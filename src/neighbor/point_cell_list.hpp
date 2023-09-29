#pragma once

#include <atomic>
#include <optional>
#include <tuple>
#include <vector>

#include "algo/basic_algorithms.hpp"
#include "algo/morton_octree.hpp"
#include "coords.hpp"
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

 public:
  static std::tuple<std::vector<SizeT>, PointCellListD> Create(
      const double in_cell_size, const std::vector<Vectord>& points) {
    const double cell_size = cell_factor_ * in_cell_size;
    std::vector<MortIdx<Morton64>> mort_ids(points.size());
#pragma omp for schedule(static)
    for (SizeT i = 0; i < points.size(); ++i) {
      mort_ids[i] = {Morton64(Coords(cell_size, points[i])), i};
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

    PointCellListD cell_list(in_cell_size, std::move(sorted_points),
                             std::move(cell_starts), std::move(octree));

    return std::make_tuple(std::move(index_map), std::move(cell_list));
  }

  static constexpr SizeT InvalidCellId() { return OctreeType::Invalid(); }

  PointCellListD() = default;

  bool NeedsUpdate() const {
    double max_d = 0.;
#pragma omp parallel for schedule(static) reduction(max : max_d)
    for (SizeT i = 0; i < size(); ++i) {
      max_d = std::max(max_d, moved_since_last_update_[i]);
    }
    const double cd = 0.5 * cell_size_ * (cell_factor_ - 1.);
    return max_d >= cd;
  }

  size_t UpdateVersion() const { return times_updated_; }

  std::optional<std::vector<SizeT>> Update() {
    if (NeedsUpdate()) {
      const size_t tmp_times_updated = times_updated_;
      std::vector<SizeT> res;
      std::tie(res, *this) = Create(cell_size_, std::move(points_));
      times_updated_ = tmp_times_updated + 1;
      return res;
    }
    return std::optional<std::vector<SizeT>>();
  }

  void SetPos(const SizeT idx, const Vectord new_pos) {
    moved_since_last_update_[idx] += Length(points_[idx] - new_pos);
    points_[idx] = new_pos;
  }

  double cell_size() const { return cell_size_; }
  constexpr double cell_factor() const { return cell_factor_; }

  const Vectord& point(const SizeT idx) const { return points_[idx]; }
  Vectord& point(const SizeT idx) { return points_[idx]; }
  SizeT num_points() const { return points_.size(); }

  const Vectord& operator[](const SizeT idx) const { return point(idx); }
  Vectord& operator[](const SizeT idx) { return point(idx); }

  operator const std::vector<Vectord>&() const { return points_; }

  SizeT size() const { return num_points(); }

  SizeT num_cells() const { return cell_starts_.size() - 1; }

  SizeT cell_id(const Coords c) const { return octree_[Morton64(c)]; }

  Coords cell_coords(const SizeT idx) const {
    return Coords(cell_size_, points_[cell_start(idx)]);
  }

  const SizeT cell_start(const SizeT idx) const { return cell_starts_[idx]; }
  const SizeT cell_end(const SizeT idx) const { return cell_starts_[idx + 1]; }

 private:
  PointCellListD(const double cell_size, std::vector<Vectord> points,
                 std::vector<SizeT> cell_starts, OctreeType octree)
      : cell_size_(cell_size),
        points_(std::move(points)),
        cell_starts_(std::move(cell_starts)),
        octree_(std::move(octree)),
        moved_since_last_update_(points_.size(), 0.) {}

  static constexpr double cell_factor_ = 1.2;

  double cell_size_ = std::numeric_limits<double>::max();

  std::vector<Vectord> points_;
  std::vector<SizeT> cell_starts_;
  OctreeType octree_;

  size_t times_updated_ = 0;
  std::vector<double> moved_since_last_update_;
};