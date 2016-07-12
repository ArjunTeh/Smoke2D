#pragma once

#include "vec.hpp"

namespace Constants {
  static constexpr float h_val = 3.0;
  static vec2f gravity = vec2f{ 0.0, -9.0 };
  static constexpr int particle_res = 6;
  static constexpr float target_density = 1.0;
  static constexpr float pressure_stiffness = 30000;
  static constexpr float viscosity_factor = 0.89;
  static constexpr float diff_const = 0.001;
}
