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

#include "materials.hpp"
#include "utils/types.hpp"

class DynamicBoundary {
 public:
  DynamicBoundary() = default;
  DynamicBoundary(const MaterialSettings& s, std::vector<Vectord> pos)
      : ref_density_(s.ref_density),
        speed_of_sound_(s.speed_of_sound),
        h_(s.dr * s.smoothing_ratio),
        mass_(std::pow(s.dr, 3) * ref_density_),
        pos_(std::move(pos)),
        dty_(size(), s.ref_density),
        vel_(0.) {}

  size_t size() const { return pos_.size(); }

  const std::vector<Vectord>& pos() const { return pos_; }
  const Vectord& pos(const size_t idx) const { return pos_[idx]; }
  Vectord& pos(const size_t idx) { return pos_[idx]; }

  const std::vector<double>& dty() const { return dty_; }
  const double& dty(const size_t idx) const { return dty_[idx]; }
  double& dty(const size_t idx) { return dty_[idx]; }

  const Vectord& vel(const size_t idx) const { return vel_; }

  double ref_density() const { return ref_density_; }
  double sos() const { return speed_of_sound_; }
  double h() const { return h_; }
  double mass() const { return mass_; }

 private:
  double ref_density_;
  double speed_of_sound_;
  double h_;
  double mass_;

  std::vector<Vectord> pos_;
  std::vector<double> dty_;

  Vectord vel_;
};