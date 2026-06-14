#include "maitre_d.h"

void maitre_d::dog_queue::enqueue(size_t dog_id, float height_edges){
    // Future behavior:
    // - append dog_id and height_edges to dogs_
    // - recalculate resolved queue target positions
    // - leave event emission to the maitre d / level boundary
    (void) dog_id;
    (void) height_edges;
}

void maitre_d::dog_queue::dequeue(size_t dog_id){
    // Future behavior:
    // - remove dog_id from dogs_
    // - recalculate resolved queue target positions for remaining dogs
    (void) dog_id;
}

bool maitre_d::dog_queue::contains(size_t dog_id) const{
    // Future behavior:
    // - return whether dog_id exists in dogs_
    (void) dog_id;
    return false;
}

bool maitre_d::dog_queue::empty() const{
    // Future behavior:
    // - return dogs_.empty()
    return true;
}

size_t maitre_d::dog_queue::size() const{
    // Future behavior:
    // - return dogs_.size()
    return 0;
}

Vector2 maitre_d::dog_queue::get_target_position(size_t dog_id) const{
    // Future behavior:
    // - find dog_id in dogs_
    // - return its resolved target_position
    (void) dog_id;
    return Vector2{};
}

void maitre_d::dog_queue::recalculate_positions(){
    // Future behavior:
    // - start at cafe_config::dog_queue_start
    // - advance along cafe_config::dog_queue_direction
    // - ceil each dog's height_edges to decide the next open edge slot
}
