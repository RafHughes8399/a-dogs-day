// Scenarios for decorations / stations: building a table or a food counter and
// inserting it into the level. Registration side-effects (registered_table ->
// maitre_d, registered_food_counter -> expediter) are a later addition; these
// cover build + insert + find only.
#include <catch2/catch_test_macros.hpp>

#include "test_game.h"
#include "events.h"
#include "events_interface.h"

using testing::test_game;

SCENARIO("a table is built and inserted into the level", "[decoration][station]"){
    GIVEN("a fresh game"){
        test_game game;
        WHEN("a table is built and inserted on the stations layer"){
            // build_main_level pre-populates ids 0-6, so use a fresh id here.
            const int table_id = 200;
            const Vector2 position{level_config::edge_weight * 8, level_config::edge_weight * 8};
            game.insert_entity(game.build_table(table_id, position), level_config::draw_layers::stations);
            THEN("the level contains a table with that id"){
                REQUIRE(game.find_entity(table_id) != nullptr);
            }
        }
    }
}

SCENARIO("a food counter is built and inserted into the level", "[decoration][station]"){
    GIVEN("a fresh game"){
        test_game game;
        WHEN("a food counter is built and inserted on the stations layer"){
            const int counter_id = 201;
            const Vector2 position{level_config::edge_weight * 10, level_config::edge_weight * 8};
            game.insert_entity(game.build_food_counter(counter_id, position), level_config::draw_layers::stations);
            THEN("the level contains a food counter with that id"){
                REQUIRE(game.find_entity(counter_id) != nullptr);
            }
        }
    }
}

SCENARIO("a station computes flanking interaction positions from its position",
        "[decoration][station][interaction]"){
    // Interaction positions are centralised on `station`, so a table and a food
    // counter share the same geometry (left = one tile left, right = two tiles
    // right). This is the fix for the waiter beelining to the world origin: the
    // counter used to publish {0,0} for its interaction position.
    test_game game;

    GIVEN("a table built at a known position"){
        const Vector2 position{level_config::edge_weight * 8, level_config::edge_weight * 8};
        auto table_entity = game.build_table(210, position);
        auto* station = dynamic_cast<entities::station*>(table_entity.get());
        REQUIRE(station != nullptr);
        THEN("its interaction positions flank the table"){
            const auto interaction = station->get_interaction_positions();
            REQUIRE(interaction.left.x == position.x - level_config::edge_weight);
            REQUIRE(interaction.left.y == position.y);
            REQUIRE(interaction.right.x == position.x + (2.0f * level_config::edge_weight));
            REQUIRE(interaction.right.y == position.y);
        }
    }

    GIVEN("a food counter built at a known position"){
        const Vector2 position{level_config::edge_weight * 18, level_config::edge_weight * 6};
        auto counter_entity = game.build_food_counter(211, position);
        auto* station = dynamic_cast<entities::station*>(counter_entity.get());
        REQUIRE(station != nullptr);
        THEN("its interaction positions flank the counter and are not the world origin"){
            const auto interaction = station->get_interaction_positions();
            REQUIRE(interaction.left.x == position.x - level_config::edge_weight);
            REQUIRE(interaction.left.y == position.y);
            REQUIRE(interaction.right.x == position.x + (2.0f * level_config::edge_weight));
            REQUIRE(interaction.right.y == position.y);
            REQUIRE_FALSE((interaction.left.x == 0.0f and interaction.left.y == 0.0f));
        }
    }
}

SCENARIO("a decoration is removed from the level via remove_entity event",
        "[decoration][station][remove]"){
    GIVEN("a fresh game with a table inserted"){
        test_game game;
        const int table_id = 202;
        const Vector2 position{level_config::edge_weight * 12, level_config::edge_weight * 12};

        const int baseline_count = game.num_entities();
        game.insert_entity(game.build_table(table_id, position),
                           level_config::draw_layers::stations);

        // Pre-condition: table is present and count grew by exactly 1.
        REQUIRE(game.num_entities() == baseline_count + 1);
        REQUIRE(game.find_entity(table_id) != nullptr);

        WHEN("a remove_entity event is executed for the table"){
            events::remove_entity remove{static_cast<size_t>(table_id)};
            event_interface::execute_event(remove);

            THEN("the count returns to baseline and the table is no longer found"){
                REQUIRE(game.num_entities() == baseline_count);
                REQUIRE(game.find_entity(table_id) == nullptr);
            }
        }
    }
}
