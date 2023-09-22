#pragma once

#include <vector>

template<typename T>
class PointerEnsuredVector
{
public:
  T* GetNewData()
  {
    if (data_.back().size() == data_.back().capacity()) {
      data_.emplace_back();
      data_.back().reserve(initCapacity);
    }
    data_.back().emplace_back();
    return &data_.back().back();
  }

private:
  static inline constexpr size_t initCapacity = 64;
  std::vector<std::vector<T>> data_ = std::vector<std::vector<T>>(1);
};