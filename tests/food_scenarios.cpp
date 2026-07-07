// Scenarios for food: correct construction of a food entity via the builder,
// and FILO storage on a food_counter (store / take / status / capacity). These
// exercise the entity layer directly - no level/maitre_d/expediter needed - so a
// test_game is constructed only to provide the hidden window the builders need.
#include <catch2/catch_test_macros.hpp>

#include <string>

#include "test_game.h"

using testing::test_game;

SCENARIO("a food entity is built with the expected identity and body", "[food]"){
    GIVEN("a game (for the window the builders need)"){
        test_game game;
        WHEN("a test food is built"){
            const int food_id = 300;
            const Vector2 position{level_config::edge_weight * 3, level_config::edge_weight * 4};
            std::unique_ptr<entities::food> food = game.build_test_food(food_id, position);
            THEN("it carries the requested id/position and a food-prefixed debug id and 1-tile hitbox"){
                REQUIRE(food != nullptr);
                REQUIRE(food->get_id() == food_id);
                REQUIRE(food->get_debug_id().starts_with(entity_config::food_debug_id_prefix));
                REQUIRE(food->get_position().x == position.x);
                REQUIRE(food->get_position().y == position.y);
                const Rectangle box = food->get_hitbox().get_box();
                REQUIRE(box.width == entity_config::test_food_attributes[entity_config::attributes::frame_width]);
                REQUIRE(box.height == entity_config::test_food_attributes[entity_config::attributes::frame_height]);
            }
        }
    }
}

SCENARIO("a food counter is built empty with the configured capacity", "[food][counter]"){
    GIVEN("a freshly built food counter"){
        test_game game;
        auto counter_entity = game.build_food_counter(310, Vector2{level_config::edge_weight * 6, level_config::edge_weight * 6});
        auto* counter = dynamic_cast<entities::food_counter*>(counter_entity.get());
        REQUIRE(counter != nullptr);
        THEN("it starts empty at zero of its max capacity"){
            REQUIRE(counter->is_empty());
            REQUIRE(counter->current_capacity() == 0);
            REQUIRE(counter->max_capacity() == entity_config::food_counter_capacity);
            REQUIRE(counter->status() == entities::food_counter::empty);
        }
    }
}

SCENARIO("a food counter stores and serves food as a FILO stack", "[food][counter]"){
    GIVEN("an empty food counter and a food-builder"){
        test_game game;
        auto counter_entity = game.build_food_counter(320, Vector2{level_config::edge_weight * 6, level_config::edge_weight * 6});
        auto* counter = dynamic_cast<entities::food_counter*>(counter_entity.get());
        REQUIRE(counter != nullptr);
        const Vector2 food_pos{counter->get_position().x + entity_config::food_draw_offset.x,
                               counter->get_position().y + entity_config::food_draw_offset.y};

        WHEN("food is stored up to capacity"){
            REQUIRE(counter->store(game.build_test_food(100, food_pos)));
            REQUIRE(counter->current_capacity() == 1);
            REQUIRE(counter->status() == entities::food_counter::has_food);

            REQUIRE(counter->store(game.build_test_food(101, food_pos)));
            REQUIRE(counter->store(game.build_test_food(102, food_pos)));
            REQUIRE(counter->current_capacity() == counter->max_capacity());
            REQUIRE(counter->status() == entities::food_counter::full);

            THEN("storing past capacity is rejected without overfilling"){
                REQUIRE_FALSE(counter->store(game.build_test_food(103, food_pos)));
                REQUIRE(counter->current_capacity() == counter->max_capacity());
            }

            THEN("take() serves the most-recently-stored food first (FILO) until empty"){
                REQUIRE(counter->take()->get_id() == 102);
                REQUIRE(counter->current_capacity() == 2);
                REQUIRE(counter->status() == entities::food_counter::has_food);

                REQUIRE(counter->take()->get_id() == 101);
                REQUIRE(counter->take()->get_id() == 100);
                REQUIRE(counter->is_empty());
                REQUIRE(counter->status() == entities::food_counter::empty);
            }
        }
    }
}
