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
  Mesh(std::vector<Vectord> vertices, std::vector<Vectoru> segments)
      : vertices_(std::move(vertices)), segments_(std::move(segments)) {}

  void FlipNormals() {
    for (Vectoru& s : segments_) {
      std::swap(s[0], s[1]);
    }
  }

  SizeT size() const { return segments_.size(); }

  Segment operator[](const size_t idx) const {
    const auto s = segments_[idx];
    return {vertices_[s[0]], vertices_[s[1]], vertices_[s[2]]};
  }

  const std::vector<Vectord>& vertices() const { return vertices_; }
  std::vector<Vectord>& vertices() { return vertices_; }

  const std::vector<Vectoru>& segments() const { return segments_; }
  std::vector<Vectoru>& segments() { return segments_; }

 private:
  std::vector<Vectord> vertices_;
  std::vector<Vectoru> segments_;
};