#include "component.h"
#include <cassert>

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
void component_helpers::register_selectable_component(size_t entity_id, components::selectable_component component){
    component_managers::selectable_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_storage_component(size_t entity_id, components::storage_component component){
    component_managers::storage_manager_.register_component(entity_id, std::move(component));
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
    std::optional<size_t> target_entity_id, std::vector<size_t> interactions){
    register_interactor_component(entity_id,
        component_builders::build_interactor_component(reach, target_entity_id, interactions));
}
void component_helpers::add_interactable_component(size_t entity_id, float reach,
    const std::array<std::optional<Vector2>, DIRECTIONS>& slot_offsets, std::vector<size_t> interactions){
    register_interactable_component(entity_id,
        component_builders::build_interactable_component(reach, slot_offsets, interactions));
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
void component_helpers::add_selectable_component(size_t entity_id, size_t kind){
    register_selectable_component(entity_id,
        component_builders::build_selectable_component(kind));
}
void component_helpers::add_storage_component(size_t entity_id){
    register_storage_component(entity_id,
        component_builders::build_storage_component());
}

void component_helpers::create_offset_position_list(Rectangle box, std::array<std::optional<Vector2>, DIRECTIONS>& positions){
    // * a half edgeweight step lands on the centre of the neighbouring cell, the
    // * point furthest from either boundary - cell_at floors, so it resolves there
    // * with no tie to break
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
void component_helpers::set_sprite_index(size_t entity_id, size_t slot, size_t index){
    if(auto* renderable = component_managers::renderable_manager_.get_component(entity_id)){
        if(auto* slot_sprites = renderable->get_sprite_component(slot)){
            if(index < slot_sprites->num_sprites()){
                slot_sprites->set_index(index);
            }
        }
    }
}
void component_helpers::add_stored_item(size_t entity_id, size_t slot, size_t item_id){
    auto* storage = component_managers::storage_manager_.get_component(entity_id);
    if(storage == nullptr){ return; }
    storage->place(item_id);
    update_item_sprite(entity_id, slot);
}
std::optional<size_t> component_helpers::take_stored_item(size_t entity_id, size_t slot){
    auto* storage = component_managers::storage_manager_.get_component(entity_id);
    if(storage == nullptr or storage->empty()){ return std::nullopt; }
    auto item_id = storage->take();
    update_item_sprite(entity_id, slot);
    return item_id;
}
void component_helpers::update_item_sprite(size_t entity_id, size_t slot){
    auto* storage = component_managers::storage_manager_.get_component(entity_id);
    auto* renderable = component_managers::renderable_manager_.get_component(entity_id);
    if(storage == nullptr or renderable == nullptr){ return; }
    if(storage->empty()){
        renderable->remove_sprite_component(slot);
        return;
    }
    auto item_id = storage->head().get_id();
    if(renderable->get_sprite_component(slot) != nullptr){
        set_sprite_index(entity_id, slot, item_id);
        return;
    }
    auto item_sprites = sprite_builders::build_food_sprites();
    if(item_id >= item_sprites.size()){ return; }
    assert(renderable->num_sprite_components() == slot
        and "stored item sprite must append into its reserved slot");
    renderable->add_sprite_component(
        component_builders::build_sprite_component(item_sprites, item_id));
}
// writes sprite and hitbox indices together. missing either component is fine
void component_helpers::set_facing_index(size_t entity_id, size_t index){
    if(auto* renderable = component_managers::renderable_manager_.get_component(entity_id)){
        if(auto* body = renderable->get_sprite_component(0)){
            if(index < body->num_sprites()){
                body->set_index(index);
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
// * the interactor/interactable claim is a two-way handshake, so tearing either
// * side down has to undo the other half first - the link lives inside the
// * component about to be erased. neither direction can recurse: release only
// * writes interactors_, stop_interacting only writes target_
void component_helpers::unregister_interactor_component(size_t entity_id){
    if(auto* interactor = component_managers::interactor_manager_.get_component(entity_id)){
        if(auto target = interactor->get_target(); target.has_value()){
            if(auto* interactable = component_managers::interactable_manager_.get_component(target.value())){
                interactable->release(entity_id);
            }
        }
    }
    component_managers::interactor_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_interactable_component(size_t entity_id){
    if(auto* interactable = component_managers::interactable_manager_.get_component(entity_id)){
        for(auto slot : interactable->get_interactors()){
            if(not slot.has_value()){ continue; }
            if(auto* interactor = component_managers::interactor_manager_.get_component(slot.value())){
                interactor->stop_interacting();
            }
        }
    }
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
void component_helpers::unregister_selectable_component(size_t entity_id){
    component_managers::selectable_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_storage_component(size_t entity_id){
    component_managers::storage_manager_.unregister_component(entity_id);
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
    unregister_selectable_component(entity_id);
    unregister_storage_component(entity_id);
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
    count += component_managers::selectable_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::storage_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
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
    component_managers::selectable_manager_.clear();
    component_managers::storage_manager_.clear();
}
