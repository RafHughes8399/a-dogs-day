#include "component.h"
#include "system.h"
#include <raymath.h>

void systems::carrier_system::update(float delta){
    (void) delta;
    for(auto& [id, carrier] : component_managers::carrier_manager_){
        auto* current_position = carrier.get_current_position();
        if(current_position == nullptr){ continue; }

        auto offset = Vector2Subtract(*current_position, carrier.get_previous_position());
        carrier.set_previous_position(*current_position);

        auto carried_entity = carrier.get_carried_entity();
        if(not carried_entity.has_value()){ continue; }
        if(Vector2Equals(offset, Vector2Zero())){ continue; }

        auto* carried_position = component_managers::positional_manager_.get_component(carried_entity.value());
        if(carried_position == nullptr){ continue; }

        movement_system::get_instance().update_position(carried_entity.value(),
            Vector2Add(carried_position->get_position(), offset));
    }
}
