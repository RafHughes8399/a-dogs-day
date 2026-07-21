#include "config.h"
#include "dogs.h"
#include "entities.h"
#include "entity.h"
#include "events.h"
#include "events_interface.h"
#include "queries.h"
#include "query_interface.h"
#include "texture.h"
#include "debug_log_interface.h"
#include "raglib.h"
#include <memory>
#include <vector>
// ------------------------------- customer dog state bases ------------------------------- //
void entities::customer_dog_state::on_path_finished(customer_dog& dog, Vector2 destination){
    (void) dog;
    (void) destination;
    return;
}

void entities::customer_dog_state::set_path(customer_dog& dog, const std::vector<Vector2>& path){
    dog.dog::set_path(path);
}
void entities::customer_dog_state::set_path(customer_dog& dog, const std::vector<Vector2>& path, int station_id, Vector2 station_position){
    // A station-targeted path always means "go sit at this table" - transition
    // the state directly here instead of round-tripping through an event. The
    // path's last waypoint is the pathfinder's snapped interaction position, so
    // customer_dog_traveling_state::on_path_finished will match it exactly on arrival.
    if(! path.empty()){
        dog.set_walking_to_table(static_cast<size_t>(station_id), station_position, path.back());
    }
    dog.dog::set_path(path);
}

void entities::customer_dog_traveling_state::on_path_finished(customer_dog& dog, Vector2 destination){
    if(Vector2Distance(destination, destination_) > level_config::edge_weight * 0.05f){
        return;
    }
    on_arrived(dog);
}

// ------------------------------- customer dog states ------------------------------- //
int entities::customer_dog::default_state::update(customer_dog& dog, float delta, int frame){
    (void) dog;
    (void) delta;
    (void) frame;
    return status_codes::nothing;
}

void entities::customer_dog::walking_to_table::on_arrived(customer_dog& dog){
    std::unique_ptr<events::event> dog_reached_station = std::make_unique<events::dog_reached_station>(
        static_cast<size_t>(dog.get_id()),
        table_id_,
        table_position_);
    event_interface::queue_event(dog_reached_station);
    dog.set_state(std::make_unique<customer_dog::seated>());
}

int entities::customer_dog::walking_to_table::update(customer_dog& dog, float delta, int frame){
    (void) dog;
    (void) delta;
    (void) frame;
    return status_codes::nothing;
}



int entities::customer_dog::seated::update(customer_dog& dog, float delta, int frame){
    (void) dog;
    (void) delta;
    (void) frame;
    // play waiting animation
    return status_codes::nothing;
}

int entities::customer_dog::eating::update(customer_dog& dog, float delta, int frame){
    (void) frame;
    (void) order_id_;
    (void) table_id_;
    (void) table_position_;
    // TODO: play animation::picking_up_food on entry (food just arrived on
    // the table) and hold for its duration before the eating timer below
    // starts counting down.
    // Count down the eating timer; when it elapses the customer is done and
    // transitions to leaving. (Pathing the customer out of the cafe is a
    // follow-up; the state transition is what the loop and tests depend on.)
    elapsed_ += delta;
    if(elapsed_ >= cafe_config::eating_duration_s){
        // TODO: play animation::placing_plate and hold for its duration
        // before calling dog.leave() below - the plate needs to visibly be
        // set down (see clear_table/expediter::dispatch_clearing_job, which
        // assumes the plate is sitting on the table by the time a waiter
        // arrives to collect it).
        // dog.leave() calls set_state(), which destroys this eating instance
        // (the unique_ptr assignment in set_state frees the old state) - it
        // must stay the last thing this function touches on `this`.
        dog.leave();
        return status_codes::nothing;
    }
    return status_codes::nothing;
}

int entities::customer_dog::leaving::update(customer_dog& dog, float delta, int frame){
    (void) delta;
    (void) frame;
    // Movement already happened this frame: stateful_npc_dog::update() calls
    // npc_dog::update() (which steps current_path_/move_paths_) *before*
    // calling state_->update() here, so there's no need to (and must not)
    // call dog.update() again - that would re-enter this same function via
    // state_->update(), since the state hasn't changed, and recurse forever.
    // Once both legs (entrance, then exit) are exhausted the dog has
    // arrived; signal dead so stateful_npc_dog::update() propagates it up to
    // the quadtree, which harvests dead entities (quadtree.cpp status_codes
    // switch).
    if(dog.current_path_.empty() && dog.move_paths_.empty()){
        return status_codes::dead;
    }
    return status_codes::nothing;
}

// ------------------------------- customer dog ------------------------------- //
void entities::customer_dog::on_give_dog_path_event(const events::give_dog_path& event){
    if(static_cast<size_t>(id_) != event.get_dog_id()){
        return;
    }
    set_path(event.get_path());
}

void entities::customer_dog::set_eating(size_t order_id, size_t table_id, Vector2 table_position){
    set_state(std::make_unique<customer_dog::eating>(order_id, table_id, table_position));
}

void entities::customer_dog::set_walking_to_table(size_t table_id, Vector2 table_position, Vector2 interaction_position){
    set_state(std::make_unique<customer_dog::walking_to_table>(table_id, table_position, interaction_position));
}


void entities::customer_dog::leave(){
    // route the dog to the exit position

    // set to leaving state
    set_state(std::make_unique<customer_dog::leaving>());
    // set the path to the cafe entrance
    const queries::path_query entrance_path_query =  queries::path_query(position_, cafe_config::cafe_entrance, direction_scalar_);
    auto entrance_path = query_interface::execute_query(queries::path_executor_, entrance_path_query);
    set_path(entrance_path);
    // and then set the subsequent path to the exit position
    const queries::path_query exit_path_query =  queries::path_query(cafe_config::cafe_entrance, cafe_config::cafe_exit, direction_scalar_);
    auto exit_path = query_interface::execute_query(queries::path_executor_, exit_path_query);
    set_path(exit_path);

    // Tells the maitre d' to free the table (see on_customer_dog_left_event,
    // maitre_d.cpp, which resolves the table and fires clear_table). Carries
    // just the id - the maitre d' looks the table up itself by matching
    // table::get_assigned_dog_id(), no dog object needed.
    std::unique_ptr<events::event> dog_left = std::make_unique<events::customer_dog_left>(
        static_cast<size_t>(get_id()));
    event_interface::queue_event(dog_left);
}