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

#include <cstddef>
#include <cstdlib>
#include <memory>
#include <stdexcept>

template <typename T, size_t ident>
class PointerWrapper {
 public:
  PointerWrapper() = default;
  explicit PointerWrapper(T* ptr) : ptr_(ptr) {}

  bool operator==(const void* ptr) const { return ptr == ptr_; }
  bool operator!=(const void* ptr) const { return ptr != ptr_; }
  bool operator==(const PointerWrapper<T, ident> ptr) const {
    return ptr.ptr_ == ptr_;
  }
  bool operator!=(const PointerWrapper<T, ident> ptr) const {
    return ptr.ptr_ != ptr_;
  }

  operator const T*() const { return ptr_; }
  operator T*() { return ptr_; }

  const T& operator[](const size_t i) const { return ptr_[i]; }
  T& operator[](const size_t i) { return ptr_[i]; }

 private:
  T* ptr_ = nullptr;
};

template <typename T>
using CpuPointer = PointerWrapper<T, 0>;

class CpuAlloc {
 public:
  template <typename T>
  using pointer = CpuPointer<T>;

  template <typename T>
  static size_t NumBytesPageAligned(const size_t n) {
    const size_t num_bytes = sizeof(T) * n;
    const size_t n_pages = (num_bytes % page_size == 0)
                               ? num_bytes / page_size
                               : (num_bytes + page_size) / page_size;
    return n_pages * page_size;
  }

  template <typename T>
  static size_t NumElementsPageAligned(const size_t n) {
    const size_t num_bytes = NumBytesPageAligned<T>(n);
    return num_bytes / sizeof(T);
  }

  template <typename T>
  static pointer<T> Allocate(const size_t n) {
    const size_t num_bytes = NumBytesPageAligned<T>(n);
    void* mem = std::aligned_alloc(page_size, num_bytes);
    if (mem == nullptr) {
      throw std::runtime_error("CPU memory allocation failed");
    }
    return pointer<T>(static_cast<T*>(mem));
  }

  template <typename T>
  static void Free(pointer<T>& mem) {
    std::free(mem);
    mem = pointer<T>(nullptr);
  }

 private:
  static constexpr size_t page_size = 4096;
};

#ifdef GPU_ENABLED

template <typename T>
using GpuPointer = PointerWrapper<T, 1>;

class GpuAlloc {
 public:
  template <typename T>
  using pointer = CpuPointer<T>;

  template <typename T>
  static pointer<T> Allocate(const size_t n) {
    const size_t num_bytes = sizeof(T) * n;
    void* mem;
    const hipError_t err = hipMalloc(&mem, num_bytes);
    if (err != hipSuccess) {
      throw std::runtime_error("GPU memory allocation failed");
    }
    return pointer(static_cast<T*>(mem));
  }

  template <typename T>
  static void Free(pointer<T>& mem) {
    const hipError_t err = hipFree(mem);
    if (err != hipSuccess) {
      throw std::runtime_error("GPU pointer invalid for free call");
    }
    mem = pointer<T>(nullptr);
  }
};
#else)

using GpuAlloc = CpuAlloc;
template <typename T>
using GpuPointer = CpuPointer<T>;

#endif