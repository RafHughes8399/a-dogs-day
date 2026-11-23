#include "entity_systems/entity_systems.hpp"
#include "game_config.h"
#include "system.h"
#include "component_managers.hpp"

// ---------------- helpers ----------------
hitbox::hitbox* systems::spatial_system::bounds_for(size_t entity_id){
    auto* collision = component_managers::entity_collision_manager_.get_component(entity_id);
    if(collision == nullptr){ return nullptr; }
    return &collision->get_hitbox_component().get_hitbox();
}

// ---------------- event handlers ----------------
void systems::spatial_system::on_created_entity(const events::create_entity& event){
    auto entity_id = event.get_id();
    auto* bounds = bounds_for(entity_id);
    if(bounds == nullptr){ return; }
    entities_.insert(entity_id, *bounds);
}

// movement_system::update_position moves the hitbox before firing this, so the
// bounds read here are already the new ones
void systems::spatial_system::on_moved_entity(const events::move_entity& event){
    auto entity_id = event.get_id();
    auto* bounds = bounds_for(entity_id);
    if(bounds == nullptr){ return; }
    entities_.move(entity_id, *bounds);
}

void systems::spatial_system::on_destroyed_entity(const events::remove_entity& event){
    entities_.erase(event.get_id());
}

bool systems::spatial_system::can_place_entity(size_t entity_id, size_t cursor_id,  Vector2 position){
    auto collision = component_managers::entity_collision_manager_.get_component(entity_id);
    if(not collision) {return false;}
    auto box = collision->get_hitbox_component().get_hitbox().get_box();
    std::function<bool(size_t)> exlcude_cursor = [cursor_id](size_t entity) -> bool {return entity != cursor_id;}; 
    return check_collision_with(entity_id, box, exlcude_cursor) == game_config::empty_entity;
}
// ---------------- frame update ----------------
// TODO (25 / 8 / 26) stub - the tree is event driven, nothing to do per frame yet
void systems::spatial_system::update(float delta){
    (void) delta;
}
// ---------------- collision checks ----------------
int systems::spatial_system::check_collision_with(size_t id, Rectangle box,
    const std::function<bool(size_t)>& filter){
    return entities_.check_collision(id, box, filter);
}
int systems::spatial_system::check_collision_with(size_t id, Vector2 position,
    const std::function<bool(size_t)>& filter){
    return entities_.check_collision(id, position, filter);
}


int systems::spatial_system::check_interactions_with(size_t id, Rectangle box){
    return entities_.check_interaction(id, box);
}