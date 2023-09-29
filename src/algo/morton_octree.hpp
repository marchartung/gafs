#pragma once

#include <array>
#include <limits>
#include <numeric>
#include <vector>

#include "morton.hpp"
#include "utils/types.hpp"

template <typename morton_type = Morton64>
class MortonOctree {
 public:
  using Node = std::array<SizeT, 8>;
  using mort_id_t = MortIdx<morton_type>;

  static constexpr SizeT max_depth = 8 * sizeof(morton_type) / 3;

  static constexpr SizeT Invalid() { return std::numeric_limits<SizeT>::max(); }

  static SizeT At(const morton_type m, const Node* nodes, const SizeT depth);

  MortonOctree() = default;
  MortonOctree(GpuVector<morton_type> sorted_mortons);

  SizeT operator[](const morton_type m) const;

  const GpuVector<Node>& nodes() const { return nodes_; }
  SizeT depth() const { return depth_; }

 private:
  static constexpr Node DefaultNode() {
    return Node{Invalid(), Invalid(), Invalid(), Invalid(),
                Invalid(), Invalid(), Invalid(), Invalid()};
  }
  SizeT depth_ = 0;
  GpuVector<Node> nodes_;
};
