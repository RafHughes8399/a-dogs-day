#include "entities.h"
#include <iostream>
// --------------------------- entity --------------------------- // 
bool entities::entity::check_collision(const hitbox::hitbox other){
    return hitbox_.check_collision(other);
}
int entities::entity::get_id(){
    return id_;
}
hitbox::hitbox& entities::entity::get_hitbox(){
    return hitbox_;
}

sprite::sprite& entities::entity::get_sprite(){
    return sprite_;
}

Vector2 entities::entity::get_position(){
    return position_;
}

void entities::entity::render(){
    sprite_.render(position_);
    auto box = hitbox_.get_box();
    DrawRectangleLines(box.x, box.y, box.width, box.height, GREEN);
}


// --------------------------- builder --------------------------- //

// defining the builder
entities::entity_builder entities::e_builder;


