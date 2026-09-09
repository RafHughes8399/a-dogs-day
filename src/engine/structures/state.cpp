#include "state.h"

#include "config.h"
#include "events.h"
#include "events_interface.h"

size_t state::state::get_state_id() const{
    return state_id_;
}
size_t state::state::get_animation() const{
    return animation_;
}
void state::state::on_transitioned_to(size_t entity, std::optional<size_t> payload){
    (void) entity;
    (void) payload;
}
void state::state::transitioning_to(size_t entity){
    (void) entity;
}

void state::idle_state::update(size_t entity, float delta){
    (void) entity;
    (void) delta;
}

void state::stationary_state::update(size_t entity, float delta){
    (void) entity;
    (void) delta;
}

void state::walking_state::update(size_t entity, float delta){
    (void) entity;
    (void) delta;
}

void state::sitting_state::update(size_t entity, float delta){
    (void) entity;
    (void) delta;
}

void state::interacting_state::update(size_t entity, float delta){
    (void) entity;
    (void) delta;
}

void state::carrying_state::update(size_t entity, float delta){
    (void) entity;
    (void) delta;
}
void state::carrying_state::on_transitioned_to(size_t entity, std::optional<size_t> payload){
    (void) entity;
    carried_item_ = payload;
}
void state::carrying_state::transitioning_to(size_t entity){
    (void) entity;
    carried_item_ = std::nullopt;
}
std::optional<size_t> state::carrying_state::get_carried_item() const{
    return carried_item_;
}

void state::eating_state::update(size_t entity, float delta){
    (void) delta;
    if(elapsed_ >= dog_config::eating_duration){ return; }

    elapsed_++;
    if(elapsed_ == dog_config::eating_duration){
        std::unique_ptr<events::event> event = std::make_unique<events::customer_finished_meal>(entity);
        event_interface::queue_event(event);
    }
}
void state::eating_state::on_transitioned_to(size_t entity, std::optional<size_t> payload){
    (void) entity;
    (void) payload;
    elapsed_ = 0;
}
int state::eating_state::get_elapsed() const{
    return elapsed_;
}
