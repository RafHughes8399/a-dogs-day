#include "component.h"
#include "system.h"
#include <raymath.h>

Vector2 systems::carrier_system::mouth_offset(size_t carrier){
    auto hitbox = component_managers::collision_manager_.get_component(carrier);
    if(not hitbox) {return Vector2Zero();}
    auto& hitboxes = hitbox->get_hitbox_component();
    auto dog_box = hitboxes.get_hitbox().get_box();
    auto side = hitboxes.get_hitbox_index() == level_config::directions::left ? -1.0f : 1.0f;
    auto half_width = dog_box.width * 0.5f;
    return Vector2{
        half_width + (half_width * side) - (entity_config::food_width * 0.5f) + (side * ((entity_config::food_width * 0.5f) - entity_config::food_carry_offset.x)),
        (dog_box.height * 0.2f) - (entity_config::food_height * 0.5f) + entity_config::food_carry_offset.y};
}

void systems::carrier_system::update(float delta){
    (void) delta;
    for(auto& [id, carrier] : component_managers::carrier_manager_){
        auto* current_position = carrier.get_current_position();
        if(current_position == nullptr){ continue; }

        auto carried_entity = carrier.get_carried_entity();
        if(not carried_entity.has_value()){ continue; }

        auto* carried_position = component_managers::positional_manager_.get_component(carried_entity.value());
        if(carried_position == nullptr){ continue; }

        auto target = Vector2Add(*current_position, mouth_offset(id));
        if(Vector2Equals(target, carried_position->get_position())){ continue; }
        movement_system::get_instance().update_position(carried_entity.value(), target);
    }
}
