#include "component.h"

// TODO implement all
components::position_component component_builders::build_positional_component(Vector2 position, Vector2 direction_scalar){
    return components::position_component(position, direction_scalar);
}

// * paths default to empty list {}
components::movement_component component_builders::build_movement_component(Vector2 move_speed, std::queue<type_config::path> paths){
    return components::movement_component(move_speed, paths);
}
components::renderable_component::sprite_component component_builders::build_sprite_component(std::vector<sprite::sprite>& sprites, size_t index){
    return components::renderable_component::sprite_component(sprites, index);
}
components::renderable_component component_builders::build_renderable_component(std::vector<components::renderable_component::sprite_component>& sprite_components){
    (void) sprite_components;
    return components::renderable_component();
}
components::collision_component component_builders::build_collision_component(){
    return components::collision_component();
}
components::interaction_component component_builders::build_interaction_component(){
    return components::interaction_component();
}
components::controls_component component_builders::build_controls_component(std::vector<game_config::control>& controls){
    return components::controls_component(controls);
}
components::state_machine_component::state_component component_builders::build_state(){
    return components::state_machine_component::state_component();
}
components::state_machine_component component_builders::build_state_machine_component(std::vector<components::state_machine_component::state_component>& state_components){
    (void) state_components;
    return components::state_machine_component();
}
components::food_component component_builders::build_food_component(){
    return components::food_component();
}