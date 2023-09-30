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