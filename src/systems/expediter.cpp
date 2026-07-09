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
    bool can_process_orders = are_waiters_available() && are_counters_available();
    if(can_process_orders){
        for(auto& order: orders_){
        // should be able to assign a waiter and counter,
        // assigns and fulfills it
        assign_waiter_to_order(order);
        pick_food_counter(order);
        fulfill_order(order);
        }
    }
}

bool expediter::expediter::are_waiters_available() const{
    return std::any_of(waiters_.begin(), waiters_.end(), [](const auto& w) -> bool {
        return !w->is_available_for_order();
    });
}

bool expediter::expediter::are_counters_available() const{
    return std::any_of(food_counters_.begin(), food_counters_.end(), [](entities::food_counter* counter) -> bool {
        return !counter->is_empty();
    });
}

void expediter::expediter::assign_waiter_to_order(order& order){
    // select a free waiter to fulfill an order. precondition assumes that there 
    // is a free waiter
    for(auto& waiter : waiters_){
        if(waiter->is_available_for_order()){
            order.waiter = waiter;
        }
    }
}
void expediter::expediter::pick_food_counter(order& order){
    // ? in effect this is always going to pick the same counter until it is depleted  
    // ? maybe include some random selected index
    for(auto& counter : food_counters_){
        if(!counter->is_empty()){
            order.counter = counter;
        }
    }
}
void expediter::expediter::register_waiter(entities::waiter_dog* dog){
    auto id = dog->get_id();
    auto existing_waiter = std::find_if(waiters_.begin(), waiters_.end(), [id](const auto& w) -> bool {
        return w->get_id() == id;
    });
    if(existing_waiter == waiters_.end()){
        waiters_.push_back(dog);
        return;
    }
}

void expediter::expediter::remove_waiter(size_t waiter_id){
    auto id = static_cast<int>(waiter_id);
    waiters_.erase(std::remove_if(waiters_.begin(), waiters_.end(), [id](const auto& w) -> bool {
        return w->get_id() == id;
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
    // creates an order request without assigng a counter and watier
    orders_.push_back(order{
        next_order_id_++,
        event.get_customer_id(),
        nullptr,
        table,
        nullptr,
        order_status::created
    });
}