#pragma once

#include "vec.hpp"

class particle {
public:
  static constexpr float sphere_of_influence = 2;//2 units of influence
  vec2f position;
  vec2f velocity;
  vec2f force;
  float mass = 1.0;
  float density = 1.0;
  float color[3];

  float kernel_function( particle* p ){
    if( p == this || !p ){ return 0.0; }
    float q = (position - p->position).length();

    if( q > 2 ) { return 0.0f; }
    float work = (2 - q) * (2 - q) * (2 - q);
    if(q > 1) { return work; }

    work -= 4* pow( (1-q) , 3 );
    return work;
  };

  particle( vec2f pos, vec2f vel ) : position(pos), velocity(vel), mass(1.0) {}

  void set_color( float r, float g, float b){
    color[0] = r, color[1] = g, color[2] = b;
  }

  //semi-implicit Euler
  void time_step(float delta){
    velocity = velocity + (force)*delta;
    position = position + velocity*delta;
  }

};

class fluidsquare {
  int size;
  float dt;
  float diff;
  float visc;

};
