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
