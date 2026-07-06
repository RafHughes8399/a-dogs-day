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

SCENARIO("a food counter is built and inserted into the level", "[expediter][food_counter][stub]"){
    SKIP("stub - not yet implemented");
    // GIVEN a fresh game
    //   test_game game;
    // WHEN a food counter is built and inserted on the stations layer
    //   (needs helper: test_game::build_food_counter(id, pos) wrapping e_builder.build_food_counter)
    //   game.insert_entity(game.build_food_counter(20, counter_pos), level_config::stations);
    // THEN the level holds it AND the expediter has registered it
    //   REQUIRE(game.find_entity(20) != nullptr);
    //   (insertion should emit events::registered_food_counter -> expediter records it;
    //    assert via an expediter counter-count accessor - needs helper)
}

SCENARIO("a food counter can be moved", "[expediter][food_counter][stub]"){
    SKIP("stub - not yet implemented");
    // GIVEN a food counter inserted and registered
    //   test_game game;
    //   game.insert_entity(game.build_food_counter(20, counter_pos), level_config::stations);
    // WHEN the food counter is moved
    //   (needs helper: test_game::move_food_counter(20, new_pos) / fire the move event)
    // THEN the expediter tracks the new position
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
