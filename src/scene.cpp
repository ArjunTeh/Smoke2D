#include "scene.hpp"

#include <chrono>
#include <cmath>
#include <iostream>

// Shader sources
const GLchar* vertexSource =
  "#version 150 core\n"
  "in vec2 position;"
  "uniform float dimensions;"
  "void main()"
  "{"
  "    vec2 screen_pos = (position)/dimensions;"
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
  unidimension = glGetUniformLocation(shader_program, "dimensions");

  glGenVertexArrays(1, &border_vao);
  glBindVertexArray(border_vao);
  glGenBuffers(1, &border);
  glBindBuffer(GL_ARRAY_BUFFER, border);
  //Finished OpenGL initialization

  density_buffer = new float[max_particles];
  pressure_buffer = new float[max_particles];

  //init border buffer
  float border_buffer[] = { -width, height,
                            -width, -height,
                            width, -height,
                            width, height };
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*sizeof(border_buffer), border_buffer, GL_STATIC_DRAW);

}

void scene::update(float t){
  //update density
  int index = 0;
  for(auto i = particles.begin(); i != particles.end(); ++i){
    //update each particle
    particle& p = *i;
    density_buffer[index] = calculate_particle_density(p);
    ++index;
  }

  //update pressure
  index = 0;
  for(auto i = particles.begin(); i != particles.end(); ++i){
    //update each particle
    particle& p = *i;
    pressure_buffer[index] = calculate_particle_pressure(p);
    ++index;
  }

  index = 0;
  for(auto i = particles.begin(); i != particles.end(); ++i){
    (*i).density = density_buffer[index]/2;
    ++index;
  }

  index = 0;
  for(auto i = particles.begin(); i != particles.end(); ++i){
    (*i).pressure = pressure_buffer[index]/2;
    ++index;
  }

  //calculate pressure acceleration
  for(auto i = particles.begin(); i != particles.end(); ++i){
    particle* p = &(*i);
    vec2f old_pos = p->position;
    //std::cout <<  i << ": " << p->position.x << ", " << p->position.y << std::endl;
    update_particle_acceleration(*p);
    p->time_step(t);
    grid.update_position(p, old_pos);
  }

}

float scene::calculate_particle_density(particle& P){
  list<particle*> adjacents;
  grid.get_adjacent(P, adjacents);
  float new_density = Constants::target_density;

  for(auto i = adjacents.begin(); i != adjacents.end(); ++i){
    particle const* b = (*i);
    new_density += P.density_from(b);
  }

  //std::cout << new_density << " " << adjacents.size() << std::endl;
  return new_density;
}

float scene::calculate_particle_pressure(particle& P){

  list<particle*> adjacents;
  grid.get_adjacent(P, adjacents);
  float new_pressure = 0.0; //minimum density? is this valid?

  for(auto i = adjacents.begin(); i != adjacents.end(); ++i){
    particle const* b = (*i);
    new_pressure += P.pressure_from(b);
  }

  //std::cout << new_pressure << " " << adjacents.size() << std::endl;
  return new_pressure;

  // float toreturn = P.pressure();
  // std::cout << toreturn << std::endl;
  // return toreturn;
}

void scene::update_particle_acceleration(particle& P){
  //get list of neighbors
  //calculate current density
  //calculate current pressure
  //calculate acceleration
  list<particle*> adjacents;
  grid.get_adjacent(P, adjacents);
  vec2f pressure_gradient{0,0};
  vec2f viscosity_damping{0,0};
  for(auto i = adjacents.begin(); i != adjacents.end(); ++i){
    pressure_gradient += P.acceleration_from(*i);
    viscosity_damping += P.viscosity_from(*i);
  }

  //std::cout << "list size: " << adjacents.size() << std::endl;
  //std::cout << pressure_gradient << std::endl;

  P.force = pressure_gradient;
  P.force += viscosity_damping;
  P.force += force_damping(P);
  P.force += force_ext(P);

  if(P.force.length2() > Constants::max_force){
    P.force = P.force.normalize() * Constants::max_force;
  }

}

vec2f scene::force_ext(particle& P){
  //if there are other things in the way then add those forces,
  vec2f const& position = P.position;
  vec2f sum_forces = gravity;
  float x = P.velocity.x;
  float y = P.velocity.y;
  if( abs(position.x) > width ){
    sum_forces.x += (-position.x / abs(position.x)) * (abs(position.x) - width) * penalty_force;
  } else if( abs(x*time_step + position.x) > width ){
    P.velocity.x = -x;
  }

  if( abs(position.y) > height ){
    //sum_forces.y += -position.y * penalty_force;
    sum_forces.y += (-position.y / abs(position.y)) * (abs(position.y) - width) * penalty_force;
  } else if( abs(y*time_step + position.y) > height){
    P.velocity.y = -y;
  }
  return sum_forces;
}

vec2f scene::force_damping(particle& P){
  return -motion_damping * P.velocity;
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
  glUniform1f(unidimension, Constants::scene_bounds);

  glPointSize(5);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), vertices.data(), GL_STREAM_DRAW);
  glDrawArrays(GL_POINTS, 0, particles.size());

  glBindVertexArray(border_vao);
  glBindBuffer(GL_ARRAY_BUFFER, border);
  glUniform3f(unicolor, 1.0f, 0.0f, 1.0f);
  glDrawArrays(GL_LINES, 0, 8);

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
  should_abort = false;
  auto t_prev = std::chrono::high_resolution_clock::now();

  //generate some particles before running
  //add_particle(-1, 0);
  int resolution = Constants::particle_res;
  for( int i = -resolution; i < resolution - 1; ++i ){
    for( int j = -resolution; j < resolution - 1; ++j){
      float x = (i+1) * (width)/(resolution*1.2);
      float y = (j+1) * (height)/(resolution*1.2);
      add_particle(x,y - (height/2));
    }
  }

  while(!glfwWindowShouldClose(window) && !should_abort) {
    auto t_now = std::chrono::high_resolution_clock::now();

    float t_delta = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_prev).count();
    if( t_delta > float(1/FPS)){

      //emit particles during sim
      if(false && particles.size() < max_particles && step_num%1 == 0){
        float x_pos = (float) step_num * 0.1;
        vec2f pos{ x_pos, 1.0f };
        vec2f vel{ 0.0f, 0.0f };
        add_particle(pos, vel);
      }

      update(time_step);
      ++step_num;
      t_prev = t_now;
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      draw();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
    //if( step_num > 50 ){ break; }
    if(should_abort){
      std::cout << "frame: " << step_num << std::endl;
    }
  }

}

void scene::add_particle(vec2f pos, vec2f vel){
  particle p{ pos, vel };
  particles.push_back(p);
  grid.add_particle(&particles.back());
}

void scene::add_particle( float x, float y ){
  vec2f pos { x, y };
  add_particle( pos, vec2f{0,0} );
}
