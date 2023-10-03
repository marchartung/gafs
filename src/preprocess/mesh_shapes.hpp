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

#include "utils/types.hpp"
#include "wsph/mesh.hpp"

class MeshShapes {
 public:
  static Mesh Cube(const Vectord dim, Vectord trans) {
    Mesh res;
    using P = Vectord;
    res.vertices() = {P{0., 0., 0.},           // 0
                      P{dim[0], 0., 0.},       // 1
                      P{dim[0], dim[1], 0.},   // 2
                      P{0., dim[1], 0.},       // 3
                      P{0., 0., dim[2]},       // 4
                      P{dim[0], 0., dim[2]},   // 5
                      dim,                     // 6
                      P{0., dim[1], dim[2]}};  // 7
    for (Vectord& v : res.vertices()) {
      v += trans;
    }

    using Q = std::array<SizeT, 4>;

    const std::array<Q, 6> sides = {Q{0, 1, 2, 3}, Q{5, 4, 7, 6},
                                    Q{1, 0, 4, 5}, Q{2, 1, 5, 6},
                                    Q{3, 2, 6, 7}, Q{0, 3, 7, 4}};

    for (const Q& s : sides) {
      res.segments().push_back(Vectoru{s[0], s[1], s[2]});
      res.segments().push_back(Vectoru{s[2], s[3], s[0]});
    }
    return res;
  };
};
