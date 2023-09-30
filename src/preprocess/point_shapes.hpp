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

#include <omp.h>

#include <vector>

#include "utils/types.hpp"

class Vectordiscretize {
 public:
  static std::vector<Vectord> Cube(const double dr, const Vectord dim,
                                   Vectord trans) {
    std::vector<Vectord> res;
    const size_t xe = 0.5 * dim[0] / dr, ye = 0.5 * dim[1] / dr,
                 ze = 0.5 * dim[2] / dr;
    for (size_t x = 0; x < xe; ++x) {
      for (size_t y = 0; y < ye; ++y) {
        for (size_t z = 0; z < ze; ++z) {
          const Vectord p = Vectord{x * dr, y * dr, z * dr} + dr / 2.;
          res.push_back(p);
        }
      }
    }
    for (size_t d = 0; d < 3; ++d) {
      const size_t nhalf = res.size();
      for (size_t i = 0; i < nhalf; ++i) {
        Vectord p = res[i];
        p[d] *= -1.;
        res.push_back(p);
      }
    }
    const Vectord offset = trans;
    for (Vectord& p : res) {
      p += offset;
    }
    return res;
  }
  static std::vector<Vectord> Ellipsoid(const double dr, const Vectord dim,
                                        Vectord trans) {
    std::vector<Vectord> res;
    const Vectord ellips_cooefs = 0.5 * dim;
    const size_t xe = 0.5 * dim[0] / dr, ye = 0.5 * dim[1] / dr,
                 ze = 0.5 * dim[2] / dr;
    for (size_t x = 0; x < dim[0] / dr; ++x) {
      for (size_t y = 0; y < dim[1] / dr; ++y) {
        for (size_t z = 0; z < dim[2] / dr; ++z) {
          const Vectord p = Vectord{x * dr, y * dr, z * dr} + dr / 2.;
          const Vectord d(p[0] / ellips_cooefs[0], p[1] / ellips_cooefs[1],
                          p[2] / ellips_cooefs[2]);
          if (Length(d) <= 1) res.push_back(p);
        }
      }
    }
    for (size_t d = 0; d < 3; ++d) {
      const size_t nhalf = res.size();
      for (size_t i = 0; i < nhalf; ++i) {
        Vectord p = res[i];
        p[d] *= -1.;
        res.push_back(p);
      }
    }
    const Vectord offset = trans;
    for (Vectord& p : res) {
      p += offset;
    }

    return res;
  }
};
