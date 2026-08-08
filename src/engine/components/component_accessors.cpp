#include "component.h"

// definitions for the accessors declared on each component in component.h

// ---------------- position_component ----------------
Vector2 components::position_component::get_position(){
    return position_;
}
void components::position_component::set_position(Vector2 position){
    position_ = position;
}

// ---------------- movement_component ----------------
type_config::path components::movement_component::get_current_path(){
    return paths_.empty() ? type_config::path{} : paths_.front();
}
Vector2 components::movement_component::get_move_speed(){
    return move_speed_;
}
Vector2 components::movement_component::get_direction_scalar(){
    return direction_scalar_;
}

// ---------------- input components ----------------
std::vector<game_config::input>& components::key_input_component::get_inputs(){
    return controls_;
}
std::vector<game_config::input>& components::mouse_input_component::get_inputs(){
    return inputs_;
}

// ---------------- renderable_component ----------------
sprite::sprite& components::renderable_component::sprite_component::get_sprite(){
    return sprites_[sprite_index_];
}
size_t components::renderable_component::sprite_component::get_sprite_index() const{
    return sprite_index_;
}
void components::renderable_component::sprite_component::set_index(size_t index){
    sprite_index_ = index;
}

std::vector<components::renderable_component::sprite_component>& components::renderable_component::get_sprites(){
    return sprites_;
}

// ---------------- collision_component ----------------
std::vector<hitbox::hitbox>& components::collision_component::hitbox_component::get_hitboxes(){
    return hitboxes_;
}
hitbox::hitbox& components::collision_component::hitbox_component::get_hitbox(){
    return hitboxes_[hitbox_index_];
}
size_t components::collision_component::hitbox_component::get_hitbox_index() const{
    return hitbox_index_;
}
void components::collision_component::hitbox_component::set_index(size_t index){
    hitbox_index_ = index;
}

components::collision_component::hitbox_component& components::collision_component::get_hitbox_component(){
    return hitbox_component_;
}
