#include "dog_behavioural_systems.h"

        
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
    (void) waiter;
    return true;
}
Rectangle dbs::waiter_idling_system::determine_idle_bounds(size_t waiter){
    (void) waiter;
    return Rectangle{0,0,0,0};
}
size_t dbs::waiter_idling_system::pick_direction(){
    return 0;
}
void dbs::waiter_idling_system::build_paths(size_t waiter, size_t direction,  Rectangle bounds){
    (void) waiter;
    (void) direction;
    (void) bounds;
    return;
}
void dbs::waiter_idling_system::update(float delta){
    (void) delta;
    for(size_t & waiter : waiters_){
        if(is_idle(waiter)){
            Rectangle bounds = determine_idle_bounds(waiter);
            size_t direction = pick_direction();
            build_paths(waiter, direction, bounds);
        }
    }
}