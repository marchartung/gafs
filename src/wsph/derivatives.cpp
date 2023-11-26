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

void Derivative::Step(const double dt, const Vectord gravity, Domain& d) {
#pragma omp parallel for schedule(static)
  for (SizeT i = 0; i < d.p.size(); ++i) {
    d.p.dty(i) += dt * dtyD[i];
    d.p.prs(i) = ComputePressure(d.p.dty(i), d.p.ref_density(),
                                 d.p.pressure_parameter());
    d.p.vel(i) += dt * (acc[i] + gravity);
    d.SetFluidPos(i, d.p.pos(i) + dt * d.p.vel(i));
  }
}

void BasicWeaklyRhs::Compute(const Domain& d, Derivative& res) {
  ComputePP(true, d.p, d.p, d.p_p_neighbors, res);
  if (d.pb.size() > 0) {
    ComputePP(false, d.p, d.pb, d.p_pb_neighbors, res);
  }
}

void BasicWeaklyRhs::ComputePP(const bool overwrite, const Particles& p,
                               const Particles& np, const SavedNeighborsD& sn,
                               Derivative& res) {
  res.Resize(p.size());
#pragma omp parallel for schedule(guided)
  for (SizeT i = 0; i < p.size(); ++i) {
    Vectord acc = 0.;
    double dtyD = 0., dtyDD = 0.;
    for (const SizeT j : sn.neighbors(i)) {
      const Vectord rij = p.pos(i) - np.pos(j);
      const double dist2 = rij * rij, dist = std::sqrt(dist2);
      const double wg = KernelGradient(dist, p.h());
      acc -= wg * np.mass() * (p.prs(i) + np.prs(j)) / (p.dty(i) * np.dty(j)) *
             rij / dist;

      const Vectord vij = p.vel(i) - np.vel(j);
      if (vij * rij < 0.) {
        acc += np.mass() * p.viscosity() * p.sos() *
               (p.h() * vij * rij / (dist2 + 0.01 * math::tpow<2>(p.h()))) /
               (0.5 * (p.dty(i) + np.dty(j))) * wg * rij / dist;
      }

      dtyD += (p.dty(i) / np.dty(j)) * wg * np.mass() * vij * rij / dist;

      dtyDD += 2. * 0.1 * (np.dty(j) - p.dty(i)) * rij /
               (dist2 + 0.01 * math::tpow<2>(p.h())) * (wg * rij) *
               (np.mass() / np.dty(j));
    }
    if (overwrite) {
      res.dtyD[i] = dtyD + p.h() * p.sos() * dtyDD;
      res.acc[i] = acc;
    } else {
      res.dtyD[i] += dtyD;
      res.acc[i] += acc;
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
  const double dt = cfl() * p.h() / (p.sos() + courant);
  return dt;
}

double BasicWeaklyRhs::ForceDt(const double h, const Derivative& d) {
  double max_acc_mag = 0.;
#pragma omp parallel for schedule(guided) reduction(max : max_acc_mag)
  for (SizeT i = 0; i < d.size(); ++i) {
    max_acc_mag = std::max(max_acc_mag, Length(d.acc[i]));
  }
  const double dt = cfl() * std::sqrt(h) / max_acc_mag;
  return dt;
}