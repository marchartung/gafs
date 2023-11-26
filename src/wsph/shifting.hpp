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

#include "basic_equations.hpp"
#include "domain.hpp"

// From Jandaghian et al, 2021: Stability and accuracy of the
// weakly compressible SPH with particle regularization techniques

class NoShifting {
 public:
  NoShifting() = default;

  void Compute(const Domain& d) {}

  void Apply(const double dt, Domain& d) {}
};

class DpcShifting {
 public:
  DpcShifting() = default;

  void Compute(const Domain& d);

  void Apply(const double dt, Domain& d);

 private:
  void ComputePP(const bool overwrite, const Particles& p, const Particles& np,
                 const SavedNeighborsD& sn);

  double prs_min_ = 0;
  double prs_max_ = std::numeric_limits<double>::max();
  std::vector<Vectord> collision_term_;
  std::vector<Vectord> repulsive_term_;
};

class LindShifting {
 public:
  LindShifting() = default;

  void Compute(const Domain& d);

  void Apply(const double dt, Domain& d);

 private:
  void ComputePP(const bool overwrite, const Particles& p, const Particles& np,
                 const SavedNeighborsD& sn);

  static constexpr double A = 2.;
  static constexpr double A_fst = 2.75;
  static constexpr double A_fsm = 3.;

  std::vector<Vectord> delta_r_;
};