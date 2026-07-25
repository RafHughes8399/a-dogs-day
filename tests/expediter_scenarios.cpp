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
#include "raymath.h" // Vector2Distance

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

// Case A: order processing across the waiter x counter availability matrix.
SCENARIO("the expediter only processes an order when a waiter and counter are both available",
         "[expediter][order][processing]"){
    test_game game;
    game.tick(1.0f / 60.0f); // register the main level's waiter, counter, tables
    const size_t customer_id = 100;
    const size_t table_id = 3; // a main-level table (binding does not depend on it)
    const Vector2 table_position{level_config::edge_weight * 6, level_config::edge_weight * 6};

    WHEN("a free waiter and a stocked counter are both available"){
        game.request_order(customer_id, table_id, table_position);
        game.tick(1.0f / 60.0f);
        THEN("the order is bound and dispatched (serving)"){
            REQUIRE(game.num_serving_jobs() == 1);
            REQUIRE(game.first_serving_job_status() == expediter::serving_job_status::serving);
        }
    }
    WHEN("a waiter is free but the counter is empty"){
        auto* counter = game.first_counter();
        REQUIRE(counter != nullptr);
        while(!counter->is_empty()){ counter->take(); }
        game.request_order(customer_id, table_id, table_position);
        game.tick(1.0f / 60.0f);
        THEN("the order stays unprocessed (created)"){
            REQUIRE(game.num_serving_jobs() == 1);
            REQUIRE(game.first_serving_job_status() == expediter::serving_job_status::created);
        }
    }
    WHEN("the counter is stocked but no waiter is free"){
        auto* waiter = game.first_waiter();
        REQUIRE(waiter != nullptr);
        waiter->set_serving(table_position); // busy -> unavailable
        game.request_order(customer_id, table_id, table_position);
        game.tick(1.0f / 60.0f);
        THEN("the order stays unprocessed (created)"){
            REQUIRE(game.first_serving_job_status() == expediter::serving_job_status::created);
        }
    }
    WHEN("neither a waiter nor a stocked counter is available"){
        auto* waiter = game.first_waiter();
        auto* counter = game.first_counter();
        REQUIRE(waiter != nullptr);
        REQUIRE(counter != nullptr);
        waiter->set_serving(table_position);
        while(!counter->is_empty()){ counter->take(); }
        game.request_order(customer_id, table_id, table_position);
        game.tick(1.0f / 60.0f);
        THEN("the order stays unprocessed (created)"){
            REQUIRE(game.first_serving_job_status() == expediter::serving_job_status::created);
        }
    }
}

// Case B: a dispatched waiter walks to the counter, collects food, then delivers
// to the table - checking the leg destinations, the counter decrement, and the
// waiter's idle -> serving -> idle transition.
SCENARIO("a dispatched waiter collects food from the counter and delivers it to the table",
         "[expediter][waiter][serving]"){
    test_game game;
    game.tick(1.0f / 60.0f); // register waiter, counter, tables
    auto* waiter = game.first_waiter();
    auto* counter = game.first_counter();
    REQUIRE(waiter != nullptr);
    REQUIRE(counter != nullptr);

    const int waiter_id = waiter->get_id();
    const size_t counter_food_before = counter->current_capacity();
    const Vector2 counter_interaction = counter->get_interaction_positions().left;

    const int table_id = 3; // main-level table at {6,6} edges
    const Vector2 table_position{level_config::edge_weight * 6, level_config::edge_weight * 6};
    auto* table = dynamic_cast<entities::table*>(game.find_entity(table_id));
    REQUIRE(table != nullptr);
    const Vector2 table_interaction = table->get_interaction_positions().right;

    GIVEN("an order is requested and dispatched"){
        REQUIRE(waiter->is_available_for_order());              // idle
        game.request_order(200, static_cast<size_t>(table_id), table_position);
        game.tick(1.0f / 60.0f); // create the job
        game.tick(1.0f / 60.0f); // dispatch; the path is assigned in the same tick

        THEN("the waiter is serving and heading to the counter's interaction node"){
            REQUIRE(game.get_waiter_dog(waiter_id).get_state_name() == "serving_counter");
            REQUIRE_FALSE(waiter->is_available_for_order());
            REQUIRE(Vector2Distance(waiter->peek_destination(), counter_interaction) < 1.0f);
        }

        WHEN("the waiter reaches the counter"){
            // Food is collected at the end of the pickup animation, by
            // walking_to_table::on_enter, which paths to the table in the same
            // call - so the destination is already set once this returns.
            const bool collected = game.tick_until([&]{ return waiter->is_carrying_food(); }, 3000);
            THEN("it took one item of food and now heads to the table"){
                REQUIRE(collected);
                REQUIRE(counter->current_capacity() == counter_food_before - 1);
                REQUIRE(Vector2Distance(waiter->peek_destination(), table_interaction) < 1.0f);
            }
            AND_WHEN("the waiter reaches the table"){
                const bool delivered = game.tick_until([&]{ return waiter->is_available_for_order(); }, 3000);
                THEN("the order is served and the waiter returns to idle, carrying nothing"){
                    REQUIRE(delivered);
                    REQUIRE(game.get_waiter_dog(waiter_id).get_state_name() == "idle");
                    REQUIRE_FALSE(waiter->is_carrying_food());
                }
            }
        }
    }
}

// Case C: table registration/removal against the EXPEDITER's table list (which
// it now tracks so it can path waiters to the delivery node).
SCENARIO("the expediter registers and removes tables", "[expediter][table][registration]"){
    test_game game;
    game.tick(1.0f / 60.0f); // register the main level's tables
    const int baseline = game.num_expediter_tables();

    GIVEN("a table inserted on the stations layer"){
        const int table_id = 80;
        game.insert_entity(game.build_table(table_id,
                               Vector2{level_config::edge_weight * 14, level_config::edge_weight * 14}),
                           level_config::draw_layers::stations);
        game.tick(1.0f / 60.0f); // process registered_table

        THEN("the expediter tracks one additional table"){
            REQUIRE(game.num_expediter_tables() == baseline + 1);
        }

        WHEN("the table is removed from the level"){
            game.remove_entity(table_id);
            THEN("the expediter no longer tracks it"){
                REQUIRE(game.num_expediter_tables() == baseline);
            }
        }
    }
}

// Effective counter capacity: dispatching an order reserves one item, so a
// second order cannot claim the same food before the first waiter collects it.
SCENARIO("the expediter reserves counter food so two orders cannot claim the same item",
         "[expediter][order][reservation]"){
    test_game game;
    game.tick(1.0f / 60.0f);
    auto* counter = game.first_counter();
    REQUIRE(counter != nullptr);
    // A second waiter so two orders *could* be dispatched in the same pass.
    game.insert_waiter_dog(70, dog_config::waiter_dog_types::basic,
                           Vector2{level_config::edge_weight * 22, level_config::edge_weight * 10});
    game.tick(1.0f / 60.0f);
    // Drain the counter to a single item.
    while(counter->current_capacity() > 1){ counter->take(); }
    REQUIRE(counter->current_capacity() == 1);
    REQUIRE(counter->available_capacity() == 1);

    WHEN("two orders are requested but only one item of food is available"){
        game.request_order(300, 3, Vector2{level_config::edge_weight * 6, level_config::edge_weight * 6});
        game.request_order(301, 4, Vector2{level_config::edge_weight * 12, level_config::edge_weight * 12});
        game.tick(1.0f / 60.0f);

        THEN("only one order reserves the food; the counter has no available capacity left"){
            REQUIRE(game.num_serving_jobs() == 2);
            REQUIRE(counter->reserved() == 1);
            REQUIRE(counter->available_capacity() == 0);
        }
    }
}
