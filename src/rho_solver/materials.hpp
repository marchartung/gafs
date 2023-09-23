#pragma once

#include "utils/types.hpp"

struct MaterialSettings {
  double ref_density = 1000.;
  double speed_of_sound = 100.;
  double smoothing_ratio = 3.;
  double dr = 0.1;

  static MaterialSettings Water() { return MaterialSettings(); }
  static MaterialSettings Wall() {
    // return MaterialSettings();
    MaterialSettings res;
    res.dr = res.dr / 2.;
    return res;
  }
};