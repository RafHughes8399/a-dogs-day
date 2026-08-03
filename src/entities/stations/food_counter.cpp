#include "config.h"
#include "entities.h"
#include <cassert>
// ------------------------ food counter storage -----------------------------------//
bool entities::food_counter::store(std::unique_ptr<food> item){
    if(stored_food_.size() >= max_capacity_){
        return false;
    }
    stored_food_.push_back(std::move(item));
    return true;
}

std::unique_ptr<entities::food> entities::food_counter::take(){
    // precondition: the counter is not empty (callers guard with !is_empty()).
    assert(not stored_food_.empty() and "food_counter::take() called on an empty counter");
    auto item = std::move(stored_food_.back());
    stored_food_.pop_back();
    return item;
}

bool entities::food_counter::is_empty() const{
    return stored_food_.empty();
}

size_t entities::food_counter::current_capacity() const{
    return stored_food_.size();
}

size_t entities::food_counter::max_capacity() const{
    return max_capacity_;
}

entities::food_counter::counter_status entities::food_counter::status() const{
    if(stored_food_.empty()){
        return counter_status::empty;
    }
    if(stored_food_.size() >= max_capacity_){
        return counter_status::full;
    }
    return counter_status::has_food;
}

void entities::food_counter::reserve(){
    ++reserved_;
}
void entities::food_counter::release_reservation(){
    if(reserved_ > 0){
        --reserved_;
    }
}
size_t entities::food_counter::reserved() const{
    return reserved_;
}
size_t entities::food_counter::available_capacity() const{
    return current_capacity() > reserved_ ? current_capacity() - reserved_ : 0;
}
bool entities::food_counter::has_available_food() const{
    return available_capacity() > 0;
}

void entities::food_counter::render(Vector2 draw_position, int frame){
    entity::render(draw_position, frame);
    if(not stored_food_.empty()){
        Vector2 food_position = Vector2{
            draw_position.x + entity_config::food_draw_offset.x,
            draw_position.y + entity_config::food_draw_offset.y
        };
        stored_food_.front()->render(food_position, frame);
    }
}

void entities::food_counter::place_down(){
    decoration::place_down();
    // Recompute the flanking interaction nodes from the (possibly moved)
    // position; the expediter reads them live off this pointer via the event.
    update_interaction_positions();
    std::unique_ptr<events::event> registered_food_counter = std::make_unique<events::registered_food_counter>(
        this);
    event_interface::queue_event(registered_food_counter);
}
