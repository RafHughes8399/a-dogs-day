#include "expediter.h"
#include "events_interface.h"

void expediter::expediter::fulfill_order(const order& order){
    (void) order;
    // fulfill an order
    return;
}
void expediter::expediter::clear_table(){
    // send an availabel dog to clear a table

    return;
}
void expediter::expediter::create_order(waiter& waiter, food_counter_record& food_counter, table_record table){
    // create an order to be fulfiled,
    // needs the position of the table and the foood counter
    // and then push back to the list of orders
    order order = {waiter, table, food_counter, order_status::created};
    orders_.push_back(order);
    return;
}

void expediter::expediter::schedule_order(table_record table){
    order order = {empty_waiter, table, empty_counter, order_status::scheduled};
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
void expediter::expediter::send_dog_to_position(Vector2 position){
    (void) position;
    // akin to the maitre_d
    return;
}
void expediter::expediter::process_orders(){
    // iterate through orders and process them
    for(const auto& order : orders_){
        fulfill_order(order); // fulfill order if it can be
    }
    // check scheduled orders every half second, not every frame
    check_scheduled_orders();

    //clean up fulfilled orders

    return;
}

expediter::expediter& expediter::expediter::get_instance(){
    static expediter instance;
    return instance;
}

expediter::expediter::expediter()
: registered_waiter_handler_([this](const events::registered_waiter& event) -> void {on_registered_waiter_event(event);}),
registered_food_counter_handler_([this](const events::registered_food_counter& event) -> void {on_registered_food_counter_event(event);}),
requested_order_service_handler_([this](const events::requested_order_service& event) -> void {on_requested_order_service_event(event);}),
dog_reached_table_handler_([this](const events::dog_reached_table& event) -> void {on_dog_reached_table_event(event);}),
waiter_arrived_at_table_handler_([this](const events::waiter_arrived_at_table& event) -> void {on_waiter_arrived_at_table_event(event);}),
waiter_served_food_handler_([this](const events::waiter_served_food& event) -> void {on_waiter_served_food_event(event);}),
customer_finished_eating_handler_([this](const events::customer_finished_eating& event) -> void {on_customer_finished_eating_event(event);}),
waiter_cleared_table_handler_([this](const events::waiter_cleared_table& event) -> void {on_waiter_cleared_table_event(event);}){
    event_interface::subscribe<events::registered_waiter>(registered_waiter_handler_);
    event_interface::subscribe<events::registered_food_counter>(registered_food_counter_handler_);
    event_interface::subscribe<events::requested_order_service>(requested_order_service_handler_);
    event_interface::subscribe<events::dog_reached_table>(dog_reached_table_handler_);
    event_interface::subscribe<events::waiter_arrived_at_table>(waiter_arrived_at_table_handler_);
    event_interface::subscribe<events::waiter_served_food>(waiter_served_food_handler_);
    event_interface::subscribe<events::customer_finished_eating>(customer_finished_eating_handler_);
    event_interface::subscribe<events::waiter_cleared_table>(waiter_cleared_table_handler_);
}

void expediter::expediter::register_waiter(size_t waiter_id){
    (void) waiter_id;
    // if(find_waiter(waiter_id) != nullptr) return;
    // waiters_.push_back(waiter_record{waiter_id, std::make_unique<available>(), empty_id});
    // assign_next_order();
}

void expediter::expediter::register_food_counter(size_t counter_id, Vector2 position){
    (void) counter_id;
    (void) position;
    // update existing counter or append a new food_counter.
    // assign_next_order();
}

void expediter::expediter::request_order_service(size_t order_id, size_t table_id, size_t customer_id, Vector2 table_position){
    (void) order_id;
    (void) table_id;
    (void) customer_id;
    (void) table_position;
    // Scaffold: this should later create an order and queue waiter assignment.
}

void expediter::expediter::on_registered_waiter_event(const events::registered_waiter& event){
    register_waiter(event.get_waiter_id());
}

void expediter::expediter::on_registered_food_counter_event(const events::registered_food_counter& event){
    register_food_counter(event.get_counter_id(), event.get_position());
}

void expediter::expediter::on_requested_order_service_event(const events::requested_order_service& event){
    request_order_service(event.get_order_id(), event.get_table_id(), event.get_customer_id(), event.get_table_position());
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
        create_order(waiter, counter, table);
    }
    else{
        // hence there should be some way for a dog to retrigger
        schedule_order(table);
    }
}

void expediter::expediter::on_waiter_arrived_at_table_event(const events::waiter_arrived_at_table& event){
    (void) event;
    // mark_waiter_arrived_at_table(event.get_waiter_id(), event.get_order_id());
}

void expediter::expediter::on_waiter_served_food_event(const events::waiter_served_food& event){
    (void) event;
    // mark_food_served(event.get_waiter_id(), event.get_order_id());
}

void expediter::expediter::on_customer_finished_eating_event(const events::customer_finished_eating& event){
    (void) event;
    // mark_customer_finished(event.get_customer_id(), event.get_order_id());
}

void expediter::expediter::on_waiter_cleared_table_event(const events::waiter_cleared_table& event){
    (void) event;
    // mark_table_cleared(event.get_waiter_id(), event.get_order_id());
}
