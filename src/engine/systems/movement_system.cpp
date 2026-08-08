#include "component.h"
#include "entity_events.h"
#include "events_interface.h"
#include "hitbox.h"
#include "system.h"
#include <raylib.h>

// ---------------- frame update ----------------
// TODO stub - the loop calls this every frame, nothing to do yet
void systems::movement_system::update(float delta){
    (void) delta;
}

// ---------------- event handlers ----------------
void systems::movement_system::on_created_entity(const events::create_entity& event){
    size_t entity_id = event.get_id();
    if(component_helpers::is_mouse_positioned(entity_id)){ return; }
    if(auto* collision = component_managers::collision_manager_.get_component(entity_id); collision != nullptr){
        graph_.update_entity(collision->get_hitbox_component().get_hitbox().get_box(),
            static_cast<int>(entity_id));
    }
}
void systems::movement_system::on_moved_entity(const events::move_entity& event){
    if(component_helpers::is_mouse_positioned(event.get_id())){ return; }
    graph_.update_entity(event.get_pre_move(), graph_config::empty_node);
    graph_.update_entity(event.get_post_move(), static_cast<int>(event.get_id()));
}
void systems::movement_system::on_destroyed_entity(const events::remove_entity& event){
    if(component_helpers::is_mouse_positioned(event.get_id())){ return; }
    if(auto* collision = component_managers::collision_manager_.get_component(event.get_id()); collision != nullptr){
        graph_.update_entity(collision->get_hitbox_component().get_hitbox().get_box(),
            graph_config::empty_node);
    }
}

// ---------------- position writes ----------------
void systems::movement_system::update_position(size_t id, Vector2 new_position){
    auto* position = component_managers::positional_manager_.get_component(id);
    if(position == nullptr){ return; }
    position->set_position(new_position);

    // the hitbox is the position stored a second time, so it moves in the same
    // write - every variant, so a later facing change lands somewhere correct
    Rectangle pre_move{};
    Rectangle post_move{};
    if(auto* collision = component_managers::collision_manager_.get_component(id)){
        pre_move = collision->get_hitbox_component().get_hitbox().get_box();
        for(auto& box : collision->get_hitbox_component().get_hitboxes()){
            box.update(new_position);
        }
        post_move = collision->get_hitbox_component().get_hitbox().get_box();
    }

    // executed, not queued - the spatial index must not lag the position by a frame
    events::move_entity moved{id, pre_move, post_move};
    event_interface::execute_event(moved);
}