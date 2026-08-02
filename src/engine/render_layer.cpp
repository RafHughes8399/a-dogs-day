#include "render_layer.h"

void render_layer::layer::add_entity(entities::entity* entity){
    entities_.push_back(entity);
}
void render_layer::layer::remove_entity(entities::entity* entity){
    // remove if address match
    auto new_end = std::remove_if(entities_.begin(), entities_.end(), [entity](auto& current_entity) -> bool {
        return entity == current_entity;
    });
    entities_.erase(new_end, entities_.end());
}

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

void render_layer::layer::remove_entities(std::vector<int> entity_ids){
    for(auto it = entities_.begin(); it != entities_.end();){
        int current_id = (*it)->get_id();
        bool removed = false;
        for(auto & remove_id : entity_ids){
            if(remove_id == current_id){
                removed = true;
                it = entities_.erase(it);
            
            }
        }
        if(! removed){
            ++it;
            }
    }
}
