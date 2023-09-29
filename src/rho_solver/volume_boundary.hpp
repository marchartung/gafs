#pragma once

#include <vector>

#include "basic_equations.hpp"
#include "utils/types.hpp"

class VolumeBoundary {
 public:
  VolumeBoundary() = default;
  VolumeBoundary(const double h, std::vector<Vectord> pos)
      : pos_(std::move(pos)), volume_(pos_.size()) {
    ComputeWallVolume(h);
  }

  void ComputeWallVolume(const double h) {
    for (size_t i = 0; i < pos_.size(); ++i) {
      double kernel_sum = 0;
      for (size_t j = 0; j < pos_.size(); ++j) {
        if (i == j) continue;
        const double dist = Distance(pos_[i], pos_[j]);
        if (dist < 2. * h) {
          kernel_sum += Wendland(dist, h);
        }
      }
      volume_[i] = 1. / kernel_sum;
    }
  }

  size_t size() const { return pos_.size(); }

  const std::vector<Vectord>& pos() const { return pos_; }
  const Vectord& pos(const size_t idx) const { return pos_[idx]; }

  const std::vector<double>& vol() const { return volume_; }
  double vol(const size_t idx) const { return volume_[idx]; }
  double& vol(const size_t idx) { return volume_[idx]; }

 private:
  std::vector<Vectord> pos_;
  std::vector<double> volume_;
};