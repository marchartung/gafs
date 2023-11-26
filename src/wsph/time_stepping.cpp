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

#include <iostream>

void ForwardEuler::TimeStep(const double dt, Domain& d) {
  shifting_.SetPrs(ComputePressure(d.p.ref_density() * 0.8, d.p.ref_density(),
                                   d.p.pressure_parameter()),
                   ComputePressure(d.p.ref_density() * 1.2, d.p.ref_density(),
                                   d.p.pressure_parameter()));
  const double t_start = omp_get_wtime();
  SizeT num_steps = 0;
  double stepped_time = 0.;
  do {
    d.pb.Interpolate(d.p);
    rhs_.Compute(d, derivative_);
    const double sub_dt = std::min(rhs_.ComputeMaxDt(d, derivative_),
                                   std::max(dt - stepped_time, 1.e-14));
    shifting_.Compute(d);
    derivative_.Step(sub_dt, gravity_, d);
    shifting_.Apply(sub_dt, d);
    d.Update();
    stepped_time += sub_dt;
    ++num_steps;
    std::cout << "sub dt: " << sub_dt << " max acc: "
              << *std::max_element(
                     derivative_.acc.begin(), derivative_.acc.end(),
                     [](auto a, auto b) { return Length(a) < Length(b); })
              << " max dtyD: "
              << *std::max_element(derivative_.dtyD.begin(),
                                   derivative_.dtyD.end())
              << " max prs: "
              << *std::max_element(d.p.prs().begin(), d.p.prs().end())
              << " max dty: "
              << *std::max_element(d.p.dty().begin(), d.p.dty().end()) << "\n";
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
    d.pb.Interpolate(d.p);
    rhs_.Compute(d, derivative_);
    const double sub_dt = std::min(rhs_.ComputeMaxDt(d, derivative_),
                                   std::max(dt - stepped_time, 1.e-14));

    init_state_.Set(d.p);
    derivative_.Step(sub_dt / 2., gravity_, d);
    d.pb.Interpolate(d.p);
    rhs_.Compute(d, derivative_);
    IntegrateFinalStep(sub_dt, d);
    d.Update();
    shifting_.Compute(d);
    shifting_.Apply(sub_dt, d);
    d.Update();
    stepped_time += sub_dt;
    ++num_steps;
  } while (stepped_time < dt);

  std::cout
      << "sub dt: " << dt / num_steps << " max acc: "
      << *std::max_element(derivative_.acc.begin(), derivative_.acc.end(),
                           [](auto a, auto b) { return Length(a) < Length(b); })
      << " max dtyD: "
      << *std::max_element(derivative_.dtyD.begin(), derivative_.dtyD.end())
      << " max prs: " << *std::max_element(d.p.prs().begin(), d.p.prs().end())
      << " max dty: " << *std::max_element(d.p.dty().begin(), d.p.dty().end())
      << " sum vel:"
      << std::accumulate(d.p.vel().begin(), d.p.vel().end(), Vectord(0.))
      << "\n";
  std::cout << "\navg dt: " << dt / num_steps << " | num steps: " << num_steps
            << " | step time: "
            << 1000. * (omp_get_wtime() - t_start) / num_steps << "ms \n";
}

void DualSPHysicsVerletTS::IntegrateFinalStep(const double dt, Domain& d) {
#pragma omp parallel for schedule(static)
  for (SizeT i = 0; i < d.p.size(); ++i) {
    d.p.vel(i) = init_state_.vel[i] + dt * (derivative_.acc[i] + gravity_);
    d.SetFluidPos(
        i, init_state_.pos[i] + 0.5 * dt * (init_state_.vel[i] + d.p.vel(i)));

    const double eps = -(derivative_.dtyD[i] / d.p.dty(i)) * dt;
    d.p.dty(i) = init_state_.dty[i] * ((2. - eps) / (2. + eps));
  }
}
