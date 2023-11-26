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

#include <vector>

#include "derivatives.hpp"
#include "domain.hpp"
#include "particles.hpp"
#include "shifting.hpp"
#include "utils/types.hpp"

struct BaseParticlesState {
  std::vector<Vectord> pos;
  std::vector<Vectord> vel;
  std::vector<double> dty;

  void Set(const Particles& p) {
    pos = p.pos();
    vel = p.vel();
    dty = p.dty();
  }
};

class ForwardEuler {
 public:
  ForwardEuler() = default;
  ForwardEuler(const Vectord gravity) : gravity_(gravity) {}

  void TimeStep(const double dt, Domain& d);

 private:
  Vectord gravity_ = Vectord(0.);
  BasicWeaklyRhs rhs_ = BasicWeaklyRhs(0.5);
  Derivative derivative_;
  DpcShifting shifting_;
};

class DualSPHysicsVerletTS {
 public:
  DualSPHysicsVerletTS() = default;
  DualSPHysicsVerletTS(const Vectord gravity) : gravity_(gravity) {}

  void TimeStep(const double dt, Domain& d);

 private:
  void IntegrateFinalStep(const double dt, Domain& d);

  Vectord gravity_;
  BasicWeaklyRhs rhs_ = BasicWeaklyRhs(1.);
  BaseParticlesState init_state_;
  Derivative derivative_;
  DpcShifting shifting_;
};