#include "maitre_d.h"

namespace{
    Vector2 dog_queue_position(size_t index){
        auto offset = static_cast<float>(index) * cafe_config::dog_queue_spacing_edges * level_config::edge_weight;
        return Vector2{
            cafe_config::dog_queue_start.x + (cafe_config::dog_queue_direction.x * offset),
            cafe_config::dog_queue_start.y + (cafe_config::dog_queue_direction.y * offset)
        };
    }
}

void maitre_d::dog_queue::enqueue(size_t dog_id, float height_edges){
    if(full() || contains(dog_id)){
        return;
    }
    dogs_.push_back(queued_dog{dog_id, height_edges, dog_queue_position(dogs_.size())});
}

void maitre_d::dog_queue::dequeue(size_t dog_id){
    auto new_end = std::remove_if(dogs_.begin(), dogs_.end(),
        [dog_id](const queued_dog& dog) -> bool {
            return dog.dog_id == dog_id;
        });
    dogs_.erase(new_end, dogs_.end());
    recalculate_positions();
}

bool maitre_d::dog_queue::contains(size_t dog_id) const{
    return std::any_of(dogs_.begin(), dogs_.end(),
        [dog_id](const queued_dog& dog) -> bool {
            return dog.dog_id == dog_id;
        });
}

bool maitre_d::dog_queue::empty() const{
    return dogs_.empty();
}

bool maitre_d::dog_queue::full() const{
    return dogs_.size() >= cafe_config::dog_queue_capacity;
}

size_t maitre_d::dog_queue::size() const{
    return dogs_.size();
}

Vector2 maitre_d::dog_queue::get_next_open_position() const{
    return dog_queue_position(dogs_.size());
}

Vector2 maitre_d::dog_queue::get_target_position(size_t dog_id) const{
    auto dog = std::find_if(dogs_.begin(), dogs_.end(),
        [dog_id](const queued_dog& queued_dog) -> bool {
            return queued_dog.dog_id == dog_id;
        });
    if(dog != dogs_.end()){
        return dog->target_position;
    }
    return Vector2{};
}

void maitre_d::dog_queue::recalculate_positions(){
    for(size_t i = 0; i < dogs_.size(); ++i){
        dogs_[i].target_position = dog_queue_position(i);
    }
}
