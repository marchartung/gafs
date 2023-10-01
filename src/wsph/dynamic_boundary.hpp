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

#include "neighbor/point_cell_list.hpp"
#include "utils/types.hpp"

class DynamicBoundary {
 public:
  DynamicBoundary() = default;
  DynamicBoundary(const double interaction_radius, const double mass,
                  const double dr, std::vector<Vectord> pos)
      : dr_(dr),
        mass_(mass),
        pos_(std::get<1>(
            PointCellListD::Create(interaction_radius, std::move(pos)))) {}

  size_t size() const { return pos_.size(); }

  const PointCellListD& pos() const { return pos_; }
  const Vectord& pos(const SizeT idx) const { return pos_[idx]; }
  void SetPos(const SizeT idx, const Vectord& new_pos) {
    pos_.SetPos(idx, new_pos);
  }

  double dr() const { return dr_; }
  double mass() const { return mass_; }

 private:
  double dr_;
  double mass_;

  PointCellListD pos_;
};