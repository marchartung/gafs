#pragma once

#include <cmath>
#include <vector>

#include "utils/math.hpp"

double ComputePressureParameter(const double ref_density,
                                const double speed_of_sound) {
  const double gamma = 7.;
  return ref_density * speed_of_sound * speed_of_sound / gamma;
}

double ComputePressure(const double density, const double ref_density,
                       const double pressure_parameter) {
  const double gamma = 7.;
  const double pg = std::pow(density / ref_density, gamma) - 1.;
  return pressure_parameter * pg;
}

double ComputePressureTaylor(const double density, const double ref_density,
                             const double sos) {
  return sos * sos * (density - ref_density);
}

std::vector<double> ComputePressure(const std::vector<double>& density,
                                    const double ref_density,
                                    const double pressure_parameter) {
  std::vector<double> pressure(density.size());
  for (size_t i = 0; i < pressure.size(); ++i) {
    pressure[i] = ComputePressure(density[i], ref_density, pressure_parameter);
  }
  return pressure;
}

double Wendland(const double distance, const double h) {
  const double a = 21. / (16. * math::pi<double>()) / (h * h * h);
  const double q = distance / h;
  const double b = 1. - 0.5 * q;
  if (q > 2.) return 0.;
  return a * b * b * b * b * (1. + 2. * q);
}

double WendlandGradient(const double distance, const double h) {
  const double a = -5. * 21. / (16. * math::pi<double>()) / (h * h * h * h * h);
  const double q = distance / h;
  const double b = 1. - 0.5 * q;
  if (q > 2.) return 0.;
  return a * q * b * b * b;
}
