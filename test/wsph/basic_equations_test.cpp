#include "wsph/basic_equations.hpp"

#include <gtest/gtest.h>

#include <iostream>

#include "utils/types.hpp"

template <typename Functor>
void ForeachCubePoint(const double length, const double dr,
                      const Vectord center, Functor f) {
  const Vectord offset = center + 0.5 * Vectord(dr) - 0.5 * length;
  double x = 0., y = 0., z = 0.;
  do {
    y = 0.;
    do {
      z = 0.;
      do {
        f(offset + Vectord(x, y, z));
      } while ((z += dr) < length);
    } while ((y += dr) < length);
  } while ((x += dr) < length);
}

TEST(BasicEquations, Kernel) {
  const double dr = 0.1, h = 10. * dr, radius = 2. * h, ref_dty = 1000.;
  const double mass = ref_dty * std::pow(dr, 3.);
  double dty = 0.;

  ForeachCubePoint(2. * radius, dr, Vectord(0.),
                   [&dty, mass, h, radius](const Vectord p) {
                     const double dist = Length(p);

                     if (dist <= radius) {
                       dty += mass * Kernel(dist, h);
                     }
                   });
  EXPECT_NEAR(dty, ref_dty, 1.e-3);
}

TEST(BasicEquations, KernelQuarter) {
  const double dr = 0.1, h = 10. * dr, radius = 2. * h, ref_dty = 1000.;
  const double mass = ref_dty * std::pow(dr, 3.);
  double dty = 0.;
  const Vectord o(radius, radius, 0.);
  ForeachCubePoint(2. * radius, dr, Vectord(0.),
                   [&dty, mass, h, radius, o](const Vectord p) {
                     const double dist = Length(o - p);

                     if (dist <= radius) {
                       dty += mass * Kernel(dist, h);
                     }
                   });
  EXPECT_NEAR(dty, 0.25 * ref_dty, 1.e-3);
}

TEST(BasicEquations, KernelGradientHalf) {
  const double dr = 0.1, h = 1.5 * dr, radius = 2. * h, ref_dty = 1000.;
  const double mass = ref_dty * std::pow(dr, 3.);
  Vectord grad = 0.;
  const Vectord o(0.5 * dr + radius, 0., 0.);
  ForeachCubePoint(2. * radius, dr, Vectord(0.),
                   [&grad, mass, h, radius, o](const Vectord p) {
                     const double dist = Length(o - p);

                     if (dist <= radius) {
                       grad += KernelGradient(dist, h) * (o - p);
                     }
                   });
  EXPECT_LT(grad[0], 0.);
  EXPECT_NEAR(grad[1], 0., 1.e-3);
  EXPECT_NEAR(grad[2], 0., 1.e-3);
}
