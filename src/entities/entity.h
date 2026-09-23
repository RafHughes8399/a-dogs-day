#ifndef ENTITY_H
#define ENTITY_H

#include "config.h"
#include "events.h"
#include "events_interface.h"
#include "hitbox.h"
#include "sprite.h"
#include "raylib.h"
#include "debug_log_interface.h"
#include "query_interface.h"
#include "queries.h"
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <queue>

#include "component.h"

// TODO (25 / 8 / 26) RENAME AFFTER REFACTOR IS COMPLETE, THIS IS TEMPOARARY WHILE THE REFFACTOR IS BEING INTERGRATED
namespace ecs_entities {
    //**
    // need builders for the following
    // * player dogs [khiri and mack]
    // * npc dogs [customers and waiters]
    // * the cursor [cursor and paw mark]
    // * decorations [tables, counters, stations]
    // * food  */
    // ! dog builders and destroyers
    void build_player(size_t player_id, size_t cursor_id);
    void build_dog(size_t id, Vector2 position,
        std::vector<sprite::sprite> sprites, hitbox::hitbox dog_hitbox,
        size_t kind, float reach, std::vector<size_t> interactor_interactions = {});
        void build_khiri(size_t id);
        void build_mack(size_t id);

    void build_customer_dog(size_t id, Vector2 position,
        std::vector<sprite::sprite> sprites, hitbox::hitbox customer_hitbox);
        void build_tex(size_t id, Vector2 position);
        void build_garfield(size_t id, Vector2 position);
        //**

        // .
        // .
        // .
        // build duck_hunt_dog();

        //  */

    void build_waiter_dog(size_t id, Vector2 position,
        std::vector<sprite::sprite> sprites, hitbox::hitbox waiter_hitbox);
    //**
    // build_saba()
    //  */
        void build_gianluca(size_t id, Vector2 position);
        void build_lionel(size_t id, Vector2 position);
    void build_cursor(size_t id);

    void build_decoration(size_t id, Vector2 position,
        sprite::sprite decoration_sprite, hitbox::hitbox decoration_hitbox);
        void build_test_decoration(size_t id, Vector2 position);
        //**
        // void build_gargoyle();
        //  */
        void build_gargoyle(size_t id, Vector2 position);
        void build_poker_table(size_t id, Vector2 position);
        void build_dog_painting(size_t id, Vector2 position);

    void build_station(size_t id, Vector2 position,
        sprite::sprite station_sprite, hitbox::hitbox station_hitbox,
        float station_reach, std::array<std::optional<Vector2>, DIRECTIONS> slot_offsets, std::vector<size_t> interactable_interactions = {});
        void build_counter(size_t id, Vector2 position, sprite::sprite counter_sprite);
            void build_food_counter(size_t id, Vector2 position);
        void build_table(size_t id, Vector2 position, sprite::sprite table_sprite);
            void build_dining_table(size_t id, Vector2 position);
            void build_tiled_table(size_t id, Vector2 position);
        void build_dishwasher(size_t id, Vector2 position);
        void build_stove(size_t id, Vector2 position);
        /**
        */

    void build_food(size_t id, Vector2 position, sprite::sprite food_sprite);
        void build_lasagna(size_t id, Vector2 position);
        void build_coffee(size_t id, Vector2 position);

    // the level backdrop - renderable only, no hitbox, no collision
    void build_background(size_t id);
}
#endif
