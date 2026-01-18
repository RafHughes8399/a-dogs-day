#include "entities.h"
#include <iostream>
// --------------------------- entity --------------------------- // 
bool entities::entity::check_collision(const hitbox::hitbox other){
    return hitboxes_[sprite_index_].check_collision(other);
}
int entities::entity::get_id(){
    return id_;
}
hitbox::hitbox& entities::entity::get_hitbox(){
    return hitboxes_[sprite_index_];
}

std::vector<sprite::sprite>& entities::entity::get_sprites(){
    return sprites_;
}

Vector2 entities::entity::get_position(){
    return position_;
}

void entities::entity::render(Vector2 draw_position ){
    sprites_[sprite_index_].render(draw_position);
    auto box = hitboxes_[sprite_index_].get_box();
    DrawRectangleLines(draw_position.x, draw_position.y, box.width, box.height, GREEN);
}


// --------------------------- builder --------------------------- //

// defining the builder
entities::entity_builder entities::e_builder;


