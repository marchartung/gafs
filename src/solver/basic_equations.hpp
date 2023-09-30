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

#include <cmath>
#include <vector>

#include "utils/macros.hpp"
#include "utils/math.hpp"

DEVICE double ComputePressureParameter(const double ref_density,
                                       const double speed_of_sound) {
  constexpr int gamma = 7;
  return ref_density * speed_of_sound * speed_of_sound / gamma;
}

DEVICE double ComputePressure(const double density, const double ref_density,
                              const double pressure_parameter) {
  constexpr int gamma = 7;
  const double pg = math::tpow<gamma>(density / ref_density) - 1.;
  return pressure_parameter * pg;
}

DEVICE double ComputePressureTaylor(const double density,
                                    const double ref_density,
                                    const double sos) {
  return sos * sos * (density - ref_density);
}

DEVICE double Wendland(const double distance, const double h) {
  const double a = 21. / (16. * math::pi<double>()) / (h * h * h);
  const double q = distance / h;
  const double b = 1. - 0.5 * q;
  return a * math::tpow<4>(b) * (1. + 2. * q);
}

DEVICE double WendlandGradient(const double distance, const double h) {
  const double a = (-5. * 21. / (16. * math::pi<double>())) / math::tpow<5>(h);
  const double q = distance / h;
  const double b = 1. - 0.5 * q;
  return a * q * math::tpow<3>(b);
}

std::vector<double> ComputePressure(const std::vector<double>& density,
                                    const double ref_density,
                                    const double pressure_parameter);
