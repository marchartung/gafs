#include <cmath>
#include <filesystem>
#include <string>
#include <vector>

#include "basic_equations.hpp"
#include "dynamic_boundary.hpp"
#include "file/vtp.hpp"
#include "materials.hpp"
#include "neighbor.hpp"
#include "particles.hpp"
#include "preprocess/point_shapes.hpp"
#include "std_rhs.hpp"
#include "utils/types.hpp"

void Write(const size_t output_num, const Particles& p) {
  VTP vtp;
  vtp.SetPoints(p.pos());
  vtp.AddPointData("velocity", p.vel().data(), p.size());
  vtp.AddPointData("density", p.dty().data(), p.size());

  const std::filesystem::path path =
      "/home/marc/test/fluid_cube_" + std::to_string(output_num) + ".vtp";
  vtp.Export(path);
}

void Write(const size_t output_num, const DynamicBoundary& b) {
  VTP vtp;
  vtp.SetPoints(b.pos());
  vtp.AddPointData("density", b.dty().data(), b.size());
  const std::filesystem::path path =
      "/home/marc/test/wall_cube_" + std::to_string(output_num) + ".vtp";
  vtp.Export(path);
}

Particles CreateParticles(const MaterialSettings& settings) {
  return Particles(settings,
                   PointDiscretize::Cube(settings.dr, Pointd(1.), Pointd(0.)));
}

DynamicBoundary CreateBoundary(const double thickness,
                               const MaterialSettings& settings) {
  return DynamicBoundary(
      settings, PointDiscretize::Cube(
                    settings.dr, Pointd(2., 2., thickness),
                    Pointd(-0.5, -0.5, -thickness - settings.dr * 0.5)));
}

int main() {
  Particles p = CreateParticles(MaterialSettings::Water());
  DynamicBoundary b = CreateBoundary(2. * p.h(), MaterialSettings::Wall());
  BasicWSphRhs rhs(Pointd{0., 0., -9.8});

  rhs.ComputeDensityWall(p, b);

  Write(0, p);
  Write(0, b);

  const size_t num_sub_steps = 100, num_outputs = 100;
  const double end_time = 1.;
  const double dt = end_time / num_outputs;
  std::cout << "dt: " << dt / num_sub_steps << "\n";
  for (size_t output = 1; output <= num_outputs; ++output) {
    for (size_t sub_step = 0; sub_step < num_sub_steps; ++sub_step) {
      rhs.Compute(p, b);
      rhs.Update(dt / num_sub_steps, p, b);
    }
    std::cout << "writing output " << output << std::endl;
    Write(output, p);
    Write(output, b);
  }

  return 0;
}