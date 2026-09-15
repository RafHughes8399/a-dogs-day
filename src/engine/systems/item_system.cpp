#include "system.h"


// here we return the item that is then to be used in create_food_from_item to create the food entity
std::optional<item_stack::item> systems::item_system::take_item(size_t counter_id){
    auto storage_component = component_managers::storage_manager_.get_component(counter_id);
    if(storage_component and not storage_component->empty()){
        auto item = storage_component->take();
        return item;
    }
    return std::nullopt;
}

void systems::item_system::place_item(size_t counter_id, size_t item){
    auto storage_component = component_managers::storage_manager_.get_component(counter_id);
    if(storage_component){
        storage_component->place(item);
    }
}