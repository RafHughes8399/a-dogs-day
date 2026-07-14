// Scenarios for the dishwasher dog: construction and insertion into the
// level. Mirrors player_scenarios.cpp's equivalent build/insert coverage -
// dishwasher_dog has no state machine or behavior of its own yet (that's
// separate follow-up work), this only covers that it can be built and placed.
#include <catch2/catch_test_macros.hpp>

#include "test_game.h"

using testing::test_game;

SCENARIO("a dishwasher dog is built and inserted into the level", "[dishwasher_dog]"){
    GIVEN("a fresh game"){
        test_game game;
        WHEN("a dishwasher dog is built and inserted on the dogs layer"){
            const int inserted_id = 101;
            const Vector2 spawn_pos{level_config::edge_weight * 5, level_config::edge_weight * 5};
            game.insert_dishwasher_dog(inserted_id, spawn_pos);
            THEN("the level contains a dog with that id"){
                REQUIRE(game.find_entity(inserted_id) != nullptr);
            }
        }
    }
}
