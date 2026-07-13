#include "config.h"
#include "entities.h"
#include "hitbox.h"
#include "texture.h"
#include <algorithm>
// ------------------------ stations -----------------------------------//
entities::station::interaction_positions entities::station::get_interaction_positions() const{
    return interaction_positions_;
}

void entities::station::interact(entity& other){
    (void) other;
    return;
}

size_t entities::station::capacity() const{
    return capacity_;
}

bool entities::station::enter(int dog_id){
    if(std::find(interacting_dog_ids_.begin(), interacting_dog_ids_.end(), dog_id) != interacting_dog_ids_.end()){
        return false;
    }
    if(interacting_dog_ids_.size() >= capacity_){
        return false;
    }
    interacting_dog_ids_.push_back(dog_id);
    return true;
}

bool entities::station::is_interacting() const{
    return ! interacting_dog_ids_.empty();
}

void entities::station::leave(int dog_id){
    interacting_dog_ids_.erase(
        std::remove(interacting_dog_ids_.begin(), interacting_dog_ids_.end(), dog_id),
        interacting_dog_ids_.end());
}

void entities::station::update_interaction_positions(){
    // Defensive: keep interaction nodes on the map. A station near an edge could
    // otherwise produce an off-map interaction position that snaps to a bogus node.
    const float max_x = level_config::world_x - level_config::edge_weight;
    const float max_y = level_config::world_y - level_config::edge_weight;
    auto clamp_position = [max_x, max_y](Vector2 p) -> Vector2 {
        return Vector2{
            std::max(0.0f, std::min(p.x, max_x)),
            std::max(0.0f, std::min(p.y, max_y))
        };
    };
    interaction_positions_ = interaction_positions{
        clamp_position(Vector2{position_.x - level_config::edge_weight, position_.y}),
        clamp_position(Vector2{position_.x + (2.0f * level_config::edge_weight), position_.y})
    };
}
