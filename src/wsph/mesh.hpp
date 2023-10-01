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
#include <vector>

#include "utils/types.hpp"

using Segment = std::array<Vectord, 3>;

class Mesh {
 public:
  Mesh() = default;
  Mesh(std::vector<Vectord> points, std::vector<std::array<size_t, 3>> segments)
      : points_(std::move(points)), segments_(std::move(segments)) {}

  size_t size() const { return segments_.size(); }

  Segment operator[](const size_t idx) const {
    const auto s = segments_[idx];
    return {points_[s[0]], points_[s[1]], points_[s[2]]};
  }

 private:
  std::vector<Vectord> points_;
  std::vector<std::array<size_t, 3>> segments_;
};