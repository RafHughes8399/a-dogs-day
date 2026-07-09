// Scenarios for the expediter / service domain: food counters (build / insert /
// move), order lifecycle (listen / create / process), and waiter navigation.
//
// STUBS: each SCENARIO calls SKIP(...) so it reports as *skipped*. Intended
// flow is in comments; `(needs helper: X)` marks an action test_game does not
// expose yet.
//
// NOTE: much of the order/waiter flow depends on production code that is still
// stubbed (waiter leaf states are no-ops; expediter order service is partial).
// These are placeholders to fill in as that logic lands.
#include <catch2/catch_test_macros.hpp>

#include "test_game.h"

using testing::test_game;

SCENARIO("the expediter registers and removes a food counter", "[expediter][food_counter][registration]"){
    // A registered_food_counter event (emitted when a counter is added to the
    // level) makes the expediter track the counter by pointer; a removed_food_counter
    // event (emitted from the quadtree when the entity leaves) drops it again.
    test_game game;
    game.tick(1.0f / 60.0f); // drain the main level's own registration events
    const int baseline = game.num_counters();

    GIVEN("a food counter inserted on the stations layer"){
        const int counter_id = 60;
        game.insert_entity(game.build_food_counter(counter_id,
                               Vector2{level_config::edge_weight * 20, level_config::edge_weight * 10}),
                           level_config::draw_layers::stations);
        game.tick(1.0f / 60.0f); // process registered_food_counter

        THEN("the expediter tracks one additional counter"){
            REQUIRE(game.num_counters() == baseline + 1);
        }

        WHEN("the counter is removed from the level"){
            // remove_entity destroys the entity and, in place, drops it from the
            // expediter - so the count updates synchronously, no tick needed.
            events::remove_entity remove{static_cast<size_t>(counter_id)};
            event_interface::execute_event(remove);

            THEN("the expediter no longer tracks it"){
                REQUIRE(game.num_counters() == baseline);
            }
        }
    }
}

SCENARIO("the expediter registers and removes a waiter dog", "[expediter][waiter][registration]"){
    // registered_waiter / removed_waiter drive the expediter's waiter pointer list.
    test_game game;
    game.tick(1.0f / 60.0f); // drain the main level's own registration events
    const int baseline = game.num_waiters();

    GIVEN("a waiter dog inserted on the dogs layer"){
        const int waiter_id = 61;
        game.insert_waiter_dog(waiter_id, dog_config::waiter_dog_types::basic,
                               Vector2{level_config::edge_weight * 22, level_config::edge_weight * 10});
        game.tick(1.0f / 60.0f); // process registered_waiter

        THEN("the expediter tracks one additional waiter"){
            REQUIRE(game.num_waiters() == baseline + 1);
        }

        WHEN("the waiter is removed from the level"){
            // remove_entity destroys the entity and, in place, drops it from the
            // expediter - so the count updates synchronously, no tick needed.
            events::remove_entity remove{static_cast<size_t>(waiter_id)};
            event_interface::execute_event(remove);

            THEN("the expediter no longer tracks it"){
                REQUIRE(game.num_waiters() == baseline);
            }
        }
    }
}

SCENARIO("the expediter listens for and records orders", "[expediter][order][stub]"){
    SKIP("stub - not yet implemented");
    // GIVEN a game with a registered waiter and food counter
    //   test_game game;
    //   (needs helper: test_game::register_waiter(id) / register_food_counter(id, pos)
    //    -> fire events::registered_waiter / registered_food_counter)
    // WHEN an order is scheduled for a seated customer
    //   (needs helper: test_game::request_order(customer_id) / fire the order event)
    //   game.tick();   // expediter.process_orders() runs inside tick
    // THEN the expediter has the order recorded as created/scheduled
    //   (assert via an expediter order-count/status accessor - needs helper)
}

SCENARIO("the expediter creates an order", "[expediter][order][stub]"){
    SKIP("stub - not yet implemented");
    // GIVEN a registered free waiter, a food counter, and a seated customer's table
    // WHEN the expediter creates an order
    //   (drives expediter::create_order via the event/tick path)
    // THEN a new order exists binding that waiter, counter, and table, status=created
}

SCENARIO("the expediter processes an order given an available waiter and food counter",
         "[expediter][order][waiter][stub]"){
    SKIP("stub - not yet implemented");
    // GIVEN a created order with an available waiter and a food counter
    //   test_game game;
    //   ... register waiter + food counter + table, schedule an order ...
    // WHEN the expediter processes orders over several ticks
    //   game.tick_until([&]{ /* order moved past 'created' */ return false; }, 120);
    // THEN the waiter is assigned and dispatched (fulfill_order): counter then table
    //   (assert waiter assigned + has a path; needs expediter/waiter accessors)
}

SCENARIO("a waiter dog navigates to the food counter and then the table",
         "[expediter][waiter][pathfinding][stub]"){
    SKIP("stub - not yet implemented");
    // GIVEN a waiter dog inserted and an order assigned to it
    //   test_game game;
    //   game.insert_waiter_dog(30, waiter_dog_type, waiter_start_pos);
    //   ... assign an order (counter + table) ...
    // WHEN the sim advances
    //   game.tick_until([&]{ /* waiter reached counter, then table */ return false; }, 240);
    // THEN the waiter paths to the food counter first, then to the table
    //   NOTE: waiter leaf states are no-op stubs today - aspirational until built out.
}
