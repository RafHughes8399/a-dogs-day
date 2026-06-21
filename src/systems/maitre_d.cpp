#include "maitre_d.h"
#include "debug_log_interface.h"
#include "events_interface.h"

namespace{
    std::string side_to_string(events::customer_queue_side queue_side){
        if(queue_side == events::customer_queue_side::left_queue){
            return "left_queue";
        }
        return "right_queue";
    }

    std::string vector_to_string(Vector2 position){
        return "{" + std::to_string(position.x) + ", " + std::to_string(position.y) + "}";
    }
}

maitre_d::maitre_d& maitre_d::maitre_d::get_instance(){
    static maitre_d instance;
    return instance;
}

maitre_d::maitre_d::maitre_d()
: seconds_since_customer_arrived_(cafe_config::dog_queue_automatic_arrival_seconds),
dogs_left_window_seconds_(0.0f),
dogs_left_in_window_(0),
pending_customer_builds_(0),
registered_table_handler_([this](const events::registered_table& event) -> void {on_registered_table_event(event);}),
registered_customer_handler_([this](const events::registered_customer& event) -> void {on_registered_customer_event(event);}),
requested_customer_table_handler_([this](const events::requested_customer_table& event) -> void {on_requested_customer_table_event(event);}),
customer_dog_arrived_handler_([this](const events::customer_dog_arrived& event) -> void {on_customer_dog_arrived_event(event);}),
customer_dog_left_handler_([this](const events::customer_dog_left& event) -> void {on_customer_dog_left_event(event);}){
    event_interface::subscribe<events::registered_table>(registered_table_handler_);
    event_interface::subscribe<events::registered_customer>(registered_customer_handler_);
    event_interface::subscribe<events::requested_customer_table>(requested_customer_table_handler_);
    event_interface::subscribe<events::customer_dog_arrived>(customer_dog_arrived_handler_);
    event_interface::subscribe<events::customer_dog_left>(customer_dog_left_handler_);
}

void maitre_d::maitre_d::register_table(size_t table_id){
    // tables_[table_id] = table_record{table_id, table_status::free, empty_id};
    (void) table_id;
}

void maitre_d::maitre_d::register_customer(size_t customer_id){
    // Customer behaviour state lives on the customer_dog entity. The maitre d'
    // only needs the id when placing that dog into a queue or assigning a table.
    (void) customer_id;
    // TODO: this does nothing at the moment
}

void maitre_d::maitre_d::request_table_for_customer(size_t customer_id){
    // Future behavior:
    // - find a free table or queue slot for customer_id
    // - update maitre d' allocation data only
    // - emit command events so the level/customer_dog can path and change state
    (void) customer_id;
}

void maitre_d::maitre_d::update(float delta){
    if(feature_flag_config::automatic_arrivals){
        check_customer_arrivals(delta);
    }
    process_events();
}

void maitre_d::maitre_d::process_events(){
    return;
}

events::customer_queue_side maitre_d::maitre_d::get_customer_queue_side() const{
    return waiting_customer_queue_.less_full_side();
}

Vector2 maitre_d::maitre_d::get_customer_spawn_position(events::customer_queue_side queue_side) const{
    return waiting_customer_queue_.get_spawn_position(queue_side);
}

void maitre_d::maitre_d::check_customer_arrivals(float delta){
    if(! feature_flag_config::automatic_arrivals){
        return;
    }

    seconds_since_customer_arrived_ += delta;
    dogs_left_window_seconds_ += delta;
    if(dogs_left_window_seconds_ >= cafe_config::dog_queue_dogs_left_window_seconds){
        dogs_left_window_seconds_ = 0.0f;
        dogs_left_in_window_ = 0;
    }

    auto outstanding_customers = waiting_customer_queue_.size() + static_cast<size_t>(pending_customer_builds_);
    auto should_seed_queue = outstanding_customers == 0;
    auto should_add_customer = seconds_since_customer_arrived_ >= cafe_config::dog_queue_automatic_arrival_seconds
        || dogs_left_in_window_ >= cafe_config::dog_queue_dogs_left_trigger;

    if(! waiting_customer_queue_.full() && pending_customer_builds_ == 0 && (should_seed_queue || should_add_customer)){
        auto queue_side = waiting_customer_queue_.less_full_side();
        auto build_position = waiting_customer_queue_.get_spawn_position(queue_side);
        debug::log(
            "[maitre_d::check_customer_arrivals, queueing automatic arrival] "
            "queue_side: " + side_to_string(queue_side)
            + ", build_position: " + vector_to_string(build_position));
        std::unique_ptr<events::event> build_dog = std::make_unique<events::build_dog>(
            cafe_config::debug_customer_dog_type,
            build_position,
            queue_side);
        event_interface::queue_event(build_dog);
        pending_customer_builds_++;
    }
    return;
}

void maitre_d::maitre_d::on_registered_table_event(const events::registered_table& event){
    register_table(event.get_table_id());
}

void maitre_d::maitre_d::on_registered_customer_event(const events::registered_customer& event){
    std::cout << "[maitre_d::on_registered_customer_event, registered customer] "
        "customer_id: " + std::to_string(event.get_customer_id()) << std::endl;
    register_customer(event.get_customer_id());
}

void maitre_d::maitre_d::on_requested_customer_table_event(const events::requested_customer_table& event){
    request_table_for_customer(event.get_customer_id());
}

void maitre_d::maitre_d::on_customer_dog_arrived_event(const events::customer_dog_arrived& event){
    auto customer_id = event.get_customer_id();
    waiting_customer_queue_.enqueue(customer_id, event.get_queue_side(), 1.0f);
    auto queue_position = waiting_customer_queue_.get_target_position(customer_id);
    auto log =
        "[maitre_d::on_customer_dog_arrived_event, entering queue at position] "
        "customer_id: " + std::to_string(customer_id)
        + ", queue_side: " + side_to_string(event.get_queue_side())
        + ", queue_position: " + vector_to_string(queue_position);
    std::cout << log << std::endl;
    
    auto send_customer_to_queue = events::send_customer_to_queue(customer_id, queue_position);
    event_interface::execute_event(send_customer_to_queue);
    seconds_since_customer_arrived_ = 0.0f;
    if(pending_customer_builds_ > 0){
        pending_customer_builds_--;
    }
}

void maitre_d::maitre_d::on_customer_dog_left_event(const events::customer_dog_left& event){
    (void) event;
    dogs_left_in_window_++;
}
