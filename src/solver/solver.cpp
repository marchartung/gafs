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

#include <cmath>
#include <filesystem>
#include <string>
#include <vector>

#include "basic_equations.hpp"
#include "domain.hpp"
#include "dynamic_boundary.hpp"
#include "file/vtp.hpp"
#include "materials.hpp"
#include "particles.hpp"
#include "preprocess/point_shapes.hpp"
#include "time_stepping.hpp"
#include "utils/types.hpp"
#include "volume_boundary.hpp"

std::string base_path = "/home/marc/colliding_droplets";

void Write(const size_t output_num, const Particles& p) {
  VTP vtp;
  vtp.SetPoints(p.pos());
  vtp.AddPointData("velocity", p.vel().data(), p.size());
  vtp.AddPointData("density", p.dty().data(), p.size());

  const std::filesystem::path path =
      base_path + "/fluid_" + std::to_string(output_num) + ".vtp";
  vtp.Export(path);
}

void Write(const size_t output_num, const DynamicBoundary& b) {
  // VTP vtp;
  // vtp.SetPoints(b.pos());
  // vtp.AddPointData("density", b.dty().data(), b.size());
  // const std::filesystem::path path =
  //     base_path + "/wall_cube_" + std::to_string(output_num) + ".vtp";
  // vtp.Export(path);
}

void Write(const size_t output_num, const VolumeBoundary& b) {
  // VTP vtp;
  // vtp.SetPoints(b.pos());
  // vtp.AddPointData("volume", b.vol().data(), b.size());
  // const std::filesystem::path path =
  //     base_path + "/wall_plane_" + std::to_string(output_num) + ".vtp";
  // vtp.Export(path);
}

Particles CreateParticles(MaterialSettings settings) {
  const double droplet_d = 0.005, rel_vel = 5.;
  settings.dr = droplet_d / 20.;
  settings.speed_of_sound = 2. * 10. * rel_vel;
  auto pos = Vectordiscretize::Ellipsoid(settings.dr, Vectord(droplet_d),
                                         Vectord(-0.5 * droplet_d, 0., 0.));
  const size_t n1 = pos.size();
  const auto pos2 = Vectordiscretize::Ellipsoid(
      settings.dr, Vectord(droplet_d), Vectord(0.5 * droplet_d, 0., 0.));
  pos.insert(pos.end(), pos2.begin(), pos2.end());
  std::vector<Vectord> vel(pos.size(), Vectord(0.));
  std::fill(vel.begin(), vel.begin() + n1, Vectord{rel_vel / 2, 0., 0.});
  std::fill(vel.begin() + n1, vel.end(), Vectord{-rel_vel / 2., 0., 0.});
  Particles p(settings, std::move(pos), std::move(vel),
              std::vector<double>(pos.size(), settings.ref_density));
  Write(0, p);
  return p;
}

Domain CreateDomain() {
  return Domain(CreateParticles(MaterialSettings::Water()));
}

int main() {
  std::filesystem::create_directories(base_path);
  const size_t num_outputs = 200;
  const double end_time = 0.02, dt = end_time / num_outputs;

  Domain d = CreateDomain();
  DualSPHysicsVerletTS ts(Vectord{0., 0., 0.});

  Write(0, d.p);
  for (size_t output = 1; output <= num_outputs; ++output) {
    ts.TimeStep(dt, d);
    Write(output, d.p);
    std::cout << "wrote output " << output << std::endl;
  }

  return 0;
}