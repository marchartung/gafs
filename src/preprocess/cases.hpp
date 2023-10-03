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

#include <string>
#include <vector>

#include "point_shapes.hpp"
#include "utils/types.hpp"
#include "wsph/domain.hpp"
#include "wsph/materials.hpp"
#include "wsph/particles.hpp"

struct CaseSetup {
  double output_dt() const { return sim_time / num_outputs; }

  double sim_time = 0.;
  size_t num_outputs = 0;
  std::string output_dir = "./";

  Vectord gravity = 0.;
  Domain d;
};

class Cases {
 public:
  static CaseSetup CollidingDroplets(const double droplet_resolution = 10.,
                                     const double relative_velocity = 5.);

  static CaseSetup SimpleTank(const double wall_density = 1000.,
                              const double tank_resolution = 10.,
                              const double tank_height = 0.5,
                              const double tank_width = 0.5);

  CaseSetup Dambreak(const double resolution = 10.);
};
