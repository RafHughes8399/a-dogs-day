#include "expediter.h"
#include "dog_actions.h"
#include "events.h"
#include "events_interface.h"
#include "raymath.h"
#include <algorithm>

void expediter::expediter::fulfill_order(order& order){
    // fulfill an order
   // TODO: fulifll the order
    /**  
     * update the order status
     * send the waiter to the food counter and then
     * to the table
     * 
     * update the watier states too
    */
    // ! use the dog action interface
    send_dog_to_counter(order.assigned_waiter.id, order.food_counter);
    send_dog_to_table(order.assigned_waiter.id, order.table);
    order.status = order_status::serving;
    return;
}
void expediter::expediter::create_order(waiter& waiter, food_counter_record& food_counter, table_record table, size_t customer_id){
    // create an order to be fulfiled,
    // needs the position of the table and the foood counter
    // and then push back to the list of orders
    order order = {next_order_id_++, customer_id, waiter, table, food_counter, order_status::created};
    orders_.push_back(order);
    return;
}

void expediter::expediter::schedule_order(table_record table, size_t customer_id){
    order order = {next_order_id_++, customer_id, empty_waiter, table, empty_counter, order_status::scheduled};
    scheduled_orders_.push_back(order);
    return;
}

bool expediter::expediter::can_create_order(waiter& waiter, food_counter_record& food_counter){
    return !(waiter == empty_waiter) && !(food_counter == empty_counter);
}

void expediter::expediter::check_scheduled_orders(){
    // Scaffold: scheduled orders should retry once waiter and food-counter availability is modeled.
    return;
}

expediter::waiter& expediter::expediter::assign_waiter_to_order(){
    // select a free waiter to fulfill an order
    for(auto & waiter : waiters_){
        if(! waiter.assigned_to_order){
            return waiter;
        }
    }
    return empty_waiter;
}

expediter::food_counter_record& expediter::expediter::find_counter(){
    if(food_counters_.empty()){
        return empty_counter;
    }
    // first counter that has food
    return food_counters_.front();
}

void expediter::expediter::send_dog_to_counter(int dog_id, food_counter_record counter){
    dog_actions::send_dog_to_furniture(dog_id, counter.interaction_position, counter.id, counter.position);
}
void expediter::expediter::send_dog_to_table(int dog_id, table_record table){
    dog_actions::send_dog_to_furniture(dog_id, table.interaction_position, table.id, table.position);
}

void expediter::expediter::process_orders(){
    // iterate through orders and process them
    for(auto& order : orders_){
        fulfill_order(order); // fulfill order if it can be
    }
    // check scheduled orders every half second, not every frame
    check_scheduled_orders();

    //clean up fulfilled orders

    return;
}

expediter::expediter::expediter()
: next_order_id_(0),
registered_waiter_handler_([this](const events::registered_waiter& event) -> void {on_registered_waiter_event(event);}),
registered_food_counter_handler_([this](const events::registered_food_counter& event) -> void {on_registered_food_counter_event(event);}),
dog_reached_table_handler_([this](const events::dog_reached_table& event) -> void {on_dog_reached_table_event(event);}){
    event_interface::subscribe<events::registered_waiter>(registered_waiter_handler_);
    event_interface::subscribe<events::registered_food_counter>(registered_food_counter_handler_);
    event_interface::subscribe<events::dog_reached_table>(dog_reached_table_handler_);
}

expediter::expediter::~expediter(){
    event_interface::unsubscribe<events::registered_waiter>(registered_waiter_handler_);
    event_interface::unsubscribe<events::registered_food_counter>(registered_food_counter_handler_);
    event_interface::unsubscribe<events::dog_reached_table>(dog_reached_table_handler_);
}

void expediter::expediter::register_waiter(size_t waiter_id){
    auto id = static_cast<int>(waiter_id);
    auto existing_waiter = std::find_if(waiters_.begin(), waiters_.end(), [id](const auto& waiter) -> bool {
        return waiter.id == id;
    });
    if(existing_waiter != waiters_.end()){
        return;
    }

    waiters_.push_back(waiter{id, Vector2{-1, -1}, false});
}

// TODO include interaction position
void expediter::expediter::register_food_counter(size_t counter_id, Vector2 position, Vector2 interaction_position){
    auto id = static_cast<int>(counter_id);
    auto existing_counter = std::find_if(food_counters_.begin(), food_counters_.end(), [id](const auto& counter) -> bool {
        return counter.id == id;
    });
    if(existing_counter != food_counters_.end()){
        existing_counter->position = position;
        return;
    }

    food_counters_.push_back(food_counter_record{id, position, interaction_position});
}

void expediter::expediter::on_registered_waiter_event(const events::registered_waiter& event){
    register_waiter(event.get_waiter_id());
}

void expediter::expediter::on_registered_food_counter_event(const events::registered_food_counter& event){
    register_food_counter(event.get_counter_id(), event.get_position(), event.get_interaction_position());
}

void expediter::expediter::on_dog_reached_table_event(const events::dog_reached_table& event){

    // there are two reasons why a order cannot be created.
    // 1. no free waiters
    // 2. no food on the counters - need to figure out the logic for this i think
    // who is going to manage the dishes on each counter
    auto waiter = assign_waiter_to_order();
    auto counter = find_counter();
    auto table = table_record{static_cast<int>(event.get_table_id()), event.get_table_position()};
    if(can_create_order(waiter, counter)){
        create_order(waiter, counter, table, event.get_customer_id());
    }
    else{
        // hence there should be some way for a dog to retrigger
        schedule_order(table, event.get_customer_id());
    }
}
