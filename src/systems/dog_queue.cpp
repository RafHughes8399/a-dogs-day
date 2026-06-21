#include "maitre_d.h"

void maitre_d::dog_queue::enqueue(size_t dog_id){
    if(full() || contains(dog_id)){
        return;
    }
    if(next_side_ == cafe_config::queue_sides::left) {left_queue_.push_back({dog_id});} 
    else if(next_side_ == cafe_config::queue_sides::right) {right_queue_.push_back({dog_id});} 
}

void maitre_d::dog_queue::dequeue(size_t dog_id){
    auto remove_dog = [dog_id](std::vector<queued_dog>& dogs) -> bool {
        auto new_end = std::remove_if(dogs.begin(), dogs.end(),
            [dog_id](const queued_dog& dog) -> bool {
                return dog.dog_id == dog_id;
            });
        bool removed = new_end != dogs.end();
        dogs.erase(new_end, dogs.end());
        return removed;
    };
    bool removed_left = remove_dog(left_queue_);
    bool removed_right = remove_dog(right_queue_);
    (void) removed_left;
    (void) removed_right;
}

bool maitre_d::dog_queue::contains(size_t dog_id) const{
    auto contains_dog = [dog_id](const std::vector<queued_dog>& dogs) -> bool {
        return std::any_of(dogs.begin(), dogs.end(),
        [dog_id](const queued_dog& dog) -> bool {
            return dog.dog_id == dog_id;
        });
    };
    return contains_dog(left_queue_) || contains_dog(right_queue_);
}

bool maitre_d::dog_queue::empty() const{
    return left_queue_.empty() && right_queue_.empty();
}

bool maitre_d::dog_queue::full() const{
    auto capacity = static_cast<size_t>(cafe_config::queue_capacity);
    return left_queue_.size() >= capacity
        && right_queue_.size() >= capacity;
}

size_t maitre_d::dog_queue::size() const{
    return left_queue_.size() + right_queue_.size();
}

int maitre_d::dog_queue::pick_side() {
    next_side_ = (left_queue_.size() <= right_queue_.size() ? cafe_config::queue_sides::left : cafe_config::queue_sides::right);
    return next_side_;
}

Vector2 maitre_d::dog_queue::get_enqueue_position(int side) const{
    auto queue_size = side == cafe_config::queue_sides::left
        ? left_queue_.size()
        : right_queue_.size();
    float offset = static_cast<float>(queue_size) * level_config::edge_weight * cafe_config::queue_gap_edges;
    float x = cafe_config::queue_start.x;
    float y = side == cafe_config::queue_sides::left
        ? cafe_config::queue_start.y - offset
        : cafe_config::queue_start.y + offset;
    return Vector2{x, y};
}
std::vector<maitre_d::queued_dog>& maitre_d::dog_queue::dogs_for_side(events::customer_queue_side queue_side){
    if(queue_side == events::customer_queue_side::left_queue){
        return left_queue_;
    }
    return right_queue_;
}

const std::vector<maitre_d::queued_dog>& maitre_d::dog_queue::dogs_for_side(events::customer_queue_side queue_side) const{
    if(queue_side == events::customer_queue_side::left_queue){
        return left_queue_;
    }
    return right_queue_;
}

Vector2 maitre_d::dog_queue::position_for_index(size_t index, events::customer_queue_side queue_side) const{
    auto direction = queue_side == events::customer_queue_side::left_queue
        ? Vector2{0.0f, -1.0f}
        : Vector2{0.0f, 1.0f};
    auto offset = static_cast<float>(index) * cafe_config::queue_gap_edges * level_config::edge_weight;
    return Vector2{
        cafe_config::queue_start.x + (direction.x * offset),
        cafe_config::queue_start.y + (direction.y * offset)
    };
}
