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

#include "basic_equations.hpp"
#include "domain.hpp"

// From Jandaghian et al, 2021: Stability and accuracy of the
// weakly compressible SPH with particle regularization techniques

class DpcShifting {
 public:
  DpcShifting() = default;
  DpcShifting(const double prs_min, const double prs_max)
      : prs_min_(prs_min), prs_max_(prs_max) {}

  void Compute(const Domain& d) {
    collision_term_.resize(d.p.size());
    repulsive_term_.resize(d.p.size());
#pragma omp parallel for schedule(guided)
    for (SizeT i = 0; i < d.p.size(); ++i) {
      Vectord coll = 0.;
      Vectord repu = 0.;
      const double prs_i = d.p.prs(i), vol_i = d.p.mass() / d.p.dty(i);
      for (const SizeT j : d.p_p_neighbors.neighbors(i)) {
        const double prs_j = d.p.prs(j), vol_j = d.p.mass() / d.p.dty(j);
        const Vectord rij = d.p.pos(i) - d.p.pos(j);
        const double dist2 = rij * rij, dist = std::sqrt(dist2);
        if (dist >= d.p.dr()) continue;

        const Vectord vij = d.p.vel(i) - d.p.vel(j);
        const double vij_rij = vij * rij;
        if (vij_rij < 0.) {
          const Vectord v_coll =
              -(vij_rij / (dist2 + 0.01 * math::tpow<2>(d.p.h()))) * rij;
          double kappa = 1.;
          if (dist >= 0.5 * d.p.dr()) {
            kappa = Chi(dist, d.p.dr());
          }
          coll += kappa * v_coll;
        } else {
          constexpr double lambda = 0.1;
          const double vol_ave = 2.0 * vol_j / (vol_i + vol_j);
          const double back_prs =
              Chi(dist, d.p.dr()) *
              std::clamp(lambda * std::abs(prs_i + prs_j), prs_min_, prs_max_);
          repu += (vol_ave *
                   (back_prs / (dist2 + 0.01 * math::tpow<2>(d.p.h()))) * rij) /
                  d.p.dty(i);
        }
      }
      collision_term_[i] = coll;
      repulsive_term_[i] = repu;
    }
  }

  void Apply(const double dt, Domain& d) {
    // Vectord dmax = 0., vel = 0.;
    // double dimax = 0.;
    // SizeT imax = -1;
#pragma omp parallel for schedule(static)
    for (SizeT i = 0; i < d.p.size(); ++i) {
      const Vectord delta = collision_term_[i] + dt * repulsive_term_[i];
      // if (Length(delta) > dimax) {
      //   dmax = delta;
      //   dimax = Length(delta);
      //   imax = i;
      //   vel = d.p.vel(i);
      // }

      d.p.vel(i) += delta;
      d.p.SetPos(i, d.p.pos(i) + dt * delta);
    }
    // std::cout << imax << " max: " << dmax << " vel: " << vel << "\n";

    // if (dimax > d.p.sos()) exit(1);
  }

 private:
  static double Chi(const double dist, const double dr) {
    // return std::sqrt(Wendland(dist, dr) / Wendland(0.5 * dr, dr));
    const double q1 = dist / dr;
    const double w1 = math::tpow<4>(1. - q1) * (4. * q1 + 1.);
    constexpr double q2 = 0.5;
    constexpr double w2 = math::tpow<4>(q2) * 3.;
    return std::sqrt(w1 / w2);
  }

  double prs_min_ = 0;
  double prs_max_ = std::numeric_limits<double>::max();
  std::vector<Vectord> collision_term_;
  std::vector<Vectord> repulsive_term_;
};