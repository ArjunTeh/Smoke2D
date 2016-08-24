#pragma once

#include <iostream>

#include "constants.hpp"
#include "vec.hpp"


class particle {
public:
  static constexpr float h_val = Constants::h_val;//2 units of influence
  static constexpr float diff_const = Constants::diff_const; //definitely can be smaller
  static constexpr float target_density = Constants::target_density;
  static constexpr float pressure_stiffness = Constants::pressure_stiffness;
  static constexpr float viscosity_factor = Constants::viscosity_factor;

  vec2f position;
  vec2f velocity;
  vec2f force;
  float mass = 0.5;
  float pressure = 1.0;
  float density = 0.3;

  float kernel_poly6( vec2f const p ) const {
    vec2f r = p - position;
    if(r.length() > h_val || r.length() < 0){
      return 0.0f;
    }
    return pow( h_val*h_val - r.length2() , 3 );
  }

  float kernel_spiky (vec2f const p ) const {
    vec2f r = p - position;
    if(r.length() > h_val || r.length() < 0){
      return 0.0f;
    }
    return pow( h_val - r.length() , 3 );
  }

  vec2f kernel_gradient_diff( particle* p ){
    vec2f xy = p->position;

    vec2f pre_x = xy;
    pre_x.x -= diff_const/2;
    float pre_x_val = kernel_spiky( pre_x );
    vec2f post_x = xy;
    post_x.x += diff_const/2;
    float post_x_val = kernel_spiky( post_x );

    vec2f pre_y = xy;
    pre_y.y -= diff_const/2;
    float pre_y_val = kernel_spiky( pre_y );
    vec2f post_y = xy;
    post_y.y += diff_const/2;
    float post_y_val = kernel_spiky( post_y );

    float grad_x = (post_x_val - pre_x_val) / diff_const;
    float grad_y = (post_y_val - pre_y_val) / diff_const;

    return vec2f{grad_x, grad_y};

  }

  float kernel_laplacian( particle* p ){
    //we are using the five-point stencil here
    float numerator = 0;
    float x = p->position.x;
    float y = p->position.y;
    //f(x+h, y)
    numerator += kernel_poly6( vec2f{ x + diff_const, y } );
    //f(x-h, y)
    numerator += kernel_poly6( vec2f{ x - diff_const, y } );
    //f(x, y+h)
    numerator += kernel_poly6( vec2f{ x, y + diff_const } );
    //f(x, y-h)
    numerator += kernel_poly6( vec2f{ x, y - diff_const } );
    //4 * f(x ,y)
    numerator -= 4 * kernel_poly6( vec2f{ x, y } );

    return numerator / (diff_const*diff_const);
  }

  //works only with specific pieces
  vec2f kernel_gradient_deriv( particle* p ){
    //use of derivative
    vec2f dir = p->position - position;
    //std::cout << "pos diff is: " << dir << std::endl;
    float q = dir.length();
    dir = dir.normalize();
    q /= h_val;
    float work = -12*q*(1-q)*(1-q);
    return -dir*work;
  }

  float density_from( particle const* p) const {
    //if( !p ) { return 0.0f; }
    return p->mass * kernel_poly6(p->position);
  }

  float pressure_from( particle const* p) const {
    //if( !p || p == this ) { return 0.0f; }
    return p->mass * kernel_spiky(p->position) * p->pressure * pressure/ (2*p->density);
  }

  float update_pressure(void){ //negative pressure is a thing
    return pressure_stiffness * ( density - target_density );
  }


  vec2f acceleration_from( particle* p ){
    if( p == this || !p ){ return vec2f{0,0}; }
    if(density < 1e-8) {
      std::cout << "density lower than zero" << std::endl;
      return vec2f{ 0.0 };
    }
    /*
    //density needs a calculation, then pressure does as well
    float factor = pressure/(density*density);
    factor += p->pressure/(p->density*p->density);
    factor *= p->mass;

    return factor * kernel_gradient(p) * density;
    */
    float factor2 = pressure + p->pressure;
    factor2 /= (2 * p->density);

    return factor2 * kernel_gradient_diff(p);
  }

  vec2f viscosity_from( particle* p ){
    if(p == this || !p ){ return vec2f{0,0}; }
    float factor = viscosity_factor * p->mass / p->density;
    vec2f relative_vel = p->velocity - velocity;
    return factor * kernel_laplacian(p) * relative_vel;
  }


  particle( vec2f pos, vec2f vel ) : position(pos), velocity(vel), mass(1.0) {}
  particle(void) = default;

  //semi-implicit Euler
  void time_step(float delta){
    velocity = velocity + (force)*delta;
    position = position + velocity*delta;
  }

};
