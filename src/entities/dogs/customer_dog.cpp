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
// ------------------------------- customer dog states ------------------------------- //
int entities::customer_dog::default_state::update(customer_dog& dog, float delta, int frame, int status){
    (void) dog;
    (void) delta;
    (void) frame;
    (void) status;
    return status_codes::nothing;
}

int entities::customer_dog::walking_to_table::update(customer_dog& dog, float delta, int frame, int status){
    (void) delta;
    (void) frame;
    if(not dog.has_arrived(status)){
        return status_codes::nothing;
    }
    std::unique_ptr<events::event> dog_reached_station = std::make_unique<events::dog_reached_station>(
        static_cast<size_t>(dog.get_id()),
        table_id_,
        table_position_);
    event_interface::queue_event(dog_reached_station);
    // set_state frees this walking_to_table instance, so it must stay the last
    // thing this function touches on `this` (same constraint as eating below).
    dog.set_state(std::make_unique<customer_dog::seated>());
    return status_codes::nothing;
}

int entities::customer_dog::seated::update(customer_dog& dog, float delta, int frame, int status){
    (void) dog;
    (void) delta;
    (void) frame;
    (void) status;
    // play waiting animation
    return status_codes::nothing;
}

int entities::customer_dog::eating::update(customer_dog& dog, float delta, int frame, int status){
    (void) frame;
    (void) status;
    (void) order_id_;
    (void) table_id_;
    (void) table_position_;
    // TODO (25 / 8 / 26) play animation::picking_up_food on entry (food just arrived on
    // the table) and hold for its duration before the eating timer below
    // starts counting down.
    // Count down the eating timer; when it elapses the customer is done and
    // transitions to leaving. (Pathing the customer out of the cafe is a
    // follow-up; the state transition is what the loop and tests depend on.)
    elapsed_ += delta;
    if(elapsed_ >= cafe_config::eating_duration_s){
        // TODO (25 / 8 / 26) play animation::placing_plate and hold for its duration
        // before calling dog.leave() below - the plate needs to visibly be
        // set down (see clear_table/expediter::process_clearing_job, which
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

int entities::customer_dog::leaving::update(customer_dog& dog, float delta, int frame, int status){
    (void) delta;
    (void) frame;
    (void) status;
    // Movement already happened this frame: customer_dog::update() calls
    // npc_dog::update() (which steps current_path_/move_paths_) *before*
    // calling state_->update() here, so there's no need to (and must not)
    // call dog.update() again - that would re-enter this same function via
    // state_->update(), since the state hasn't changed, and recurse forever.
    //
    // leave() queues two legs (entrance, then exit), so this deliberately
    // checks both queues rather than reacting to `status`: a completed_path
    // for the first leg is not an exit, and a dog whose path queries both
    // came back empty must still be harvested rather than idling forever at
    // the table. Signalling dead propagates through customer_dog::update() to
    // the quadtree, which harvests dead entities (quadtree.cpp status_codes
    // switch).
    if(dog.current_path_.empty() and dog.move_paths_.empty()){
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

void entities::customer_dog::set_path(const std::vector<Vector2>& path, int station_id, Vector2 station_position){
    // A station-targeted path always means "go sit at this table" - transition
    // the state directly here instead of round-tripping through an event, or
    // through a set_path hook on the state (which every state implemented
    // identically anyway).
    if(not path.empty()){
        set_walking_to_table(static_cast<size_t>(station_id), station_position);
    }
    dog::set_path(path);
}

void entities::customer_dog::set_eating(size_t order_id, size_t table_id, Vector2 table_position){
    set_state(std::make_unique<customer_dog::eating>(order_id, table_id, table_position));
}

void entities::customer_dog::set_walking_to_table(size_t table_id, Vector2 table_position){
    set_state(std::make_unique<customer_dog::walking_to_table>(table_id, table_position));
}

int entities::customer_dog::update(float delta, int frame){
    auto status = npc_dog::update(delta, frame);
    // Movement reports what the path system did, the state adds what it did.
    // A state returning `nothing` (0) contributes nothing rather than erasing
    // movement, so a state that repositions the dog can return `moved` and
    // have it reach the quadtree.
    return status | state_->update(*this, delta, frame, status);
}


void entities::customer_dog::leave(){
    set_state(std::make_unique<customer_dog::leaving>());
    // Two legs: out to the entrance, then off the map. Failures are ignored
    // deliberately - leaving::update harvests a dog with no path left, so an
    // unreachable exit still gets cleaned up rather than idling at the table.
    path_to(cafe_config::cafe_entrance);
    path_to(cafe_config::cafe_entrance, cafe_config::cafe_exit);

    // Tells the maitre d' to free the table (see on_customer_dog_left_event,
    // maitre_d.cpp, which resolves the table and fires clear_table). Carries
    // just the id - the maitre d' looks the table up itself by matching
    // table::get_assigned_dog_id(), no dog object needed.
    std::unique_ptr<events::event> dog_left = std::make_unique<events::customer_dog_left>(
        static_cast<size_t>(get_id()));
    event_interface::queue_event(dog_left);
}