#pragma once

#include "vec.hpp"

namespace Constants {
  static constexpr float h_val = 0.6;
  static vec2f gravity = vec2f{ 1.0, -9.0 };
  static constexpr int particle_res = 3;
}
