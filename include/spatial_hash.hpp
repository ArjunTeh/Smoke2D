#pragma once

#include <unordered_map>
#include <list>
#include <stdexcept>
#include <iostream>

#include "vec.hpp"

//Obj will most likely be particle exclusively (lol)

using namespace std;

template<typename Obj>
class spatial_hash{
  float cell_size; //we shall do squares for now
  int width, height;
  int num_objects;
  unordered_map<int, list<Obj*> > grid;
public:

  spatial_hash(float w, float h, float s) : grid(int(4*width*height/(s*s) + 1)), cell_size(s),
                                            width(w), height(h),
                                            num_objects(0) {}

  void update_position(Obj* p, vec2f& old_pos){
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

  void get_adjacent(Obj& p, list<Obj*>& toreturn){

    int obj_index = shash(p.position);
    for(int r = -1; r < 2; ++r){
      for(int c = -1; c < 2; ++c){
        if(r < 0 || r >= height || c < 0 || c >= width){ continue; }
        int index = obj_index + r * 2 * width + c;
        list<Obj*>& bucket = grid[index];
        toreturn.insert(toreturn.end(), bucket.begin(), bucket.end());
        //std::cout << "indexing at " << index << std::endl;
      }
    }
    //return an append? of lists?
    //should be a linked list
  }

  void add_particle(Obj* p){
    int index = shash(p->position);

    list<Obj*>& bucket = grid[index];
    bucket.push_back(p);
    ++num_objects;
    //should not add if already exists?
  }

  Obj* remove_particle(Obj* p){
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
    --num_objects;
    return p;
  }

  int size(void){ return num_objects; }

private:

  //to get an index for a dimension: floor( x / h ); //  x as input
  int shash(vec2f const& pos){
    float x = pos.x;
    float y = pos.y;
    int index =  floor((x+width) / cell_size);
    index +=  floor((y+height) / cell_size) * width * 2;//2 because width is half
    return index;
  }

  //this spatial hash will need to be able to return a list of all surrounding neighbors


};
