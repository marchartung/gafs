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

#include "parstd/ranges.hpp"
#include "point_cell_list.hpp"
#include "utils/math.hpp"
#include "utils/types.hpp"

class SavedNeighborsD {
 public:
  using ConstRange = IteratorRange<const SizeT*>;
  using Range = IteratorRange<SizeT*>;

  SavedNeighborsD() = default;

  SavedNeighborsD(const PointCellListD& point_list);

  SavedNeighborsD(const PointCellListD& src_list,
                  const PointCellListD& trg_list);

  SizeT size() const { return neighbors_.size(); }

  ConstRange neighbors(const SizeT idx) const {
    return ConstRange(neighbors_[idx].data(),
                      neighbors_[idx].data() + neighbors_[idx].size());
  }
  Range neighbors(const SizeT idx) {
    return Range(neighbors_[idx].data(),
                 neighbors_[idx].data() + neighbors_[idx].size());
  }

  ConstRange operator[](const SizeT idx) const { return neighbors(idx); }
  Range operator[](const SizeT idx) { return neighbors(idx); }

  void Update(const PointCellListD& point_list);
  void Update(const PointCellListD& src_list, const PointCellListD& trg_list);

 private:
  template <bool IsSameList>
  void RecomputeNeighbors(const PointCellListD& src_list,
                          const PointCellListD& trg_list);

  std::vector<std::vector<SizeT>> neighbors_;
};