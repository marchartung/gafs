#pragma once

#include "utils/types.hpp"

struct MaterialSettings {
  double ref_density = 1000.;
  double speed_of_sound = 50.;
  double smoothing_ratio = 1.5;
  double dr = 0.00121 / 20.;

  static MaterialSettings Water() { return MaterialSettings(); }
  static MaterialSettings Wall() {
    // return MaterialSettings();
    MaterialSettings res;
    res.dr = res.dr / 2.;
    return res;
  }
};