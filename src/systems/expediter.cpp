#include "expediter.h"

#include "events_interface.h"

bool expediter::available::is_available() const{
    return true;
}

bool expediter::assigned::is_available() const{
    return false;
}

bool expediter::busy::is_available() const{
    return false;
}

void expediter::waiting_for_waiter::on_assigned(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
    // manager.set_order_state(order, std::make_unique<waiter_assigned>());
}

void expediter::waiting_for_waiter::on_waiter_arrived(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::waiting_for_waiter::on_food_served(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::waiting_for_waiter::on_customer_finished(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::waiting_for_waiter::on_table_cleared(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::waiter_assigned::on_assigned(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::waiter_assigned::on_waiter_arrived(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
    // manager.set_order_state(order, std::make_unique<serving_food>());
}

void expediter::waiter_assigned::on_food_served(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::waiter_assigned::on_customer_finished(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::waiter_assigned::on_table_cleared(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::serving_food::on_assigned(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::serving_food::on_waiter_arrived(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::serving_food::on_food_served(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
    // manager.set_order_state(order, std::make_unique<customer_eating>());
}

void expediter::serving_food::on_customer_finished(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::serving_food::on_table_cleared(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::customer_eating::on_assigned(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::customer_eating::on_waiter_arrived(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::customer_eating::on_food_served(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::customer_eating::on_customer_finished(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
    // manager.set_order_state(order, std::make_unique<waiting_to_clear>());
    // manager.release_waiter(order.waiter_id);
    // order.waiter_id = empty_id;
    // manager.queue_clear_order(order.order_id);
    // manager.assign_next_order();
}

void expediter::customer_eating::on_table_cleared(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::waiting_to_clear::on_assigned(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::waiting_to_clear::on_waiter_arrived(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::waiting_to_clear::on_food_served(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::waiting_to_clear::on_customer_finished(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::waiting_to_clear::on_table_cleared(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
    // manager.set_order_state(order, std::make_unique<complete>());
    // manager.release_waiter(order.waiter_id);
    // manager.assign_next_order();
}

void expediter::complete::on_assigned(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::complete::on_waiter_arrived(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::complete::on_food_served(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::complete::on_customer_finished(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

void expediter::complete::on_table_cleared(expediter& manager, order_record& order){
    (void) manager;
    (void) order;
}

expediter::expediter& expediter::expediter::get_instance(){
    static expediter instance;
    return instance;
}

expediter::expediter::expediter()
: registered_waiter_handler_([this](const events::registered_waiter& event) -> void {on_registered_waiter_event(event);}),
registered_food_counter_handler_([this](const events::registered_food_counter& event) -> void {on_registered_food_counter_event(event);}),
requested_order_service_handler_([this](const events::requested_order_service& event) -> void {on_requested_order_service_event(event);}),
waiter_arrived_at_table_handler_([this](const events::waiter_arrived_at_table& event) -> void {on_waiter_arrived_at_table_event(event);}),
waiter_served_food_handler_([this](const events::waiter_served_food& event) -> void {on_waiter_served_food_event(event);}),
customer_finished_eating_handler_([this](const events::customer_finished_eating& event) -> void {on_customer_finished_eating_event(event);}),
waiter_cleared_table_handler_([this](const events::waiter_cleared_table& event) -> void {on_waiter_cleared_table_event(event);}){
    event_interface::subscribe<events::registered_waiter>(registered_waiter_handler_);
    event_interface::subscribe<events::registered_food_counter>(registered_food_counter_handler_);
    event_interface::subscribe<events::requested_order_service>(requested_order_service_handler_);
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
    // create order_record with waiting_for_waiter state.
    // pending_order_ids_.push(order_id);
    // assign_next_order();
}

void expediter::expediter::mark_waiter_arrived_at_table(size_t waiter_id, size_t order_id){
    (void) waiter_id;
    (void) order_id;
    // find order and invoke order.state->on_waiter_arrived(*this, order).
}

void expediter::expediter::mark_food_served(size_t waiter_id, size_t order_id){
    (void) waiter_id;
    (void) order_id;
    // find order and invoke order.state->on_food_served(*this, order).
}

void expediter::expediter::mark_customer_finished(size_t customer_id, size_t order_id){
    (void) customer_id;
    (void) order_id;
    // find order and invoke order.state->on_customer_finished(*this, order).
}

void expediter::expediter::mark_table_cleared(size_t waiter_id, size_t order_id){
    (void) waiter_id;
    (void) order_id;
    // find order and invoke order.state->on_table_cleared(*this, order).
}

void expediter::expediter::process_events(){
    return;
}

void expediter::expediter::set_order_state(order_record& order, std::unique_ptr<order_state> state){
    (void) order;
    (void) state;
    // order.state = std::move(state);
}

void expediter::expediter::set_waiter_status(waiter_record& waiter, std::unique_ptr<waiter_status> status){
    (void) waiter;
    (void) status;
    // waiter.status = std::move(status);
}

void expediter::expediter::release_waiter(size_t waiter_id){
    (void) waiter_id;
    // waiter.current_order_id = empty_id;
    // set_waiter_status(waiter, std::make_unique<available>());
}

void expediter::expediter::queue_clear_order(size_t order_id){
    (void) order_id;
    // pending_clear_order_ids_.push(order_id);
}

void expediter::expediter::assign_next_order(){
    // assign pending service orders to the first available waiter.
    // choose_food_counter(order.table_position) should pick the nearest counter.
    // queue send_waiter_to_table(waiter_id, order_id, pickup_point, table_position).
    //
    // assign pending clear orders to the first available waiter.
    // queue send_waiter_to_clear_table(waiter_id, order_id, table_position).
}

void expediter::expediter::on_registered_waiter_event(const events::registered_waiter& event){
    (void) event;
    // register_waiter(event.get_waiter_id());
}

void expediter::expediter::on_registered_food_counter_event(const events::registered_food_counter& event){
    (void) event;
    // register_food_counter(event.get_counter_id(), event.get_position());
}

void expediter::expediter::on_requested_order_service_event(const events::requested_order_service& event){
    (void) event;
    // request_order_service(event.get_order_id(), event.get_table_id(), event.get_customer_id(), event.get_table_position());
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

expediter::waiter_record* expediter::expediter::find_waiter(size_t waiter_id){
    (void) waiter_id;
    // search waiters_ by id.
    return nullptr;
}

expediter::waiter_record* expediter::expediter::find_available_waiter(){
    // search waiters_ for waiter.status->is_available().
    return nullptr;
}

expediter::food_counter* expediter::expediter::choose_food_counter(Vector2 table_position){
    (void) table_position;
    // return the registered food counter nearest to table_position.
    return nullptr;
}

expediter::order_record* expediter::expediter::find_order(size_t order_id){
    (void) order_id;
    // search orders_ by id.
    return nullptr;
}
