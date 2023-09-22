#include "timer.hpp"

Timer::TimestampType
Timer::Now()
{
  return std::chrono::steady_clock::now();
}

double
Timer::getMs()
{
  return std::round(static_cast<double>(
                      std::chrono::duration_cast<std::chrono::microseconds>(
                        Now() - start_)
                        .count()) /
                    100.) /
         10.;
}
