#pragma once

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

#include <iterator>
#include <type_traits>

template <typename IndexType>
class IndexIterator {
 public:
  IndexIterator() = default;
  IndexIterator(const IndexType idx) : idx_(idx) {}

  bool operator==(const IndexIterator<IndexType> it) const {
    return idx_ == it.idx_;
  }
  bool operator!=(const IndexIterator<IndexType> it) const {
    return idx_ == it.idx_;
  }

  IndexType operator*() const { return idx_; }
  IndexType& operator*() { return idx_; }

  IndexIterator<IndexType>& operator++() {
    ++idx_;
    return *this;
  }
  IndexIterator<IndexType> operator++(int) {
    IndexIterator<IndexType> res = *this;
    ++(*this);
    return res;
  }

  IndexIterator<IndexType>& operator--() {
    --idx_;
    return *this;
  }
  IndexIterator<IndexType> operator--(int) {
    IndexIterator<IndexType> res = *this;
    --(*this);
    return res;
  }

  IndexIterator<IndexType>& operator+=(const IndexType n) {
    idx_ += n;
    return *this;
  }
  IndexIterator<IndexType> operator+(const IndexType n) {
    IndexIterator<IndexType> res = *this;
    return res += n;
  }

  IndexIterator<IndexType>& operator-=(const IndexType n) {
    idx_ -= n;
    return *this;
  }
  IndexIterator<IndexType> operator-(const IndexType n) {
    IndexIterator<IndexType> res = *this;
    return res -= n;
  }

 private:
  IndexType idx_;
};

template <typename IndexType>
class IndexRange {
 public:
  using value_type = IndexType;
  using const_iterator = IndexIterator<IndexType>;

  IndexRange() = default;
  IndexRange(const IndexType first, const IndexType last)
      : sidx_(first), eidx_(last) {
    static_assert(std::is_integral_v<IndexType>,
                  "IndexRange can only be used with integral types");
  }
  IndexRange(const IndexType last) : IndexRange<IndexType>(0, last) {}

  IndexType size() const { return eidx_ - sidx_; }
  bool empty() const { return sidx_ == eidx_; }

  const_iterator begin() const { return const_iterator(sidx_); }
  const_iterator end() const { return const_iterator(eidx_); }

  IndexType operator[](const IndexType idx) const { return sidx_ + idx; }

 private:
  IndexType sidx_ = 0;
  IndexType eidx_ = 0;
};

template <typename IteratorType>
class IteratorRange {
 public:
  using size_type = uint32_t;
  IteratorRange() = default;
  IteratorRange(const IteratorType first, const IteratorType last)
      : first_(first), last_(last) {}

  template <typename Container>
  IteratorRange(Container& c)
      : IteratorRange<IteratorType>(c.begin(), c.end()) {}

  size_type size() const {
    using std::distance;
    return distance(first_, last_);
  }

  bool empty() const { return first_ == last_; }

  IteratorType begin() const { return first_; }
  IteratorType end() const { return last_; }

  auto operator[](const size_type idx) const { return *(first_ + idx); }
  auto& operator[](const size_type idx) { return *(first_ + idx); }

 private:
  IteratorType first_;
  IteratorType last_;
};