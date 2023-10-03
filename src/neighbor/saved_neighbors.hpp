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

#include "point_cell_list.hpp"
#include "utils/math.hpp"
#include "utils/span.hpp"
#include "utils/types.hpp"

class SavedNeighborsD {
 public:
  SavedNeighborsD() = default;

  SavedNeighborsD(const PointCellListD& point_list) {
    RecomputeNeighbors<true>(point_list, point_list);
  }

  SavedNeighborsD(const PointCellListD& src_list,
                  const PointCellListD& trg_list) {
    RecomputeNeighbors<false>(src_list, trg_list);
  }

  Span<const SizeT> neighbors(const SizeT idx) const {
    return Span<const SizeT>(neighbors_[idx].data(), num_active_[idx]);
  }

  void Update(const PointCellListD& point_list);

  void Update(const PointCellListD& src_list, const PointCellListD& trg_list);

 private:
  void SetActiveNeighbors(const PointCellListD& src_list,
                          const PointCellListD& trg_list);

  template <bool IsSameList>
  void RecomputeNeighbors(const PointCellListD& src_list,
                          const PointCellListD& trg_list);

  size_t src_update_version_ = 0;
  size_t trg_update_version_ = 0;
  std::vector<std::vector<SizeT>> neighbors_;
  std::vector<SizeT> num_active_;
};