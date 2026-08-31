#include "component.h"
#include "config.h"
#include "dog_behavioural_systems.h"
#include "entity_events.h"
#include "events_interface.h"
#include <raylib.h>
#include <random>
        
void dbs::waiter_idling_system::register_waiter(size_t waiter_id){
    waiters_.push_back(waiter_id);
}
void dbs::waiter_idling_system::unregister_waiter(size_t waiter_id){
        waiters_.erase(std::remove(waiters_.begin(), waiters_.end(), waiter_id), waiters_.end());
}
void dbs::waiter_idling_system::clear(){
    waiters_.clear();
}

bool dbs::waiter_idling_system::is_idle(size_t waiter){
    auto movement_component = component_managers::movement_manager_.get_component(waiter); 
    // ! temp implmentation, pending state machine, for now just checks if stationary 
    if(movement_component and not movement_component->get_paths().empty()){
        return true;
    }
    return false;
}
std::optional<Rectangle> dbs::waiter_idling_system::determine_idle_bounds(size_t waiter){
    auto position_component = component_managers::positional_manager_.get_component(waiter);
    if(position_component){
        auto position = position_component->get_position();
        return Rectangle{
            std::max(position.x - (level_config::edge_weight * 3), level_config::cafe_x),
            std::max(position.y - (level_config::edge_weight * 3), level_config::cafe_y), 
            std::min(position.x + (level_config::edge_weight * 5), level_config::cafe_x + level_config::cafe_width),
            std::min(position.y + (level_config::edge_weight * 5), level_config::cafe_y + level_config::cafe_height)};

    }
    return std::nullopt;
}
size_t dbs::waiter_idling_system::pick_direction(){
    return level_config::directions::right;
}
void dbs::waiter_idling_system::build_paths(size_t waiter, size_t points,  Rectangle bounds){
    (void) waiter;
    (void) points;
    (void) bounds;
    // ? create a spanning tree for the n points
    std::unique_ptr<events::event> create_idle_paths;
    event_interface::queue_event(create_idle_paths);
    return;
}
void dbs::waiter_idling_system::update(float delta){
    (void) delta;
    for(size_t & waiter : waiters_){
        if(is_idle(waiter)){
            std::optional<Rectangle> bounds_opt = determine_idle_bounds(waiter);
            if(bounds_opt.has_value()){
                Rectangle bounds = bounds_opt.value();
                std::random_device rd;  // a seed source for the random number engine
                std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
                std::uniform_int_distribution<> distrib(2, 4);
                size_t n_points = distrib(gen);
                build_paths(waiter, n_points, bounds);
            }
        }
    }
}