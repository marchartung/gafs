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

#include "shifting.hpp"

void DpcShifting::Compute(const Domain& d) {
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
#pragma omp parallel for schedule(guided)
  for (SizeT i = 0; i < d.p.size(); ++i) {
    Vectord coll = 0.;
    Vectord repu = 0.;
    for (const SizeT j : d.p_dbc_neighbors.neighbors(i)) {
      const Vectord rij = d.p.pos(i) - d.dbc.pos(j);
      const double dist2 = rij * rij, dist = std::sqrt(dist2);

      const double w = Wendland(dist, d.p.h()),
                   ww0 = w + d.p.ref_density() / d.p.mass();
    }
    collision_term_[i] += coll;
    repulsive_term_[i] += repu;
  }
}

void DpcShifting::Apply(const double dt, Domain& d) {
#pragma omp parallel for schedule(static)
  for (SizeT i = 0; i < d.p.size(); ++i) {
    const Vectord delta = collision_term_[i] + dt * repulsive_term_[i];
    d.p.vel(i) += delta;
    d.p.SetPos(i, d.p.pos(i) + dt * delta);
  }
}