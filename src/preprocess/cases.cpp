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

#include "cases.hpp"

#include <algorithm>
#include <cmath>

#include "mesh_shapes.hpp"
#include "solid_discretize.hpp"
#include "wsph/particle_boundary.hpp"

CaseSetup Cases::CollidingDroplets(const double droplet_resolution,
                                   const double relative_velocity) {
  const double droplet_size = 0.01, droplet_distance = 0.1 * droplet_size;
  MaterialSettings settings = MaterialSettings::Water();
  const double rel_vel = relative_velocity;
  settings.dr = droplet_size / droplet_resolution;
  settings.speed_of_sound = 10. * rel_vel;
  auto pos = PointDiscretize::Ellipsoid(
      settings.dr, Vectord(droplet_size),
      Vectord(-0.5 * (droplet_size + droplet_distance), 0., 0.));
  const size_t n1 = pos.size();
  const auto pos2 = PointDiscretize::Ellipsoid(
      settings.dr, Vectord(droplet_size),
      Vectord(0.5 * (droplet_size + droplet_distance), 0., 0.));
  pos.insert(pos.end(), pos2.begin(), pos2.end());
  std::vector<Vectord> vel(pos.size(), Vectord(0.));
  std::fill(vel.begin(), vel.begin() + n1, Vectord{rel_vel / 2, 0., 0.});
  std::fill(vel.begin() + n1, vel.end(), Vectord{-rel_vel / 2., 0., 0.});
  const size_t n = pos.size();
  Particles p(settings, std::move(pos), std::move(vel),
              std::vector<double>(n, settings.ref_density));

  CaseSetup setup;
  setup.num_outputs = 100;
  setup.sim_time = 30. * droplet_size / relative_velocity;
  setup.output_dir = "./colliding_droplets";
  setup.d = Domain(std::move(p));
  return setup;
}

CaseSetup Cases::CollidingCubes(const double droplet_resolution,
                                const double relative_velocity) {
  MaterialSettings settings = MaterialSettings::Water();
  const double droplet_d = 0.5, rel_vel = relative_velocity;
  settings.dr = droplet_d / droplet_resolution;
  settings.speed_of_sound = 2. * 100. * relative_velocity;
  auto pos = PointDiscretize::Cube(settings.dr, Vectord(droplet_d),
                                   Vectord(-0.5 * droplet_d, 0., 0.));
  const size_t n1 = pos.size();
  const auto pos2 = PointDiscretize::Cube(settings.dr, Vectord(droplet_d),
                                          Vectord(0.5 * droplet_d, 0., 0.));
  pos.insert(pos.end(), pos2.begin(), pos2.end());
  std::vector<Vectord> vel(pos.size(), Vectord(0.));
  std::fill(vel.begin(), vel.begin() + n1, Vectord{rel_vel / 2, 0., 0.});
  std::fill(vel.begin() + n1, vel.end(), Vectord{-rel_vel / 2., 0., 0.});
  const size_t n = pos.size();
  Particles p(settings, std::move(pos), std::move(vel),
              std::vector<double>(n, settings.ref_density));

  CaseSetup setup;
  setup.num_outputs = 50;
  setup.sim_time = 0.5;
  setup.output_dir = "./colliding_cubes";
  setup.d = Domain(std::move(p));
  return setup;
}

CaseSetup Cases::SimpleTank(const double tank_resolution,
                            const double tank_height, const double tank_width) {
  const double gravity = -9.81;
  MaterialSettings settings = MaterialSettings::Water();
  settings.dr =
      std::cbrt(tank_width * tank_width * tank_height) / tank_resolution;
  settings.speed_of_sound = 100. * std::abs(gravity) *
                            std::sqrt(2. * tank_height / std::abs(gravity));

  auto fluid_pos = PointDiscretize::Cube(
      settings.dr, Vectord(tank_width, tank_width, 0.9 * tank_height),
      Vectord(0.));

  auto mesh = MeshShapes::Cube(Vectord(tank_width, tank_width, tank_height),
                               Vectord(0.));
  auto solid = SolidDiscretize::Discretize(
      std::ceil(settings.smoothing_ratio * 2.), settings.dr, mesh);

  const size_t nf = fluid_pos.size();
  Particles p(settings, std::move(fluid_pos),
              std::vector<Vectord>(nf, Vectord(0.)),
              std::vector<double>(nf, settings.ref_density));

  const size_t npb = std::get<0>(solid).size();
  ParticleBoundary pb(settings, std::move(std::get<0>(solid)),
                      std::move(std::get<1>(solid)),
                      std::vector<Vectord>(npb, Vectord(0.)));
  CaseSetup s;
  s.d = Domain(std::move(p), std::move(pb));
  s.d.m = mesh;
  s.gravity = Vectord(0., 0., gravity);
  s.num_outputs = 50;
  s.sim_time = .5;
  s.output_dir = "./simple_tank";
  return s;
}

CaseSetup Cases::Dambreak(const double resolution) {
  const double gravity = -9.81, tank_width = 1., tank_length = 3.,
               tank_height = 1.;
  MaterialSettings settings = MaterialSettings::Water();
  settings.dr = std::cbrt(tank_width * tank_width * tank_width) / resolution;
  settings.speed_of_sound =
      100. * std::abs(gravity) * std::sqrt(tank_height / std::abs(gravity));

  auto fluid_pos = PointDiscretize::Cube(
      settings.dr, Vectord(tank_width, tank_width, 0.5 * tank_height),
      Vectord(0.));

  auto mesh = MeshShapes::Cube(Vectord(tank_width, tank_length, tank_height),
                               Vectord(0.));
  auto solid = SolidDiscretize::Discretize(3, settings.dr, mesh);

  const size_t nf = fluid_pos.size();
  Particles p(settings, std::move(fluid_pos),
              std::vector<Vectord>(nf, Vectord(0.)),
              std::vector<double>(nf, settings.ref_density));

  const size_t npb = std::get<0>(solid).size();
  ParticleBoundary pb(settings, std::move(std::get<0>(solid)),
                      std::move(std::get<1>(solid)),
                      std::vector<Vectord>(npb, Vectord(0.)));
  CaseSetup s;
  s.d = Domain(std::move(p), std::move(pb));
  s.d.m = mesh;
  s.gravity = Vectord(0., 0., gravity);
  s.num_outputs = 100;
  s.sim_time = 2.;
  s.output_dir = "./dambreak";
  return s;
}