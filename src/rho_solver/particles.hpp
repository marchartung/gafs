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
  }

  void Update() {
    const auto idx_map = pos_.Update();
    if (idx_map.has_value()) {
      vel_ = ApplyIndexMap(idx_map.value(), std::move(vel_));
      dty_ = ApplyIndexMap(idx_map.value(), std::move(dty_));
    }
  }

  size_t UpdateVersion() const { return pos_.UpdateVersion(); }

  SizeT size() const { return pos_.size(); }

  const PointCellListD& pos() const { return pos_; }
  const Vectord& pos(const SizeT idx) const { return pos_[idx]; }
  void SetPos(const SizeT idx, const Vectord& new_pos) {
    pos_.SetPos(idx, new_pos);
  }

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
  double h() const { return h_; }
  double dr() const { return dr_; }
  double sos() const { return speed_of_sound_; }
  double viscosity() const { return 0.01; }

 private:
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
};