#include <cmath>
#include <filesystem>
#include <string>
#include <vector>

#include "akinci_solver.hpp"
#include "basic_equations.hpp"
#include "dynamic_boundary.hpp"
#include "file/vtp.hpp"
#include "materials.hpp"
#include "particles.hpp"
#include "preprocess/point_shapes.hpp"
#include "utils/types.hpp"
#include "volume_boundary.hpp"

std::string base_path = "/home/marc/colliding_cubes_no_shifting";

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
  const double droplet_d = 0.00121, rel_vel = 5.13;
  settings.dr = droplet_d / 20.;
  settings.speed_of_sound = 2. * 10. * rel_vel;
  auto pos = Vectordiscretize::Ellipsoid(settings.dr, Vectord(droplet_d),
                                         Vectord(-0.5 * droplet_d, 0., 0.));
  const size_t n1 = pos.size();
  auto pos2 = Vectordiscretize::Ellipsoid(settings.dr, Vectord(droplet_d),
                                          Vectord(0.5 * droplet_d, 0., 0.));
  pos.insert(pos.end(), pos2.begin(), pos2.end());
  std::vector<Vectord> vel(pos.size());
  std::fill(vel.begin(), vel.begin() + n1, Vectord{rel_vel / 2., 0., 0.});
  std::fill(vel.begin() + n1, vel.end(), Vectord{-rel_vel / 2., 0., 0.});
  Particles p(settings, std::move(pos), std::move(vel),
              std::vector<double>(2 * n1, settings.ref_density));
  Write(0, p);
  return p;
}

int main() {
  std::filesystem::create_directories(base_path);
  const size_t num_outputs = 500;
  const double end_time = 0.002, dt = end_time / num_outputs;
  AkinciSolver s(Vectord{0., 0., 0.},
                 CreateParticles(MaterialSettings::Water()), VolumeBoundary());

  Write(0, s.particles());
  for (size_t output = 1; output <= num_outputs; ++output) {
    const double ave_dt = s.TimeStep(dt);
    Write(output, s.particles());
    std::cout << "wrote output " << output << std::endl;
  }

  return 0;
}