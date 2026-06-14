#include "maitre_d.h"

#include "events_interface.h"

maitre_d::maitre_d& maitre_d::maitre_d::get_instance(){
    static maitre_d instance;
    return instance;
}

maitre_d::maitre_d::maitre_d()
: registered_table_handler_([this](const events::registered_table& event) -> void {on_registered_table_event(event);}),
registered_customer_handler_([this](const events::registered_customer& event) -> void {on_registered_customer_event(event);}),
requested_customer_table_handler_([this](const events::requested_customer_table& event) -> void {on_requested_customer_table_event(event);}),
customer_dog_arrived_handler_([this](const events::customer_dog_arrived& event) -> void {on_customer_dog_arrived_event(event);}){
    event_interface::subscribe<events::registered_table>(registered_table_handler_);
    event_interface::subscribe<events::registered_customer>(registered_customer_handler_);
    event_interface::subscribe<events::requested_customer_table>(requested_customer_table_handler_);
    event_interface::subscribe<events::customer_dog_arrived>(customer_dog_arrived_handler_);
}

void maitre_d::maitre_d::register_table(size_t table_id){
    // tables_[table_id] = table_record{table_id, table_status::free, empty_id};
    (void) table_id;
}

void maitre_d::maitre_d::register_customer(size_t customer_id){
    // customers_[customer_id] = customer_record{customer_id, customer_status::registered, empty_id};
    (void) customer_id;
}

void maitre_d::maitre_d::request_table_for_customer(size_t customer_id){
    // auto customer = customers_.find(customer_id);
    // if(customer == customers_.end()){
    //     register_customer(customer_id);
    //     customer = customers_.find(customer_id);
    // }
    //
    // if(customer->second.status == customer_status::waiting_for_table){
    //     return;
    // }
    //
    // customer->second.status = customer_status::waiting_for_table;
    // customer->second.table_id = empty_id;
    // waiting_customer_ids_.push(customer_id);
    (void) customer_id;
}

void maitre_d::maitre_d::process_events(){
    return;
}

void maitre_d::maitre_d::on_registered_table_event(const events::registered_table& event){
    register_table(event.get_table_id());
}

void maitre_d::maitre_d::on_registered_customer_event(const events::registered_customer& event){
    register_customer(event.get_customer_id());
}

void maitre_d::maitre_d::on_requested_customer_table_event(const events::requested_customer_table& event){
    request_table_for_customer(event.get_customer_id());
}

void maitre_d::maitre_d::on_customer_dog_arrived_event(const events::customer_dog_arrived& event){
    // Future queue behavior:
    // - register the customer if needed
    // - find the first queue_slot whose dog_id == empty_id
    // - set that slot's dog_id to event.get_customer_id()
    // - emit/request pathing to queue_slot.position
    (void) event;
}
