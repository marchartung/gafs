#pragma once

#include <vector>

#include "basic_equations.hpp"
#include "materials.hpp"
#include "utils/types.hpp"

class Particles {
 public:
  Particles() = default;
  Particles(const MaterialSettings& s, std::vector<Pointd> pos)
      : ref_density_(s.ref_density),
        speed_of_sound_(s.speed_of_sound),
        pressure_parameter_(
            ComputePressureParameter(ref_density_, speed_of_sound_)),
        mass_(std::pow(s.dr, 3) * ref_density_),
        h_(s.dr * s.smoothing_ratio),
        pos_(std::move(pos)),
        vel_(size()),
        dty_(size(), s.ref_density) {}

  size_t size() const { return pos_.size(); }

  const std::vector<Pointd>& pos() const { return pos_; }
  const Pointd& pos(const size_t idx) const { return pos_[idx]; }
  Pointd& pos(const size_t idx) { return pos_[idx]; }

  const std::vector<Pointd>& vel() const { return vel_; }
  const Pointd& vel(const size_t idx) const { return vel_[idx]; }
  Pointd& vel(const size_t idx) { return vel_[idx]; }

  const std::vector<double>& dty() const { return dty_; }
  const double& dty(const size_t idx) const { return dty_[idx]; }
  double& dty(const size_t idx) { return dty_[idx]; }

  double pressure_parameter() const { return pressure_parameter_; }
  double ref_density() const { return ref_density_; }
  double mass() const { return mass_; }
  double h() const { return h_; }
  double sos() const { return speed_of_sound_; }

 private:
  double ref_density_;
  double speed_of_sound_;
  double pressure_parameter_;
  double mass_;
  double h_;

  std::vector<Pointd> pos_;
  std::vector<Pointd> vel_;
  std::vector<double> dty_;
};