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