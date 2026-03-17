#include "entities.h"
// --------------------------- entity --------------------------- // 
bool entities::entity::check_collision(const hitbox::hitbox other){
    return body_.get_hitbox().check_collision(other);
}
body::body& entities::entity::get_body(){
    return body_;
}
hitbox::hitbox& entities::entity::get_hitbox(){
    return body_.get_hitbox();
}
int entities::entity::get_id(){
    return id_;
}

Vector2 entities::entity::get_position(){
    return position_;
}

void entities::entity::render(Vector2 draw_position, int frame){
    body_.render(draw_position, frame);

}
void entities::entity::move(Vector2 new_position){
    // update hte position
    position_ = new_position;

    // update the hitboxes
    body_.update_hitboxes(position_);
    // move in the quadtree
    std::unique_ptr<events::event> move_event = std::make_unique<events::move_entity>(id_);
    event_interface::queue_event(move_event);
}

// --------------------------- builder --------------------------- //

// defining the builder
entities::entity_builder entities::e_builder;


