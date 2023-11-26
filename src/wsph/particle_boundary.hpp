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

#include "materials.hpp"
#include "particles.hpp"

class ParticleBoundary : public Particles {
 public:
  ParticleBoundary() = default;
  ParticleBoundary(const MaterialSettings& s, std::vector<Vectord> pos,
                   std::vector<Vectord> normals, std::vector<Vectord> vel)
      : Particles(s, std::move(pos), std::move(vel)) {
    normal_ = ApplyIndexMap(idx_map(), std::move(normals));
  }

  void Interpolate(const Particles& p);

  const std::vector<Vectord>& normal() const { return normal_; }

  const Vectord& normal(const SizeT idx) const { return normal_[idx]; }
  Vectord& normal(const SizeT idx) { return normal_[idx]; }

 private:
  std::vector<Vectord> normal_;
};