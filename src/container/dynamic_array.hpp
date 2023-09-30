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

#include <memory>
#include <type_traits>

template <typename T, typename Allocator = std::allocator<T>>
class DynamicArray {
  using MemoryContainer = std::unique_ptr<T>;

 public:
  using iterator = T*;
  using const_iterator = const T*;

  DynamicArray() = default;
  ~DynamicArray() { clear(); }

  DynamicArray(const size_t n) { resize(n); }
  DynamicArray(const size_t n, const T value) { resize(n, value); }

  DynamicArray<T>& operator=(const DynamicArray<T>& in) {
    if constexpr (!std::is_trivially_destructible_v<T>) {
      clear();
    }
    reserve(in.sz_);
    if (cap_ < in.sz_) {
    }
    std::uninitialized_copy(in.begin(), in.end(), data());
    return *this;
  }
  DynamicArray<T>& operator=(DynamicArray<T>&& in) {
    std::swap(in.sz_, sz_);
    std::swap(in.cap_, cap_);
    std::swap(in.mem_, mem_);
    return *this;
  }

  void resize(const size_t n) {
    if (n < sz_) {
      if constexpr (!std::is_trivially_destructible_v<T>) {
        std::destroy(data() + n, data() + sz_);
      }
      sz_ = n;
    } else {
      if (n > cap_) {
        DynamicArray<T> new_dyn_arr;
        new_dyn_arr.reserve(n);
        new_dyn_arr = *this;
        *this = std::move(new_dyn_arr);
      }
      if constexpr (!std::is_trivially_constructible_v<T>) {
        std::uninitialized_fill(data() + sz_, data() + n, T());
      }
      sz_ = n;
    }
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

  void reserve(const size_t n) {
    if (n > cap_) {
      MemoryContainer new_mem(std::allocator<T>().allocate(n));
      std::uninitialized_copy(data(), data() + sz_, new_mem.get());
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

  T* release() {
    T* res = mem_.release();
    sz_ = 0;
    cap_ = 0;
    return res;
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

 private:
  size_t sz_ = 0;
  size_t cap_ = 0;
  MemoryContainer mem_;
};