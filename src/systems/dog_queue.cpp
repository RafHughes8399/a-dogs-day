#include "maitre_d.h"
#include "debug_log_interface.h"

namespace{
    std::string vector_to_string(Vector2 position){
        return "{" + std::to_string(position.x) + ", " + std::to_string(position.y) + "}";
    }

    std::string side_to_string(int side){
        if(side == cafe_config::queue_sides::left){
            return "left";
        }
        if(side == cafe_config::queue_sides::right){
            return "right";
        }
        return "unknown";
    }
}

void maitre_d::dog_queue::enqueue(size_t dog_id){
    if(full() || contains(dog_id)){
        debug::log(
            "[dog_queue::enqueue, blocked insert] "
            "dog_id: " + std::to_string(dog_id)
            + ", queue_full: " + std::to_string(full())
            + ", already_contains: " + std::to_string(contains(dog_id)));
        return;
    }
    lane_for_side(previous_side_).dogs.push_back({dog_id});
    debug::log(
        "[dog_queue::enqueue, inserted dog] "
        "dog_id: " + std::to_string(dog_id)
        + ", side: " + side_to_string(previous_side_)
        + ", left_queue_size: " + std::to_string(left_queue_.dogs.size())
        + ", right_queue_size: " + std::to_string(right_queue_.dogs.size()));
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
    bool removed_left = remove_dog(left_queue_.dogs);
    bool removed_right = remove_dog(right_queue_.dogs);
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
    return contains_dog(left_queue_.dogs) || contains_dog(right_queue_.dogs);
}

bool maitre_d::dog_queue::empty() const{
    return left_queue_.dogs.empty() && right_queue_.dogs.empty();
}

bool maitre_d::dog_queue::full() const{
    auto capacity = static_cast<size_t>(cafe_config::queue_capacity);
    return left_queue_.dogs.size() >= capacity
        && right_queue_.dogs.size() >= capacity;
}

size_t maitre_d::dog_queue::size() const{
    return left_queue_.dogs.size() + right_queue_.dogs.size();
}

int maitre_d::dog_queue::pick_side() {
    previous_side_ = previous_side_ == cafe_config::queue_sides::left
        ? cafe_config::queue_sides::right
        : cafe_config::queue_sides::left;
    debug::log(
        "[dog_queue::pick_side, selected side] "
        "left_queue_size: " + std::to_string(left_queue_.dogs.size())
        + ", right_queue_size: " + std::to_string(right_queue_.dogs.size())
        + ", previous_side: " + side_to_string(previous_side_));
    return previous_side_;
}

Vector2 maitre_d::dog_queue::get_enqueue_position(int side) const{
    auto& lane = lane_for_side(side);
    auto queue_size = lane.dogs.size();
    float offset = static_cast<float>(queue_size) * level_config::edge_weight * cafe_config::queue_gap_edges;
    float x = lane.head.x;
    float y = side == cafe_config::queue_sides::left
        ? lane.head.y - offset
        : lane.head.y + offset;
    auto enqueue_position = Vector2{x, y};
    debug::log(
        "[dog_queue::get_enqueue_position, calculated position] "
        "side: " + side_to_string(side)
        + ", queue_size: " + std::to_string(queue_size)
        + ", offset: " + std::to_string(offset)
        + ", queue_head: " + vector_to_string(lane.head)
        + ", enqueue_position: " + vector_to_string(enqueue_position));
    return enqueue_position;
}

maitre_d::queue_lane& maitre_d::dog_queue::lane_for_side(int queue_side){
    if(queue_side == cafe_config::queue_sides::left){
        return left_queue_;
    }
    return right_queue_;
}

const maitre_d::queue_lane& maitre_d::dog_queue::lane_for_side(int queue_side) const{
    if(queue_side == cafe_config::queue_sides::left){
        return left_queue_;
    }
    return right_queue_;
}

std::vector<maitre_d::queued_dog>& maitre_d::dog_queue::dogs_for_side(events::customer_queue_side queue_side){
    if(queue_side == events::customer_queue_side::left_queue){
        return left_queue_.dogs;
    }
    return right_queue_.dogs;
}

const std::vector<maitre_d::queued_dog>& maitre_d::dog_queue::dogs_for_side(events::customer_queue_side queue_side) const{
    if(queue_side == events::customer_queue_side::left_queue){
        return left_queue_.dogs;
    }
    return right_queue_.dogs;
}

Vector2 maitre_d::dog_queue::position_for_index(size_t index, events::customer_queue_side queue_side) const{
    auto direction = queue_side == events::customer_queue_side::left_queue
        ? Vector2{0.0f, -1.0f}
        : Vector2{0.0f, 1.0f};
    auto offset = static_cast<float>(index) * cafe_config::queue_gap_edges * level_config::edge_weight;
    auto head = queue_side == events::customer_queue_side::left_queue
        ? left_queue_.head
        : right_queue_.head;
    return Vector2{
        head.x + (direction.x * offset),
        head.y + (direction.y * offset)
    };
}
