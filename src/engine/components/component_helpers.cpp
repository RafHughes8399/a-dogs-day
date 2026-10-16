#include "component_helpers.hpp"
#include "component_managers.hpp"
#include <cassert>
#include <cmath>

void component_helpers::register_entity_positional_component(size_t entity_id, components::position_component component){
    component_managers::entity_positional_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_entity_movement_component(size_t entity_id, components::movement_component component){
    component_managers::entity_movement_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_entity_renderable_component(size_t entity_id, components::renderable_component component){
    component_managers::entity_renderable_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_entity_collision_component(size_t entity_id, components::collision_component component){
    component_managers::entity_collision_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_entity_interactor_component(size_t entity_id, components::interactor_component component){
    component_managers::entity_interactor_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_entity_interactable_component(size_t entity_id, components::interactable_component component){
    component_managers::entity_interactable_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_entity_key_input_component(size_t entity_id, components::key_input_component component){
    component_managers::entity_control_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_entity_mouse_input_component(size_t entity_id, components::mouse_input_component component){
    component_managers::entity_mouse_input_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_entity_state_machine_component(size_t entity_id, components::state_machine_component component){
    component_managers::entity_state_machine_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_entity_selectable_component(size_t entity_id, components::selectable_component component){
    component_managers::entity_selectable_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_entity_storage_component(size_t entity_id, components::storage_component component){
    component_managers::entity_storage_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_entity_food_component(size_t entity_id, components::food_component component){
    component_managers::entity_food_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_entity_carrier_component(size_t entity_id, components::carrier_component component){
    component_managers::entity_carrier_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_entity_recipes_component(size_t entity_id, components::recipes_component component){
    component_managers::entity_recipes_manager_.register_component(entity_id, component);
}
void component_helpers::register_hud_component(size_t entity_id, components::hud_component component){
    component_managers::hud_manager_.register_component(entity_id, std::move(component));
}
void component_helpers::register_hud_positional_component(size_t hud_id, components::position_component component){
    component_managers::hud_positional_manager_.register_component(hud_id, std::move(component));
}
void component_helpers::register_hud_renderable_component(size_t hud_id, components::renderable_component component){
    component_managers::hud_renderable_manager_.register_component(hud_id, std::move(component));
}
void component_helpers::register_hud_collision_component(size_t hud_id, components::collision_component component){
    component_managers::hud_collision_manager_.register_component(hud_id, std::move(component));
}
void component_helpers::register_hud_state_machine_component(size_t hud_id, components::state_machine_component component){
    component_managers::hud_state_machine_manager_.register_component(hud_id, std::move(component));
}
void component_helpers::register_hud_clickable_component(size_t hud_id, components::clickable_component component){
    component_managers::hud_clickable_manager_.register_component(hud_id, std::move(component));
}
void component_helpers::register_hud_owner_component(size_t hud_id, components::owner_component component){
    component_managers::hud_owner_manager_.register_component(hud_id, std::move(component));
}
void component_helpers::add_entity_positional_component(size_t entity_id, Vector2 position){
    register_entity_positional_component(entity_id,
        component_builders::build_entity_positional_component(position));
}
void component_helpers::add_entity_movement_component(size_t entity_id, Vector2 move_speed,
    Vector2 direction_scalar, std::queue<path::path> paths){
    register_entity_movement_component(entity_id,
        component_builders::build_entity_movement_component(move_speed, direction_scalar, std::move(paths)));
}
void component_helpers::add_entity_renderable_component(size_t entity_id,
    std::vector<components::renderable_component::body>& bodys){
    register_entity_renderable_component(entity_id,
        component_builders::build_entity_renderable_component(bodys));
}
void component_helpers::add_entity_collision_component(size_t entity_id,
    components::collision_component::hitbox_component hitbox){
    register_entity_collision_component(entity_id,
        component_builders::build_entity_collision_component(std::move(hitbox)));
}
void component_helpers::add_entity_interactor_component(size_t entity_id, float reach,
    std::optional<size_t> target_entity_id, std::vector<size_t> interactions){
    register_entity_interactor_component(entity_id,
        component_builders::build_entity_interactor_component(reach, target_entity_id, interactions));
}
void component_helpers::add_entity_interactable_component(size_t entity_id, float reach,
    const std::array<std::optional<Vector2>, DIRECTIONS>& slot_offsets, std::vector<size_t> interactions){
    register_entity_interactable_component(entity_id,
        component_builders::build_entity_interactable_component(reach, slot_offsets, interactions));
}
void component_helpers::add_entity_key_input_component(size_t entity_id, std::vector<game_config::input>& controls){
    register_entity_key_input_component(entity_id,
        component_builders::build_entity_key_input_component(controls));
}
void component_helpers::add_entity_mouse_input_component(size_t entity_id, std::vector<game_config::input>& inputs){
    register_entity_mouse_input_component(entity_id,
        component_builders::build_entity_mouse_input_component(inputs));
}
void component_helpers::add_entity_state_machine_component(size_t entity_id, state_machine::state_machine machine){
    register_entity_state_machine_component(entity_id,
        component_builders::build_entity_state_machine_component(std::move(machine)));
}
void component_helpers::add_entity_selectable_component(size_t entity_id, size_t kind){
    register_entity_selectable_component(entity_id,
        component_builders::build_entity_selectable_component(kind));
}
void component_helpers::add_entity_storage_component(size_t entity_id){
    register_entity_storage_component(entity_id,
        component_builders::build_entity_storage_component());
}
void component_helpers::add_entity_food_component(size_t entity_id, size_t item_id){
    register_entity_food_component(entity_id,
        component_builders::build_entity_food_component(item_id));
}
void component_helpers::add_entity_carrier_component(size_t entity_id, Vector2 previous_position, Vector2* current_position, std::optional<size_t> carried_entity){
    register_entity_carrier_component(entity_id,
        component_builders::build_entity_carrier_component(previous_position, current_position, carried_entity));
}
void component_helpers::add_entity_recipes_component(size_t entity_id, std::vector<recipe::recipe>& recipes){
    register_entity_recipes_component(entity_id,
        component_builders::build_entity_recipes_component(recipes));
}
void component_helpers::add_hud_component(size_t entity_id, sprite::nine_sprite& base, Vector2 offset){
    register_hud_component(entity_id,
        component_builders::build_hud_component(base, offset));
}
void component_helpers::add_hud_positional_component(size_t hud_id, Vector2 position){
    register_hud_positional_component(hud_id,
        component_builders::build_hud_positional_component(position));
}
void component_helpers::add_hud_renderable_component(size_t hud_id,
    std::vector<components::renderable_component::body>& bodys){
    register_hud_renderable_component(hud_id,
        component_builders::build_hud_renderable_component(bodys));
}
void component_helpers::add_hud_collision_component(size_t hud_id,
    components::collision_component::hitbox_component hitbox){
    register_hud_collision_component(hud_id,
        component_builders::build_hud_collision_component(std::move(hitbox)));
}
void component_helpers::add_hud_state_machine_component(size_t hud_id, state_machine::state_machine machine){
    register_hud_state_machine_component(hud_id,
        component_builders::build_hud_state_machine_component(std::move(machine)));
}
void component_helpers::add_hud_clickable_component(size_t hud_id, size_t action, int payload){
    register_hud_clickable_component(hud_id,
        component_builders::build_hud_clickable_component(action, payload));
}
void component_helpers::add_hud_owner_component(size_t hud_id, size_t owner){
    register_hud_owner_component(hud_id,
        component_builders::build_hud_owner_component(owner));
}

void component_helpers::create_offset_position_list(Rectangle box, std::array<std::optional<Vector2>, DIRECTIONS>& positions){
    // * a half edgeweight step lands on the centre of the neighbouring cell, the
    // * point furthest from either boundary - cell_at floors, so it resolves there
    // * with no tie to break
    // * left is - 0.5 edgeweight x, height / 2 y
    if(positions[level_config::directions::left].has_value()){
        positions[level_config::directions::left]->x = level_config::edge_weight * -1.0f;
        positions[level_config::directions::left]->y = box.height * 0.5f;
    }
    // * right is width + 0.5 edgeweight x, height / 2 y
    if(positions[level_config::directions::right].has_value()){
        positions[level_config::directions::right]->x =
            std::ceil(box.width / level_config::edge_weight) * level_config::edge_weight;
        positions[level_config::directions::right]->y = box.height * 0.5f;
    }
    // * up is width / 2 x, - 0.5 edgeweight y
    if(positions[level_config::directions::up].has_value()){
        positions[level_config::directions::up]->x = box.width * 0.5f;
        positions[level_config::directions::up]->y = level_config::edge_weight * -0.5f;
    }
    // * down is width / 2 x, height + 0.5 edgeweight y
    if(positions[level_config::directions::down].has_value()){
        positions[level_config::directions::down]->x = box.width * 0.5f;
        positions[level_config::directions::down]->y = box.height + level_config::edge_weight * 0.5f;
    }
}
bool component_helpers::is_mouse_positioned(size_t entity_id){
    return component_managers::entity_mouse_input_manager_.get_component(entity_id) != nullptr;
}
void component_helpers::set_active_index(size_t entity_id, size_t slot, size_t index){
    if(auto* renderable = component_managers::entity_renderable_manager_.get_component(entity_id)){
        if(auto* slot_sprites = renderable->get_body(slot)){
            if(index < slot_sprites->num_sprites()){
                slot_sprites->set_index(index);
            }
        }
    }
}
void component_helpers::add_stored_item(size_t entity_id, size_t slot, size_t item_id){
    auto* storage = component_managers::entity_storage_manager_.get_component(entity_id);
    if(storage == nullptr){ return; }
    storage->place(item_id);
    update_item_sprite(entity_id, slot);
}
std::optional<size_t> component_helpers::take_stored_item(size_t entity_id, size_t slot){
    auto* storage = component_managers::entity_storage_manager_.get_component(entity_id);
    if(storage == nullptr or storage->empty()){ return std::nullopt; }
    auto item_id = storage->take();
    update_item_sprite(entity_id, slot);
    return item_id;
}
void component_helpers::update_item_sprite(size_t entity_id, size_t slot){
    auto* storage = component_managers::entity_storage_manager_.get_component(entity_id);
    auto* renderable = component_managers::entity_renderable_manager_.get_component(entity_id);
    if(storage == nullptr or renderable == nullptr){ return; }
    if(storage->empty()){
        renderable->remove_body(slot);
        return;
    }
    auto item_id = storage->head().get_id();
    if(renderable->get_body(slot) != nullptr){
        set_active_index(entity_id, slot, item_id);
        return;
    }
    auto item_sprites = sprite_builders::build_food_sprites();
    if(item_id >= item_sprites.size()){ return; }
    assert(renderable->num_bodys() == slot
        and "stored item sprite must append into its reserved slot");
    renderable->add_body(
        component_builders::build_entity_body(item_sprites, item_id));
}
// writes sprite and hitbox indices together. missing either component is fine
void component_helpers::set_facing_index(size_t entity_id, size_t index){
    if(auto* renderable = component_managers::entity_renderable_manager_.get_component(entity_id)){
        for(auto& slot : renderable->get_layers()){
            if(index < slot.num_sprites()){
                slot.set_index(index);
            }
        }
    }
    if(auto* collision = component_managers::entity_collision_manager_.get_component(entity_id)){
        auto& hitboxes = collision->get_hitbox_component();
        if(index < hitboxes.num_hitboxes()){
            hitboxes.set_index(index);
        }
    }
}

void component_helpers::unregister_entity_positional_component(size_t entity_id){
    component_managers::entity_positional_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_entity_movement_component(size_t entity_id){
    component_managers::entity_movement_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_entity_recipes_component(size_t entity_id){
    component_managers::entity_recipes_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_entity_renderable_component(size_t entity_id){
    component_managers::entity_renderable_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_entity_collision_component(size_t entity_id){
    component_managers::entity_collision_manager_.unregister_component(entity_id);
}
// * the interactor/interactable claim is a two-way handshake, so tearing either
// * side down has to undo the other half first - the link lives inside the
// * component about to be erased. neither direction can recurse: release only
// * writes interactors_, stop_interacting only writes target_
void component_helpers::unregister_entity_interactor_component(size_t entity_id){
    if(auto* interactor = component_managers::entity_interactor_manager_.get_component(entity_id)){
        if(auto target = interactor->get_target(); target.has_value()){
            if(auto* interactable = component_managers::entity_interactable_manager_.get_component(target.value())){
                interactable->release(entity_id);
            }
        }
    }
    component_managers::entity_interactor_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_entity_interactable_component(size_t entity_id){
    if(auto* interactable = component_managers::entity_interactable_manager_.get_component(entity_id)){
        for(auto slot : interactable->get_interactors()){
            if(not slot.has_value()){ continue; }
            if(auto* interactor = component_managers::entity_interactor_manager_.get_component(slot.value())){
                interactor->stop_interacting();
            }
        }
    }
    component_managers::entity_interactable_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_entity_key_input_component(size_t entity_id){
    component_managers::entity_control_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_entity_mouse_input_component(size_t entity_id){
    component_managers::entity_mouse_input_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_entity_state_machine_component(size_t entity_id){
    component_managers::entity_state_machine_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_entity_selectable_component(size_t entity_id){
    component_managers::entity_selectable_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_entity_storage_component(size_t entity_id){
    component_managers::entity_storage_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_entity_food_component(size_t entity_id){
    component_managers::entity_food_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_entity_carrier_component(size_t entity_id){
    component_managers::entity_carrier_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_hud_component(size_t entity_id){
    component_managers::hud_manager_.unregister_component(entity_id);
}
void component_helpers::unregister_hud_positional_component(size_t hud_id){
    component_managers::hud_positional_manager_.unregister_component(hud_id);
}
void component_helpers::unregister_hud_renderable_component(size_t hud_id){
    component_managers::hud_renderable_manager_.unregister_component(hud_id);
}
void component_helpers::unregister_hud_collision_component(size_t hud_id){
    component_managers::hud_collision_manager_.unregister_component(hud_id);
}
void component_helpers::unregister_hud_state_machine_component(size_t hud_id){
    component_managers::hud_state_machine_manager_.unregister_component(hud_id);
}
void component_helpers::unregister_hud_clickable_component(size_t hud_id){
    component_managers::hud_clickable_manager_.unregister_component(hud_id);
}
void component_helpers::unregister_hud_owner_component(size_t hud_id){
    component_managers::hud_owner_manager_.unregister_component(hud_id);
}

// blanket teardown - erase on a missing key is a no-op, so this is correct
// for every entity kind without tracking what a builder registered
void component_helpers::unregister_all_entity_components(size_t entity_id){
    unregister_entity_positional_component(entity_id);
    unregister_entity_movement_component(entity_id);
    unregister_entity_renderable_component(entity_id);
    unregister_entity_collision_component(entity_id);
    unregister_entity_interactor_component(entity_id);
    unregister_entity_interactable_component(entity_id);
    unregister_entity_key_input_component(entity_id);
    unregister_entity_mouse_input_component(entity_id);
    unregister_entity_state_machine_component(entity_id);
    unregister_entity_selectable_component(entity_id);
    unregister_entity_storage_component(entity_id);
    unregister_entity_food_component(entity_id);
    unregister_entity_carrier_component(entity_id);
    unregister_entity_recipes_component(entity_id);
    unregister_hud_component(entity_id);
}
void component_helpers::unregister_all_hud_components(size_t hud_id){
    unregister_hud_positional_component(hud_id);
    unregister_hud_renderable_component(hud_id);
    unregister_hud_collision_component(hud_id);
    unregister_hud_state_machine_component(hud_id);
    unregister_hud_clickable_component(hud_id);
    unregister_hud_owner_component(hud_id);
}

// total components registered across every manager
size_t component_helpers::num_registered_entity_components(size_t entity_id){
    size_t count = 0;
    count += component_managers::entity_positional_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::entity_movement_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::entity_renderable_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::entity_collision_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::entity_interactor_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::entity_interactable_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::entity_control_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::entity_mouse_input_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::entity_state_machine_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::entity_selectable_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::entity_storage_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::entity_food_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::entity_carrier_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::entity_recipes_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    count += component_managers::hud_manager_.get_component(entity_id) != nullptr ? 1u : 0u;
    return count;
}
size_t component_helpers::num_registered_hud_components(size_t hud_id){
    size_t count = 0;
    count += component_managers::hud_positional_manager_.get_component(hud_id) != nullptr ? 1u : 0u;
    count += component_managers::hud_renderable_manager_.get_component(hud_id) != nullptr ? 1u : 0u;
    count += component_managers::hud_collision_manager_.get_component(hud_id) != nullptr ? 1u : 0u;
    count += component_managers::hud_state_machine_manager_.get_component(hud_id) != nullptr ? 1u : 0u;
    count += component_managers::hud_clickable_manager_.get_component(hud_id) != nullptr ? 1u : 0u;
    count += component_managers::hud_owner_manager_.get_component(hud_id) != nullptr ? 1u : 0u;
    return count;
}

// wipes every manager - for the test harness between scenarios
void component_helpers::clear_all_components(){
    clear_all_entity_components();
    clear_all_hud_components();
}
void component_helpers::clear_all_hud_components(){
    component_managers::hud_positional_manager_.clear();
    component_managers::hud_renderable_manager_.clear();
    component_managers::hud_collision_manager_.clear();
    component_managers::hud_state_machine_manager_.clear();
    component_managers::hud_clickable_manager_.clear();
    component_managers::hud_owner_manager_.clear();
}
void component_helpers::clear_all_entity_components(){
    component_managers::entity_positional_manager_.clear();
    component_managers::entity_movement_manager_.clear();
    component_managers::entity_renderable_manager_.clear();
    component_managers::entity_collision_manager_.clear();
    component_managers::entity_interactor_manager_.clear();
    component_managers::entity_interactable_manager_.clear();
    component_managers::entity_control_manager_.clear();
    component_managers::entity_mouse_input_manager_.clear();
    component_managers::entity_state_machine_manager_.clear();
    component_managers::entity_selectable_manager_.clear();
    component_managers::entity_storage_manager_.clear();
    component_managers::entity_food_manager_.clear();
    component_managers::entity_carrier_manager_.clear();
    component_managers::entity_recipes_manager_.clear();
    component_managers::hud_manager_.clear();
}
