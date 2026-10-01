#include "recipe.hpp"
#include "config.h"

void recipe::recipe::cook(float delta){
    elapsed_ += game_config::cook_speed * delta;
        count_++;
        elapsed_ = 0;
}
size_t recipe::recipe::get_food_id(){
    return food_id_;
}
size_t recipe::recipe::get_count(){
    return count_;
}
void recipe::recipe::materialise(){
    // ! need to emit some form of build event / call

    count_ = 0;
}