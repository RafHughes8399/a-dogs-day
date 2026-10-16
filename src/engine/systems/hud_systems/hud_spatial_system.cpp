#include "hud_systems/hud_systems.hpp"

void hud_systems::hud_spatial_system::on_created_hud_element(const events::create_hud_element& event){
    track(event.get_id());
}
void hud_systems::hud_spatial_system::on_removed_hud_element(const events::remove_hud_element& event){
    untrack(event.get_id());
}

void hud_systems::hud_spatial_system::on_moved_entity(const events::move_entity& event){
    auto* hud = component_managers::hud_manager_.get_component(event.get_id());
    if(hud == nullptr){ return; }
    auto pre_move = event.get_pre_move();
    auto post_move = event.get_post_move();
    move_hud(*hud, Vector2{post_move.x - pre_move.x, post_move.y - pre_move.y});
}
void hud_systems::hud_spatial_system::move_hud(components::hud_component& hud, Vector2 delta){
    for(auto element : hud.get_elements()){
        auto* position = component_managers::hud_positional_manager_.get_component(element);
        auto* collision = component_managers::hud_collision_manager_.get_component(element);
        if(position == nullptr or collision == nullptr){ continue; }
        auto moved = Vector2Add(position->get_position(), delta);
        position->set_position(moved);
        for(auto& box : collision->get_hitbox_component().get_hitboxes()){
            box.update(moved);
        }
        elements_.move(element, collision->get_hitbox_component().get_hitbox());
    }
}
void hud_systems::hud_spatial_system::track(size_t hud_id){
    auto* collision = component_managers::hud_collision_manager_.get_component(hud_id);
    if(collision == nullptr){ return; }
    elements_.insert(hud_id, collision->get_hitbox_component().get_hitbox());
}
void hud_systems::hud_spatial_system::untrack(size_t hud_id){
    elements_.erase(hud_id);
}
bool hud_systems::hud_spatial_system::is_tracked(size_t hud_id){
    return elements_.contains(hud_id);
}
size_t hud_systems::hud_spatial_system::tracked_count(){
    return elements_.size();
}
int hud_systems::hud_spatial_system::check_collision_with(Vector2 position){
    return elements_.check_collision(static_cast<size_t>(game_config::empty_entity), position,
        [](size_t hud_id) -> bool {
            auto* owner = component_managers::hud_owner_manager_.get_component(hud_id);
            if(owner == nullptr){ return false; }
            auto* hud = component_managers::hud_manager_.get_component(owner->get_owner());
            return hud != nullptr and hud->is_active();
        });
}
void hud_systems::hud_spatial_system::clear(){
    elements_.clear();
}
