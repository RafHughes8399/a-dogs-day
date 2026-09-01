#include "component.h"
#include "config.h"
#include "debug_log_interface.h"
#include "dog_behavioural_systems.h"
#include "entity_events.h"
#include "events_interface.h"
#include "raglib.h"
#include "system.h"
#include <algorithm>
#include <raylib.h>
#include <random>
#include <string>

float dbs::waiter_idling_system::roll_cooldown(){
    std::uniform_real_distribution<float> cooldown(dog_config::waiter_idle_cooldown_min,
        dog_config::waiter_idle_cooldown_max);
    return cooldown(rng_);
}

void dbs::waiter_idling_system::register_waiter(size_t waiter_id){
    waiters_.emplace_back(waiter_id, roll_cooldown());
}
void dbs::waiter_idling_system::unregister_waiter(size_t waiter_id){
    std::erase_if(waiters_, [waiter_id](const idle_waiter& waiter) -> bool {
        return waiter.id() == waiter_id;
    });
}
void dbs::waiter_idling_system::clear(){
    waiters_.clear();
}

bool dbs::waiter_idling_system::is_idle(size_t waiter){
    auto interactor = component_managers::interactor_manager_.get_component(waiter);
    if(interactor and interactor->is_interacting()){ return false; }
    auto movement_component = component_managers::movement_manager_.get_component(waiter);
    return movement_component != nullptr and movement_component->get_paths().empty();
}
std::optional<Rectangle> dbs::waiter_idling_system::determine_idle_bounds(size_t waiter){
    auto position_component = component_managers::positional_manager_.get_component(waiter);
    if(position_component == nullptr){ return std::nullopt; }

    auto position = position_component->get_position();
    auto reach = level_config::edge_weight * dog_config::waiter_idle_bounds_edges;

    auto min_x = std::max(position.x - reach, level_config::cafe_x);
    auto min_y = std::max(position.y - reach, level_config::cafe_y);
    auto max_x = std::min(position.x + reach, level_config::cafe_x + level_config::cafe_width);
    auto max_y = std::min(position.y + reach, level_config::cafe_y + level_config::cafe_height);

    if(max_x - min_x < level_config::edge_weight){ return std::nullopt; }
    if(max_y - min_y < level_config::edge_weight){ return std::nullopt; }

    return Rectangle{min_x, min_y, max_x - min_x, max_y - min_y};
}
std::vector<Vector2> dbs::waiter_idling_system::walkable_positions(Rectangle bounds){
    auto& movement = systems::movement_system::get_instance();
    std::vector<Vector2> positions;
    for(auto x = bounds.x; x < bounds.x + bounds.width; x += level_config::edge_weight){
        for(auto y = bounds.y; y < bounds.y + bounds.height; y += level_config::edge_weight){
            auto* node = movement.node_at(Vector2{x, y});
            if(node == nullptr){ continue; }
            if(not movement.is_walkable(node->position_)){ continue; }
            positions.push_back(node->position_);
        }
    }
    return positions;
}
std::vector<Vector2> dbs::waiter_idling_system::pick_points(const std::vector<Vector2>& candidates,
    size_t points){
    if(candidates.size() < points){ return {}; }

    std::vector<size_t> indices(candidates.size());
    for(size_t index = 0; index < indices.size(); ++index){ indices[index] = index; }
    std::shuffle(indices.begin(), indices.end(), rng_);

    std::vector<Vector2> picked;
    for(size_t index = 0; index < points; ++index){
        picked.push_back(candidates[indices[index]]);
    }
    return picked;
}
void dbs::waiter_idling_system::order_points(Vector2 from, std::vector<Vector2>& points){
    auto current = from;
    for(auto it = points.begin(); it != points.end(); ++it){
        auto nearest = std::min_element(it, points.end(), [current](Vector2 a, Vector2 b) -> bool {
            return Vector2Distance(current, a) < Vector2Distance(current, b);
        });
        std::iter_swap(it, nearest);
        current = *it;
    }
}
bool dbs::waiter_idling_system::build_paths(size_t waiter, size_t points, Rectangle bounds){
    auto movement = component_managers::movement_manager_.get_component(waiter);
    auto position = component_managers::positional_manager_.get_component(waiter);
    if(movement == nullptr or position == nullptr){ return false; }

    auto candidates = walkable_positions(bounds);

    for(size_t attempt = 0; attempt < dog_config::waiter_idle_max_attempts; ++attempt){
        auto picked = pick_points(candidates, points);
        if(picked.empty()){ break; }
        order_points(position->get_position(), picked);

        events::create_path_to route{waiter, picked.back(), path::replace,
            std::vector<Vector2>(picked.begin(), picked.end() - 1)};
        event_interface::execute_event(route);

        if(not movement->get_paths().empty()){
            debug::log("[waiter_idling_system::build_paths] waiter: " + std::to_string(waiter)
                + ", points: " + std::to_string(picked.size())
                + ", destination: " + raglib::vector_to_string(picked.back())
                + ", attempt: " + std::to_string(attempt + 1));
            return true;
        }
    }
    debug::log("[waiter_idling_system::build_paths] waiter: " + std::to_string(waiter)
        + " found no idle route in " + std::to_string(dog_config::waiter_idle_max_attempts)
        + " attempts, candidates: " + std::to_string(candidates.size()));
    return false;
}
void dbs::waiter_idling_system::update(float delta){
    std::uniform_int_distribution<size_t> point_count(dog_config::waiter_idle_min_points,
        dog_config::waiter_idle_max_points);

    for(auto& waiter : waiters_){
        waiter.tick(delta);
        // TODO pending state machine implmentatino, but here is where the states should be checked 
        // to go from not idle -> idle
        if(not waiter.ready()){ continue; 
        }
        if(not is_idle(waiter.id())){ continue;
         }

        auto bounds = determine_idle_bounds(waiter.id());
        if(bounds.has_value()){
            build_paths(waiter.id(), point_count(rng_), bounds.value());
        }
        // TODO: update state from not idle to idle
        waiter.start_cooldown(roll_cooldown());
    }
}
