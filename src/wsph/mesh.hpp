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
      : vertices_(std::move(vertices)), segments_(std::move(segments)) {
    ComputeNormals();
  }

  void FlipNormals() {
    for (Vectoru& s : segments_) {
      std::swap(s[0], s[1]);
    }
    ComputeNormals();
  }

  SizeT size() const { return segments_.size(); }

  Segment operator[](const size_t idx) const {
    const auto s = segments_[idx];
    return {vertices_[s[0]], vertices_[s[1]], vertices_[s[2]]};
  }

  void ComputeNormals() {
    normals_.clear();
    for (size_t i = 0; i < segments_.size(); ++i) {
      const Segment seg = operator[](i);
      const Vectord n = CrossProduct(seg[1] - seg[0], seg[2] - seg[0]);
      normals_.push_back(n / Length(n));
    }
  }

  const std::vector<Vectord>& vertices() const { return vertices_; }
  std::vector<Vectord>& vertices() { return vertices_; }

  const std::vector<Vectoru>& segments() const { return segments_; }
  std::vector<Vectoru>& segments() { return segments_; }

  const std::vector<Vectord>& normal() const { return normals_; }
  const Vectord& normal(const SizeT idx) const { return normals_[idx]; }

 private:
  std::vector<Vectord> vertices_;
  std::vector<Vectoru> segments_;
  std::vector<Vectord> normals_;
};