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

CaseSetup Cases::CollidingDroplets(const double droplet_resolution,
                                   const double relative_velocity) {
  MaterialSettings settings = MaterialSettings::Water();
  const double droplet_d = 0.005, rel_vel = relative_velocity;
  settings.dr = droplet_d / droplet_resolution;
  settings.speed_of_sound = 2. * 10. * rel_vel;
  auto pos = PointDiscretize::Ellipsoid(settings.dr, Vectord(droplet_d),
                                        Vectord(-0.5 * droplet_d, 0., 0.));
  const size_t n1 = pos.size();
  const auto pos2 = PointDiscretize::Ellipsoid(
      settings.dr, Vectord(droplet_d), Vectord(0.5 * droplet_d, 0., 0.));
  pos.insert(pos.end(), pos2.begin(), pos2.end());
  std::vector<Vectord> vel(pos.size(), Vectord(0.));
  std::fill(vel.begin(), vel.begin() + n1, Vectord{rel_vel / 2, 0., 0.});
  std::fill(vel.begin() + n1, vel.end(), Vectord{-rel_vel / 2., 0., 0.});
  Particles p(settings, std::move(pos), std::move(vel),
              std::vector<double>(pos.size(), settings.ref_density));

  CaseSetup setup;
  setup.num_outputs = 200;
  setup.sim_time = 0.02;
  setup.output_dir = "./colliding_droplets";
  setup.d = Domain(std::move(p));
  return setup;
}

CaseSetup Cases::SimpleTank(const double tank_resolution,
                            const double tank_height, const double tank_width) {
  // TODO

  CaseSetup s;
  return s;
}