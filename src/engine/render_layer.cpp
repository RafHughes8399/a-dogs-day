#include "render_layer.h"

void render_layer::ecs_layer::add_entity(size_t entity_id){
    entities_.push_back(entity_id);
}
void render_layer::ecs_layer::remove_entity(size_t entity_id){
    auto new_end = std::remove(entities_.begin(), entities_.end(), entity_id);
    entities_.erase(new_end, entities_.end());
}
void render_layer::ecs_layer::remove_entities(const std::vector<size_t>& entity_ids){
    auto new_end = std::remove_if(entities_.begin(), entities_.end(),
        [&entity_ids](size_t current_id) -> bool {
            return std::find(entity_ids.begin(), entity_ids.end(), current_id) != entity_ids.end();
        });
    entities_.erase(new_end, entities_.end());
}
