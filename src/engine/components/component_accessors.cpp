#include "component.h"

// definitions for the accessors declared on each component in component.h

Vector2 components::position_component::get_position(){
    return position_;
}
type_config::path components::movement_component::get_current_path(){
    return paths_.empty() ? type_config::path{} : paths_.front();
}
Vector2 components::movement_component::get_move_speed(){
    return move_speed_;
}
Vector2 components::movement_component::get_direction_scalar(){
    return direction_scalar_;
}

std::vector<game_config::control>& components::key_input_component::get_controls(){
    return controls_;
}
std::vector<game_config::mouse_input>& components::mouse_input_component::get_inputs(){
    return inputs_;
}

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
