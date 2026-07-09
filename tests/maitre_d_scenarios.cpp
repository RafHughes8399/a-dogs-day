// Scenarios for the maitre d' domain: customer arrival + queueing, tables
// (build / insert / move), table assignment, and customer state transitions.
//
// STUBS: each SCENARIO calls SKIP(...) so it reports as *skipped*. Intended
// flow is in comments; `(needs helper: X)` marks an action test_game does not
// expose yet.
#include <catch2/catch_test_macros.hpp>

#include "test_game.h"

using testing::test_game;

SCENARIO("a customer dog arrives and paths to the queue head", "[maitre_d][customer][pathfinding][stub]"){
    SKIP("stub - not yet implemented");
    // GIVEN a customer dog inserted at the entrance
    //   test_game game;
    //   game.insert_customer_dog(1, maitre_d::entrance_);
    // WHEN a debug arrival is fired and the sim advances
    //   game.customer_arrives();
    //   game.tick_until([&]{ return game.get_customer_dog(1).get_state_name()
    //                               != std::string("default_state"); }, 60);
    // THEN the dog has a real path toward its queue slot / the queue head
    //   REQUIRE_FALSE(game.get_customer_dog(1).get_current_path().empty());
    //   (needs accessors: get_state_name / get_current_path; DOG_DAYS_TESTING, spec step 5)
}

SCENARIO("the maitre d' registers and removes a table", "[maitre_d][table][registration]"){
    // registered_table (emitted when a table is added to the level) makes the
    // maitre d' track the table by pointer; removed_table (from the quadtree when
    // the entity leaves) drops it again.
    test_game game;
    game.tick(1.0f / 60.0f); // drain the main level's own table registrations
    const int baseline = game.num_tables();

    GIVEN("a table inserted on the stations layer"){
        const int table_id = 60;
        game.insert_entity(game.build_table(table_id,
                               Vector2{level_config::edge_weight * 16, level_config::edge_weight * 16}),
                           level_config::draw_layers::stations);
        game.tick(1.0f / 60.0f); // process registered_table

        THEN("the maitre d' tracks one additional table"){
            REQUIRE(game.num_tables() == baseline + 1);
        }

        WHEN("the table is removed from the level"){
            // remove_entity destroys the entity and, in place, drops it from the
            // maitre d' - so the count updates synchronously, no tick needed.
            events::remove_entity remove{static_cast<size_t>(table_id)};
            event_interface::execute_event(remove);

            THEN("the maitre d' no longer tracks it"){
                REQUIRE(game.num_tables() == baseline);
            }
        }
    }
}

SCENARIO("the maitre d' registers an arriving customer and drops it once seated",
         "[maitre_d][customer][registration]"){
    // A customer_dog_created event enqueues the customer into the maitre d's
    // queue (registration). The customer leaves that queue when the maitre d'
    // seats it at a free table (removal): it dequeues once the customer has
    // reached its queue slot and a table is available.
    test_game game;
    game.tick(1.0f / 60.0f); // register the main level's two tables so seating can occur
    REQUIRE(game.num_customers() == 0);

    // Spawn on the left lane: a y above the queue midpoint routes to the left side.
    const size_t customer_id = 70;
    const Vector2 queue_slot = cafe_config::left_queue_positions[0];

    GIVEN("a customer that has arrived and enqueued"){
        events::customer_dog_created created{customer_id, queue_slot};
        event_interface::execute_event(created);

        THEN("the maitre d' is tracking one queued customer"){
            REQUIRE(game.num_customers() == 1);
        }

        WHEN("the customer reaches its queue slot and the maitre d' processes assignments"){
            // dog_completed_path is how the maitre d' learns a dog reached a position;
            // arriving exactly at the queue slot marks it ready to be seated.
            events::dog_completed_path reached{customer_id, queue_slot};
            event_interface::execute_event(reached);
            game.tick(1.0f / 60.0f); // maitre_d.update -> assign a free table -> dequeue

            THEN("the customer is seated and no longer queued"){
                REQUIRE(game.num_customers() == 0);
            }
        }
    }
}

SCENARIO("the maitre d' assigns a queued customer to a free table", "[maitre_d][queue][table][stub]"){
    SKIP("stub - not yet implemented");
    // GIVEN a registered free table and a customer that has reached the queue head
    //   test_game game;
    //   game.insert_entity(game.build_table(10, table_pos), level_config::stations);
    //   game.insert_customer_dog(1, maitre_d::entrance_);
    //   game.customer_arrives();
    //   game.tick(delta, /*frames=*/ enough_to_reach_head);
    // WHEN the maitre d' processes assignments (inside tick: maitre_d_.update)
    //   game.tick_until([&]{ return game.get_customer_dog(1).get_state_name()
    //                               == std::string("walking_to_table"); }, 120);
    // THEN the customer is walking to the assigned table and the table is reserved
    //   REQUIRE_FALSE(game.get_customer_dog(1).get_current_path().empty());
    //   (table-reserved assertion needs a maitre_d accessor - helper)
}

// Case D: customer state transitions following the setters/events that drive
// them. (The waiter's idle -> serving -> idle transitions are covered by the
// serving-journey scenario in expediter_scenarios.cpp.)
SCENARIO("a customer dog transitions through its lifecycle states", "[customer][transitions]"){
    test_game game;
    const int customer_id = 90;
    const Vector2 table_position{level_config::edge_weight * 6, level_config::edge_weight * 6};
    game.insert_customer_dog(customer_id, table_position);

    GIVEN("a newly inserted customer"){
        THEN("it starts in the default state"){
            REQUIRE(game.get_customer_dog(customer_id).get_state_name() == "default_state");
        }

        WHEN("it is sent walking to a table"){
            game.get_customer_dog(customer_id).set_walking_to_table(3, table_position, table_position);
            THEN("it is walking to the table"){
                REQUIRE(game.get_customer_dog(customer_id).get_state_name() == "walking_to_table");
            }
        }

        WHEN("an order_served event is delivered for the customer"){
            std::unique_ptr<events::event> served = std::make_unique<events::order_served>(
                0, 6, static_cast<size_t>(customer_id), 3, table_position);
            event_interface::queue_event(served);
            game.tick(1.0f / 60.0f); // level routes order_served -> customer.set_eating
            THEN("the customer starts eating"){
                REQUIRE(game.get_customer_dog(customer_id).get_state_name() == "eating");
            }
        }
    }
}

// Case E: the eating duration is defined in config, and the customer leaves once
// it elapses.
SCENARIO("a customer leaves after the configured eating duration", "[customer][eating]"){
    test_game game;
    const int customer_id = 91;
    const Vector2 table_position{level_config::edge_weight * 6, level_config::edge_weight * 6};
    game.insert_customer_dog(customer_id, table_position);
    game.get_customer_dog(customer_id).set_eating(0, 3, table_position);
    REQUIRE(game.get_customer_dog(customer_id).get_state_name() == "eating");

    WHEN("less than the eating duration elapses"){
        game.tick(cafe_config::eating_duration_s * 0.5f, 1);
        THEN("the customer is still eating"){
            REQUIRE(game.get_customer_dog(customer_id).get_state_name() == "eating");
        }
    }
    WHEN("the full eating duration elapses"){
        game.tick(cafe_config::eating_duration_s + 1.0f, 1);
        THEN("the customer transitions to leaving"){
            REQUIRE(game.get_customer_dog(customer_id).get_state_name() == "leaving");
        }
    }
}
