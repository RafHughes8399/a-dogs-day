#ifndef INTERACTIONS_H
#define INTERACTIONS_H
#include <cstddef>

namespace interactions{
    // * behaviour signature is (interactor, interactee, delta) - the same shape
    // * interaction_system::defined_interactions_ is declared with, indexed by
    // * the interaction_config::interactions enum

    // customer holds the table's slot, sits for a while, then releases and leaves
    inline void customer_table_sit(size_t interactor, size_t interactee, float delta){
        (void) interactor;
        (void) interactee;
        (void) delta;
    }

    // waiter carries food to the table, hands it over, then releases the slot
    inline void waiter_table_serve(size_t interactor, size_t interactee, float delta){
        (void) interactor;
        (void) interactee;
        (void) delta;
    }
}
#endif
