#include "maitre_d.h"
#include "debug_log_interface.h"
#include "events_interface.h"

namespace{
    std::string vector_to_string(Vector2 position){
        return "{" + std::to_string(position.x) + ", " + std::to_string(position.y) + "}";
    }

    std::string side_to_string(int side){
        if(side == cafe_config::queue_sides::left){
            return "left";
        }
        if(side == cafe_config::queue_sides::right){
            return "right";
        }
        return "unknown";
    }
}

maitre_d::maitre_d& maitre_d::maitre_d::get_instance(){
    static maitre_d instance;
    return instance;
}

maitre_d::maitre_d::maitre_d()
: seconds_since_customer_arrived_(cafe_config::queue_arrival_s),
dogs_left_window_seconds_(0.0f),
dogs_left_in_window_(0),
registered_table_handler_([this](const events::registered_table& event) -> void {on_registered_table_event(event);}),
registered_customer_handler_([this](const events::registered_customer& event) -> void {on_registered_customer_event(event);}),
requested_customer_table_handler_([this](const events::requested_customer_table& event) -> void {on_requested_customer_table_event(event);}),
customer_dog_created_handler_([this](const events::customer_dog_created& event) -> void {on_customer_dog_created_event(event);}),
customer_dog_left_handler_([this](const events::customer_dog_left& event) -> void {on_customer_dog_left_event(event);}){
    event_interface::subscribe<events::registered_table>(registered_table_handler_);
    event_interface::subscribe<events::registered_customer>(registered_customer_handler_);
    event_interface::subscribe<events::requested_customer_table>(requested_customer_table_handler_);
    event_interface::subscribe<events::customer_dog_created>(customer_dog_created_handler_);
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
    if(IsKeyPressed(KEY_L)){
        request_customer_arrival();
    }
    process_events();
}

void maitre_d::maitre_d::process_events(){
    return;
}

void maitre_d::maitre_d::check_customer_arrivals(float delta){
    if(! feature_flag_config::automatic_arrivals){
        return;
    }
    seconds_since_customer_arrived_ += delta;
    dogs_left_window_seconds_ += delta;
    if(dogs_left_window_seconds_ >= cafe_config::queue_left_window_s){
        dogs_left_window_seconds_ = 0.0f;
        dogs_left_in_window_ = 0;
    }

    auto should_seed_queue = customer_queue_.empty();
    auto should_add_customer = seconds_since_customer_arrived_ >= cafe_config::queue_arrival_s
        || dogs_left_in_window_ >= cafe_config::queue_left_trigger;
    if((should_seed_queue || should_add_customer) && can_request_customer_arrival()){
        request_customer_arrival();
    }
}

bool maitre_d::maitre_d::can_request_customer_arrival() const{
    return ! customer_queue_.full();
}

void maitre_d::maitre_d::request_customer_arrival(){
    if(! can_request_customer_arrival()){
        debug::log(
            "[maitre_d::request_customer_arrival, blocked request] "
            "queue_full: " + std::to_string(customer_queue_.full()));
        return;
    }

    int queue_side = customer_queue_.pick_side();
    Vector2 spawn_position = cafe_config::customer_spawn_positions[queue_side];
    Vector2 destination = customer_queue_.get_enqueue_position(queue_side);
    debug::log(
        "[maitre_d::request_customer_arrival, reserved queue slot] "
        "queue_side: " + side_to_string(queue_side)
        + ", spawn_position: " + vector_to_string(spawn_position)
        + ", destination: " + vector_to_string(destination));
    std::unique_ptr<events::event> build_customer_dog = std::make_unique<events::build_customer_dog>(
        cafe_config::customer_dog_type,
        spawn_position,
        destination);
    event_interface::queue_event(build_customer_dog);
    debug::log(
        "[maitre_d::request_customer_arrival, queued build event] "
        "dog_type: " + std::to_string(cafe_config::customer_dog_type)
        + ", queue_side: " + side_to_string(queue_side)
        + ", spawn_position: " + vector_to_string(spawn_position)
        + ", destination: " + vector_to_string(destination));
}

void maitre_d::maitre_d::on_registered_table_event(const events::registered_table& event){
    register_table(event.get_table_id());
}

void maitre_d::maitre_d::on_registered_customer_event(const events::registered_customer& event){
    debug::log(
        "[maitre_d::on_registered_customer_event, registered customer] "
        "customer_id: " + std::to_string(event.get_customer_id()));
    register_customer(event.get_customer_id());
}

void maitre_d::maitre_d::on_requested_customer_table_event(const events::requested_customer_table& event){
    request_table_for_customer(event.get_customer_id());
}

void maitre_d::maitre_d::on_customer_dog_created_event(const events::customer_dog_created& event){
    debug::log(
        "[maitre_d::on_customer_dog_created_event, confirming customer arrival] "
        "customer_id: " + std::to_string(event.get_customer_id()));
    customer_queue_.enqueue(event.get_customer_id());
    seconds_since_customer_arrived_ = 0.0f;
    debug::log(
        "[maitre_d::on_customer_dog_created_event, reset arrival timer] "
        "customer_id: " + std::to_string(event.get_customer_id())
        + ", seconds_since_customer_arrived: " + std::to_string(seconds_since_customer_arrived_));
}

void maitre_d::maitre_d::on_customer_dog_left_event(const events::customer_dog_left& event){
    (void) event;
    dogs_left_in_window_++;
}
