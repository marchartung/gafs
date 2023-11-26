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

#include <iterator>
#include <memory>
#include <type_traits>

template <typename T, typename Allocator = std::allocator<T>>
class CpuStdVector {
  using MemoryContainer = std::unique_ptr<T>;

 public:
  using iterator = T*;
  using const_iterator = const T*;

  CpuStdVector() = default;
  ~CpuStdVector() { clear(); }

  CpuStdVector(const size_t n) { resize(n); }
  CpuStdVector(const size_t n, const T value) { resize(n, value); }

  CpuStdVector<T>& operator=(const CpuStdVector<T>& in) {
    if constexpr (!std::is_trivially_destructible_v<T>) {
      clear();
    }
    reserve(in.sz_);
    if (cap_ < in.sz_) {
    }
    std::uninitialized_copy(in.begin(), in.end(), data());
    return *this;
  }
  CpuStdVector<T>& operator=(CpuStdVector<T>&& in) {
    std::swap(in.sz_, sz_);
    std::swap(in.cap_, cap_);
    std::swap(in.mem_, mem_);
    return *this;
  }

  size_t size() const { return sz_; };
  size_t capacity() const { return cap_; }

  const T* data() const { return mem_.get(); }
  T* data() { return mem_.get(); }

  const_iterator begin() const { return mem_.get(); }
  iterator begin() { return mem_.get(); }

  const_iterator end() const { return mem_.get() + sz_; }
  iterator end() { return mem_.get() + sz_; }

  const T& operator[](const size_t idx) const { return data()[idx]; }
  T& operator[](const size_t idx) { return data()[idx]; }

  void reserve(const size_t n) {
    if (n > cap_) {
      MemoryContainer new_mem(std::allocator<T>().allocate(n));
      std::uninitialized_move(data(), data() + sz_, new_mem.get());
      std::swap(new_mem, mem_);
      cap_ = n;
    }
  }

  void clear() {
    if constexpr (!std::is_trivially_destructible_v<T>) {
      std::destroy(data(), data() + sz_);
    }
    sz_ = 0;
  }

  void resize(const size_t n, const T value) {
    if (n < sz_) {
      if constexpr (!std::is_trivially_destructible_v<T>) {
        std::destroy(data() + n, data() + sz_);
      }
      sz_ = n;
    } else {
      if (n > cap_) {
        reserve(n);
      }
      std::uninitialized_fill(data() + sz_, data() + n, value);
      sz_ = n;
    }
  }

  void resize(const size_t n) { resize(n, T()); }

  MemoryContainer release() {
    MemoryContainer res = std::move(mem_);
    sz_ = 0;
    cap_ = 0;
    return res;
  }

  template <typename InputIt>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    const size_t n = std::distance(first, last),
                 n_remaining_old = std::distance(begin(), pos),
                 n_move_old = std::distance(pos, end());
    CpuStdVector new_vec;
    new_vec.reserve(size() + n);

    std::uninitialized_move(begin(), begin() + n_remaining_old,
                            new_vec.begin());

    std::uninitialized_copy(
        first, last,
        begin() + n_remaining_old);  // InputIt with move iterator will not work
    std::uninitialized_move(begin() + n_remaining_old, end(),
                            new_vec.begin() + (n_remaining_old + n));
    new_vec.sz_ = size() + n;
    *this = std::move(new_vec);
    return begin() + n_remaining_old;
  }

  iterator erase(const_iterator first, const_iterator last) {
    const size_t n_remaining_begin = std::distance(begin(), first),
                 n_remain_end = std::distance(begin(), last),
                 n_remove = std::distance(first, last);
    if constexpr (!std::is_trivially_destructible_v<T>) {
      std::destroy(begin() + n_remaining_begin, begin() + n_remain_end);
    }
    const size_t n_uninit_move_end =
        n_remain_end + std::min(size() - n_remain_end, n_remove);
    std::uninitialized_move(begin() + n_remain_end, begin() + n_uninit_move_end,
                            begin() + n_remaining_begin);
    std::copy(begin() + n_uninit_move_end, end(),
              begin() + (n_remaining_begin + n_uninit_move_end));
    sz_ -= n_remove;
    return begin() + n_remaining_begin;
  }

 private:
  size_t sz_ = 0;
  size_t cap_ = 0;
  MemoryContainer mem_;
};