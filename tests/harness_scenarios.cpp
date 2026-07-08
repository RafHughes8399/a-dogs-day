// Scenarios for the test harness itself: game/level construction.
//
// STUBS: each SCENARIO calls SKIP(...) so it reports as *skipped* (not passed).
// The intended flow is sketched in comments - uncomment and fill in as the
// matching test_game methods get implemented. `(needs helper: X)` marks an
// action the test_game interface does not expose yet.
#include <catch2/catch_test_macros.hpp>

#include "test_game.h"

using testing::test_game;

SCENARIO("test_game constructs a game with a built level", "[harness]"){
    GIVEN("a fresh test_game"){
        test_game game; // ctor: InitWindow(hidden) + build_main_level()
        THEN("the pre-built main level contains the player dogs, and unknown ids are absent"){
            REQUIRE(game.find_entity(static_cast<int>(level_config::mack_id)) != nullptr);
            REQUIRE(game.find_entity(static_cast<int>(level_config::khiri_id)) != nullptr);
            REQUIRE(game.find_entity(9999) == nullptr);
        }
    }
}
