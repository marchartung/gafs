#pragma once

#include <array>
#include <vector>

#include "utils/types.hpp"

using Segment = std::array<Vectord, 3>;

class Mesh {
 public:
  Mesh() = default;
  Mesh(std::vector<Vectord> points, std::vector<std::array<size_t, 3>> segments)
      : points_(std::move(points)), segments_(std::move(segments)) {}

  size_t size() const { return segments_.size(); }

  Segment operator[](const size_t idx) const {
    const auto s = segments_[idx];
    return {points_[s[0]], points_[s[1]], points_[s[2]]};
  }

 private:
  std::vector<Vectord> points_;
  std::vector<std::array<size_t, 3>> segments_;
};