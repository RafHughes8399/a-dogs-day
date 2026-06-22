#include "maitre_d.h"
#include "debug_log_interface.h"
#include "raymath.h"

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
    auto queue_position = get_enqueue_position(previous_side_);
    lane_for_side(previous_side_).dogs.push_back(queued_dog{
        static_cast<int>(dog_id),
        Vector2{0.0f, 0.0f},
        queue_position,
        false
    });
    debug::log(
        "[dog_queue::enqueue, inserted dog] "
        "dog_id: " + std::to_string(dog_id)
        + ", side: " + side_to_string(previous_side_)
        + ", queue_position: " + vector_to_string(queue_position)
        + ", left_queue_size: " + std::to_string(left_queue_.dogs.size())
        + ", right_queue_size: " + std::to_string(right_queue_.dogs.size()));
}
bool maitre_d::dog_queue::dog_at_head(queue_lane queue){
    return queue.dogs.empty() ? false : queue.dogs.front().reached_queue_position;
}
maitre_d::queued_dog maitre_d::dog_queue::dequeue(){
    queued_dog dog = {-1, Vector2Zero(), Vector2Zero(), false};
    if(dog_at_head(left_queue_)){
        dog = left_queue_.dogs.front();
        left_queue_.dogs.erase(left_queue_.dogs.begin());
    } else if(dog_at_head(right_queue_)){
            dog = right_queue_.dogs.front();
            right_queue_.dogs.erase(right_queue_.dogs.begin());
    }
    return dog;
}

void maitre_d::dog_queue::update_dog_position(size_t dog_id, Vector2 position){
    auto update_dog = [dog_id, position](std::vector<queued_dog>& dogs) -> bool {
        auto dog = std::find_if(dogs.begin(), dogs.end(),
            [dog_id](const queued_dog& record) -> bool {
                return record.dog_id == dog_id;
            });
        if(dog == dogs.end()){
            return false;
        }
        dog->dog_position = position;
        dog->reached_queue_position = Vector2Distance(position, dog->queue_position) <= level_config::edge_weight * 0.05f;
        return true;
    };

    bool updated_left = update_dog(left_queue_.dogs);
    bool updated_right = update_dog(right_queue_.dogs);
    debug::log(
        "[dog_queue::update_dog_position, updated dog position] "
        "dog_id: " + std::to_string(dog_id)
        + ", position: " + vector_to_string(position)
        + ", updated: " + std::to_string(updated_left || updated_right)
        + ", reached_queue_position: " + std::to_string(dog_has_reached_queue_position(dog_id)));
}

bool maitre_d::dog_queue::dog_has_reached_queue_position(size_t dog_id) const{
    auto check_dog = [dog_id](const std::vector<queued_dog>& dogs) -> bool {
        auto dog = std::find_if(dogs.begin(), dogs.end(),
            [dog_id](const queued_dog& record) -> bool {
                return record.dog_id == dog_id;
            });
        return dog != dogs.end() && dog->reached_queue_position;
    };
    return check_dog(left_queue_.dogs) || check_dog(right_queue_.dogs);
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
    auto enqueue_position = get_position(side, queue_size);
    debug::log(
        "[dog_queue::get_enqueue_position, calculated position] "
        "side: " + side_to_string(side)
        + ", queue_size: " + std::to_string(queue_size)
        + ", queue_head: " + vector_to_string(lane.head)
        + ", enqueue_position: " + vector_to_string(enqueue_position));
    return enqueue_position;
}

Vector2 maitre_d::dog_queue::get_position(int side, size_t index) const{
    const auto& positions = side == cafe_config::queue_sides::left
        ? cafe_config::left_queue_positions
        : cafe_config::right_queue_positions;
    if(index >= positions.size()){
        debug::log(
            "[dog_queue::get_position, index outside configured positions] "
            "side: " + side_to_string(side)
            + ", index: " + std::to_string(index)
            + ", configured_positions: " + std::to_string(positions.size()));
        return lane_for_side(side).head;
    }
    return positions[index];
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
