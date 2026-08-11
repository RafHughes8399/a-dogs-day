// Scenarios for the dishwasher STATION (entities::dishwasher): construction,
// registration with the expediter, repositioning, removal, and the expediter's
// reaction to each. The dishwasher DOG is a different entity entirely and has
// its own file (dishwasher_dog_scenarios.cpp).
#include <catch2/catch_test_macros.hpp>

#include "test_game.h"
#include "events.h"
#include "events_interface.h"
#include "raymath.h" // Vector2Distance

using testing::test_game;

SCENARIO("the expediter registers and removes a dishwasher", "[dishwasher][expediter][registration]"){
    // registered_dishwasher / removed_dishwasher drive the expediter's
    // dishwasher list, the same shape as counters and tables.
    test_game game;
    game.tick(1.0f / 60.0f); // drain the main level's own registration events
    const int baseline = game.num_dishwashers();

    THEN("the main level's own dishwasher is already tracked"){
        REQUIRE(baseline == 1);
    }

    GIVEN("a dishwasher inserted on the stations layer"){
        const int dishwasher_id = 90;
        const Vector2 position{level_config::edge_weight * 20, level_config::edge_weight * 16};
        game.insert_dishwasher(dishwasher_id, position);
        game.tick(1.0f / 60.0f); // process registered_dishwasher

        THEN("the expediter tracks one additional dishwasher"){
            REQUIRE(game.num_dishwashers() == baseline + 1);
        }

        THEN("it is built at the requested position"){
            auto* dishwasher = game.find_dishwasher(dishwasher_id);
            REQUIRE(dishwasher != nullptr);
            REQUIRE(Vector2Distance(dishwasher->get_position(), position) < 0.01f);
        }

        WHEN("the dishwasher is removed from the level"){
            // remove_entity destroys the entity and drops it from the expediter
            // in place, so the count updates synchronously - no tick needed.
            game.remove_entity(dishwasher_id);

            THEN("the expediter no longer tracks it and the entity is gone"){
                REQUIRE(game.num_dishwashers() == baseline);
                REQUIRE(game.find_dishwasher(dishwasher_id) == nullptr);
            }
        }
    }
}

SCENARIO("a dishwasher keeps its registration when moved", "[dishwasher][expediter][move]"){
    // The expediter tracks stations by pointer, not by position, so relocating
    // one must change where it is and nothing else.
    test_game game;
    game.tick(1.0f / 60.0f);

    GIVEN("a registered dishwasher"){
        const int dishwasher_id = 91;
        const Vector2 start{level_config::edge_weight * 20, level_config::edge_weight * 16};
        game.insert_dishwasher(dishwasher_id, start);
        game.tick(1.0f / 60.0f);
        const int tracked = game.num_dishwashers();
        auto* dishwasher = game.find_dishwasher(dishwasher_id);
        REQUIRE(dishwasher != nullptr);

        WHEN("it is moved to a new position"){
            const Vector2 destination{level_config::edge_weight * 10, level_config::edge_weight * 16};
            game.move_entity(dishwasher_id, destination);
            game.tick(1.0f / 60.0f); // process move_entity

            THEN("it reports the new position and is still tracked"){
                REQUIRE(Vector2Distance(dishwasher->get_position(), destination) < 0.01f);
                REQUIRE(game.num_dishwashers() == tracked);
            }

        }
    }
}

SCENARIO("clearing jobs are not dispatched without a dishwasher", "[dishwasher][expediter][clearing]"){
    // are_dishwashers_available() gates clearing dispatch: with nowhere to put
    // the plate the job is recorded but left unassigned, rather than sending a
    // waiter to the table to strand it there.
    test_game game;
    game.tick(1.0f / 60.0f);
    auto* waiter = game.first_waiter();
    auto* dishwasher = game.first_dishwasher();
    REQUIRE(waiter != nullptr);
    REQUIRE(dishwasher != nullptr);

    GIVEN("the only dishwasher has been removed"){
        game.remove_entity(dishwasher->get_id());
        REQUIRE(game.num_dishwashers() == 0);

        WHEN("a table asks to be cleared"){
            game.request_clear_table(3);
            game.tick(1.0f / 60.0f, 10);

            THEN("the job is recorded but no waiter is dispatched"){
                REQUIRE(game.num_clearing_jobs() == 1);
                REQUIRE(waiter->is_available_for_order());
                REQUIRE(waiter->get_state_name() == "idle");
            }

            AND_WHEN("a dishwasher is added back"){
                game.insert_dishwasher(92,
                    Vector2{level_config::edge_weight * 18, level_config::edge_weight * 12});
                game.tick(1.0f / 60.0f, 10);

                THEN("the waiting job is picked up"){
                    REQUIRE(game.num_dishwashers() == 1);
                    REQUIRE_FALSE(waiter->is_available_for_order());
                }
            }
        }
    }
}
