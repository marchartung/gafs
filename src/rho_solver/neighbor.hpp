#pragma once

#include <vector>

#include "utils/types.hpp"

template <bool exclude_same_index, typename Functor>
void ForeachInDistance(const double dist, const std::vector<Pointd>& a,
                       const std::vector<Pointd>& b, Functor f) {
  const double dist2 = dist * dist;
#pragma omp parallel
  {
    std::vector<size_t> neighbors;
#pragma omp for schedule(static)
    for (size_t i = 0; i < a.size(); ++i) {
      neighbors.clear();
      for (size_t j = 0; j < b.size(); ++j) {
        if (exclude_same_index && i == j) continue;
        const Pointd rij = a[i] - b[j];
        const double d = rij * rij;
        if (d < dist2 && d > 0.) {
          neighbors.push_back(j);
        }
      }
      f(i, neighbors);
    }
  }
}
template <typename Functor>
void ForeachInDistance(const double dist, const std::vector<Pointd>& a,
                       Functor f) {
  ForeachInDistance<true>(dist, a, a, f);
}
