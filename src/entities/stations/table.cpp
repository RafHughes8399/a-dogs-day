#include "config.h"
#include "entities.h"
// ------------------------ tables -----------------------------------//
bool entities::table::can_accept_dog(){
    return state_ == table_state::available;
}

bool entities::table::reserve_for(int dog_id){
    if(! can_accept_dog()){
        return false;
    }

    state_ = table_state::reserved;
    assigned_dog_id_ = dog_id;
    return true;
}

void entities::table::occupy(){
    enter(assigned_dog_id_);
    state_ = table_state::occupied;
}

void entities::table::clear(){
    leave(assigned_dog_id_);
    state_ = table_state::available;
    assigned_dog_id_ = level_config::empty_node;
}

entities::table::table_state entities::table::get_state(){
    return state_;
}

int entities::table::get_assigned_dog_id(){
    return assigned_dog_id_;
}

void entities::table::place_down(){
    decoration::place_down();
    update_interaction_positions();
    std::unique_ptr<events::event> registered_table = std::make_unique<events::registered_table>(this);
    event_interface::queue_event(registered_table);
}
