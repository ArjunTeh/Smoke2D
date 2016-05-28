#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "vec.hpp"
#include "simulation_units.hpp"

class scene {
  static constexpr float FPS = 60.0;
  static constexpr int max_particles = 300;
  GLFWwindow* window;

  GLuint vbo, vao;
  GLuint vertex_shader, fragment_shader, shader_program;
  GLuint unicolor;
  bool should_run;
  float width, height;

  vec2f gravity;

  //paper says to use a linked list
  std::vector<particle> particles;
  std::vector<GLfloat> vertices;
public:

  scene(void) : width(20), height(20), should_run(true), gravity(0, -0.1) {}

  //window stuff goes here
  //smoke starts in the center
  void init();
  void teardown();
  void update(float t);
  void update_particle(particle& P);
  vec2f force_ext(particle& P);
  void draw();
  void run();

  //getters and setters
  void set_width(float w){ width = w; }
  void set_height(float l){ height = l; }

  float get_width(){ return width; }
  float get_height(){ return height; }

private:
  void add_particle(vec2f pos, vec2f vel);
  void add_particle(float x, float y);
};
