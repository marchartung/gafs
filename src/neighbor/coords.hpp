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

#include <array>
#include <cstdint>

#include "utils/types.hpp"

class Coords : public Array<int32_t, 3> {
  using Base = Array<int32_t, 3>;

 public:
  static constexpr std::array<Coords, 27> NeighborCoords() {
    using C = Coords;
    return std::array<C, 27>{
        C{-1, 0, 0},   C{0, 0, 0},   C{1, 0, 0},   C{-1, -1, 0}, C{0, -1, 0},
        C{1, -1, 0},   C{-1, 0, -1}, C{0, 0, -1},  C{1, 0, -1},  C{-1, 1, 0},
        C{0, 1, 0},    C{1, 1, 0},   C{-1, 0, 1},  C{0, 0, 1},   C{1, 0, 1},

        C{-1, -1, -1}, C{0, -1, -1}, C{1, -1, -1}, C{-1, -1, 1}, C{0, -1, 1},
        C{1, -1, 1},   C{-1, 1, -1}, C{0, 1, -1},  C{1, 1, -1},  C{-1, 1, 1},
        C{0, 1, 1},    C{1, 1, 1},
    };
  }

  Coords() = default;
  constexpr Coords(const Base b) : Base(b) {}
  constexpr Coords(const int32_t x, const int32_t y, const int32_t z)
      : Base(x, y, z) {}
  constexpr Coords(const double cell_size, const Vectord& point)
      : Coords(point[0] / cell_size, point[1] / cell_size,
               point[2] / cell_size) {}
};
