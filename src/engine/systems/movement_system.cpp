#include "component.h"
#include "entity_events.h"
#include "events_interface.h"
#include "hitbox.h"
#include "system.h"

// TODO stub - the loop calls this every frame, nothing to do yet
void systems::movement_system::update(float delta){
    (void) delta;
}

void systems::movement_system::update_position(size_t id, Vector2 new_position){
    auto* position = component_managers::positional_manager_.get_component(id);
    if(position == nullptr){ return; }
    position->set_position(new_position);

    // the hitbox is the position stored a second time, so it moves in the same
    // write - every variant, so a later facing change lands somewhere correct
    if(auto* collision = component_managers::collision_manager_.get_component(id)){
        for(auto& box : collision->get_hitbox_component().get_hitboxes()){
            box.update(new_position);
        }
    }

    // executed, not queued - the spatial index must not lag the position by a frame
    events::move_entity moved{id};
    event_interface::execute_event(moved);
}