#include "maitre_d.h"
#include "debug_log_interface.h"
#include "events_interface.h"
#include <iostream>

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

    int queue_side_for_position(Vector2 position){
        return position.y < cafe_config::queue_midpoint_y
            ? cafe_config::queue_sides::left
            : cafe_config::queue_sides::right;
    }

    void debug_log_and_cout(const std::string& message){
        debug::log(message);
        std::cout << message << std::endl;
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
removed_table_handler_([this](const events::removed_table& event) -> void {on_removed_table_event(event);}),
registered_customer_handler_([this](const events::registered_customer& event) -> void {on_registered_customer_event(event);}),
requested_customer_table_handler_([this](const events::requested_customer_table& event) -> void {on_requested_customer_table_event(event);}),
customer_dog_created_handler_([this](const events::customer_dog_created& event) -> void {on_customer_dog_created_event(event);}),
customer_dog_left_handler_([this](const events::customer_dog_left& event) -> void {on_customer_dog_left_event(event);}),
dog_path_compelte_handler_([this](const events::dog_completed_path& event) -> void {on_dog_completed_path_event(event);}){
    event_interface::subscribe<events::registered_table>(registered_table_handler_);
    event_interface::subscribe<events::removed_table>(removed_table_handler_);
    event_interface::subscribe<events::registered_customer>(registered_customer_handler_);
    event_interface::subscribe<events::requested_customer_table>(requested_customer_table_handler_);
    event_interface::subscribe<events::customer_dog_created>(customer_dog_created_handler_);
    event_interface::subscribe<events::customer_dog_left>(customer_dog_left_handler_);
    event_interface::subscribe<events::dog_completed_path>(dog_path_compelte_handler_);
}

void maitre_d::maitre_d::register_table(size_t table_id, Vector2 position){
    register_table(table_id, position, events::table_interaction_positions{position, position});
}

void maitre_d::maitre_d::register_table(size_t table_id, Vector2 position, events::table_interaction_positions interaction_positions){
    debug_log_and_cout(
        "[maitre_d::register_table, received table record] "
        "table_id: " + std::to_string(table_id)
        + ", position: " + vector_to_string(position)
        + ", table_count_before: " + std::to_string(tables_.size()));

    auto existing_table = std::find_if(tables_.begin(), tables_.end(), [table_id](const auto& table) -> bool {
        return table.table_id == table_id;
    });
    if(existing_table != tables_.end()){
        existing_table->position = position;
        existing_table->interaction_positions = interaction_positions;
        std::sort(tables_.begin(), tables_.end(), table_comparator{});
        debug_log_and_cout(
            "[maitre_d::register_table, updated existing table record] "
            "table_id: " + std::to_string(table_id)
            + ", position: " + vector_to_string(position)
            + ", is_free: " + std::to_string(existing_table->is_free)
            + ", customer_id: " + std::to_string(existing_table->customer_id)
            + ", table_count_after: " + std::to_string(tables_.size()));
        return;
    }

    tables_.push_back(table_record{
        table_id,
        position,
        interaction_positions,
        true,
        empty_id
    });
    std::sort(tables_.begin(), tables_.end(), table_comparator{});
    debug_log_and_cout(
        "[maitre_d::register_table, inserted new table record] "
        "table_id: " + std::to_string(table_id)
        + ", position: " + vector_to_string(position)
        + ", is_free: 1"
        + ", customer_id: " + std::to_string(empty_id)
        + ", table_count_after: " + std::to_string(tables_.size()));
}

void maitre_d::maitre_d::remove_table(size_t table_id){
    auto new_end = std::remove_if(tables_.begin(), tables_.end(), [table_id](const auto& table) -> bool {
        return table.table_id == table_id;
    });
    auto removed_count = static_cast<size_t>(tables_.end() - new_end);
    tables_.erase(new_end, tables_.end());
    debug_log_and_cout(
        "[maitre_d::remove_table, removed table record] "
        "table_id: " + std::to_string(table_id)
        + ", removed_count: " + std::to_string(removed_count)
        + ", table_count_after: " + std::to_string(tables_.size()));
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
    // debug arrival behaviour //
    if(IsKeyPressed(KEY_L)){
        request_customer_arrival();
    }
    // assign_tables()
    assign_tables();
}

void maitre_d::maitre_d::update_dog_position(size_t id, Vector2 position){
    customer_queue_.update_dog_position(id, position);
}

void maitre_d::maitre_d::assign_tables(){
    // if head not empty and reach and free tables
    // try the left and try the right
    if(! are_tables_free()){
        debug_log_and_cout(
            "[maitre_d::assign_tables, no free table available] "
            "table_count: " + std::to_string(tables_.size()));
        return;
    }

    auto dequeue_result = customer_queue_.dequeue();
    auto dog_at_head = dequeue_result.dog;
    if(dog_at_head != empty_dog){
        auto& table = pick_table();
        auto table_position = pick_interaction_position(table, dog_at_head.dog_position);
        debug_log_and_cout(
            "[maitre_d::assign_tables, assigning head dog to table] "
            "dog_id: " + std::to_string(dog_at_head.dog_id)
            + ", dog_position: " + vector_to_string(dog_at_head.dog_position)
            + ", queue_position: " + vector_to_string(dog_at_head.queue_position)
            + ", table_id: " + std::to_string(table.table_id)
            + ", table_position: " + vector_to_string(table_position));
        table.is_free = false;
        table.customer_id = static_cast<size_t>(dog_at_head.dog_id);
        send_dog_to_table(static_cast<size_t>(dog_at_head.dog_id), table_position);
        for(const auto& moved_dog : dequeue_result.moved_dogs){
            send_dog_to_table(static_cast<size_t>(moved_dog.dog_id), moved_dog.queue_position);
        }
    }
    return;
}

bool maitre_d::maitre_d::are_tables_free(){
    auto free_count = static_cast<size_t>(std::count_if(tables_.begin(), tables_.end(), [](const auto& table) -> bool {
        return table.is_free;
    }));
    auto has_free_table = free_count > 0;
    debug_log_and_cout(
        "[maitre_d::are_tables_free, checked table availability] "
        "table_count: " + std::to_string(tables_.size())
        + ", free_table_count: " + std::to_string(free_count)
        + ", has_free_table: " + std::to_string(has_free_table));
    return has_free_table;
}

void maitre_d::maitre_d::send_dog_to_table(size_t id, Vector2 position){
    debug_log_and_cout(
        "[maitre_d::send_dog_to_table, queued customer path command] "
        "dog_id: " + std::to_string(id)
        + ", destination: " + vector_to_string(position));
    std::unique_ptr<events::event> send_customer = std::make_unique<events::send_customer_to_queue>(id, position);
    event_interface::queue_event(send_customer);
}
maitre_d::table_record& maitre_d::maitre_d::pick_table(){
    // TABLES ARE SORTED BY POSITION SO PICKING A TABLE IS REALLY EASY
    for(auto& table : tables_){
        if(table.is_free) {return table;}
    }
    assert(false && "pick_table called with no free tables");
    return tables_.front();
}
Vector2 maitre_d::maitre_d::pick_interaction_position(const table_record& table, Vector2 dog_position) const{
    if(dog_position.x < table.position.x){
        return table.interaction_positions.left;
    }
    return table.interaction_positions.right;
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
    debug::log(
        "[maitre_d::request_customer_arrival, queued customer build] "
        "queue_side: " + side_to_string(queue_side)
        + ", spawn_position: " + vector_to_string(spawn_position));
    std::unique_ptr<events::event> build_customer_dog = std::make_unique<events::build_customer_dog>(
        cafe_config::customer_dog_type,
        spawn_position);
    event_interface::queue_event(build_customer_dog);
    debug::log(
        "[maitre_d::request_customer_arrival, queued build event] "
        "dog_type: " + std::to_string(cafe_config::customer_dog_type)
        + ", queue_side: " + side_to_string(queue_side)
        + ", spawn_position: " + vector_to_string(spawn_position));
}

void maitre_d::maitre_d::on_registered_table_event(const events::registered_table& event){
    debug_log_and_cout(
        "[maitre_d::on_registered_table_event, registered table] "
        "table_id: " + std::to_string(event.get_table_id())
        + ", position: " + vector_to_string(event.get_position()));
    register_table(event.get_table_id(), event.get_position(), event.get_interaction_positions());
}

void maitre_d::maitre_d::on_removed_table_event(const events::removed_table& event){
    remove_table(event.get_table_id());
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
        "customer_id: " + std::to_string(event.get_customer_id())
        + ", position: " + vector_to_string(event.get_position()));
    auto queue_side = queue_side_for_position(event.get_position());
    auto queued_dog = customer_queue_.enqueue(event.get_customer_id(), queue_side);
    if(queued_dog != empty_dog){
        debug::log(
            "[maitre_d::on_customer_dog_created_event, sending customer to queue slot] "
            "customer_id: " + std::to_string(event.get_customer_id())
            + ", queue_side: " + side_to_string(queue_side)
            + ", queue_position: " + vector_to_string(queued_dog.queue_position));
        send_dog_to_table(event.get_customer_id(), queued_dog.queue_position);
    }
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
void maitre_d::maitre_d::on_dog_completed_path_event(const events::dog_completed_path& event){
    update_dog_position(event.get_id(), event.get_destination());
    debug::log(
        "[maitre_d::on_dog_compelted_path, dog completed path ] "
        "customer_id: " + std::to_string(event.get_id())
        + ", position: " + vector_to_string(event.get_destination()));
}
