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

#include <iostream>

#include "io/write_domain.hpp"
#include "preprocess/cases.hpp"
#include "preprocess/mesh_shapes.hpp"
#include "preprocess/solid_discretize.hpp"
#include "utils/types.hpp"
#include "wsph/time_stepping.hpp"

int main() {
  CaseSetup setup = Cases::SimpleTank(100.);
  std::filesystem::create_directories(setup.output_dir);
  DualSPHysicsVerletTS ts(setup.gravity);

  Write(0, setup.output_dir, setup.d);
  for (size_t output = 1; output <= setup.num_outputs; ++output) {
    ts.TimeStep(setup.output_dt(), setup.d);
    Write(output, setup.output_dir, setup.d);
  }

  return 0;
}