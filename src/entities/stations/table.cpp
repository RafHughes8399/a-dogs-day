#include "config.h"
#include "entities.h"
#include "stations.h"
// ------------------------ tables -----------------------------------//
// TODO fix up table 

void entities::table::place_down(){
    decoration::place_down();
    update_interaction_positions();
    std::unique_ptr<events::event> registered_table = std::make_unique<events::registered_table>(this);
    event_interface::queue_event(registered_table);
}
