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

#include <iostream>  // FIXME

void DpcShifting::Compute(const Domain& d) {
  ComputePP(true, d.p, d.p, d.p_p_neighbors);

  if (d.pb.size() > 0) {
    ComputePP(false, d.p, d.pb, d.p_pb_neighbors);
  }
}

void DpcShifting::ComputePP(const bool overwrite, const Particles& p,
                            const Particles& np, const SavedNeighborsD& sn) {
  collision_term_.resize(p.size());
  repulsive_term_.resize(p.size());
#pragma omp parallel for schedule(guided)
  for (SizeT i = 0; i < p.size(); ++i) {
    Vectord coll = 0.;
    Vectord repu = 0.;
    const double prs_i = p.prs(i), vol_i = p.mass() / p.dty(i);
    for (const SizeT j : sn.neighbors(i)) {
      const double prs_j = np.prs(j), vol_j = np.mass() / np.dty(j);
      const Vectord rij = p.pos(i) - np.pos(j);
      const double dist2 = rij * rij, dist = std::sqrt(dist2);
      if (dist >= p.dr()) continue;

      const Vectord vij = p.vel(i) - np.vel(j);
      const double vij_rij = vij * rij;
      if (vij_rij < 0.) {
        const Vectord v_coll =
            -(vij_rij / (dist2 + 0.01 * math::tpow<2>(p.h()))) * rij;
        double kappa = 1.;
        if (dist >= 0.5 * p.dr()) {
          kappa = Chi(dist, p.dr());
        }
        coll += kappa * v_coll;
      } else {
        constexpr double lambda = 0.1;
        const double vol_ave = 2.0 * vol_j / (vol_i + vol_j);
        const double back_prs =
            Chi(dist, p.dr()) *
            std::clamp(lambda * std::abs(prs_i + prs_j), prs_min_, prs_max_);
        repu += (vol_ave * (back_prs / (dist2 + 0.01 * math::tpow<2>(p.h()))) *
                 rij) /
                p.dty(i);
      }
    }
    if (overwrite) {
      collision_term_[i] = coll;
      repulsive_term_[i] = repu;
    } else {
      collision_term_[i] += coll;
      repulsive_term_[i] += repu;
    }
  }
}

void DpcShifting::Apply(const double dt, Domain& d) {
#pragma omp parallel for schedule(static)
  for (SizeT i = 0; i < d.p.size(); ++i) {
    const Vectord delta = collision_term_[i] + dt * repulsive_term_[i];
    d.p.vel(i) += delta;
    d.SetFluidPos(i, d.p.pos(i) + dt * delta);
  }
}

void LindShifting::Compute(const Domain& d) {
  ComputePP(true, d.p, d.p, d.p_p_neighbors);
  if (d.pb.size() > 0) ComputePP(false, d.p, d.pb, d.p_pb_neighbors);
}

void LindShifting::Apply(const double dt, Domain& d) {
#pragma omp parallel for schedule(static)
  for (SizeT i = 0; i < d.p.size(); ++i) {
    d.SetFluidPos(i, d.p.pos(i) + dt * delta_r_[i]);
  }
}

void LindShifting::ComputePP(const bool overwrite, const Particles& p,
                             const Particles& np, const SavedNeighborsD& sn) {
  delta_r_.resize(p.size());
  // #pragma omp parallel for schedule(guided)
  for (SizeT i = 0; i < p.size(); ++i) {
    Vectord c = 0.;
    double nr = 0.;
    for (const SizeT j : sn.neighbors(i)) {
      const Vectord rij = p.pos(i) - np.pos(j);
      const double dist2 = rij * rij, dist = std::sqrt(dist2);
      const double vol = np.dty(j) / np.mass();
      const Vectord wg = KernelGradient(dist, p.h()) * rij;

      c += vol * wg;
      nr += (vol * rij) * wg;
    }
    const double A_fsc = (nr - A_fst) / (A_fsm - A_fst);

    Vectord shift = -A * p.h() * c;
    if (nr - A_fst < 0) shift *= A_fsc;
    if (overwrite) {
      delta_r_[i] = shift;
    } else {
      delta_r_[i] += shift;
    }
  }
}
