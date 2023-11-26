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

#include "morton_points.hpp"

#include <omp.h>

#include <iostream>

#include "parstd/parstd.hpp"
#include "utils/types.hpp"

template <typename MortonType>
std::tuple<GpuVector<Vectorf>, GpuVector<MortonType>> CreateCellRefPoints(
    const double cell_size, const GpuVector<Vectord> &points) {
  const double offset = 0.5 * cell_size;
  GpuVector<Vectorf> gpu_Vectorfs(points.size());
  GpuVector<MortonType> gpu_point_mortons(points.size());
  ForEachIndex(
      points.size(),
      [offset, cell_size](const size_t i, const Vectord *pd, Vectorf *pf,
                          MortonType *mortons) {
        const auto coords =
            Cast<typename MortonType::Coords::value_type>(pd[i] / cell_size);
        mortons[i] = MortonType(coords);
        pf[i] = Cast<float>(pd[i] - Cast<double>(coords) * cell_size + offset);
      },
      points, gpu_Vectorfs, gpu_point_mortons);
  return std::make_tuple(std::move(gpu_Vectorfs), std::move(gpu_point_mortons));
}

template <typename morton_type>
MortonPoints<morton_type>::MortonPoints(const double cell_size,
                                        GpuVector<Vectorf> points,
                                        GpuVector<morton_type> cell_mortons,
                                        GpuVector<size_type> cell_starts)
    : cell_size_(cell_size),
      points_(std::move(points)),
      cell_mortons_(std::move(cell_mortons)),
      cell_starts_(std::move(cell_starts)) {}

template <typename morton_type>
std::tuple<GpuVector<typename MortonPoints<morton_type>::size_type>,
           MortonPoints<morton_type>>
MortonPoints<morton_type>::Create(const double cell_size,
                                  const GpuVector<Vectord> &points) {
  auto [unordered_gpu_points, point_mortons] =
      CreateCellRefPoints<morton_type>(cell_size, points);
  GpuVector<mort_id_t> mort_ids(points.size());

  ForEachIndex(
      points.size(),
      [](const size_type i, const morton_type *ms, mort_id_t *mortids) {
        mortids[i] = {ms[i], i};
      },
      point_mortons, mort_ids);

  Sort(mort_ids);

  GpuVector<size_type> idx_map(points.size());
  ForEachIndex(
      points.size(),
      [](const size_type i, mort_id_t *mids, size_type *map) {
        const size_type idx = mids[i].idx;
        map[i] = idx;
        mids[i].idx = i;
      },
      mort_ids, idx_map);
  Unique(mort_ids);

  GpuVector<size_type> cell_starts(mort_ids.size() + 1, points.size());
  GpuVector<morton_type> cell_mortons(mort_ids.size());

  ForEachIndex(
      mort_ids.size(),
      [](const size_type i, const mort_id_t *mids, size_type *cstarts,
         morton_type *cmortons) {
        const morton_type m = mids[i].morton;
        const size_type cs = mids[i].idx;
        cstarts[i] = cs;
        cmortons[i] = m;
      },
      mort_ids, cell_starts, cell_mortons);

  GpuVector<Vectorf> gpu_points(points.size());
  ForEachIndex(
      points.size(),
      [](const size_type i, const size_type *map, const Vectorf *p_src,
         Vectorf *p_trg) {
        const size_type idx = map[i];
        p_trg[i] = p_src[idx];
      },
      idx_map, unordered_gpu_points, gpu_points);
  return std::make_tuple(
      std::move(idx_map),
      MortonPoints(cell_size, std::move(gpu_points), std::move(cell_mortons),
                   std::move(cell_starts)));
}

template <typename morton_type>
GpuVector<Vectord> MortonPoints<morton_type>::Vectords() const {
  const double cell_size = cell_size_;
  const double cell_center_offset(0.5 * cell_size_);
  GpuVector<Vectord> res(points_.size());
  ForEachIndex(
      num_cells(),
      [cell_center_offset, cell_size](
          const size_type i, const morton_type *cell_mortons,
          const size_type *cell_starts, const Vectorf *points, Vectord *res) {
        const auto coords = cell_mortons[i].coords();
        const Vectord cell_offset = Cast<double>(coords) * cell_size;
        for (size_type j = cell_starts[i]; j < cell_starts[i + 1]; ++j) {
          res[j] = (Cast<double>(points[j]) - cell_center_offset) + cell_offset;
        }
      },
      cell_mortons_, cell_starts_, points_, res);
  return res;
}

template class MortonPoints<Morton64>;
template class MortonPoints<Morton32>;