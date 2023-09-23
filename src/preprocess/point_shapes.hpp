#pragma once

#include <omp.h>

#include <vector>

#include "utils/types.hpp"

class PointDiscretize {
 public:
  static std::vector<Pointd> Cube(const double dr, const Pointd dim,
                                  Pointd trans) {
    const double offset = dr / 2.;
    std::vector<Pointd> res;
    Pointd cur;
    for (size_t x = 0; x < dim[0] / dr; ++x) {
      cur[0] = offset + x * dr;
      for (size_t y = 0; y < dim[1] / dr; ++y) {
        cur[1] = offset + y * dr;
        for (size_t z = 0; z < dim[2] / dr; ++z) {
          cur[2] = offset + z * dr;
          res.push_back(cur + trans);
        }
      }
    }
    return res;
  }
};
