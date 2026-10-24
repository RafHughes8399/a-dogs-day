#ifndef DOG_CONFIG_H
#define DOG_CONFIG_H

#include "cafe_config.h"
#include "game_config.h"
#include "raylib.h"
#include <cstddef>

namespace dog_config{
    inline const Vector2 dog_move_speed = {level_config::edge_weight, level_config::edge_weight};
    inline const int eating_duration = game_config::frames * 10;
    inline const float customer_spawn_interval = 20.0f;
    inline const float dog_reach = level_config::edge_weight * 0.3f;

    inline const float waiter_idle_bounds_edges = 3.0f;
    inline const size_t waiter_idle_min_points = 2;
    inline const size_t waiter_idle_max_points = 4;
    inline const size_t waiter_wander_odds = 6;
    inline const size_t waiter_idle_max_attempts = 3;
}
#endif
