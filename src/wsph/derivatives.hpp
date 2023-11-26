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

#include "domain.hpp"
#include "particles.hpp"
#include "utils/types.hpp"

struct Derivative {
  SizeT size() const { return acc.size(); }

  void Resize(const size_t n) {
    acc.resize(n);
    dtyD.resize(n);
  }

  void Step(const double dt, const Vectord gravity, Domain& d);

  std::vector<Vectord> acc;
  std::vector<double> dtyD;
};

class BasicWeaklyRhs {
 public:
  BasicWeaklyRhs() = default;
  BasicWeaklyRhs(const double cfl) : cfl_(cfl) {}

  double cfl() const { return cfl_; }

  void Compute(const Domain& d, Derivative& res);
  double ComputeMaxDt(const Domain& d, const Derivative& derivative);

 private:
  void ComputePP(const bool overwrite, const Particles& p, const Particles& np,
                 const SavedNeighborsD& sn, Derivative& res);

  double CourantViscDt(const Particles& p, const SavedNeighborsD& neighbors);

  double ForceDt(const double h, const Derivative& d);

  double cfl_ = 1.1;
};