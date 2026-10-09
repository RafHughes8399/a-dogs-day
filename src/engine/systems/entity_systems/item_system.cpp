#include "system.h"

// here we return the item that is then to be used in create_food_from_item to create the food entity
std::optional<size_t> systems::item_system::take_item(size_t counter_id){
    return component_helpers::take_stored_item(counter_id,
        entity_config::counter_sprite_slots::counter_food);
}

void systems::item_system::place_item(size_t counter_id, size_t item){
    component_helpers::add_stored_item(counter_id,
        entity_config::counter_sprite_slots::counter_food, item);
}
