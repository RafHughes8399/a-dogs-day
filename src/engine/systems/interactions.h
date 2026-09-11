#ifndef INTERACTIONS_H
#define INTERACTIONS_H
#include <cstddef>
#include "debug_log_interface.h"
#include "debug_logger.h"
namespace interactions{
    // * behaviour signature is (interactor, interactee, delta) - the same shape
    // * interaction_system::defined_interactions_ is declared with, indexed by
    // * the interaction_config::interactions enum

    // customer holds the table's slot, sits for a while, then releases and leaves
    inline void customer_table_sit(size_t interactor, size_t interactee, float delta){
        (void) interactor;
        (void) interactee;
        (void) delta;
        debug::log("customer table sit interaction attempt");

    }

    // waiter carries food to the table, hands it over, then releases the slot
    inline void waiter_table_serve(size_t interactor, size_t interactee, float delta){
        (void) interactor;
        (void) interactee;
        (void) delta;
    }
    inline void waiter_counter_pickup(size_t interactor, size_t interactee, float delta){
        (void) interactor;
        (void) interactee;
        (void) delta;
        debug::log("waiter counter pickup interaction attempt");
    }
}
#endif
