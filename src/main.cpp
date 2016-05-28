#include <iostream>

#include "scene.hpp"
#include "vec.hpp"
#include "simulation_units.hpp"

int main(void) {
  std::cout << "setting up scene" << std::endl;
  scene s;
  s.init();

  std::cout << "running simulation" << std::endl;
  s.run();

  std::cout << "cleaning up simulation" << std::endl;
  s.teardown();

  /* test vector
  vec2f zero = vec2f{0,0};
  vec2f one = vec2f{1,1};
  vec2f two = vec2f{2,1};

  vec2f zero_one = zero - one;
  vec2f one_two = zero_one + two;
  one_two = one_two / 2.0f;
  one_two = one_two * 3.0f;
  std::cout << -two << std::endl;
  std::cout << zero_one << std::endl;
  std::cout << one_two << std::endl;
  */
}
