#include "hud_systems/hud_systems.hpp"
#include "debug_log_interface.h"
#include "input_events.h"
#include "system.h"
#include "menus.h"
#include <string>

void hud_systems::hud_spatial_system::on_camera_moved(const events::camera_moved& event){
    (void) event;
    auto& camera = systems::rendering_system::get_instance().get_camera();
    auto target = GetScreenToWorld2D(hud_config::inventory_page_anchor, camera);
    auto pages = systems::menu_navigation_system::get_instance().pages_of(menus::menu_ids::inventory);
    for(auto root : pages){
        auto* position = component_managers::entity_positional_manager_.get_component(root);
        auto* hud = component_managers::hud_manager_.get_component(root);
        if(position == nullptr or hud == nullptr){ continue; }
        auto delta = Vector2Subtract(target, position->get_position());
        position->set_position(target);
        move_hud(*hud, delta);
    }
}
void hud_systems::hud_spatial_system::on_cursor_moved(const events::cursor_moved& event){
    auto collision = component_managers::entity_collision_manager_.get_component(event.get_id());
    if(not collision){ return; }
    auto& cursor_hitbox = collision->get_hitbox_component().get_hitbox();
    
    // look for collisions and then adjust the hud renderable componetn to do this
    auto colliding_hud_element_id = check_collisions_with(event.get_id(), cursor_hitbox.get_box());

    // send an event to the hud rendering system to scale the hover componet of a button
    // todo implement the hover componetn and the event

}
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

bool hud_systems::hud_spatial_system::is_hud_element_active(size_t element_id){
    auto hud_owner = component_managers::hud_owner_manager_.get_component(element_id);
    if(not hud_owner) {return false;}
    auto hud = component_managers::hud_manager_.get_component(hud_owner->get_owner());
    if(not hud){return false;}
    return hud->is_active();
}
int hud_systems::hud_spatial_system::check_collision_with(Vector2 position){
    debug::log("[hud_spatial_system::check_collision_with] position: " + std::to_string(position.x)
        + ", " + std::to_string(position.y) + ", tracked elements: " + std::to_string(tracked_count()));
    return elements_.check_collision(static_cast<size_t>(game_config::empty_entity), position,
        [this](size_t hud_id) -> bool {
            return is_hud_element_active(hud_id);
        }); 
}
int hud_systems::hud_spatial_system::check_collisions_with(size_t cursor_id, Rectangle box){
    // are there colliisions
    auto collisions = elements_.check_collisions(cursor_id, box, [this](size_t hud_id) -> bool {
        return is_hud_element_active(hud_id);
    
    });
    // sort collisions
    if(collisions.empty()) {return game_config::empty_entity;}
    if(collisions.size() == 1) { return collisions[0];}
    else{
        std::sort(collisions.begin(), collisions.end(), [box](const int& a, const int& b) -> bool {
            // looking for a  a > b comparison on size
            auto a_collision = component_managers::hud_collision_manager_.get_component(a);
            auto b_collision = component_managers::hud_collision_manager_.get_component(b);
            // to maintain the order a > b - a sort in ascending order
            if(not a_collision){return false;}
            if(not b_collision) {return true;}
            auto a_overlap = GetCollisionRec(box, a_collision->get_hitbox_component().get_hitbox().get_box());
            auto b_overlap = GetCollisionRec(box, b_collision->get_hitbox_component().get_hitbox().get_box());
            
            return (a_overlap.width * a_overlap.height) > (b_overlap.width * b_overlap.height);
        });
    return collisions[0];
    }
    // return head;
}
void hud_systems::hud_spatial_system::clear(){
    elements_.clear();
}
