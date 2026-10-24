#ifndef RECIPE_CONFIG_H
#define RECIPE_CONFIG_H

#include "enum_config.h"
#include <vector>

namespace recipe_config{
    inline std::vector<int> stove_recipes = {
        entity_config::foods::cutlets,
        entity_config::foods::boiled_chicken,
        entity_config::foods::risotto,
        entity_config::foods::carbonara,
        entity_config::foods::escargot,
    };
    inline std::vector<int> oven_recipes = {
        entity_config::foods::lasagna,
        entity_config::foods::bone_biscuits,
        entity_config::foods::mutton,
    };
    inline std::vector<int> espresso_station_recipes = {
        entity_config::foods::espresso,
        entity_config::foods::pup_cup
    };
    inline std::vector<int> bush_recipes = {
        entity_config::foods::bird,
        entity_config::foods::sausage_on_a_stick,
        entity_config::foods::lemongrass,
        entity_config::foods::pork_knuckles,
    };
    inline std::vector<int> chopping_board_recipes = {
        entity_config::foods::charpoocherie_board,
        entity_config::foods::cheddar,
        entity_config::foods::parmigiano,
        entity_config::foods::shishamo,
        entity_config::foods::rabbit_foot
    };
    inline std::vector<int> fridge_recipes = {
        entity_config::foods::brisket_leftovers,
        entity_config::foods::steak_leftovers,
        entity_config::foods::muttoff,
        entity_config::foods::prozac,
    };
}
#endif
