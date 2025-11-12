#include "hitbox.h"

hitbox::hitbox_builder hitbox::h_builder_;

bool hitbox::hitbox::check_collision(const hitbox& other){
    for(auto & box : rectangles_){
        for(auto & other_box : other.rectangles_){
            if(CheckCollisionRecs(box, other_box)) {
                return true;
            }
        }
    }
    return false;
}
std::vector<Rectangle> hitbox::hitbox::get_hitbox(){
    return rectangles_;
}
Rectangle hitbox::hitbox::get_box(size_t index){
    return rectangles_[index];
}