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
    // Customer behaviour state lives on the customer_dog entity. The maitre d'
    // only needs the id when placing that dog into a queue or assigning a table.
    (void) customer_id;
}

void maitre_d::maitre_d::request_table_for_customer(size_t customer_id){
    // Future behavior:
    // - find a free table or queue slot for customer_id
    // - update maitre d' allocation data only
    // - emit command events so the level/customer_dog can path and change state
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
    // - find the first queue_slot whose dog_id == empty_id
    // - set that slot's dog_id to event.get_customer_id()
    // - emit/request pathing to queue_slot.position
    (void) event;
}
