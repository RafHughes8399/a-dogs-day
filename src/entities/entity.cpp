#include "entities.h"
#include <string>
// --------------------------- entity --------------------------- //
bool entities::entity::check_collision(const hitbox::hitbox other){
    return body_.get_hitbox().check_collision(other);
}
body::body& entities::entity::get_body(){
    return body_;
}
const std::string& entities::entity::get_debug_id(){
    return debug_id_;
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

void entities::entity::move(Vector2 new_position){
    move_without_event(new_position);
    std::unique_ptr<events::event> move_event = std::make_unique<events::move_entity>(id_);
    event_interface::queue_event(move_event);
}
// TODO: change the name of this function
void entities::entity::move_without_event(Vector2 new_position){
    position_ = new_position;
    body_.update_hitboxes(position_);
}

void entities::entity::render(Vector2 draw_position, int frame){
    body_.render(draw_position, frame);

}
