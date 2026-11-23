#include "factories.hpp"
#include "component_helpers.hpp"
#include "items.h"
void factories::station_factory::build_station(size_t station, size_t entity_id, Vector2 position){
    if(station >= station_builders_.size()){ return; }
    station_builders_[station](entity_id, position);
    component_helpers::add_entity_item_component(entity_id,
        items::item_ref{entity_config::station_category, station});
}
