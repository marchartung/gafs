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

#include "write_domain.hpp"

#include <iostream>

#include "file/vtp.hpp"
#include "wsph/dynamic_boundary.hpp"
#include "wsph/particles.hpp"

void Write(const size_t output_num, const std::filesystem::path output_dir,
           const Particles& p) {
  VTP vtp;
  vtp.SetPoints(p.pos());
  vtp.AddPointData("velocity", p.vel().data(), p.size());
  vtp.AddPointData("density", p.dty().data(), p.size());

  const std::filesystem::path path =
      output_dir / ("fluid_" + std::to_string(output_num) + ".vtp");
  vtp.Export(path);
}

void Write(const size_t output_num, const std::filesystem::path output_dir,
           const DynamicBoundary& b) {
  VTP vtp;
  vtp.SetPoints(b.pos());
  const std::filesystem::path path =
      output_dir / ("solid_" + std::to_string(output_num) + ".vtp");
  vtp.Export(path);
}

void Write(const size_t output_num, const std::filesystem::path output_dir,
           const Mesh& m) {
  VTP vtp;
  vtp.SetMesh(m.vertices(), m.segments());
  const std::filesystem::path path =
      output_dir / ("mesh" + std::to_string(output_num) + ".vtp");
  vtp.Export(path);
}

void Write(const size_t output_num, const std::filesystem::path output_dir,
           const Domain& d) {
  Write(output_num, output_dir, d.p);
  Write(output_num, output_dir, d.dbc);
  Write(output_num, output_dir, d.m);
  std::cout << "wrote output " << output_num << " num particles: " << d.p.size()
            << " num dpc: " << d.dbc.size() << std::endl;
}