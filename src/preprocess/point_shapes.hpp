#pragma once

#include "types.hpp"
#include <omp.h>
#include <vector>

namespace shape {
std::vector<Pointd>
Cube(const size_t n_per_dim, const double dr)
{
  std::vector<Pointd> res(n_per_dim * n_per_dim * n_per_dim);
#pragma omp parallel for schedule(static)
  for (size_t x = 0; x < n_per_dim; ++x)
    for (size_t y = 0; y < n_per_dim; ++y)
      for (size_t z = 0; z < n_per_dim; ++z) {
        res[n_per_dim * n_per_dim * x + n_per_dim * y + z] = { dr * x,
                                                               dr * y,
                                                               dr * z };
      }
  return res;
}
}