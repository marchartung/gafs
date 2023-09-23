#include <cmath>
#include <filesystem>
#include <string>
#include <vector>

#include "file/vtp.hpp"
#include "preprocess/point_shapes.hpp"
#include "utils/types.hpp"

#define PI 3.14159265359f

struct Particles {
  size_t size() const { return points.size(); }

  std::vector<Pointd> points;
  std::vector<Pointd> velocity;
  std::vector<double> density;
  std::vector<double> pressure;
  double mass;
};

struct SphSettings {
  double gamma = 7.;
  double ref_density = 1000.;
  // double max_velocity = 10.;
  // double pressure_parameter =
  //     100. * ref_density * max_velocity * max_velocity / gamma;
  // double speed_of_sound = std::sqrt(gamma * pressure_parameter /
  // ref_density);
  double speed_of_sound = 50.;
  double pressure_parameter =
      ref_density * speed_of_sound * speed_of_sound / gamma;

  double viscosity = 1.e-3;
  double smoothing_length = 3.;
  double h = 0.2;
  double dr = h / smoothing_length;
  double mass = dr * dr * dr * ref_density;
  Pointd gravity = {0., 0., -9.8};
};

double Wendland(const double distance, const double h) {
  //   const double q = distance / h;
  //   const double fac = (1. - 0.5 * q);
  //   const double fac4 = fac * fac * fac * fac;
  //   return fac4 * (2. * q + 1.);
  const double q = distance / h;
  if (q < 1.) {
    return (1. - 1.4 * q * q + 0.75 * q * q * q) / (PI * h * h * h);
  } else {
    return 0.25 * (2. - q) * (2. - q) * (2. - q) / (PI * h * h * h);
  }
}

double WendlandGradient(const double distance, const double h) {
  const double q = distance / h;
  if (q < 1.) {
    return (-3. * q + 2.25 * q * q) / (PI * h * h * h);
  } else {
    return 0.75 * (2. - q) * (2. - q) / (PI * h * h * h);
  }
}

std::vector<double> ComputeDensity(const Particles& p, const SphSettings& s) {
  const double radius2 = 4.0 * s.h * s.h;
  std::vector<double> density(p.size());
  for (size_t i = 0; i < p.size(); ++i) {
    double kernel_sum = 0.;
    for (size_t j = 0; j < p.size(); ++j) {
      if (i == j) continue;
      const Pointd delta = p.points[i] - p.points[j];
      const double dist2 = delta * delta;
      if (dist2 < radius2) {
        kernel_sum += Wendland(std::sqrt(dist2), s.h);
      }
    }
    density[i] = kernel_sum * p.mass;
  }
  return density;
}

double ComputePressure(const double density, const SphSettings& s) {
  const double pg = std::pow(density / s.ref_density, s.gamma) - 1.;
  return s.pressure_parameter * pg;
}

std::vector<double> ComputePressure(const std::vector<double>& density,
                                    const SphSettings& s) {
  std::vector<double> pressure(density.size());
  for (size_t i = 0; i < pressure.size(); ++i) {
    pressure[i] = ComputePressure(density[i], s);
  }
  return pressure;
}

std::vector<Pointd> ComputeAcceleration(const Particles& p,
                                        const std::vector<double>& density,
                                        const SphSettings& s) {
  const double radius2 = 4.0 * s.h * s.h;
  std::vector<Pointd> acc(p.size());
  for (size_t i = 0; i < p.size(); ++i) {
    Pointd acc_i(0.);
    const double pi = ComputePressure(density[i], s);
    for (size_t j = 0; j < p.size(); ++j) {
      if (i == j) continue;
      const Pointd delta = p.points[i] - p.points[j];
      const double dist2 = delta * delta;
      if (dist2 < radius2) {
        const double dist = std::sqrt(dist2);
        const double wg = WendlandGradient(dist, s.h);
        double pj = ComputePressure(density[j], s);
        const double prs_term =
            pi / (density[i] * density[i]) + pj / (density[j] * density[j]);

        acc_i -= p.mass * prs_term * wg * delta;
        acc_i += (2. * wg * s.viscosity * p.mass * dist2) /
                 ((dist2 + 0.001 * s.h) * density[j] * density[i]) *
                 (p.velocity[i] - p.velocity[j]);
      }
    }
    acc[i] = acc_i + s.gravity;
  }

  return acc;
}

void UpdateState(const double dt, const std::vector<Pointd>& acc, Particles& p,
                 const SphSettings& settings) {
  for (size_t i = 0; i < p.size(); ++i) {
    p.velocity[i] += dt * acc[i];
    p.points[i] += dt * p.velocity[i];
    if (p.points[i][2] < 0.) p.points[i][2] = 0.;
  }
  p.density = ComputeDensity(p, settings);
  p.pressure = ComputePressure(p.density, settings);
}

void Timestep(const double dt, Particles& p, const SphSettings& s) {
  const std::vector<double> density = ComputeDensity(p, s);
  const std::vector<Pointd> acc = ComputeAcceleration(p, density, s);
  UpdateState(dt, acc, p, s);
}

void Write(const size_t output_num, const Particles& p) {
  std::cout << " density: " << p.density[375] << "\n";
  VTP vtp;
  vtp.SetPoints(p.points);
  vtp.AddPointData("velocity", p.velocity.data(), p.size());
  vtp.AddPointData("density", p.density.data(), p.size());
  vtp.AddPointData("pressure", p.pressure.data(), p.size());

  const std::filesystem::path path =
      "/home/marc/test/fluid_cube_" + std::to_string(output_num) + ".vtp";
  vtp.Export(path);
}

int main() {
  const SphSettings settings;
  std::cout << "h: " << settings.h << " dr: " << settings.dr
            << " mass: " << settings.mass << "\n";
  const double dt = 0.001;
  Particles p;
  p.mass = settings.mass;
  p.points = shape::Cube(8, settings.dr);
  p.velocity.resize(p.points.size(), Pointd{0., 0., 0.});
  p.density = ComputeDensity(p, settings);
  p.pressure = ComputePressure(p.density, settings);
  Write(0, p);
  for (size_t output = 1; output <= 100; ++output) {
    for (size_t sub_step = 0; sub_step < 10; ++sub_step) {
      Timestep(dt, p, settings);
    }
    Write(output, p);
  }

  return 0;
}