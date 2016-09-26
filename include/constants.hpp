#pragma once

#include "vec.hpp"

namespace Constants {
  //boundary of the simulation
  static constexpr float scene_bounds = 20;

  //beginning conditions
  static constexpr int particle_res = 6;

  //external forces / boundary conditions
  static vec2f gravity = vec2f{ 0.0, -9.0 };
  static constexpr float max_force = 20.0;

  //particle properties
  static constexpr float target_density = 1.0;
  static constexpr float pressure_stiffness = 3;
  static constexpr float viscosity_factor = 0.89;

  //computational numbers and constants
  static constexpr float diff_const = 0.001;
  static constexpr float h_val = 3.0;

}
