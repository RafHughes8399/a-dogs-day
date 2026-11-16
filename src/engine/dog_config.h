#ifndef DOG_CONFIG_H
#define DOG_CONFIG_H

#include "cafe_config.h"
#include "events/event_core.h"
#include "game_config.h"
#include "raylib.h"
#include <cstddef>

namespace dog_config{
    enum player_dog_states{
        player_idle = 0,
        player_walking,
        player_interacting,
        player_states_size
    };
    enum customer_dog_states{
        customer_walking = 0,
        customer_sitting,
        customer_eating,
        customer_states_size
    };
    enum waiter_dog_states{
        waiter_stationary = 0,
        waiter_idle,
        waiter_carrying,
        waiter_states_size
    };
    enum state_transitions{
        path_created = events::ids::dog_started_path_id,
        path_finished = events::ids::dog_path_complete,
        player_work_station = events::ids::player_work_station_id,
        interaction_finished = events::ids::interaction_finished_id,
        order_served = events::ids::order_served_id,
        food_dropped = events::ids::food_dropped_id,
        food_collected = events::ids::waiter_collected_food_id,
        meal_finished = events::ids::customer_finished_meal_id,
        customer_leaving = events::ids::customer_left
    };

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
