#include "factories.hpp"
void factories::station_factory::build_station(size_t id, Vector2 position){
    (void) id;
    (void) position;
    return;
}
void factories::station_factory::build_table(size_t table, size_t entity_id, Vector2 position){
    auto table_builder = table_builders_[table];
    table_builder(entity_id, position);
}
void factories::station_factory::build_stove(size_t id, Vector2 position){
    (void) id;
    (void) position;
    return;
}