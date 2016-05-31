#pragma once

#include <cmath>
#include <ostream>

template<typename T>
struct vec2 {
public:
  T x, y;

  //  vec2(void) = default;
  vec2(void) : x(0), y(0) {}
  template<typename R>
  vec2(vec2<R>& rhs) : x(rhs.x), y(rhs.y) {}
  vec2(T X, T Y) : x(X), y(Y) {}
  vec2(T W) : x(W), y(W) {}

  inline vec2 operator+= ( vec2<T> rhs){
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  inline vec2 operator-= (vec2<T> rhs){
    (*this) += -rhs;
    return *this;
  }

  inline vec2 operator-(void){
    vec2<T> v;
    v.x = -x;
    v.y = -y;
    return v;
  }

  T length(void){
    return sqrt(x*x + y*y);
  }
};

template<typename T>
inline vec2<T> operator+ (vec2<T> const& lhs, vec2<T> const& rhs){
  return vec2<T>( lhs.x + rhs.x, lhs.y + rhs.y);
}

template<typename T>
inline vec2<T> operator- (vec2<T> const& lhs, vec2<T> const& rhs){
  return vec2<T>(lhs.x - rhs.x, lhs.y -rhs.y);
}

//dot product
template<typename T>
inline T operator* (vec2<T> const& lhs, vec2<T> const& rhs){
  return (lhs.x*rhs.x) + (lhs.y*rhs.y);
}

template<typename T>
inline vec2<T> operator* (vec2<T> const& lhs, T const& rhs){
  return vec2<T>{ lhs.x*rhs, lhs.y*rhs };
}

template<typename T>
inline vec2<T> operator* (T const& lhs, vec2<T> const& rhs){
  return vec2<T>{ lhs*rhs.x, lhs*rhs.y };
}

template<typename T>
inline vec2<T> operator/ (vec2<T> const& lhs, T const& rhs){
  return vec2<T>{ lhs.x/rhs, lhs.y/rhs };
}

template<typename T>
std::ostream& operator<< ( std::ostream& out, vec2<T> v ){
  out << "{" << v.x << ", " << v.y << "}";
  return out;
}

using vec2f = vec2<float>;
