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
    // draw the hitbox at its actual place 

    DrawRectangleLines(box.x, box.y, box.width, box.height, GREEN);
}
void entities::entity::move(Vector2 new_position){
    // update hte position
    position_ = new_position;

    // update the hitboxes
    std::for_each(hitboxes_.begin(), hitboxes_.end(), [this](hitbox::hitbox& h) -> void {h.update(position_);});
    // move in the quadtree
    std::unique_ptr<events::event> move_event = std::make_unique<events::move_entity>(id_);
    event_interface::queue_event(move_event);
}

// --------------------------- builder --------------------------- //

// defining the builder
entities::entity_builder entities::e_builder;


