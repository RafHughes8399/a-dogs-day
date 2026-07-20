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
    dog_actions::send_dog_to_station(order.waiter->get_id(), counter_interaction,
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
        counter->reserve(); // this item is promised to this order until collected
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
        return counter->has_available_food();
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
        if(counter->has_available_food()){
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
            // If the food hadn't been collected yet, give the reservation back so
            // the counter's effective capacity is not permanently depleted.
            if(order.counter != nullptr && !order.waiter->is_carrying_food()){
                order.counter->release_reservation();
            }
            order.waiter = nullptr;
            order.counter = nullptr;
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

void expediter::expediter::on_clear_table(const events::clear_table& event){
    // Records the job; process_clearing_jobs() (called alongside
    // process_orders() from the game loop) picks it up once a waiter is
    // free, same shape as on_dog_reached_station_event creating an order for
    // process_orders() to pick up.
    auto* table = event.get_table();
    clearing_jobs_.push_back(clearing_job{
        next_clearing_job_id_++,
        table_record{
            table->get_id(),
            table->get_interaction_positions().left,
            table->get_position()
        },
        nullptr,
        order_status::created
    });
}

void expediter::expediter::process_clearing_jobs(){
    // Mirrors process_orders(): drop finished jobs, then assign a free
    // waiter to each newly-created job and dispatch it.
    clearing_jobs_.erase(std::remove_if(clearing_jobs_.begin(), clearing_jobs_.end(), [](const clearing_job& j) -> bool {
        return j.status == order_status::cleared;
    }), clearing_jobs_.end());

    for(auto& job : clearing_jobs_){
        if(job.status != order_status::created){
            continue;
        }
        if(!are_waiters_available()){
            break;
        }
        auto* waiter = assign_waiter_to_clear_table(job);
        if(waiter == nullptr){
            break;
        }
        waiter->set_clearing();
        job.status = order_status::clearing;
        dispatch_clearing_job(job);
    }
}

entities::waiter_dog* expediter::expediter::assign_waiter_to_clear_table(clearing_job& job){
    for(auto* waiter : waiters_){
        if(waiter->is_available_for_order()){
            job.waiter = waiter;
            return waiter;
        }
    }
    job.waiter = nullptr;
    return nullptr;
}

void expediter::expediter::dispatch_clearing_job(clearing_job& job){
    // Future behavior:
    // - send job.waiter to job.table.interaction_position first, mirroring
    //   fulfill_order()'s dispatch to the counter (dog_actions::send_dog_to_station)
    // - on arrival (on_dog_completed_path_event) the waiter should: pick up
    //   the dirty plate (see animation TODOs on waiter_dog::clearing /
    //   customer_dog for pickup/placement timing), then path to a registered
    //   dishwasher station (entities::dishwasher, stations.h) and place the
    //   plate down
    // - on_dog_completed_path_event currently distinguishes a serving
    //   waiter's leg via is_carrying_food(); a clearing waiter will need an
    //   equivalent signal (e.g. searching clearing_jobs_ by waiter pointer,
    //   same as orders_ is searched today) to tell "reached the table" apart
    //   from "reached the dishwasher"
    // - once placed, flip job.status to order_status::cleared and call
    //   job.waiter->set_idle() to free the waiter, mirroring the existing
    //   served -> fulfilled transition below
    (void) job;
}

void expediter::expediter::on_dog_reached_station_event(const events::dog_reached_station& event){
    // The seated customer has requested an order. Record it as created and
    // unassigned; process_orders() binds a waiter + counter once both are
    // available.
    auto table = table_record{
        static_cast<int>(event.get_station_id()),
        Vector2Zero(),
        event.get_station_position()
    };
    orders_.push_back(order{
        next_order_id_++,
        event.get_dog_id(),
        nullptr,
        table,
        nullptr,
        order_status::created
    });
}

void expediter::expediter::on_dog_completed_path_event(const events::dog_completed_path& event){
    auto dog_id = static_cast<int>(event.get_id());
    // TODO: this only matches serving waiters against orders_. A waiter in
    // waiter_dog::clearing completing a leg of the table -> dishwasher
    // journey isn't handled here yet - see dispatch_clearing_job() above for
    // the intended shape (an equivalent find_if against clearing_jobs_ by
    // waiter id, branching on plate-carrying state instead of
    // is_carrying_food()).
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
        // Reached the counter: collect food (the reservation is now fulfilled)
        // and head to the table.
        if(active_order.counter != nullptr && !active_order.counter->is_empty()){
            active_order.waiter->hold_food(active_order.counter->take());
            active_order.counter->release_reservation();
        }
        auto* table = find_table(active_order.table.id);
        Vector2 table_interaction = (table != nullptr)
            ? table->get_interaction_positions().right
            : active_order.table.position;
        dog_actions::send_dog_to_station(dog_id, table_interaction, active_order.table.id, active_order.table.position);
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
