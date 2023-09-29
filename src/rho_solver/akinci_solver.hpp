#pragma once

#include <cassert>
#include <iostream>
#include <vector>

#include "basic_equations.hpp"
#include "dynamic_boundary.hpp"
#include "mesh.hpp"
#include "neighbor/saved_neighbors.hpp"
#include "particles.hpp"
#include "utils/math.hpp"
#include "volume_boundary.hpp"

class AkinciSolver {
 public:
  AkinciSolver() = default;
  AkinciSolver(const Vectord gravity, Particles p, VolumeBoundary wall)
      : gravity_(gravity),
        pcloud_(std::move(p)),
        p_neighbors_(pcloud_.pos()),
        wall_(std::move(wall)) {}

  const Particles& particles() const { return pcloud_; }
  const VolumeBoundary& wall() const { return wall_; }

  double cfl() const { return 1.1; }

  double CourantViscDt() {
    double courant = 0.;
#pragma omp parallel for schedule(dynamic, 27) reduction(max : courant)
    for (SizeT i = 0; i < pcloud_.size(); ++i) {
      double tmp_c = 0.;
      for (const SizeT j : p_neighbors_.neighbors(i)) {
        const Vectord rij = pcloud_.pos(i) - pcloud_.pos(j);
        const double dist2 = rij * rij, dist = std::sqrt(dist2);
        const double vij_rij = (pcloud_.vel(i) - pcloud_.vel(j)) * rij;
        const double tmp =
            std::abs(pcloud_.h() * vij_rij / (dist2 + 0.01 * pcloud_.h()));
        tmp_c = std::max(tmp, tmp_c);
      }
      courant = std::max(courant, tmp_c);
    }
    return cfl() * pcloud_.h() / (pcloud_.sos() + courant);
  }

  double ForceDt() {
    double max_acc_mag = 0.;
#pragma omp parallel for schedule(guided) reduction(max : max_acc_mag)
    for (SizeT i = 0; i < pcloud_.size(); ++i) {
      max_acc_mag = std::max(max_acc_mag, Length(derivative_.acc[i]));
    }
    return cfl() * std::sqrt(pcloud_.h()) / max_acc_mag;
  }

  double TimeStep(const double final_dt) {
    const double t_start = omp_get_wtime();
    const size_t init_version = pcloud_.UpdateVersion();
    SizeT num_steps = 0, steps_limited_by_courant = 0;
    double stepped_time = 0.;
    do {
      ComputeDerivative();
      const double cdt = CourantViscDt(), fdt = ForceDt();
      const double dt = std::min(std::min(fdt, cdt),
                                 std::max(final_dt - stepped_time, 1.e-14));
      if (cdt < fdt) ++steps_limited_by_courant;
      InitVerlet();
      ApplyDerivative(dt / 2.);
      ComputeDerivative();
      VerletStep(dt);
      UpdateNeighborhood();
      stepped_time += dt;
      ++num_steps;
    } while (stepped_time < final_dt);

    std::cout << "\navg dt: " << final_dt / num_steps
              << " | num steps: " << num_steps
              << " | limiting timestep criteria: "
              << ((steps_limited_by_courant >= 0.5 * num_steps) ? "courant"
                                                                : "force")
              << " | step time: "
              << 1000. * (omp_get_wtime() - t_start) / num_steps << "ms \n";
    return final_dt / num_steps;
  }

 private:
  void UpdateNeighborhood() {
    pcloud_.Update();
    p_neighbors_.Update(pcloud_.pos());
  }

  void ComputeDerivative() {
    derivative_.Resize(pcloud_.size());
#pragma omp parallel for schedule(guided)
    for (SizeT i = 0; i < pcloud_.size(); ++i) {
      Vectord acc = 0.;
      double dtyD = 0.;
      const double prs_i = pcloud_.prs(i);
      for (const SizeT j : p_neighbors_.neighbors(i)) {
        const double prs_j = pcloud_.prs(j);
        const Vectord rij = pcloud_.pos(i) - pcloud_.pos(j);
        const double dist2 = rij * rij, dist = std::sqrt(dist2);
        const double wg = WendlandGradient(dist, pcloud_.h());
        const double fm = -(prs_i / math::tpow<2>(pcloud_.dty(i)) +
                            prs_j / math::tpow<2>(pcloud_.dty(j)));

        const double vij_rij = (pcloud_.vel(i) - pcloud_.vel(j)) * rij;
        double fv = 0.;
        if (vij_rij < 0.) {
          const double v = 2. * 0.01 * pcloud_.h() * pcloud_.sos() /
                           (pcloud_.dty(i) + pcloud_.dty(j));
          fv = v * (vij_rij / (dist2 + 0.001 * pcloud_.h() * pcloud_.h()));
        }
        acc += (fm + fv) * wg * rij;
        dtyD += vij_rij * wg;
      }
      derivative_.dtyD[i] = pcloud_.mass() * dtyD;
      derivative_.acc[i] = pcloud_.mass() * acc;
    }
  }

  void ApplyDerivative(const double dt) {
#pragma omp parallel for schedule(static)
    for (SizeT i = 0; i < pcloud_.size(); ++i) {
      pcloud_.dty(i) += dt * derivative_.dtyD[i];
      pcloud_.prs(i) = ComputePressure(pcloud_.dty(i), pcloud_.ref_density(),
                                       pcloud_.pressure_parameter());
      pcloud_.vel(i) += dt * (derivative_.acc[i] + gravity_);
      pcloud_.SetPos(i, pcloud_.pos(i) + dt * pcloud_.vel(i));
    }
  }

  void VerletStep(const double dt) {
#pragma omp parallel for schedule(static)
    for (SizeT i = 0; i < pcloud_.size(); ++i) {
      pcloud_.vel(i) =
          verlet_.init_vel[i] + dt * (derivative_.acc[i] + gravity_);
      pcloud_.SetPos(i, verlet_.init_pos[i] +
                            0.5 * dt * (verlet_.init_vel[i] + pcloud_.vel(i)));

      const double eps = -(derivative_.dtyD[i] / pcloud_.dty(i)) * dt;
      pcloud_.dty(i) = verlet_.init_dty[i] * ((2. - eps) / (2. + eps));
    }
  }

  void InitVerlet() {
    verlet_.init_pos = pcloud_.pos();
    verlet_.init_vel = pcloud_.vel();
    verlet_.init_dty = pcloud_.dty();
  }

  struct DualSphyVerlet {
    std::vector<Vectord> init_pos;
    std::vector<Vectord> init_vel;
    std::vector<double> init_dty;
  };

  struct Derivative {
    std::vector<Vectord> acc;
    std::vector<double> dtyD;

    void Resize(const size_t n) {
      acc.resize(n);
      dtyD.resize(n);
    }
  };

  DualSphyVerlet verlet_;

  Vectord gravity_ = Vectord(0.);
  Particles pcloud_;
  SavedNeighborsD p_neighbors_;

  VolumeBoundary wall_;

  Derivative derivative_;
};