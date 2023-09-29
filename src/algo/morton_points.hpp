#pragma once

#include <tuple>
#include <vector>

#include "morton.hpp"
#include "utils/macros.hpp"

template <typename morton_type = Morton64>
class MortonPoints {
 public:
  using mort_id_t = MortIdx<morton_type>;
  using size_type = typename mort_id_t::size_type;
  using Coords = typename morton_type::Coords;
  using Range = std::tuple<size_type, size_type>;

  static std::tuple<GpuVector<size_type>, MortonPoints> Create(
      const double cell_size, const GpuVector<Vectord>& points);

  MortonPoints() = default;

  size_type num_cells() const { return cell_mortons_.size(); }
  size_type num_points() const { return points_.size(); }
  size_t size() const { return num_points(); }

  const GpuVector<morton_type>& cell_mortons() const { return cell_mortons_; }
  GpuVector<morton_type>& cell_mortons() { return cell_mortons_; }

  const GpuVector<size_type>& cell_point_starts() const { return cell_starts_; }

  const GpuVector<Vectorf>& Vectorfs() const { return points_; }
  GpuVector<Vectorf>& Vectorfs() { return points_; }

  GpuVector<Vectord> Vectords() const;

 private:
  MortonPoints(const double cell_size, GpuVector<Vectorf> points,
               GpuVector<morton_type> cell_mortons,
               GpuVector<size_type> cell_starts);

  double cell_size_;
  GpuVector<Vectorf> points_;
  GpuVector<morton_type> cell_mortons_;
  GpuVector<size_type> cell_starts_;
};
