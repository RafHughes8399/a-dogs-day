#include "expediter.h"
#include "dog_actions.h"
#include "events.h"
#include "events_interface.h"
#include "raymath.h"
#include <algorithm>

void expediter::expediter::fulfill_order(order& order){
    // Dispatch: send the assigned waiter to the counter to collect food. The
    // rest of the journey (counter -> table -> served) is driven by
    // on_dog_completed_path_event as the waiter reaches each stop.
    auto counter_interaction = order.counter->get_interaction_positions().left;
    dog_actions::send_dog_to_furniture(order.waiter->get_id(), counter_interaction,
        order.counter->get_id(), order.counter->get_position());
}

void expediter::expediter::process_orders(){
    // Drop completed orders so their (now idle) waiter is free again.
    orders_.erase(std::remove_if(orders_.begin(), orders_.end(), [](const order& o) -> bool {
        return o.status == order_status::fulfilled;
    }), orders_.end());

    // Status + availability driven: only dispatch a created order when both a
    // free waiter and a stocked counter exist. Binding a waiter flips it to
    // unavailable, so a second order in the same pass can't grab the same one.
    for(auto& order : orders_){
        if(order.status != order_status::created){
            continue;
        }
        if(!are_waiters_available() || !are_counters_available()){
            break;
        }
        auto* waiter = assign_waiter_to_order(order);
        auto* counter = pick_food_counter(order);
        if(waiter == nullptr || counter == nullptr){
            break;
        }
        waiter->set_serving();
        order.status = order_status::serving;
        fulfill_order(order);
    }
}

bool expediter::expediter::are_waiters_available() const{
    return std::any_of(waiters_.begin(), waiters_.end(), [](entities::waiter_dog* w) -> bool {
        return w->is_available_for_order();
    });
}

bool expediter::expediter::are_counters_available() const{
    return std::any_of(food_counters_.begin(), food_counters_.end(), [](entities::food_counter* counter) -> bool {
        return !counter->is_empty();
    });
}

entities::waiter_dog* expediter::expediter::assign_waiter_to_order(order& order){
    for(auto* waiter : waiters_){
        if(waiter->is_available_for_order()){
            order.waiter = waiter;
            return waiter;
        }
    }
    order.waiter = nullptr;
    return nullptr;
}

entities::food_counter* expediter::expediter::pick_food_counter(order& order){
    for(auto* counter : food_counters_){
        if(!counter->is_empty()){
            order.counter = counter;
            return counter;
        }
    }
    order.counter = nullptr;
    return nullptr;
}

entities::table* expediter::expediter::find_table(int table_id){
    for(auto* table : tables_){
        if(table->get_id() == table_id){
            return table;
        }
    }
    return nullptr;
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
    // Abort any in-flight order bound to this waiter so we never dereference the
    // dangling pointer later; the order re-queues as created for another waiter.
    for(auto& order : orders_){
        if(order.waiter != nullptr && order.waiter->get_id() == id){
            order.waiter = nullptr;
            order.status = order_status::created;
        }
    }
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
    for(auto& order : orders_){
        if(order.counter != nullptr && order.counter->get_id() == id){
            order.counter = nullptr;
            order.status = order_status::created;
        }
    }
    food_counters_.erase(std::remove_if(food_counters_.begin(), food_counters_.end(), [id](entities::food_counter* c) -> bool {
        return c->get_id() == id;
    }), food_counters_.end());
}

void expediter::expediter::register_table(entities::table* table){
    auto id = table->get_id();
    auto existing_table = std::find_if(tables_.begin(), tables_.end(), [id](entities::table* t) -> bool {
        return t->get_id() == id;
    });
    if(existing_table != tables_.end()){
        *existing_table = table;
        return;
    }

    tables_.push_back(table);
}

void expediter::expediter::remove_table(size_t table_id){
    auto id = static_cast<int>(table_id);
    tables_.erase(std::remove_if(tables_.begin(), tables_.end(), [id](entities::table* t) -> bool {
        return t->get_id() == id;
    }), tables_.end());
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

void expediter::expediter::on_registered_table_event(const events::registered_table& event){
    register_table(event.get_table());
}

void expediter::expediter::on_removed_table_event(const events::removed_table& event){
    remove_table(event.get_table_id());
}

void expediter::expediter::on_dog_reached_table_event(const events::dog_reached_table& event){
    // The seated customer has requested an order. Record it as created and
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
        nullptr,
        table,
        nullptr,
        order_status::created
    });
}

void expediter::expediter::on_dog_completed_path_event(const events::dog_completed_path& event){
    auto dog_id = static_cast<int>(event.get_id());
    // Match the completed path to the in-flight order for this waiter. Which leg
    // it is (counter vs table) is told by whether the waiter is already carrying
    // food: not carrying -> just reached the counter; carrying -> reached the table.
    auto it = std::find_if(orders_.begin(), orders_.end(), [dog_id](const order& o) -> bool {
        return o.status == order_status::serving && o.waiter != nullptr && o.waiter->get_id() == dog_id;
    });
    if(it == orders_.end()){
        return;
    }
    auto& active_order = *it;

    if(!active_order.waiter->is_carrying_food()){
        // Reached the counter: collect food and head to the table.
        if(active_order.counter != nullptr && !active_order.counter->is_empty()){
            active_order.waiter->hold_food(active_order.counter->take());
        }
        auto* table = find_table(active_order.table.id);
        Vector2 table_interaction = (table != nullptr)
            ? table->get_interaction_positions().right
            : active_order.table.position;
        dog_actions::send_dog_to_furniture(dog_id, table_interaction, active_order.table.id, active_order.table.position);
    }
    else{
        // Reached the table carrying food: serve it, free the waiter.
        std::unique_ptr<events::event> served = std::make_unique<events::order_served>(
            active_order.order_id,
            static_cast<size_t>(dog_id),
            active_order.customer_id,
            static_cast<size_t>(active_order.table.id),
            active_order.table.position);
        event_interface::queue_event(served);
        active_order.waiter->release_food();
        active_order.waiter->set_idle();
        active_order.status = order_status::fulfilled;
    }
}
