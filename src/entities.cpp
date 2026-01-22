#include "entities.h"
#include <iostream>
// --------------------------- entity --------------------------- // 
bool entities::entity::check_collision(const hitbox::hitbox other){
    return hitboxes_[sprites_.index()].check_collision(other);
}
hitbox::hitbox& entities::entity::get_hitbox(){
    return hitboxes_[sprites_.index()];
}
int entities::entity::get_id(){
    return id_;
}

sprite::spriteset& entities::entity::get_spriteset(){
    return sprites_;
}

Vector2 entities::entity::get_position(){
    return position_;
}

void entities::entity::render(Vector2 draw_position ){
    sprites_.render(draw_position);
    auto box = hitboxes_[sprites_.index()].get_box();
    DrawRectangleLines(draw_position.x, draw_position.y, box.width, box.height, GREEN);
}


// --------------------------- builder --------------------------- //

// defining the builder
entities::entity_builder entities::e_builder;


