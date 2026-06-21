#include "maitre_d.h"

void maitre_d::dog_queue::enqueue(size_t dog_id, events::customer_queue_side queue_side, float height_edges){
    if(full() || contains(dog_id)){
        return;
    }
    auto& dogs = dogs_for_side(queue_side);
    auto target_position = position_for_index(dogs.size(), queue_side);
    dogs.push_back(queued_dog{dog_id, height_edges, target_position});
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

    if(remove_dog(left_queue_)){
        recalculate_positions(left_queue_, events::customer_queue_side::left_queue);
    }
    if(remove_dog(right_queue_)){
        recalculate_positions(right_queue_, events::customer_queue_side::right_queue);
    }
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
    return left_queue_.size() >= cafe_config::dog_queue_capacity
        && right_queue_.size() >= cafe_config::dog_queue_capacity;
}

size_t maitre_d::dog_queue::size() const{
    return left_queue_.size() + right_queue_.size();
}

events::customer_queue_side maitre_d::dog_queue::less_full_side() const{
    if(left_queue_.size() <= right_queue_.size()){
        return events::customer_queue_side::left_queue;
    }
    return events::customer_queue_side::right_queue;
}

Vector2 maitre_d::dog_queue::get_spawn_position(events::customer_queue_side queue_side) const{
    auto spawn_y = queue_side == events::customer_queue_side::left_queue
        ? 0.0f - cafe_config::dog_spawn_out_of_bounds_distance
        : level_config::screen_height + cafe_config::dog_spawn_out_of_bounds_distance;
    auto spawn_position = Vector2{cafe_config::dog_queue_start.x, spawn_y};
    return spawn_position;
}

Vector2 maitre_d::dog_queue::get_target_position(size_t dog_id) const{
    auto find_dog = [dog_id](const std::vector<queued_dog>& dogs) -> const queued_dog* {
        auto dog = std::find_if(dogs.begin(), dogs.end(),
            [dog_id](const queued_dog& queued_dog) -> bool {
                return queued_dog.dog_id == dog_id;
            });
        if(dog != dogs.end()){
            return &(*dog);
        }
        return nullptr;
    };

    auto left_dog = find_dog(left_queue_);
    if(left_dog != nullptr){
        return left_dog->target_position;
    }
    auto right_dog = find_dog(right_queue_);
    if(right_dog != nullptr){
        return right_dog->target_position;
    }
    return Vector2{};
}

void maitre_d::dog_queue::recalculate_positions(std::vector<queued_dog>& dogs, events::customer_queue_side queue_side){
    for(size_t i = 0; i < dogs.size(); ++i){
        dogs[i].target_position = position_for_index(i, queue_side);
    }
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
    auto offset = static_cast<float>(index) * cafe_config::dog_queue_spacing_edges * level_config::edge_weight;
    return Vector2{
        cafe_config::dog_queue_start.x + (direction.x * offset),
        cafe_config::dog_queue_start.y + (direction.y * offset)
    };
}
