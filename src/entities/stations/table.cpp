#include "config.h"
#include "entities.h"
#include "stations.h"
// ------------------------ tables -----------------------------------//

bool entities::table::can_accept_dog(){
    return cafe_state_ == table_state::available;
}

void entities::table::clear(){
    // Free the seat as well as the reservation: the customer that was sitting
    // here is gone, so the station stops interacting and the table becomes
    // claimable again.
    if(assigned_dog_id_ != no_dog_id){
        leave(assigned_dog_id_);
    }
    assigned_dog_id_ = no_dog_id;
    cafe_state_ = table_state::available;
}

int entities::table::get_assigned_dog_id(){
    return assigned_dog_id_;
}

entities::table::table_state entities::table::get_state() const{
    return cafe_state_;
}

void entities::table::occupy(){
    // Only a reserved table can be occupied - the assigned dog id is what
    // enter() records, and an available table has nobody to seat.
    if(cafe_state_ != table_state::reserved){
        return;
    }
    enter(assigned_dog_id_);
    cafe_state_ = table_state::occupied;
}

bool entities::table::reserve_for(int dog_id){
    if(cafe_state_ != table_state::available){
        return false;
    }
    assigned_dog_id_ = dog_id;
    cafe_state_ = table_state::reserved;
    return true;
}

void entities::table::place_down(){
    decoration::place_down();
    update_interaction_positions();
    std::unique_ptr<events::event> registered_table = std::make_unique<events::registered_table>(this);
    event_interface::queue_event(registered_table);
}
