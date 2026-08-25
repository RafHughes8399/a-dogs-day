#include "system.h"

// ---------------- helpers ----------------
hitbox::hitbox* systems::spatial_system::bounds_for(size_t entity_id){
    auto* collision = component_managers::collision_manager_.get_component(entity_id);
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

// ---------------- frame update ----------------
// TODO stub - the tree is event driven, nothing to do per frame yet
void systems::spatial_system::update(float delta){
    (void) delta;
}
// ---------------- collision checks ----------------
int systems::spatial_system::check_collision_with(size_t id, Rectangle box){
    return entities_.check_collision(id, box);
}
int systems::spatial_system::check_collision_with(size_t id, Vector2 position){
    return entities_.check_collision(id, position);
}


int systems::spatial_system::check_interactions_with(size_t id, Rectangle box){
    return entities_.check_interaction(id, box);
}