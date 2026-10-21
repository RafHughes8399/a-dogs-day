#ifndef INTERACTION_CONFIG_H
#define INTERACTION_CONFIG_H

#include "enum_config.h"
#include <cstddef>
#include <vector>

namespace interaction_config{
    // * ---------------------- DOG INTERACTOR INTERACTIONS ------------------------- //
    inline std::vector<size_t> waiter_dog_interactor = {
        interactions::waiter_table_serve,
        interactions::waiter_counter_pickup,
        interactions::waiter_counter_place_down,
    };
    inline std::vector<size_t> customer_dog_interactor = {
        interactions::customer_table_sit
    };

    inline std::vector<size_t> player_dog_interactor =  {
        interactions::player_station_cook
    };

    // * ------------------- STATION INTERACTEE INTERACTIONS ----------------------------- //
    inline std::vector<size_t> table_interactee = {
        interactions::customer_table_sit,
        interactions::waiter_table_serve,
    };
    inline std::vector<size_t> counter_interactee = {
        interactions::waiter_counter_pickup,
        interactions::waiter_counter_place_down
    };

    inline std::vector<size_t> cooking_station_interactee = {
        interactions::player_station_cook
    };
}
#endif
