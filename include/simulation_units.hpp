#pragma once

#include <iostream>

#include "vec.hpp"

class particle {
public:
  static constexpr float sphere_of_influence = 0.5;//2 units of influence
  static constexpr float diff_const = 0.001; //definitely can be smaller
  static constexpr float target_density = 0.3;
  static constexpr float pressure_stiffness = 50;

  vec2f position;
  vec2f velocity;
  vec2f force;
  float mass = 0.2;
  float pressure = 1.0;
  float density = 0.3;

  float kernel_function( vec2f p ){
    float q = (position - p).length();
    q /= sphere_of_influence;
    if( q < 0) { std::cout<< "we fucked up" << std::endl; }

    if( q > 2 ) { return 0.0f; }
    float work = (2 - q) * (2 - q) * (2 - q);
    if(q > 1) { return work; }
    work -= 4 * pow( (1-q) , 3 );

    //float work = pow( (1-q), 3 );

    return work;
  };

  vec2f kernel_gradient( particle* p ){
    vec2f xy = p->position;

    vec2f pre_x = xy;
    pre_x.x -= diff_const/2;
    float pre_x_val = kernel_function( pre_x );
    vec2f post_x = xy;
    post_x.x += diff_const/2;
    float post_x_val = kernel_function( post_x );

    vec2f pre_y = xy;
    pre_y.y -= diff_const/2;
    float pre_y_val = kernel_function( pre_y );
    vec2f post_y = xy;
    post_y.y += diff_const/2;
    float post_y_val = kernel_function( post_y );

    float grad_x = (post_x_val - pre_x_val) / diff_const;
    float grad_y = (post_y_val - pre_y_val) / diff_const;

    return vec2f{grad_x, grad_y};

  }

  vec2f acceleration_from( particle* p ){
    if( p == this || !p ){ return vec2f{0,0}; }
    if(density < 1e-8) {
      std::cout << "density lower than zero" << std::endl;
      return vec2f{ 0.0 };
    }

    //density needs a calculation, then pressure does as well
    float factor = pressure/(density*density);
    factor += p->pressure/(p->density*p->density);
    factor *= p->mass;
    return factor * kernel_gradient(p);
  }

  float density_from( particle const* p){
    if( !p ) { return 0.0f; }
    return p->mass * kernel_function(p->position);
  }

  float pressure_from( particle const* p){
    if( !p || p == this ) { return 0.0f; }
    return p->mass * kernel_function(p->position) * p->pressure / p->density;
  }

  float update_pressure(void){
    return pressure_stiffness * ( density - target_density );
  }

  particle( vec2f pos, vec2f vel ) : position(pos), velocity(vel), mass(1.0) {}
  particle(void) = default;

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
