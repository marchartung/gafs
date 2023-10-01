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

#include "time_stepping.hpp"

#include <omp.h>

void ForwardEuler::TimeStep(const double dt, Domain& d) {
  const double t_start = omp_get_wtime();
  SizeT num_steps = 0;
  double stepped_time = 0.;
  do {
    rhs_.Compute(d, derivative_);
    const double sub_dt = std::min(rhs_.ComputeMaxDt(d, derivative_),
                                   std::max(dt - stepped_time, 1.e-14));
    derivative_.Step(sub_dt / 2., gravity_, d.p);
    d.Update();
    stepped_time += sub_dt;
    ++num_steps;
  } while (stepped_time < dt);

  std::cout << "\navg dt: " << dt / num_steps << " | num steps: " << num_steps
            << " | step time: "
            << 1000. * (omp_get_wtime() - t_start) / num_steps << "ms \n";
}

void DualSPHysicsVerletTS::TimeStep(const double dt, Domain& d) {
  const double t_start = omp_get_wtime();
  SizeT num_steps = 0;
  double stepped_time = 0.;
  do {
    rhs_.Compute(d, derivative_);
    const double sub_dt = std::min(rhs_.ComputeMaxDt(d, derivative_),
                                   std::max(dt - stepped_time, 1.e-14));
    shifting_.Compute(d);
    init_state_.Set(d.p);
    derivative_.Step(sub_dt / 2., gravity_, d.p);
    rhs_.Compute(d, derivative_);
    IntegrateFinalStep(sub_dt, d);
    shifting_.Apply(sub_dt, d);
    d.Update();
    stepped_time += sub_dt;
    ++num_steps;
  } while (stepped_time < dt);

  std::cout << "\navg dt: " << dt / num_steps << " | num steps: " << num_steps
            << " | num neighbors: " << d.p.pos().UpdateVersion()
            << " | step time: "
            << 1000. * (omp_get_wtime() - t_start) / num_steps << "ms \n";
}

void DualSPHysicsVerletTS::IntegrateFinalStep(const double dt, Domain& d) {
#pragma omp parallel for schedule(static)
  for (SizeT i = 0; i < d.p.size(); ++i) {
    d.p.vel(i) = init_state_.vel[i] + dt * (derivative_.acc[i] + gravity_);
    d.p.SetPos(
        i, init_state_.pos[i] + 0.5 * dt * (init_state_.vel[i] + d.p.vel(i)));

    const double eps = -(derivative_.dtyD[i] / d.p.dty(i)) * dt;
    d.p.dty(i) = init_state_.dty[i] * ((2. - eps) / (2. + eps));
  }
}
