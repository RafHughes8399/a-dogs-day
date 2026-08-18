#include "component.h"

void component_helpers::register_positional_component(size_t entity_id, components::position_component component){
    component_managers::positional_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_movement_component(size_t entity_id, components::movement_component component){
    component_managers::movement_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_renderable_component(size_t entity_id, components::renderable_component component){
    component_managers::renderable_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_collision_component(size_t entity_id, components::collision_component component){
    component_managers::collision_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_interactor_component(size_t entity_id, components::interactor_component component){
    component_managers::interactor_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_interactable_component(size_t entity_id, components::interactable_component component){
    component_managers::interactable_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_key_input_component(size_t entity_id, components::key_input_component component){
    component_managers::control_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_mouse_input_component(size_t entity_id, components::mouse_input_component component){
    component_managers::mouse_input_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_state_machine_component(size_t entity_id, components::state_machine_component component){
    component_managers::state_machine_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_food_component(size_t entity_id, components::food_component component){
    component_managers::food_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_selectable_component(size_t entity_id, components::selectable_component component){
    component_managers::selectable_manager_.register_component(entity_id, std::move(component));
}

void component_helpers::add_positional_component(size_t entity_id, Vector2 position){
    register_positional_component(entity_id,
        component_builders::build_positional_component(position));
}
void component_helpers::add_movement_component(size_t entity_id, Vector2 move_speed,
    Vector2 direction_scalar, std::queue<path::path> paths){
    register_movement_component(entity_id,
        component_builders::build_movement_component(move_speed, direction_scalar, std::move(paths)));
}
void component_helpers::add_renderable_component(size_t entity_id,
    std::vector<components::renderable_component::sprite_component>& sprite_components){
    register_renderable_component(entity_id,
        component_builders::build_renderable_component(sprite_components));
}
void component_helpers::add_collision_component(size_t entity_id,
    components::collision_component::hitbox_component hitbox){
    register_collision_component(entity_id,
        component_builders::build_collision_component(std::move(hitbox)));
}
void component_helpers::add_interactor_component(size_t entity_id, float reach,
    std::optional<size_t> target_entity_id){
    register_interactor_component(entity_id,
        component_builders::build_interactor_component(reach, target_entity_id));
}
void component_helpers::add_interactable_component(size_t entity_id, float reach,
    const std::array<std::optional<Vector2>, DIRECTIONS>& slot_offsets){
    register_interactable_component(entity_id,
        component_builders::build_interactable_component(reach, slot_offsets));
}
void component_helpers::add_key_input_component(size_t entity_id, std::vector<game_config::input>& controls){
    register_key_input_component(entity_id,
        component_builders::build_key_input_component(controls));
}
void component_helpers::add_mouse_input_component(size_t entity_id, std::vector<game_config::input>& inputs){
    register_mouse_input_component(entity_id,
        component_builders::build_mouse_input_component(inputs));
}
void component_helpers::add_state_machine_component(size_t entity_id,
    std::vector<components::state_machine_component::state_component>& state_components){
    register_state_machine_component(entity_id,
        component_builders::build_state_machine_component(state_components));
}
void component_helpers::add_food_component(size_t entity_id){
    register_food_component(entity_id, component_builders::build_food_component());
}
void component_helpers::add_selectable_component(size_t entity_id, size_t kind){
    register_selectable_component(entity_id,
        component_builders::build_selectable_component(kind));
}

void component_helpers::create_offset_position_list(Rectangle box, std::array<std::optional<Vector2>, DIRECTIONS>& positions){
    // * left is - 0.5 edgeweight x, height / 2 y
    if(positions[level_config::directions::left].has_value()){
        positions[level_config::directions::left]->x = level_config::edge_weight * -0.5;
        positions[level_config::directions::left]->y = box.height * 0.5;
    }
    // * right is width + 0.5 edgeweight x, height / 2 y
    if(positions[level_config::directions::right].has_value()){
        positions[level_config::directions::right]->x = box.width + level_config::edge_weight * 0.5;
        positions[level_config::directions::right]->y = box.height * 0.5;
    }
    // * up is width / 2 x, - 0.5 edgeweight y
    if(positions[level_config::directions::up].has_value()){
        positions[level_config::directions::up]->x = box.width * 0.5;
        positions[level_config::directions::up]->y = level_config::edge_weight * -0.5;
    }
    // * down is width / 2 x, height + 0.5 edgeweight y
    if(positions[level_config::directions::down].has_value()){
        positions[level_config::directions::down]->x = box.width * 0.5;
        positions[level_config::directions::down]->y = box.height + level_config::edge_weight * 0.5;
    }
}
bool component_helpers::is_mouse_positioned(size_t entity_id){
    return component_managers::mouse_input_manager_.get_component(entity_id) != nullptr;
}
// writes sprite and hitbox indices together. missing either component is fine
void component_helpers::set_facing_index(size_t entity_id, size_t index){
    if(auto* renderable = component_managers::renderable_manager_.get_component(entity_id)){
        for(auto& sprite_component : renderable->get_sprites()){
            if(index < sprite_component.num_sprites()){
                sprite_component.set_index(index);
            }
        }
    }
    if(auto* collision = component_managers::collision_manager_.get_component(entity_id)){
        auto& hitboxes = collision->get_hitbox_component();
        if(index < hitboxes.num_hitboxes()){
            hitboxes.set_index(index);
        }
    }
}

void component_helpers::unregister_positional_component(size_t entity_id){
    component_managers::positional_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_movement_component(size_t entity_id){
    component_managers::movement_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_renderable_component(size_t entity_id){
    component_managers::renderable_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_collision_component(size_t entity_id){
    component_managers::collision_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_interactor_component(size_t entity_id){
    component_managers::interactor_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_interactable_component(size_t entity_id){
    component_managers::interactable_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_key_input_component(size_t entity_id){
    component_managers::control_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_mouse_input_component(size_t entity_id){
    component_managers::mouse_input_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_state_machine_component(size_t entity_id){
    component_managers::state_machine_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_food_component(size_t entity_id){
    component_managers::food_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_selectable_component(size_t entity_id){
    component_managers::selectable_manager_.unregister_component(entity_id);
}

// blanket teardown - erase on a missing key is a no-op, so this is correct
// for every entity kind without tracking what a builder registered
void component_helpers::unregister_all_components(size_t entity_id){
    unregister_positional_component(entity_id);
    unregister_movement_component(entity_id);
    unregister_renderable_component(entity_id);
    unregister_collision_component(entity_id);
    unregister_interactor_component(entity_id);
    unregister_interactable_component(entity_id);
    unregister_key_input_component(entity_id);
    unregister_mouse_input_component(entity_id);
    unregister_state_machine_component(entity_id);
    unregister_food_component(entity_id);
    unregister_selectable_component(entity_id);
}

// total components registered across every manager
size_t component_helpers::num_registered_components(size_t entity_id){
    size_t count = 0;
    count += component_managers::positional_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::movement_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::renderable_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::collision_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::interactor_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::interactable_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::control_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::mouse_input_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::state_machine_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::food_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::selectable_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    return count;
}

// wipes every manager - for the test harness between scenarios
void component_helpers::clear_all_components(){
    component_managers::positional_manager_.clear();
    component_managers::movement_manager_.clear();
    component_managers::renderable_manager_.clear();
    component_managers::collision_manager_.clear();
    component_managers::interactor_manager_.clear();
    component_managers::interactable_manager_.clear();
    component_managers::control_manager_.clear();
    component_managers::mouse_input_manager_.clear();
    component_managers::state_machine_manager_.clear();
    component_managers::food_manager_.clear();
    component_managers::selectable_manager_.clear();
}
