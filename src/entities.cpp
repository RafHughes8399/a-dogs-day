#include "entities.h"
// --------------------------- entity --------------------------- // 
raglib::bounding_box_2& entities::entity::get_bounds(){
    return bounds_;
}

Vector2 entities::entity::get_position(){
    return position_;
}
int entities::entity::get_id(){
    return id_;
}

void entities::entity::render(){
    sprite_.render(position_);
}

// --------------------------- cursor --------------------------- // 

int entities::cursor::update(float delta){
    (void) delta;
    return status_codes::nothing;
}

void entities::cursor::interact(entity& other){
    (void) other;
    return;
}