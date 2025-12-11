#include "render_layer.h"
void render_layer::layer::draw(){
    for(auto & entity : entities_){
        entity->render();
    }
}

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
