#include <atomic>

template<typename T>
class AtomicWrapper
{
public:
  AtomicWrapper() = default;
  AtomicWrapper(T&& value)
  {
    value_.store(std::forward<T>(value), std::memory_order_relaxed);
  }
  AtomicWrapper(const AtomicWrapper<T>& v)
    : AtomicWrapper<T>(v.load(std::memory_order_relaxed))
  {
  }
  AtomicWrapper(AtomicWrapper<T>&& v)
    : AtomicWrapper<T>(v.load(std::memory_order_relaxed))
  {
  }
  AtomicWrapper& operator=(T&& val)
  {
    value_.store(std::forward<T>(val), std::memory_order_relaxed);
    return *this;
  }
  AtomicWrapper& operator=(const AtomicWrapper<T>& val)
  {
    *this = val.load(std::memory_order_relaxed);
    return *this;
  }
  AtomicWrapper& operator=(AtomicWrapper<T>&& val)
  {
    *this = val.load(std::memory_order_relaxed);
    return *this;
  }

  ~AtomicWrapper() = default;

  void store(T&& value,
             const std::memory_order order = std::memory_order_seq_cst)
  {
    value_.store(std::forward<T>(value), order);
  }

  T load(const std::memory_order order = std::memory_order_seq_cst) const
  {
    return value_.load(order);
  }

  bool compare_exchange_strong(T& expected, T desired)
  {
    return value_.compare_exchange_strong(expected, desired);
  }

private:
  std::atomic<T> value_;
};