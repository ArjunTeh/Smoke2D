#include "scene.hpp"

#include <chrono>
#include <cmath>
#include <iostream>

// Shader sources
const GLchar* vertexSource =
  "#version 150 core\n"
  "in vec2 position;"
  "void main()"
  "{"
  "    vec2 screen_pos = (position)/20.0;"
  "    gl_Position = vec4(screen_pos, 0.0, 1.0);"
  "}";
const GLchar* fragmentSource =
  "#version 150 core\n"
  "out vec4 outColor;"
  "uniform vec3 color;"
  "void main()"
  "{"
  "    outColor = vec4(color, 1.0);"
  "}";

using namespace std;

void scene::init(void){
  //initialize all the particles
  //set t = 0;
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  window = glfwCreateWindow(800, 600, "Fluid2D", nullptr, nullptr); // Windowed
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  glewInit();

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertexSource, NULL);
  glCompileShader(vertex_shader);

  GLint success;
  GLchar info_log[512];
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    cout << "Error compiling vertex shader" << endl << info_log << endl;
    cout << "Source:" << endl;
    cout << vertexSource << endl;
  }

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragmentSource, NULL);
  glCompileShader(fragment_shader);

  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
    cout << "Error compiling fragment shader" << endl << info_log << endl;
    cout << "Source:" << endl;
    cout << fragmentSource << endl;
  }

  // Link the vertex and fragment shader into a shader program
  shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glBindFragDataLocation(shader_program, 0, "outColor");
  glLinkProgram(shader_program);
  glUseProgram(shader_program);

  // Specify the layout of the vertex data
  GLint posAttrib = glGetAttribLocation(shader_program, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

  // Get the location of the color uniform
  unicolor = glGetUniformLocation(shader_program, "color");
}

void scene::update(float t){
  uint64_t size = particles.size();
  for(int i = 0; i < size; ++i){
    //update each particle
    particle& p = particles[i];
    update_particle(p);
    p.force += force_ext(p);
    p.time_step(t);
    //std::cout <<  i << ": " << p.position.x << ", " << p.position.y << std::endl;
  }

}

void scene::update_particle(particle& P){
  //get list of neighbors
  //calculate current density
  //calculate current pressure
  //calculate acceleration
  uint64_t size = particles.size();
  for(uint64_t i = 0; i < size; ++i){
    //    P.kernel_function(&particles[i]);
  }

  P.force = vec2f{ 0, 0 };

}

vec2f scene::force_ext(particle& P){
  //if there are other things in the way then add those forces,
  vec2f const& position = P.position;
  vec2f sum_forces = gravity;
  float x = position.x;
  float y = position.y;
  if(abs(x) > width){ P.velocity.x = -x; }
  if(abs(y) > height){ P.velocity.y = -y; }
  return sum_forces;
}


void scene::draw(){
  //draw everything to screen
  //std::cout << "frame" << std::endl;
  vertices.clear();
  for( auto i = particles.begin(); i != particles.end(); ++i){
    particle const& p = *i;
    //std::cout << p.position << std::endl;
    vertices.push_back(p.position.x);
    vertices.push_back(p.position.y);
  }

  glUseProgram(shader_program);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glUniform3f(unicolor, 1.0f, 1.0f, 1.0f);

  glPointSize(5);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), vertices.data(), GL_STREAM_DRAW);
  glDrawArrays(GL_POINTS, 0, particles.size());

  //std::cout << " vertices" << std::endl;
  /*
  for( auto i = vertices.begin(); i != vertices.end(); ++i ){
    std::cout << *i << std::endl;
  }
  */
  //meh
}

void scene::teardown(){
  //delete vertices, this doesn't work, so we just let the mem leak (FIX THIS)
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
  //delete window
  glfwTerminate();
}

void scene::run(void){
  int step_num = 0;
  auto t_prev = std::chrono::high_resolution_clock::now();
  //add_particle(-1, 0);

  while(!glfwWindowShouldClose(window)) {
    auto t_now = std::chrono::high_resolution_clock::now();


    float t_delta = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_prev).count();
    if( t_delta > float(1/FPS)){

      if(particles.size() < max_particles){
        add_particle(0.5f, 0.5f);
      }

      update(0.0166);
      ++step_num;
      t_prev = t_now;
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      draw();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
    //if( step_num > 10 ){ break; }
  }

}

void scene::add_particle(vec2f pos, vec2f vel){
  particle p{ pos, vel };
  particles.push_back(p);
}

void scene::add_particle( float x, float y ){
  vec2f pos { x, y };
  add_particle( pos, vec2f{0,0} );
}
