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

#include <stdexcept>
#include <vector>

#include "basic_equations.hpp"
#include "materials.hpp"
#include "neighbor/point_cell_list.hpp"
#include "utils/types.hpp"

class Particles {
 public:
  Particles() = default;

  Particles(const MaterialSettings& s, std::vector<Vectord> pos,
            std::vector<Vectord> vel, std::vector<double> dty) {
    if (pos.size() != vel.size() || pos.size() != dty.size()) {
      std::runtime_error("Particles init vectors have not the same size");
    }
    const double cell_size = 2. * s.dr * s.smoothing_ratio;
    auto [idx_map, points] = PointCellListD::Create(cell_size, std::move(pos));
    vel = ApplyIndexMap(idx_map, std::move(vel));
    dty = ApplyIndexMap(idx_map, std::move(dty));
    *this = Particles(s, std::move(points), std::move(vel), std::move(dty));
    idx_map_ = std::move(idx_map);
  }

  Particles(const MaterialSettings& s, std::vector<Vectord> pos,
            std::vector<Vectord> vel) {
    std::vector<double> dtyinit(pos.size(), s.ref_density);
    *this = Particles(s, std::move(pos), std::move(vel), std::move(dtyinit));
  }

  void Update() {
    const auto idx_map = pos_.Update();
    vel_ = ApplyIndexMap(idx_map, std::move(vel_));
    dty_ = ApplyIndexMap(idx_map, std::move(dty_));
  }

  SizeT size() const { return pos_.size(); }

  const PointCellListD& pos() const { return pos_; }
  const Vectord& pos(const SizeT idx) const { return pos_[idx]; }
  Vectord& pos(const SizeT idx) { return pos_[idx]; }

  const std::vector<Vectord>& vel() const { return vel_; }
  std::vector<Vectord>& vel() { return vel_; }
  const Vectord& vel(const SizeT idx) const { return vel_[idx]; }
  Vectord& vel(const SizeT idx) { return vel_[idx]; }

  const std::vector<double>& dty() const { return dty_; }
  const double& dty(const SizeT idx) const { return dty_[idx]; }
  double& dty(const SizeT idx) { return dty_[idx]; }

  const std::vector<double>& prs() const { return prs_; }
  const double& prs(const SizeT idx) const { return prs_[idx]; }
  double& prs(const SizeT idx) { return prs_[idx]; }

  double pressure_parameter() const { return pressure_parameter_; }
  double ref_density() const { return ref_density_; }
  double mass() const { return mass_; }
  double& mass() { return mass_; }
  double h() const { return h_; }
  double dr() const { return dr_; }
  double sos() const { return speed_of_sound_; }
  double viscosity() const { return 0.01; }

  const std::vector<SizeT>& idx_map() const { return idx_map_; }

 private:
  Particles(const MaterialSettings& s, PointCellListD pos,
            std::vector<Vectord> vel, std::vector<double> dty)
      : ref_density_(s.ref_density),
        speed_of_sound_(s.speed_of_sound),
        pressure_parameter_(
            ComputePressureParameter(ref_density_, speed_of_sound_)),
        mass_(std::pow(s.dr, 3) * ref_density_),
        h_(s.dr * s.smoothing_ratio),
        dr_(s.dr),
        pos_(std::move(pos)),
        vel_(std::move(vel)),
        dty_(std::move(dty)),
        prs_(ComputePressure(dty_, ref_density_, pressure_parameter_)) {}

  double ref_density_;
  double speed_of_sound_;
  double pressure_parameter_;
  double mass_;
  double h_;
  double dr_;

  PointCellListD pos_;
  std::vector<Vectord> vel_;
  std::vector<double> dty_;
  std::vector<double> prs_;

  std::vector<SizeT> idx_map_;
};