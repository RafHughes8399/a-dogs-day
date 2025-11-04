#include "entities.h"
#include <iostream>
// --------------------------- entity --------------------------- // 
int entities::entity::get_id(){
    return id_;
}
raglib::bounding_box_2& entities::entity::get_bounds(){
    return bounds_;
}

sprite::sprite& entities::entity::get_sprite(){
    return sprite_;
}

Vector2 entities::entity::get_position(){
    return position_;
}


void entities::entity::update_bounds(Vector2 delta){
    bounds_.min = Vector2Add(bounds_.min, delta);
    bounds_.max = Vector2Add(bounds_.max, delta);
}
void entities::entity::render(){
    sprite_.render(position_);
}


// --------------------------- builder --------------------------- //

// defining the builder
entities::entity_builder entities::e_builder;


