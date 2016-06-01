#pragma once

#include <vector>

#include "vec.hpp"

//Obj will most likely be particle exclusively (lol)
namespace Bucket{

  template<typename T>
  struct bucket_item{
    T* data;
    bucket_item<T>* next;
  };

  template<typename Obj>
  struct bucket{
    bucket_item<Obj> head;
    bucket_item<Obj> tail;
    bucket<Obj>* next;

    struct iterator : public std::forward_iterator_tag {
      bucket<Obj>* current_bucket;
      bucket_item<Obj>* current_item;

      Obj* operator*(void){
        return current_bucket->data;
      }

      void operator++(void){
        current_item = current_item->next;
        if(!current_item){
          current_bucket = current_bucket->next;
          if(current_bucket){
            current_item = current_bucket->head;
          }
        }
      }

      bool operator==(bucket<Obj>::iterator* rhs){
        return current_item == rhs.current_item && current_bucket == rhs.current_bucket;
      }

      bool operator!=(bucket<Obj>::iterator* rhs){
        return !( (*this) == (*rhs) );
      }
    };
  };


};

using namespace Bucket;

template<typename Obj>
class spatial_hash{
  float cell_size; //we shall do squares for now
  int width, height;
  std::vector<bucket<Obj>> grid;
public:

  spatial_hash(int width, int height, float h) : grid(width*height), cell_size(h) {}

  void update_position(Obj* p, vec2f old_pos){
    vec2f const& pos = p->position;
    int old_index = shash(old_pos);
    int new_index = shash(pos);

    bucket<Obj>& old_bucket = grid[old_index];
    bucket<Obj>& new_bucket = grid[new_index];
  }

  typename bucket<Obj>::iterator get_adjacent(Obj* p){
    //return an appaend? of lists?
    //should be a linked list
  }

  void add_particle(Obj* p){
    //should not add if already exists?
  }

  void remove_particle(Obj* p){
    //what happens when it doesn't exist?
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
