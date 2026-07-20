#include "config.h"
#include "entities.h"
#include "hitbox.h"
#include "stations.h"
#include "texture.h"
#include <algorithm>
// ------------------------ stations -----------------------------------//
bool entities::station::can_accept_dog(){
    return capacity_ > interacting_dog_ids_.size();
}
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
    return state_->enter(*this, dog_id);
}

bool entities::station::is_interacting() const{
    return state_->is_interacting();
}

void entities::station::leave(int dog_id){
    state_->leave(*this, dog_id);
}

std::unique_ptr<entities::station::station_state> entities::station::default_state(){
    return std::make_unique<unworked>();
}

bool entities::station::unworked::enter(station& station, int dog_id){
    // Unworked implies the dog set is already empty, so no duplicate/capacity
    // check is needed here - that's worked's job once occupied.
    station.interacting_dog_ids_.push_back(dog_id);
    station.set_state(std::make_unique<worked>());
    return true;
}

void entities::station::unworked::leave(station& station, int dog_id){
    (void) station;
    (void) dog_id;
    // No-op: nothing to remove while unworked.
}

bool entities::station::worked::enter(station& station, int dog_id){
    if(std::find(station.interacting_dog_ids_.begin(), station.interacting_dog_ids_.end(), dog_id)
       != station.interacting_dog_ids_.end()){
        return false;
    }
    if(station.interacting_dog_ids_.size() >= station.capacity_){
        return false;
    }
    station.interacting_dog_ids_.push_back(dog_id);
    return true;
}

void entities::station::worked::leave(station& station, int dog_id){
    auto& ids = station.interacting_dog_ids_;
    ids.erase(std::remove(ids.begin(), ids.end(), dog_id), ids.end());
    if(ids.empty()){
        station.set_state(std::make_unique<unworked>());
    }
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
