#include "entities.h"
#include "texture.h"
#include "debug_log_interface.h"
#include "raglib.h"
#include <vector>
// ------------------------------- waiter dog state bases ------------------------------- //
void entities::waiter_dog_state::on_path_finished(waiter_dog& dog, Vector2 destination){
    (void) dog;
    (void) destination;
    return;
}

void entities::waiter_dog_state::set_path(waiter_dog& dog, const std::vector<Vector2>& path){
    dog.dog::set_path(path);
}
void entities::waiter_dog_state::set_path(waiter_dog& dog, const std::vector<Vector2>& path, int station_id, Vector2 station_position){
    (void) station_id;
    (void) station_position;
    dog.dog::set_path(path);
}

bool entities::waiter_dog_traveling_state::is_available_for_order(){
    return false;
}

void entities::waiter_dog_traveling_state::on_path_finished(waiter_dog& dog, Vector2 destination){
    if(Vector2Distance(destination, destination_) > level_config::edge_weight * 0.05f){
        return;
    }
    on_arrived(dog);
}

// ------------------------------- waiter dog states ------------------------------- //
bool entities::waiter_dog::idle::is_available_for_order(){
    return true;
}
void entities::waiter_dog::idle::update(waiter_dog& dog, float delta, int frame){
    (void) dog;
    (void) delta;
    (void) frame;
    // Idle waiters do nothing until the expediter assigns them an order.
}

bool entities::waiter_dog::serving::is_available_for_order(){
    return false;
}
void entities::waiter_dog::serving::update(waiter_dog& dog, float delta, int frame){
    (void) dog;
    (void) delta;
    (void) frame;
    // The expediter drives the serving journey via dog_completed_path; the
    // serving state itself just marks the waiter busy.
}

// ------------------------------- waiter dog ------------------------------- //
entities::waiter_dog::~waiter_dog() = default;

void entities::waiter_dog::hold_food(std::unique_ptr<food> item){
    held_food_ = std::move(item);
}
bool entities::waiter_dog::is_available_for_order(){
    return state_->is_available_for_order();
}
bool entities::waiter_dog::is_carrying_food() const{
    return held_food_ != nullptr;
}
std::unique_ptr<entities::food> entities::waiter_dog::release_food(){
    return std::move(held_food_);
}
void entities::waiter_dog::set_idle(){
    set_state(std::make_unique<idle>());
}
void entities::waiter_dog::set_serving(){
    set_state(std::make_unique<serving>());
}
