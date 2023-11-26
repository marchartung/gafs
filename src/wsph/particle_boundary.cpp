#include "particle_boundary.hpp"

#include "neighbor/saved_neighbors.hpp"

void ParticleBoundary::Interpolate(const Particles& p) {
  SavedNeighborsD saved(pos(), p.pos());
#pragma omp parallel for schedule(guided)
  for (SizeT i = 0; i < size(); ++i) {
    double dty_loc = 0., renorm = 0.;
    Vectord vel_loc(0.);
    for (const SizeT j : saved.neighbors(i)) {
      const Vectord rij = pos(i) - p.pos(j);
      const double dist2 = rij * rij, dist = std::sqrt(dist2);
      const double w = Kernel(dist, p.h());
      renorm += w;
      dty_loc += w * p.dty(j);
      vel_loc += -w * p.vel(j);
    }
    if (renorm != 0.) {
      dty(i) = dty_loc / renorm;
      prs(i) = ComputePressure(dty(i), p.ref_density(), p.pressure_parameter());
      vel(i) = ((vel_loc / renorm) * normal(i)) * normal(i);
    } else {
      dty(i) = p.ref_density();
      prs(i) = 0.;
      vel(i) = 0.;
    }
  }
}