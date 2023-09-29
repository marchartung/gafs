#pragma once

#include "types.hpp"

template <typename T>
class Span {
 public:
  using iterator = T*;

  Span() = default;
  Span(T* data, const SizeT n) : data_(data), sz_(n) {}

  T& operator[](const SizeT idx) const { return data_[idx]; }

  T* data() const { return data_; }
  SizeT size() const { return sz_; }

  iterator begin() const { return data_; }
  iterator end() const { return data_ + sz_; }

 private:
  T* data_ = nullptr;
  SizeT sz_ = 0;
};