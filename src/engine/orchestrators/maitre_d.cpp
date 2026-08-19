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
removed_customer_handler_([this](const events::removed_customer& event) -> void {on_removed_customer_event(event);}),
dog_path_compelte_handler_([this](const events::dog_completed_path& event) -> void {on_dog_completed_path_event(event);}),
dog_reached_station_handler_([this](const events::dog_reached_station& event) -> void {on_dog_reached_station_event(event);}){
    event_interface::subscribe<events::registered_table>(registered_table_handler_);
    event_interface::subscribe<events::removed_table>(removed_table_handler_);
    event_interface::subscribe<events::registered_customer>(registered_customer_handler_);
    event_interface::subscribe<events::requested_customer_table>(requested_customer_table_handler_);
    event_interface::subscribe<events::customer_dog_created>(customer_dog_created_handler_);
    event_interface::subscribe<events::customer_dog_left>(customer_dog_left_handler_);
    event_interface::subscribe<events::removed_customer>(removed_customer_handler_);
    event_interface::subscribe<events::dog_completed_path>(dog_path_compelte_handler_);
    event_interface::subscribe<events::dog_reached_station>(dog_reached_station_handler_);
}

maitre_d::maitre_d::~maitre_d(){
    event_interface::unsubscribe<events::registered_table>(registered_table_handler_);
    event_interface::unsubscribe<events::removed_table>(removed_table_handler_);
    event_interface::unsubscribe<events::registered_customer>(registered_customer_handler_);
    event_interface::unsubscribe<events::requested_customer_table>(requested_customer_table_handler_);
    event_interface::unsubscribe<events::customer_dog_created>(customer_dog_created_handler_);
    event_interface::unsubscribe<events::customer_dog_left>(customer_dog_left_handler_);
    event_interface::unsubscribe<events::removed_customer>(removed_customer_handler_);
    event_interface::unsubscribe<events::dog_completed_path>(dog_path_compelte_handler_);
    event_interface::unsubscribe<events::dog_reached_station>(dog_reached_station_handler_);
}

void maitre_d::maitre_d::register_table(entities::table* table){
    auto table_id = static_cast<size_t>(table->get_id());
    tables_[table_id] = table;
    debug::log(
        "[maitre_d::register_table, tracked table pointer] "
        "table_id: " + std::to_string(table_id)
        + ", position: " + vector_to_string(table->get_position())
        + ", table_count_after: " + std::to_string(tables_.size()));
}

void maitre_d::maitre_d::remove_table(size_t table_id){
    auto removed_count = tables_.erase(table_id);
    debug::log(
        "[maitre_d::remove_table, removed table record] "
        "table_id: " + std::to_string(table_id)
        + ", removed_count: " + std::to_string(removed_count)
        + ", table_count_after: " + std::to_string(tables_.size()));
}

entities::table* maitre_d::maitre_d::find_table(size_t table_id){
    auto entry = tables_.find(table_id);
    return entry == tables_.end() ? nullptr : entry->second;
}

void maitre_d::maitre_d::register_customer(entities::customer_dog* customer){
    // Customer *behaviour* still lives on the customer_dog entity - this only
    // tracks the pointer so the maitre d' can read live dog state (position,
    // current state) and command the dog directly, mirroring how the expediter
    // tracks waiters_. Queue membership stays keyed by id in customer_queue_.
    if(customer == nullptr){
        return;
    }
    auto customer_id = static_cast<size_t>(customer->get_id());
    customers_[customer_id] = customer;
    debug::log(
        "[maitre_d::register_customer, tracked customer pointer] "
        "customer_id: " + std::to_string(customer_id)
        + ", position: " + vector_to_string(customer->get_position())
        + ", customer_count_after: " + std::to_string(customers_.size()));
}

void maitre_d::maitre_d::remove_customer(size_t customer_id){
    // Pointer bookkeeping only. The queue is left alone here: a customer that
    // is removed mid-queue is dropped from customer_queue_ lazily by
    // assign_tables(), the same way the expediter's reconciliation passes drop
    // jobs whose ids stop resolving.
    auto removed_count = customers_.erase(customer_id);
    debug::log(
        "[maitre_d::remove_customer, removed customer record] "
        "customer_id: " + std::to_string(customer_id)
        + ", removed_count: " + std::to_string(removed_count)
        + ", customer_count_after: " + std::to_string(customers_.size()));
}

entities::customer_dog* maitre_d::maitre_d::find_customer(size_t customer_id){
    auto entry = customers_.find(customer_id);
    return entry == customers_.end() ? nullptr : entry->second;
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
    if(not are_tables_free()){
        return;
    }

    auto dequeue_result = customer_queue_.dequeue();
    auto dog_at_head = dequeue_result.dog;
    if(dog_at_head != empty_dog){
        // The queue caches each dog's position (updated on dog_completed_path),
        // but the tracked pointer is the live entity - prefer it, and treat a
        // failed lookup as "this customer was removed while queued", dropping
        // it instead of reserving a table for a dog that no longer exists.
        auto* customer = find_customer(static_cast<size_t>(dog_at_head.dog_id));
        if(customer == nullptr){
            debug::log(
                "[maitre_d::assign_tables, dropping dequeued customer with no live entity] "
                "dog_id: " + std::to_string(dog_at_head.dog_id));
            return;
        }
        auto dog_position = customer->get_position();
        auto* table = pick_table();
        auto interaction_position = pick_interaction_position(table, dog_position);
        debug::log(
            "[maitre_d::assign_tables, assigning head dog to table] "
            "dog_id: " + std::to_string(dog_at_head.dog_id)
            + ", dog_position: " + vector_to_string(dog_position)
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
    auto free_count = static_cast<size_t>(std::count_if(tables_.begin(), tables_.end(), [](const auto& entry) -> bool {
        return entry.second->can_accept_dog();
    }));
    auto has_free_table = free_count > 0;
    return has_free_table;
}


void maitre_d::maitre_d::send_dog_to_queue_position(size_t id, Vector2 position){
    dog_actions::send_dog_to_position(static_cast<int>(id), position);
}


entities::table* maitre_d::maitre_d::pick_table(){
    // tables_ is a map keyed by id, not kept in any particular order, so the
    // nearest available table is found by an explicit distance comparison
    // rather than relying on pre-sorted order.
    entities::table* nearest = nullptr;
    float nearest_distance = 0.0f;
    for(auto& entry : tables_){
        auto* table = entry.second;
        if(not table->can_accept_dog()){
            continue;
        }
        auto distance = Vector2Distance(table->get_position(), entrance_);
        if(nearest == nullptr or distance < nearest_distance){
            nearest = table;
            nearest_distance = distance;
        }
    }
    assert(nearest != nullptr and "pick_table called with no free tables");
    return nearest;
}
Vector2 maitre_d::maitre_d::pick_interaction_position(entities::table* table, Vector2 dog_position) const{
    auto interaction_positions = table->get_interaction_positions();
    if(dog_position.x < table->get_position().x){
        return interaction_positions.left;
    }
    return interaction_positions.right;
}
void maitre_d::maitre_d::check_customer_arrivals(float delta){
    if(not feature_flag_config::automatic_arrivals){
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
        or dogs_left_in_window_ >= cafe_config::queue_left_trigger;
    if((should_seed_queue or should_add_customer) and can_request_customer_arrival()){
        request_customer_arrival();
    }
}

bool maitre_d::maitre_d::can_request_customer_arrival() const{
    return not customer_queue_.full();
}

void maitre_d::maitre_d::request_customer_arrival(){
    if(not can_request_customer_arrival()){
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
    // Carries an id only, and currently has no emitter anywhere in the
    // codebase - customer_dog_created is the event that actually registers a
    // customer, because it is the one the level fires with the live pointer.
    // Left subscribed as a trace point rather than deleted.
    debug::log(
        "[maitre_d::on_registered_customer_event, registered customer] "
        "customer_id: " + std::to_string(event.get_customer_id()));
}

void maitre_d::maitre_d::on_requested_customer_table_event(const events::requested_customer_table& event){
    request_table_for_customer(event.get_customer_id());
}

void maitre_d::maitre_d::on_customer_dog_created_event(const events::customer_dog_created& event){
    debug::log(
        "[maitre_d::on_customer_dog_created_event, confirming customer arrival] "
        "customer_id: " + std::to_string(event.get_customer_id())
        + ", position: " + vector_to_string(event.get_position()));
    register_customer(event.get_customer());
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

    // Resolve which table this customer occupied by id - no dog object
    // needed. clear_table is the sole notification for this fact; the
    // expediter is the only other subscriber (dispatches a waiter to
    // physically clear it). Maitre d' doesn't also listen to its own
    // clear_table broadcast - it already has the table pointer right here,
    // so there's nothing an event round-trip would tell it that it doesn't
    // already know.
    // Keyed by table id, not customer id, so this stays a scan over the
    // tracked tables rather than an O(1) lookup - the same complexity as
    // before the switch to unordered_map.
    auto customer_id = event.get_customer_id();
    auto entry = std::find_if(tables_.begin(), tables_.end(), [customer_id](const auto& e) -> bool {
        return e.second->get_assigned_dog_id() == static_cast<int>(customer_id);
    });
    if(entry == tables_.end()){
        return;
    }
    std::unique_ptr<events::event> clear_table_event = std::make_unique<events::clear_table>(entry->second);
    event_interface::queue_event(clear_table_event);
}
void maitre_d::maitre_d::on_removed_customer_event(const events::removed_customer& event){
    remove_customer(event.get_customer_id());
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
    auto table_id = event.get_station_id();
    auto* table = find_table(table_id);
    if(table == nullptr){
        return;
    }
    if(table->get_state() != entities::table::table_state::reserved
       or table->get_assigned_dog_id() != dog_id){
        return;
    }
    debug::log(
        "[maitre_d::on_dog_reached_station_event, occupying table] "
        "dog_id: " + std::to_string(dog_id)
        + ", table_id: " + std::to_string(table_id));
    table->occupy();
}
