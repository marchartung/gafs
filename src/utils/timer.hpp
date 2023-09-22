#pragma once

#include <chrono>
#include <cmath>

class Timer
{
public:
  using TimestampType = std::chrono::time_point<std::chrono::steady_clock>;

  Timer()
    : start_(Now())
  {
  }

  static TimestampType Now();

  void reset() { start_ = Now(); }

  double getMs();

private:
  TimestampType start_;
};
