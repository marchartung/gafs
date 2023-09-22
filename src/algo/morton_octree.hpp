#pragma once

#include <array>
#include <limits>
#include <numeric>
#include <vector>

#include "morton.hpp"

template <typename morton_type = Morton64>
class MortonOctree {
 public:
  using size_type = uint32_t;
  using Node = std::array<size_type, 8>;
  using mort_id_t = MortIdx<morton_type>;

  static constexpr size_t max_depth = 8 * sizeof(morton_type) / 3;

  static constexpr size_type Invalid() {
    return std::numeric_limits<size_type>::max();
  }

  static size_type At(const morton_type m, const Node* nodes,
                      const size_type depth);

  MortonOctree() = default;
  MortonOctree(GpuVector<morton_type> sorted_mortons);

  size_type operator[](const morton_type m) const;

  const GpuVector<Node>& nodes() const { return nodes_; }
  size_type depth() const { return depth_; }

 private:
  static constexpr Node DefaultNode() {
    return Node{Invalid(), Invalid(), Invalid(), Invalid(),
                Invalid(), Invalid(), Invalid(), Invalid()};
  }
  size_type depth_ = 0;
  GpuVector<Node> nodes_;
};
