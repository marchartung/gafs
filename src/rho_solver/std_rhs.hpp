#pragma once

#include <iostream>
#include <vector>

#include "basic_equations.hpp"
#include "dynamic_boundary.hpp"
#include "neighbor.hpp"
#include "particles.hpp"

class BasicWSphRhs {
 public:
  BasicWSphRhs() = default;
  BasicWSphRhs(const Pointd gravity) : gravity_(gravity) {}

  double dtyD(const size_t idx) const { return dtyD_[idx]; }
  Pointd acc(const size_t idx) const { return acc_[idx]; }

  void Compute(const Particles& p, const DynamicBoundary& b) {
    const double radius2 = 4.0 * p.h() * p.h();
    acc_.resize(p.size());
    dtyD_.resize(p.size());

    auto rhs = [&](const size_t i, const std::vector<size_t>& neighbors) {
      Pointd acc_i(0.);
      double dtyD_i = 0.;
      const double pi =
          ComputePressure(p.dty(i), p.ref_density(), p.pressure_parameter());
      for (const size_t j : neighbors) {
        const Pointd rij = p.pos(i) - p.pos(j);
        const double dist2 = rij * rij, dist = std::sqrt(dist2);
        const double wg = WendlandGradient(dist, p.h());
        double pj =
            ComputePressure(p.dty(j), p.ref_density(), p.pressure_parameter());
        const double prs_term =
            pi / (p.dty(i) * p.dty(i)) + pj / (p.dty(j) * p.dty(j));

        double visc_term = (pi + pj) / (p.dty(i) * p.dty(j));
        const Pointd vij = p.vel(i) - p.vel(j);
        const double vij_rij = vij * rij;
        if (vij_rij < 0.) {
          const double dty_avg = 0.5 * (p.dty(i) + p.dty(j));
          const double alpha = 0.01;
          const double mu = p.h() * vij_rij / (dist2 + 0.01 * p.h() * p.h());
          visc_term += -alpha * p.sos() * mu / dty_avg;
        }
        acc_i -= p.mass() * (prs_term + visc_term) * wg * rij;
        dtyD_i += p.mass() * wg * vij_rij;
      }
      dtyD_[i] = dtyD_i;
      acc_[i] = acc_i;
    };
    ForeachInDistance(2. * p.h(), p.pos(), rhs);

    auto rhs_boundary = [&](const size_t i,
                            const std::vector<size_t>& neighbors) {
      Pointd acc_i(0.);
      double dtyD_i = 0.;
      const double pi =
          ComputePressure(p.dty(i), p.ref_density(), p.pressure_parameter());
      for (const size_t j : neighbors) {
        const Pointd rij = p.pos(i) - b.pos(j);
        const double dist2 = rij * rij, dist = std::sqrt(dist2);
        const double wg = WendlandGradient(dist, p.h());
        double pj = ComputePressureTaylor(b.dty(i), b.ref_density(), b.sos());
        const double prs_term =
            pi / (p.dty(i) * p.dty(i)) + pj / (b.dty(i) * b.dty(i));

        double visc_term = (pi + pj) / (p.dty(i) * b.dty(i));
        const Pointd vij = p.vel(i) - b.vel(j);
        const double vij_rij = vij * rij;
        if (vij_rij < 0.) {
          const double dty_avg = 0.5 * (p.dty(i) + b.dty(i));
          const double alpha = 0.01;
          const double mu = p.h() * vij_rij / (dist2 + 0.01 * p.h() * p.h());
          visc_term += -alpha * b.sos() * mu / dty_avg;
        }
        acc_i -= b.mass() * (prs_term + visc_term) * wg * rij;
        dtyD_i += b.mass() * wg * vij_rij;
      }
      dtyD_[i] += dtyD_i;
      acc_[i] += acc_i;
    };
    ForeachInDistance<false>(2. * p.h(), p.pos(), b.pos(), rhs_boundary);
  }

  void Update(const double dt, Particles& p, DynamicBoundary& b) {
    for (size_t i = 0; i < p.size(); ++i) {
      p.dty(i) += dt * dtyD_[i];
      p.vel(i) += dt * (acc_[i] + gravity_);
      p.pos(i) += dt * p.vel(i);
    }

    ComputeDensityWall(p, b);
  }

  void ComputeDensityWall(const Particles& p, DynamicBoundary& b) {
    auto density_wall = [&](const size_t i,
                            const std::vector<size_t>& neighbors) {
      double dty = 0.;
      for (const size_t j : neighbors) {
        const Pointd rij = b.pos(i) - p.pos(j);
        const double dist2 = rij * rij, dist = std::sqrt(dist2);
        const double w = Wendland(dist, b.h());
        dty += p.mass() * w;
      }
      b.dty(i) = dty;
    };
    ForeachInDistance<false>(2. * b.h(), b.pos(), p.pos(), density_wall);

    auto density_wall2 = [&](const size_t i,
                             const std::vector<size_t>& neighbors) {
      double dty = 0.;
      for (const size_t j : neighbors) {
        const Pointd rij = b.pos(i) - b.pos(j);
        const double dist2 = rij * rij, dist = std::sqrt(dist2);
        const double w = Wendland(dist, b.h());
        dty += b.mass() * w;
      }
      b.dty(i) += dty;
      b.dty(i) = std::max(b.dty(i), b.ref_density());
    };
    ForeachInDistance(2. * b.h(), b.pos(), density_wall2);
  }

 private:
  Pointd gravity_ = Pointd(0.);

  std::vector<double> dtyD_;
  std::vector<Pointd> acc_;
};