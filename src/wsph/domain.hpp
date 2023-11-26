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

#include "mesh.hpp"
#include "neighbor/position_tracker.hpp"
#include "neighbor/saved_neighbors.hpp"
#include "neighbor/verlet_neighbors.hpp"
#include "particle_boundary.hpp"
#include "particles.hpp"

struct Domain {
  Domain() = default;
  Domain(Particles p_in, ParticleBoundary pb_in)
      : p(std::move(p_in)),
        p_p_neighbors(p.pos()),
        pb(std::move(pb_in)),
        p_pb_neighbors(p.pos(), pb.pos()) {}

  Domain(Particles p_in) : Domain(std::move(p_in), ParticleBoundary()) {}

  void SetFluidPos(const SizeT part_id, const Vectord pos) {
    p.pos(part_id) = pos;
  }

  void Update() {
    p.Update();
    p_p_neighbors = SavedNeighborsD(p.pos());
    if (pb.size() > 0) {
      p_pb_neighbors = SavedNeighborsD(p.pos(), pb.pos());
    }
    fluid_pos_tracker.Reset(p.size());
  }

  double verlet_factor = 1.2;

  Particles p;
  PositionTracker fluid_pos_tracker;
  SavedNeighborsD p_p_neighbors;

  Mesh m;
  ParticleBoundary pb;
  SavedNeighborsD p_pb_neighbors;
};