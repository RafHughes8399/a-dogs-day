#include "maitre_d.h"
#include "debug_log_interface.h"
#include "dog_actions.h"
#include "events.h"
#include "events/event_core.h"
#include "events_interface.h"
#include <algorithm>
#include <memory>

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
dog_path_compelte_handler_([this](const events::dog_completed_path& event) -> void {on_dog_completed_path_event(event);}),
dog_reached_station_handler_([this](const events::dog_reached_station& event) -> void {on_dog_reached_station_event(event);}),
clear_table_handler_([this](const events::clear_table& event) -> void {on_clear_table_event(event);}){
    event_interface::subscribe<events::registered_table>(registered_table_handler_);
    event_interface::subscribe<events::removed_table>(removed_table_handler_);
    event_interface::subscribe<events::registered_customer>(registered_customer_handler_);
    event_interface::subscribe<events::requested_customer_table>(requested_customer_table_handler_);
    event_interface::subscribe<events::customer_dog_created>(customer_dog_created_handler_);
    event_interface::subscribe<events::customer_dog_left>(customer_dog_left_handler_);
    event_interface::subscribe<events::dog_completed_path>(dog_path_compelte_handler_);
    event_interface::subscribe<events::dog_reached_station>(dog_reached_station_handler_);
    event_interface::subscribe<events::clear_table>(clear_table_handler_);
}

maitre_d::maitre_d::~maitre_d(){
    event_interface::unsubscribe<events::registered_table>(registered_table_handler_);
    event_interface::unsubscribe<events::removed_table>(removed_table_handler_);
    event_interface::unsubscribe<events::registered_customer>(registered_customer_handler_);
    event_interface::unsubscribe<events::requested_customer_table>(requested_customer_table_handler_);
    event_interface::unsubscribe<events::customer_dog_created>(customer_dog_created_handler_);
    event_interface::unsubscribe<events::customer_dog_left>(customer_dog_left_handler_);
    event_interface::unsubscribe<events::dog_completed_path>(dog_path_compelte_handler_);
    event_interface::unsubscribe<events::clear_table>(clear_table_handler_);
    event_interface::unsubscribe<events::dog_reached_station>(dog_reached_station_handler_);
}

void maitre_d::maitre_d::register_table(entities::table* table){
    auto table_id = table->get_id();
    auto existing_table = std::find_if(tables_.begin(), tables_.end(), [table_id](entities::table* t) -> bool {
        return t->get_id() == table_id;
    });
    if(existing_table != tables_.end()){
        *existing_table = table;
        std::sort(tables_.begin(), tables_.end(), table_comparator{});
        debug::log(
            "[maitre_d::register_table, updated existing table pointer] "
            "table_id: " + std::to_string(table_id)
            + ", position: " + vector_to_string(table->get_position())
            + ", table_count_after: " + std::to_string(tables_.size()));
        return;
    }

    tables_.push_back(table);
    std::sort(tables_.begin(), tables_.end(), table_comparator{});
    debug::log(
        "[maitre_d::register_table, inserted new table pointer] "
        "table_id: " + std::to_string(table_id)
        + ", position: " + vector_to_string(table->get_position())
        + ", table_count_after: " + std::to_string(tables_.size()));
}

void maitre_d::maitre_d::remove_table(size_t table_id){
    auto id = static_cast<int>(table_id);
    auto new_end = std::remove_if(tables_.begin(), tables_.end(), [id](entities::table* t) -> bool {
        return t->get_id() == id;
    });
    auto removed_count = static_cast<size_t>(tables_.end() - new_end);
    tables_.erase(new_end, tables_.end());
    debug::log(
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
        return;
    }

    auto dequeue_result = customer_queue_.dequeue();
    auto dog_at_head = dequeue_result.dog;
    if(dog_at_head != empty_dog){
        auto* table = pick_table();
        auto interaction_position = pick_interaction_position(table, dog_at_head.dog_position);
        debug::log(
            "[maitre_d::assign_tables, assigning head dog to table] "
            "dog_id: " + std::to_string(dog_at_head.dog_id)
            + ", dog_position: " + vector_to_string(dog_at_head.dog_position)
            + ", queue_position: " + vector_to_string(dog_at_head.queue_position)
            + ", table_id: " + std::to_string(table->get_id())
            + ", table_position: " + vector_to_string(table->get_position())
            + ", interaction_position: " + vector_to_string(interaction_position));
        table->reserve_for(dog_at_head.dog_id);
        dog_actions::send_dog_to_station(dog_at_head.dog_id, interaction_position, table->get_id(), table->get_position());
        for(const auto& moved_dog : dequeue_result.moved_dogs){
            send_dog_to_queue_position(static_cast<size_t>(moved_dog.dog_id), moved_dog.queue_position);
        }
    }
    return;
}

bool maitre_d::maitre_d::are_tables_free(){
    auto free_count = static_cast<size_t>(std::count_if(tables_.begin(), tables_.end(), [](entities::table* table) -> bool {
        return table->can_accept_dog();
    }));
    auto has_free_table = free_count > 0;
    return has_free_table;
}


void maitre_d::maitre_d::send_dog_to_queue_position(size_t id, Vector2 position){
    dog_actions::send_dog_to_position(static_cast<int>(id), position);
}


entities::table* maitre_d::maitre_d::pick_table(){
    // TABLES ARE SORTED BY POSITION SO PICKING A TABLE IS REALLY EASY
    for(auto* table : tables_){
        if(table->can_accept_dog()) {return table;}
    }
    assert(false && "pick_table called with no free tables");
    return tables_.front();
}
Vector2 maitre_d::maitre_d::pick_interaction_position(entities::table* table, Vector2 dog_position) const{
    auto interaction_positions = table->get_interaction_positions();
    if(dog_position.x < table->get_position().x){
        return interaction_positions.left;
    }
    return interaction_positions.right;
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
}

void maitre_d::maitre_d::on_registered_table_event(const events::registered_table& event){
    register_table(event.get_table());
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
        send_dog_to_queue_position(event.get_customer_id(), queued_dog.queue_position);
    }
    seconds_since_customer_arrived_ = 0.0f;
}

void maitre_d::maitre_d::on_customer_dog_left_event(const events::customer_dog_left& event){
    // TODO drop the dog from the list
    dogs_left_in_window_++;

    auto* customer = event.get_dog();
    auto entry = std::find_if(tables_.begin(), tables_.end(), [customer](entities::table* table) -> bool {
        return table->get_assigned_dog_id() == customer->get_id();
    });
    if(entry == tables_.end()){
        return;
    }
    std::unique_ptr<events::event> clear_table_event = std::make_unique<events::clear_table>(customer, *entry);
    event_interface::queue_event(clear_table_event);
}

void maitre_d::maitre_d::on_clear_table_event(const events::clear_table& event){
    // Future behavior:
    // - event.get_table() names the table that needs clearing (the customer
    //   has already left it, see on_customer_dog_left_event above)
    // - switch the table out of "occupied" into a not-yet-available state
    //   (a new table_state value, e.g. needs_clearing/dirty, since it isn't
    //   safe to reassign until a waiter has actually cleared it - see
    //   entities::table::table_state, stations.h)
    // - the expediter is separately subscribed to this same event
    //   (expediter::on_clear_table) and is responsible for dispatching a
    //   waiter to physically clear the table; once that's done the table
    //   should transition back to available

    auto table = event.get_table();
    auto dog = event.get_customer();
    table->leave(dog->get_id());
}
void maitre_d::maitre_d::on_dog_completed_path_event(const events::dog_completed_path& event){
    update_dog_position(event.get_id(), event.get_destination());
    debug::log(
        "[maitre_d::on_dog_compelted_path, dog completed path ] "
        "customer_id: " + std::to_string(event.get_id())
        + ", position: " + vector_to_string(event.get_destination()));
}

void maitre_d::maitre_d::on_dog_reached_station_event(const events::dog_reached_station& event){
    auto dog_id = static_cast<int>(event.get_dog_id());
    auto table_id = static_cast<int>(event.get_station_id());
    auto entry = std::find_if(tables_.begin(), tables_.end(), [table_id](entities::table* table) -> bool {
        return table->get_id() == table_id;
    });
    if(entry == tables_.end()){
        return;
    }
    auto* table = *entry;
    if(table->get_state() != entities::table::table_state::reserved
       || table->get_assigned_dog_id() != dog_id){
        return;
    }
    debug::log(
        "[maitre_d::on_dog_reached_station_event, occupying table] "
        "dog_id: " + std::to_string(dog_id)
        + ", table_id: " + std::to_string(table_id));
    table->occupy();
}
