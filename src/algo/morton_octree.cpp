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

#include "morton_octree.hpp"

#include <omp.h>

#include <cassert>
#include <execution>
#include <tuple>

#include "algo/basic_algorithms.hpp"
#include "algo/morton_points.hpp"

template <typename morton_type>
MortonOctree<morton_type>::MortonOctree(GpuVector<morton_type> sorted_mortons) {
  nodes_.reserve(2 * sorted_mortons.size());
  nodes_.resize(1);  // for root node
  GpuVector<mort_id_t> mort_ids(sorted_mortons.size());
  ForEachIndex(
      sorted_mortons.size(),
      [](const SizeT i, const morton_type* sorted_mortons,
         mort_id_t* mort_ids) {
        mort_ids[i] = {sorted_mortons[i] >> 3, i};
      },
      sorted_mortons, mort_ids);

  SizeT start_range = 0, end_range = sorted_mortons.size();

  for (SizeT layer = 0; layer < max_depth; ++layer) {
    Unique(mort_ids);

    const SizeT new_start_range = (mort_ids.size() == 1) ? 0 : nodes_.size(),
                new_end_range = nodes_.size() + mort_ids.size();

    if (mort_ids.size() > 1) {
      nodes_.resize(new_end_range);
    }

    ForEachIndex(
        mort_ids.size(),
        [start_range, end_range, new_start_range](
            const SizeT i, const SizeT num_ids, const mort_id_t* mort_ids,
            const morton_type* sorted_mortons, Node* nodes) {
          const SizeT sidx = mort_ids[i].idx, eidx = (i + 1 == num_ids)
                                                         ? end_range
                                                         : mort_ids[i + 1].idx;
          Node node = DefaultNode();
          for (SizeT j = sidx; j != eidx; ++j) {
            node[sorted_mortons[j - start_range].GetLast3Bits()] = j;
          }
          nodes[i + new_start_range] = node;
        },
        mort_ids.size(), mort_ids, sorted_mortons, nodes_);

    if (mort_ids.size() == 1) {
      depth_ = layer;
      break;
    }

    sorted_mortons.resize(mort_ids.size());

    ForEachIndex(
        mort_ids.size(),
        [new_start_range](const SizeT i, morton_type* sorted_mortons,
                          mort_id_t* mort_ids) {
          const morton_type mort = mort_ids[i].morton;
          sorted_mortons[i] = mort;
          mort_ids[i] = {mort >> 3, i + new_start_range};
        },
        sorted_mortons, mort_ids);

    start_range = new_start_range;
    end_range = new_end_range;
  }
}

template <typename morton_type>
SizeT MortonOctree<morton_type>::At(morton_type m, const Node* nodes,
                                    const SizeT depth) {
  morton_type it = 0;
  for (SizeT i = 0; i < depth; ++i) {
    it = (it << 3) | (m & 7);
    m = m >> 3;
  }
  SizeT cur = nodes[0][(m & 7).value()];
  for (size_t i = 0; i < depth; ++i) {
    if (cur == Invalid()) {
      break;
    }
    cur = nodes[cur][(it & 7).value()];
    it = it >> 3;
  }
  return cur;
}

template <typename morton_type>
SizeT MortonOctree<morton_type>::operator[](morton_type m) const {
  morton_type it = 0;
  for (size_t i = 0; i < depth_; ++i) {
    it = (it << 3) | (m & 7);
    m = m >> 3;
  }
  SizeT cur = nodes_[0][(m & 7).value()];
  for (size_t i = 0; i < depth_; ++i) {
    if (cur == Invalid()) {
      break;
    }
    cur = nodes_[cur][(it & 7).value()];
    it = it >> 3;
  }
  return cur;
}

template class MortonOctree<Morton64>;
template class MortonOctree<Morton32>;