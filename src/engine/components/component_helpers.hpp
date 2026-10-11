#ifndef COMPONENT_HELPERS_HPP
#define COMPONENT_HELPERS_HPP

#include "component.h"

// thin forwarders to the right manager; defined in component_helpers.cpp
namespace component_helpers{
    void register_entity_positional_component(size_t entity_id, components::position_component component);
    void register_entity_movement_component(size_t entity_id, components::movement_component component);
    void register_entity_renderable_component(size_t entity_id, components::renderable_component component);
    void register_entity_collision_component(size_t entity_id, components::collision_component component);
    void register_entity_interactor_component(size_t entity_id, components::interactor_component component);
    void register_entity_interactable_component(size_t entity_id, components::interactable_component component);
    void register_entity_key_input_component(size_t entity_id, components::key_input_component component);
    void register_entity_mouse_input_component(size_t entity_id, components::mouse_input_component component);
    void register_entity_state_machine_component(size_t entity_id, components::state_machine_component component);
    void register_entity_selectable_component(size_t entity_id, components::selectable_component component);
    void register_entity_storage_component(size_t entity_id, components::storage_component component);
    void register_entity_food_component(size_t entity_id, components::food_component component);
    void register_entity_carrier_component(size_t entity_id, components::carrier_component component);
    void register_entity_recipes_component(size_t entity_id, components::recipes_component component);
    void register_hud_component(size_t entity_id, components::hud_component component);
    void register_hud_positional_component(size_t hud_id, components::position_component component);
    void register_hud_renderable_component(size_t hud_id, components::renderable_component component);
    void register_hud_collision_component(size_t hud_id, components::collision_component component);
    void register_hud_state_machine_component(size_t hud_id, components::state_machine_component component);
    void register_hud_clickable_component(size_t hud_id, components::clickable_component component);
    void register_hud_owner_component(size_t hud_id, components::owner_component component);


    void add_entity_positional_component(size_t entity_id, Vector2 position);
    void add_entity_movement_component(size_t entity_id, Vector2 move_speed,
        Vector2 direction_scalar = level_config::direction_scalars[level_config::directions::right],
        std::queue<path::path> paths = {});
    void add_entity_recipes_component(size_t entity_id, std::vector<recipe::recipe>& recipes);
    void add_hud_component(size_t entity_id, sprite::nine_sprite& base, Vector2 offset = Vector2Zero());
    void add_hud_positional_component(size_t hud_id, Vector2 position);
    void add_hud_renderable_component(size_t hud_id,
        std::vector<components::renderable_component::body>& bodys);
    void add_hud_collision_component(size_t hud_id,
        components::collision_component::hitbox_component hitbox);
    void add_hud_state_machine_component(size_t hud_id, state_machine::state_machine machine);
    void add_hud_clickable_component(size_t hud_id, size_t action, int payload = 0);
    void add_hud_owner_component(size_t hud_id, size_t owner);
    void add_entity_renderable_component(size_t entity_id,
        std::vector<components::renderable_component::body>& bodys);
    void add_entity_collision_component(size_t entity_id,
        components::collision_component::hitbox_component hitbox);
    void add_entity_interactor_component(size_t entity_id, float reach,
        std::optional<size_t> target_entity_id = std::nullopt, std::vector<size_t> interactions = {});
    void add_entity_interactable_component(size_t entity_id, float reach,
        const std::array<std::optional<Vector2>, DIRECTIONS>& slot_offsets, std::vector<size_t> interactions = {});
    void add_entity_key_input_component(size_t entity_id, std::vector<game_config::input>& controls);
    void add_entity_mouse_input_component(size_t entity_id, std::vector<game_config::input>& inputs);
    void add_entity_state_machine_component(size_t entity_id, state_machine::state_machine machine);
    void add_entity_selectable_component(size_t entity_id, size_t kind);
    void add_entity_storage_component(size_t entity_id);
    void add_entity_food_component(size_t entity_id, size_t item_id);
    void add_entity_carrier_component(size_t entity_id, Vector2 previous_position, Vector2* current_position, std::optional<size_t> carried_entity = std::nullopt);
    void add_stored_item(size_t entity_id, size_t slot, size_t item_id);
    std::optional<size_t> take_stored_item(size_t entity_id, size_t slot);
    void update_item_sprite(size_t entity_id, size_t slot);

    void create_offset_position_list(Rectangle box, std::array<std::optional<Vector2>, DIRECTIONS>& positions);
    bool is_mouse_positioned(size_t entity_id);
    void set_facing_index(size_t entity_id, size_t index);
    void set_active_index(size_t entity_id, size_t slot, size_t index);

    void unregister_entity_positional_component(size_t entity_id);
    void unregister_entity_movement_component(size_t entity_id);
    void unregister_entity_recipes_component(size_t entity_id);
    void unregister_entity_renderable_component(size_t entity_id);
    void unregister_entity_collision_component(size_t entity_id);
    void unregister_entity_interactor_component(size_t entity_id);
    void unregister_entity_interactable_component(size_t entity_id);
    void unregister_entity_key_input_component(size_t entity_id);
    void unregister_entity_mouse_input_component(size_t entity_id);
    void unregister_entity_state_machine_component(size_t entity_id);
    void unregister_entity_selectable_component(size_t entity_id);
    void unregister_entity_storage_component(size_t entity_id);
    void unregister_entity_food_component(size_t entity_id);
    void unregister_entity_carrier_component(size_t entity_id);
    void unregister_hud_component(size_t entity_id);
    void unregister_hud_positional_component(size_t hud_id);
    void unregister_hud_renderable_component(size_t hud_id);
    void unregister_hud_collision_component(size_t hud_id);
    void unregister_hud_state_machine_component(size_t hud_id);
    void unregister_hud_clickable_component(size_t hud_id);
    void unregister_hud_owner_component(size_t hud_id);
    void unregister_all_entity_components(size_t entity_id);
    void unregister_all_hud_components(size_t hud_id);

    size_t num_registered_entity_components(size_t entity_id);
    size_t num_registered_hud_components(size_t hud_id);
    void clear_all_entity_components();
    void clear_all_hud_components();
    void clear_all_components();
}

#endif
