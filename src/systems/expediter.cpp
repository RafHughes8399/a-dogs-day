#include "expediter.h"
#include "dog_actions.h"
#include "events.h"
#include "events_interface.h"
#include "raymath.h"
#include <algorithm>

void expediter::expediter::fulfill_order(order& order){
    // TODO(Plan 2): drive the assigned waiter to the counter to collect food and
    // then to the table to deliver it, firing order_served on arrival and
    // returning the waiter to idle. Plan 1 only assigns resources to the order.
    (void) order;
}

void expediter::expediter::process_orders(){
    // Status + availability driven: only advance a scheduled order when both a
    // free waiter and a stocked counter exist. Anything that cannot be served
    // now stays scheduled and is retried on a later tick when availability
    // returns (a freed waiter, a restocked counter).
    for(auto& order : orders_){
        if(order.status != order_status::scheduled){
            continue;
        }
        if(!are_waiters_available() || !are_counters_available()){
            break;
        }
        auto& assigned = assign_waiter_to_order();
        auto* counter = find_counter();
        if(assigned == empty_waiter || counter == nullptr){
            break;
        }
        assigned.assigned_to_order = true;
        order.waiter_id = assigned.id;
        order.waiter = assigned.ptr;
        order.counter = counter;
        order.status = order_status::created;
        fulfill_order(order);
    }
}

bool expediter::expediter::are_waiters_available() const{
    return std::any_of(waiters_.begin(), waiters_.end(), [](const waiter& w) -> bool {
        return !w.assigned_to_order;
    });
}

bool expediter::expediter::are_counters_available() const{
    return std::any_of(food_counters_.begin(), food_counters_.end(), [](entities::food_counter* counter) -> bool {
        return !counter->is_empty();
    });
}

expediter::waiter& expediter::expediter::assign_waiter_to_order(){
    // select a free waiter to fulfill an order
    for(auto& waiter : waiters_){
        if(! waiter.assigned_to_order){
            return waiter;
        }
    }
    return empty_waiter;
}

entities::food_counter* expediter::expediter::find_counter(){
    // first counter that has food
    for(auto* counter : food_counters_){
        if(! counter->is_empty()){
            return counter;
        }
    }
    return nullptr;
}

expediter::expediter::expediter()
: next_order_id_(0),
registered_waiter_handler_([this](const events::registered_waiter& event) -> void {on_registered_waiter_event(event);}),
removed_waiter_handler_([this](const events::removed_waiter& event) -> void {on_removed_waiter_event(event);}),
registered_food_counter_handler_([this](const events::registered_food_counter& event) -> void {on_registered_food_counter_event(event);}),
removed_food_counter_handler_([this](const events::removed_food_counter& event) -> void {on_removed_food_counter_event(event);}),
dog_reached_table_handler_([this](const events::dog_reached_table& event) -> void {on_dog_reached_table_event(event);}){
    event_interface::subscribe<events::registered_waiter>(registered_waiter_handler_);
    event_interface::subscribe<events::removed_waiter>(removed_waiter_handler_);
    event_interface::subscribe<events::registered_food_counter>(registered_food_counter_handler_);
    event_interface::subscribe<events::removed_food_counter>(removed_food_counter_handler_);
    event_interface::subscribe<events::dog_reached_table>(dog_reached_table_handler_);
}

expediter::expediter::~expediter(){
    event_interface::unsubscribe<events::registered_waiter>(registered_waiter_handler_);
    event_interface::unsubscribe<events::removed_waiter>(removed_waiter_handler_);
    event_interface::unsubscribe<events::registered_food_counter>(registered_food_counter_handler_);
    event_interface::unsubscribe<events::removed_food_counter>(removed_food_counter_handler_);
    event_interface::unsubscribe<events::dog_reached_table>(dog_reached_table_handler_);
}

void expediter::expediter::register_waiter(entities::waiter_dog* dog){
    auto id = dog->get_id();
    auto existing_waiter = std::find_if(waiters_.begin(), waiters_.end(), [id](const waiter& w) -> bool {
        return w.id == id;
    });
    if(existing_waiter != waiters_.end()){
        existing_waiter->ptr = dog;
        return;
    }

    waiters_.push_back(waiter{id, dog, false});
}

void expediter::expediter::remove_waiter(size_t waiter_id){
    auto id = static_cast<int>(waiter_id);
    waiters_.erase(std::remove_if(waiters_.begin(), waiters_.end(), [id](const waiter& w) -> bool {
        return w.id == id;
    }), waiters_.end());
}

void expediter::expediter::register_food_counter(entities::food_counter* counter){
    auto id = counter->get_id();
    auto existing_counter = std::find_if(food_counters_.begin(), food_counters_.end(), [id](entities::food_counter* c) -> bool {
        return c->get_id() == id;
    });
    if(existing_counter != food_counters_.end()){
        *existing_counter = counter;
        return;
    }

    food_counters_.push_back(counter);
}

void expediter::expediter::remove_food_counter(size_t counter_id){
    auto id = static_cast<int>(counter_id);
    food_counters_.erase(std::remove_if(food_counters_.begin(), food_counters_.end(), [id](entities::food_counter* c) -> bool {
        return c->get_id() == id;
    }), food_counters_.end());
}

void expediter::expediter::on_registered_waiter_event(const events::registered_waiter& event){
    register_waiter(event.get_waiter());
}

void expediter::expediter::on_removed_waiter_event(const events::removed_waiter& event){
    remove_waiter(event.get_waiter_id());
}

void expediter::expediter::on_registered_food_counter_event(const events::registered_food_counter& event){
    register_food_counter(event.get_counter());
}

void expediter::expediter::on_removed_food_counter_event(const events::removed_food_counter& event){
    remove_food_counter(event.get_counter_id());
}

void expediter::expediter::on_dog_reached_table_event(const events::dog_reached_table& event){
    // The seated customer has requested an order. Record it as scheduled and
    // unassigned; process_orders() binds a waiter + counter once both are
    // available.
    auto table = table_record{
        static_cast<int>(event.get_table_id()),
        Vector2Zero(),
        event.get_table_position()
    };
    orders_.push_back(order{
        next_order_id_++,
        event.get_customer_id(),
        -1,
        nullptr,
        table,
        nullptr,
        order_status::scheduled
    });
}
