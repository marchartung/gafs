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

#include "parstd/parstd.hpp"
#include "utils/types.hpp"

class PositionTracker {
 public:
  PositionTracker() = default;
  PositionTracker(const double max_dist, const SizeT num_elements)
      : max_dist_(max_dist), dists_(num_elements, 0.) {}

  bool MovedToFar() const {
    return max_dist_ == 0. ||
           Reduce(dists_, 0., [](const double acc, const double d) {
             return std::max(acc, d);
           });
  }
  void Reset(const SizeT num_to_track) {
    // ForEachIndex(dists_.size(),[])
  }

 private:
  double max_dist_ = 0.;
  GpuVector<double> dists_;
};