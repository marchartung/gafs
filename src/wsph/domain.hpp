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

#include "dynamic_boundary.hpp"
#include "neighbor/saved_neighbors.hpp"
#include "particles.hpp"

struct Domain {
  Domain() = default;
  Domain(Particles p_in, DynamicBoundary dbc_in)
      : p(std::move(p_in)),
        p_p_neighbors(p.pos()),
        dbc(std::move(dbc_in)),
        p_dbc_neighbors(p.pos(), dbc.pos()) {}
  Domain(Particles p_in) : Domain(std::move(p_in), DynamicBoundary()) {}

  void Update() {
    p.Update();
    p_p_neighbors.Update(p.pos());
  }

  Particles p;
  SavedNeighborsD p_p_neighbors;

  DynamicBoundary dbc;
  SavedNeighborsD p_dbc_neighbors;
};