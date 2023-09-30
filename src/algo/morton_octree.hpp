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
