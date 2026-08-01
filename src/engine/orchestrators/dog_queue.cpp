#include "maitre_d.h"
#include "debug_log_interface.h"
#include "raymath.h"
#include <algorithm>

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

maitre_d::queued_dog maitre_d::dog_queue::enqueue(size_t dog_id, int side){
    if(full() || contains(dog_id)){
        debug::log(
            "[dog_queue::enqueue, blocked insert] "
            "dog_id: " + std::to_string(dog_id)
            + ", queue_full: " + std::to_string(full())
            + ", already_contains: " + std::to_string(contains(dog_id)));
        return empty_dog;
    }
    auto queue_position = get_enqueue_position(side);
    auto dog = queued_dog{
        static_cast<int>(dog_id),
        Vector2{0.0f, 0.0f},
        queue_position,
        false
    };
    lane_for_side(side).dogs.push_back(dog);
    debug::log(
        "[dog_queue::enqueue, inserted dog] "
        "dog_id: " + std::to_string(dog_id)
        + ", side: " + side_to_string(side)
        + ", queue_position: " + vector_to_string(queue_position)
        + ", left_queue_size: " + std::to_string(left_queue_.dogs.size())
        + ", right_queue_size: " + std::to_string(right_queue_.dogs.size()));
    return dog;
}
bool maitre_d::dog_queue::dog_at_head(queue_lane queue){
    return queue.dogs.empty() ? false : queue.dogs.front().reached_queue_position;
}
maitre_d::dequeue_result maitre_d::dog_queue::dequeue(){
    dequeue_result result{empty_dog, {}};
    if(dog_at_head(left_queue_)){
        result.dog = left_queue_.dogs.front();
        left_queue_.dogs.erase(left_queue_.dogs.begin());
        result.moved_dogs = compact_lane(cafe_config::queue_sides::left);
    } else if(dog_at_head(right_queue_)){
        result.dog = right_queue_.dogs.front();
        right_queue_.dogs.erase(right_queue_.dogs.begin());
        result.moved_dogs = compact_lane(cafe_config::queue_sides::right);
    }
    return result;
}

std::vector<maitre_d::queued_dog> maitre_d::dog_queue::compact_lane(int queue_side){
    auto& lane = lane_for_side(queue_side);
    auto moved_dogs = std::vector<queued_dog>{};
    for(size_t index = 0; index < lane.dogs.size(); ++index){
        auto next_position = get_position(queue_side, index);
        if(Vector2Equals(lane.dogs[index].queue_position, next_position)){
            continue;
        }
        lane.dogs[index].queue_position = next_position;
        lane.dogs[index].reached_queue_position = false;
        moved_dogs.push_back(lane.dogs[index]);
    }
    return moved_dogs;
}

void maitre_d::dog_queue::update_dog_position(size_t dog_id, Vector2 position){
    auto update_dog = [dog_id, position](std::vector<queued_dog>& dogs, const std::string& side) -> bool {
        auto dog = std::find_if(dogs.begin(), dogs.end(),
            [dog_id](const queued_dog& record) -> bool {
                return static_cast<size_t>(record.dog_id) == dog_id;
            });
        if(dog == dogs.end()){
            return false;
        }
        auto was_at_queue_position = dog->reached_queue_position;
        dog->dog_position = position;
        dog->reached_queue_position = Vector2Distance(position, dog->queue_position) <= level_config::edge_weight * 0.05f;
        if(! was_at_queue_position && dog->reached_queue_position && dog == dogs.begin()){
            debug::log(
                "[dog_queue::update_dog_position, dog reached head of queue] "
                "dog_id: " + std::to_string(dog_id)
                + ", side: " + side
                + ", position: " + vector_to_string(position)
                + ", queue_position: " + vector_to_string(dog->queue_position));
        }
        return true;
    };

    update_dog(left_queue_.dogs, "left");
    update_dog(right_queue_.dogs, "right");
}

bool maitre_d::dog_queue::dog_has_reached_queue_position(size_t dog_id) const{
    auto check_dog = [dog_id](const std::vector<queued_dog>& dogs) -> bool {
        auto dog = std::find_if(dogs.begin(), dogs.end(),
            [dog_id](const queued_dog& record) -> bool {
                return static_cast<size_t>(record.dog_id) == dog_id;
            });
        return dog != dogs.end() && dog->reached_queue_position;
    };
    return check_dog(left_queue_.dogs) || check_dog(right_queue_.dogs);
}

bool maitre_d::dog_queue::contains(size_t dog_id) const{
    auto contains_dog = [dog_id](const std::vector<queued_dog>& dogs) -> bool {
        return std::any_of(dogs.begin(), dogs.end(),
        [dog_id](const queued_dog& dog) -> bool {
            return static_cast<size_t>(dog.dog_id) == dog_id;
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
    return previous_side_;
}

Vector2 maitre_d::dog_queue::get_enqueue_position(int side) const{
    auto& lane = lane_for_side(side);
    auto queue_size = lane.dogs.size();
    auto enqueue_position = get_position(side, queue_size);
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
