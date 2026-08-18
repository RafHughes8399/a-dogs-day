#include "component.h"

// TODO implement all
components::position_component component_builders::build_positional_component(Vector2 position){
    return components::position_component(position);
}

// * direction defaults to right, paths default to empty list {}
components::movement_component component_builders::build_movement_component(Vector2 move_speed, Vector2 direction_scalar, std::queue<path::path> paths){
    return components::movement_component(move_speed, direction_scalar, paths);
}
components::renderable_component::sprite_component component_builders::build_sprite_component(std::vector<sprite::sprite>& sprites, size_t index){
    return components::renderable_component::sprite_component(sprites, index);
}
components::renderable_component component_builders::build_renderable_component(std::vector<components::renderable_component::sprite_component>& sprite_components){
    return components::renderable_component(sprite_components);
}
components::collision_component::hitbox_component component_builders::build_hitbox_component(std::vector<hitbox::hitbox>& hitboxes, size_t index){
    return components::collision_component::hitbox_component(hitboxes, index);
}
components::collision_component component_builders::build_collision_component(components::collision_component::hitbox_component hitboxes){
    return components::collision_component(std::move(hitboxes));
}
components::interactor_component component_builders::build_interactor_component(float reach, std::optional<size_t> entity_id){
    return components::interactor_component(reach, entity_id);
}
components::interactable_component component_builders::build_interactable_component(float reach,
    const std::array<std::optional<Vector2>, DIRECTIONS>& slot_offsets){
    return components::interactable_component(reach, slot_offsets);
}
components::key_input_component component_builders::build_key_input_component(std::vector<game_config::input>& controls){
    return components::key_input_component(controls);
}
components::mouse_input_component component_builders::build_mouse_input_component(std::vector<game_config::input>& inputs){
    return components::mouse_input_component(inputs);
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
components::selectable_component component_builders::build_selectable_component(size_t kind){
    return components::selectable_component(kind);
}
