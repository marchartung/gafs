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

#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include "algo/morton.hpp"
#include "neighbor/coords.hpp"
#include "utils/macros.hpp"
#include "utils/math.hpp"
#include "utils/types.hpp"
#include "wsph/mesh.hpp"

DEVICE Vectord ClosestPoint(Vectord const& p, const Segment& s) {
  const Vectord &a = s[0], &b = s[1], &c = s[2];
  const Vectord ab = b - a;
  const Vectord ac = c - a;
  const Vectord ap = p - a;

  const double d1 = ab * ap;
  const double d2 = ac * ap;
  if (d1 <= 0.f && d2 <= 0.f) return a;  // #1

  const Vectord bp = p - b;
  const double d3 = (ab * bp);
  const double d4 = (ac * bp);
  if (d3 >= 0.f && d4 <= d3) return b;  // #2

  const Vectord cp = p - c;
  const double d5 = (ab * cp);
  const double d6 = (ac * cp);
  if (d6 >= 0.f && d5 <= d6) return c;  // #3

  const double vc = d1 * d4 - d3 * d2;
  if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f) {
    const double v = d1 / (d1 - d3);
    return a + v * ab;  // #4
  }

  const double vb = d5 * d2 - d1 * d6;
  if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f) {
    const double v = d2 / (d2 - d6);
    return a + v * ac;  // #5
  }

  const double va = d3 * d6 - d5 * d4;
  if (va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f) {
    const double v = (d4 - d3) / ((d4 - d3) + (d5 - d6));
    return b + v * (c - b);  // #6
  }

  const double denom = 1.f / (va + vb + vc);
  const double v = vb * denom;
  const double w = vc * denom;
  return a + v * ab + w * ac;  // #0
}

DEVICE Vectord Normal(const Segment& s) {
  return Normal(s[1] - s[0], s[2] - s[1]);
}

DEVICE double Distance(Vectord const& p, const Segment& s) {
  return Length(ClosestPoint(p, s) - p);
}

DEVICE double SignedDistance(Vectord const& p, const Segment& s) {
  const double d = Distance(p, s);
  if ((p - s[0]) * Normal(s) < 0.) {
    return -d;
  } else
    return d;
}

class SolidDiscretize {
 public:
  static std::tuple<std::vector<Vectord>, std::vector<Vectord>> Discretize(
      const SizeT nlayers, const double dr, const Mesh& mesh) {
    const double cell_size = nlayers * dr;
    Vectord dmin(std::numeric_limits<double>::max());

    for (const auto& v : mesh.vertices()) {
      dmin = Min(v, dmin);
    }
    dmin += 4. * cell_size;
    std::unordered_set<uint64_t> cells;
    for (SizeT i = 0; i < mesh.size(); ++i) {
      const Segment s = mesh[i];
      const Vectord l1 = s[1] - s[0], l2 = s[2] - s[0];
      const double n =
          std::ceil(std::max(Length(l1) / cell_size, Length(l2) / cell_size));
      const Vectord d1 = l1 / n, d2 = l2 / n;
      for (SizeT k = 0; k <= n; ++k)
        for (SizeT l = 0; l <= n - k; ++l) {
          const Vectord p = (s[0] + dmin) + (k * d1 + l * d2);
          const Coords c = Cast<int32_t>(p / cell_size);
          for (const auto d : Coords::NeighborCoords()) {
            const auto m = Morton64(c + d).value();
            cells.insert(m);
          }
        }
    }
    std::vector<uint64_t> cell_mortons(cells.begin(), cells.end());
    const SizeT pos_per_cell = math::tpow<3>(nlayers);
    std::vector<Vectord> pos(cell_mortons.size() * pos_per_cell);
#pragma omp parallel for schedule(static)
    for (SizeT ci = 0; ci < cell_mortons.size(); ++ci) {
      Coords c = Morton64(cell_mortons[ci]).coords();
      const Vectord offset =
          Vectord(c[0] * cell_size, c[1] * cell_size, c[2] * cell_size) - dmin +
          0.5 * dr;
      SizeT padded = 0;
      for (SizeT x = 0; x < nlayers; ++x) {
        for (SizeT y = 0; y < nlayers; ++y) {
          for (SizeT z = 0; z < nlayers; ++z) {
            pos[ci * pos_per_cell + padded] =
                offset + Vectord(x * dr, y * dr, z * dr);
            ++padded;
          }
        }
      }
    }

    SizeT j = 0;
    std::vector<Vectord> normals;
    for (SizeT i = 0; i < pos.size(); ++i) {
      double min_dist = std::numeric_limits<double>::max();
      Vectord n(0.);
      for (SizeT mi = 0; mi < mesh.size(); ++mi) {
        const Segment s = mesh[mi];
        const double sdist = -SignedDistance(pos[i], s);
        if (sdist <= 0.) continue;
        if (min_dist > sdist) {
          min_dist = sdist;
          n = mesh.normal(mi);
        }
      }
      if (min_dist <= cell_size) {
        pos[j++] = pos[i];
        normals.push_back(n);
      }
    }
    pos.resize(j);
    return std::make_tuple(std::move(pos), std::move(normals));
  }

  static std::tuple<std::vector<Vectord>, std::vector<Vectord>> DiscretizeNew(
      const SizeT nlayers, const double dr, const Mesh& mesh) {
    struct CellInfo {
      SizeT layer_num = std::numeric_limits<SizeT>::max();
      SizeT seg_i = std::numeric_limits<SizeT>::max();
    };

    Vectord dmin(std::numeric_limits<double>::max());
    for (const auto& v : mesh.vertices()) {
      dmin = Min(v, dmin);
    }
    dmin += 4. * nlayers * dr;
    std::unordered_map<uint64_t, CellInfo> cells;
    for (SizeT i = 0; i < mesh.size(); ++i) {
      const Segment s = mesh[i];
      const Vectord normal = mesh.normal(i);
      const Vectord l1 = s[1] - s[0], l2 = s[2] - s[0];
      const double n = std::ceil(std::max(Length(l1) / dr, Length(l2) / dr));
      const Vectord d1 = l1 / n, d2 = l2 / n;
      for (SizeT k = 0; k <= n; ++k)
        for (SizeT l = 0; l <= n - k; ++l) {
          for (SizeT cur_layer = 0; cur_layer < nlayers; ++cur_layer) {
            const Vectord p =
                s[0] - ((0.5 + cur_layer) * dr * normal) + (k * d1 + l * d2);
            const Coords c = Cast<int32_t>((p + dmin) / dr);
            const Morton64 m(c);
            CellInfo& ci = cells[m.value()];
            if (ci.layer_num > cur_layer) {
              ci.layer_num = cur_layer;
              ci.seg_i = i;
            }
          }
        }
    }

    std::vector<Vectord> pos, normals;
    for (auto [k, v] : cells) {
      Vectord p = Cast<double>(Morton64(k).coords()) * dr - dmin;
      pos.push_back(p);
      normals.push_back(mesh.normal(v.seg_i));
    }
    return std::make_tuple(std::move(pos), std::move(normals));
  }
};