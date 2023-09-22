
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
      [](const size_type i, const morton_type* sorted_mortons,
         mort_id_t* mort_ids) {
        mort_ids[i] = {sorted_mortons[i] >> 3, i};
      },
      sorted_mortons, mort_ids);

  size_type start_range = 0, end_range = sorted_mortons.size();

  for (size_type layer = 0; layer < max_depth; ++layer) {
    Unique(mort_ids);

    const size_type new_start_range =
                        (mort_ids.size() == 1) ? 0 : nodes_.size(),
                    new_end_range = nodes_.size() + mort_ids.size();

    if (mort_ids.size() > 1) {
      nodes_.resize(new_end_range);
    }

    ForEachIndex(
        mort_ids.size(),
        [start_range, end_range, new_start_range](
            const size_type i, const size_type num_ids,
            const mort_id_t* mort_ids, const morton_type* sorted_mortons,
            Node* nodes) {
          const size_type sidx = mort_ids[i].idx,
                          eidx = (i + 1 == num_ids) ? end_range
                                                    : mort_ids[i + 1].idx;
          Node node = DefaultNode();
          for (size_type j = sidx; j != eidx; ++j) {
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
        [new_start_range](const size_type i, morton_type* sorted_mortons,
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
typename MortonOctree<morton_type>::size_type MortonOctree<morton_type>::At(
    morton_type m, const Node* nodes, const size_type depth) {
  morton_type it = 0;
  for (size_t i = 0; i < depth; ++i) {
    it = (it << 3) | (m & 7);
    m = m >> 3;
  }
  size_type cur = nodes[0][(m & 7).value()];
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
typename MortonOctree<morton_type>::size_type
MortonOctree<morton_type>::operator[](morton_type m) const {
  morton_type it = 0;
  for (size_t i = 0; i < depth_; ++i) {
    it = (it << 3) | (m & 7);
    m = m >> 3;
  }
  size_type cur = nodes_[0][(m & 7).value()];
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