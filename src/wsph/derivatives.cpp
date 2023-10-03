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

#include "derivatives.hpp"

void Derivative::Step(const double dt, const Vectord gravity, Particles& p) {
#pragma omp parallel for schedule(static)
  for (SizeT i = 0; i < p.size(); ++i) {
    p.dty(i) += dt * dtyD[i];
    p.prs(i) =
        ComputePressure(p.dty(i), p.ref_density(), p.pressure_parameter());
    p.vel(i) += dt * (acc[i] + gravity);
    p.SetPos(i, p.pos(i) + dt * p.vel(i));
  }
}

void BasicWeaklyRhs::Compute(const Domain& d, Derivative& res) {
  ComputePP(true, d.p, d.p, d.p_p_neighbors, res);
  ComputePP(false, d.p, d.pb, d.p_pb_neighbors, res);
}

void BasicWeaklyRhs::ComputePP(const bool overwrite, const Particles& p,
                               const Particles& np, const SavedNeighborsD& sn,
                               Derivative& res) {
  res.Resize(p.size());
#pragma omp parallel for schedule(guided)
  for (SizeT i = 0; i < p.size(); ++i) {
    Vectord acc = 0.;
    double dtyD = 0.;
    const double prs_i = p.prs(i);
    for (const SizeT j : sn.neighbors(i)) {
      const double prs_j = np.prs(j);
      const Vectord rij = p.pos(i) - np.pos(j);
      const double dist2 = rij * rij, dist = std::sqrt(dist2);
      const double fm =
          -(prs_i / math::tpow<2>(p.dty(i)) + prs_j / math::tpow<2>(np.dty(j)));

      const Vectord vij = p.vel(i) - np.vel(j);
      const double vij_rij = vij * rij;
      double fv = 0.;
      if (vij_rij < 0.) {
        const double v =
            2. * p.viscosity() * p.h() * p.sos() / (p.dty(i) + np.dty(j));
        fv = v * (vij_rij / (dist2 + 0.01 * math::tpow<2>(p.h())));
      }

      const double wg = WendlandGradient(dist, p.h());
      acc += (fm + fv) * wg * rij;
      dtyD += (p.mass() / np.dty(j)) * vij * (wg * rij);
    }
    if (overwrite) {
      res.dtyD[i] = p.dty(i) * dtyD;
      res.acc[i] = p.mass() * acc;
    } else {
      res.dtyD[i] += p.dty(i) * dtyD;
      res.acc[i] += p.mass() * acc;
    }
  }
}

double BasicWeaklyRhs::ComputeMaxDt(const Domain& d,
                                    const Derivative& derivative) {
  return std::min(CourantViscDt(d.p, d.p_p_neighbors),
                  ForceDt(d.p.h(), derivative));
}

double BasicWeaklyRhs::CourantViscDt(const Particles& p,
                                     const SavedNeighborsD& neighbors) {
  double courant = 0.;
#pragma omp parallel for schedule(dynamic, 27) reduction(max : courant)
  for (SizeT i = 0; i < p.size(); ++i) {
    double tmp_c = 0.;
    for (const SizeT j : neighbors.neighbors(i)) {
      const Vectord rij = p.pos(i) - p.pos(j);
      const double dist2 = rij * rij;
      const double vij_rij = (p.vel(i) - p.vel(j)) * rij;
      const double tmp =
          std::abs(p.h() * vij_rij / (dist2 + 0.01 * math::tpow<2>(p.h())));
      tmp_c = std::max(tmp, tmp_c);
    }
    courant = std::max(courant, tmp_c);
  }
  return cfl() * p.h() / (p.sos() + courant);
}

double BasicWeaklyRhs::ForceDt(const double h, const Derivative& d) {
  double max_acc_mag = 0.;
#pragma omp parallel for schedule(guided) reduction(max : max_acc_mag)
  for (SizeT i = 0; i < d.size(); ++i) {
    max_acc_mag = std::max(max_acc_mag, Length(d.acc[i]));
  }
  return cfl() * std::sqrt(h) / max_acc_mag;
}