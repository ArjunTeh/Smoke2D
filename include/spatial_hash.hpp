#pragma once

#include <vector>
#include <list>
#include <stdexcept>

#include "vec.hpp"

//Obj will most likely be particle exclusively (lol)

using namespace std;

template<typename Obj>
class spatial_hash{
  float cell_size; //we shall do squares for now
  int width, height;
  vector< list<Obj*> > grid;
public:

  spatial_hash(float w, float h, float s) : grid(int(width*height/s + 1)), cell_size(s), width(w), height(h) {}

  void update_position(Obj* p, vec2f old_pos){
    vec2f const& new_pos = p->position;
    int old_index = shash(old_pos);
    int new_index = shash(new_pos);

    list<Obj*>& old_bucket = grid[old_index];
    bool exists = false;
    for(auto i = old_bucket.begin(); i != old_bucket.end(); ++i){
      if((*i) == p){
        old_bucket.erase(i);
        exists = true;
        break;//we've modified the list, we invalidated the iterator
      }
    }
    if( !exists ) { throw std::invalid_argument("object doesn't exist in grid already"); }

    list<Obj*>& new_bucket = grid[new_index];
    new_bucket.push_back(p);
  }

  list<Obj*>& get_adjacent(Obj& p){
    for(int r = -1; r < 2; ++r){
      for(int c = -1; c < 2; ++c){
        int index = r * 2 * width + c;
      }
    }
    //return an appaend? of lists?
    //should be a linked list
  }

  void add_particle(Obj* p){
    int index = shash(p->position);
    list<Obj*>& bucket = grid[index];
    bucket.push_back(p);
    //should not add if already exists?
  }

  void remove_particle(Obj* p){
    //what happens when it doesn't exist?
    int index = shash(p->position);
    list<Obj*>& bucket = grid[index];
    bool exists = false;
    for(auto i = bucket.begin(); i != bucket.end(); ++i){
      if((*i) == p){
        bucket.erase(i);
        exists = true;
        break;//we've modified the list, we invalidated the iterator
      }
    }
    if( !exists ) { throw std::invalid_argument("object doesn't exist in grid already"); }
  }

private:

  //to get an index for a dimension: floor( x / h ); //  x as input
  int shash(vec2f const& pos){
    float x = pos.x;
    float y = pos.y;
    float index =  floor((x+width) / cell_size);
    index +=  floor((y+height) / cell_size) * width * 2;//2 because width is half
    return int( index );
  }

  //this spatial hash will need to be able to return a list of all surrounding neighbors


};
