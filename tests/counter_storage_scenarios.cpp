#include <catch2/catch_test_macros.hpp>

#include "ecs_test_game.h"
#include "component.h"
#include "config.h"

namespace {
    const Vector2 counter_spot{level_config::edge_weight * 8, level_config::edge_weight * 8};

    components::renderable_component::sprite_component* food_slot(size_t counter_id){
        auto* renderable = component_managers::renderable_manager_.get_component(counter_id);
        return renderable == nullptr
            ? nullptr
            : renderable->get_sprite_component(entity_config::counter_sprite_slots::counter_food);
    }
    size_t slot_count(size_t counter_id){
        auto* renderable = component_managers::renderable_manager_.get_component(counter_id);
        return renderable == nullptr ? 0 : renderable->num_sprite_components();
    }
    components::storage_component* storage_of(size_t counter_id){
        return component_managers::storage_manager_.get_component(counter_id);
    }
    void push(size_t counter_id, size_t food_id){
        component_helpers::add_stored_item(counter_id,
            entity_config::counter_sprite_slots::counter_food, food_id);
    }
    std::optional<size_t> pop(size_t counter_id){
        return component_helpers::take_stored_item(counter_id,
            entity_config::counter_sprite_slots::counter_food);
    }
}

SCENARIO("a counter is built empty with only its body sprite", "[counter][storage][construction]"){
    GIVEN("a fresh ecs world"){
        testing::ecs_test_game game;

        WHEN("a food counter is built"){
            auto counter_id = game.create_food_counter(counter_spot);

            THEN("it carries a storage component"){
                REQUIRE(game.has_storage(counter_id));
            }
            THEN("its storage starts empty"){
                REQUIRE(storage_of(counter_id)->empty());
                REQUIRE(storage_of(counter_id)->size() == 0);
            }
            THEN("only the body sprite slot exists"){
                REQUIRE(slot_count(counter_id) == 1);
                REQUIRE(food_slot(counter_id) == nullptr);
            }
            THEN("the body slot is at index counter_body"){
                auto* renderable = component_managers::renderable_manager_.get_component(counter_id);
                REQUIRE(renderable->get_sprite_component(
                    entity_config::counter_sprite_slots::counter_body) != nullptr);
            }
        }
    }
}

SCENARIO("pushing food onto an empty counter creates the food slot", "[counter][storage][push]"){
    GIVEN("an empty counter"){
        testing::ecs_test_game game;
        auto counter_id = game.create_food_counter(counter_spot);
        REQUIRE(slot_count(counter_id) == 1);

        WHEN("a lasagna is pushed"){
            push(counter_id, entity_config::foods::lasagna);

            THEN("the stack holds one item"){
                REQUIRE_FALSE(storage_of(counter_id)->empty());
                REQUIRE(storage_of(counter_id)->size() == 1);
                REQUIRE(storage_of(counter_id)->head().get_count() == 1);
            }
            THEN("the food slot is created at counter_food"){
                REQUIRE(slot_count(counter_id) == 2);
                REQUIRE(food_slot(counter_id) != nullptr);
            }
            THEN("the food slot renders the lasagna sprite"){
                REQUIRE(food_slot(counter_id)->get_sprite_index()
                    == entity_config::foods::lasagna);
            }
            THEN("the food sprite carries the counter's draw offset"){
                auto offset = food_slot(counter_id)->get_sprite().get_draw_position_offset();
                REQUIRE(offset.x == entity_config::food_draw_offset.x);
                REQUIRE(offset.y == entity_config::food_draw_offset.y);
            }
            THEN("the body slot is untouched"){
                auto* renderable = component_managers::renderable_manager_.get_component(counter_id);
                REQUIRE(renderable->get_sprite_component(
                    entity_config::counter_sprite_slots::counter_body)->get_sprite_index() == 0);
            }
        }
    }
}

SCENARIO("pushing onto a counter whose head is the same item stacks the count",
    "[counter][storage][push]"){
    GIVEN("a counter holding one lasagna"){
        testing::ecs_test_game game;
        auto counter_id = game.create_food_counter(counter_spot);
        push(counter_id, entity_config::foods::lasagna);
        REQUIRE(storage_of(counter_id)->size() == 1);

        WHEN("a second lasagna is pushed"){
            push(counter_id, entity_config::foods::lasagna);

            THEN("the stack still holds one item, with a count of two"){
                REQUIRE(storage_of(counter_id)->size() == 1);
                REQUIRE(storage_of(counter_id)->head().get_count() == 2);
            }
            THEN("no new sprite slot is added"){
                REQUIRE(slot_count(counter_id) == 2);
            }
            THEN("the rendered sprite is still lasagna"){
                REQUIRE(food_slot(counter_id)->get_sprite_index()
                    == entity_config::foods::lasagna);
            }
        }
    }
}

SCENARIO("pushing onto a counter whose head is a different item swaps the sprite",
    "[counter][storage][push]"){
    GIVEN("a counter holding one lasagna"){
        testing::ecs_test_game game;
        auto counter_id = game.create_food_counter(counter_spot);
        push(counter_id, entity_config::foods::lasagna);
        REQUIRE(food_slot(counter_id)->get_sprite_index() == entity_config::foods::lasagna);

        WHEN("a coffee is pushed on top"){
            push(counter_id, entity_config::foods::coffee);

            THEN("the stack holds two items"){
                REQUIRE(storage_of(counter_id)->size() == 2);
                REQUIRE(storage_of(counter_id)->head().get_id() == entity_config::foods::coffee);
                REQUIRE(storage_of(counter_id)->head().get_count() == 1);
            }
            THEN("the slot count is unchanged - the index moves, not the slot"){
                REQUIRE(slot_count(counter_id) == 2);
            }
            THEN("the food slot renders coffee"){
                REQUIRE(food_slot(counter_id)->get_sprite_index()
                    == entity_config::foods::coffee);
            }
            THEN("the draw offset survives the swap"){
                auto offset = food_slot(counter_id)->get_sprite().get_draw_position_offset();
                REQUIRE(offset.x == entity_config::food_draw_offset.x);
                REQUIRE(offset.y == entity_config::food_draw_offset.y);
            }
        }
    }
}

SCENARIO("popping a counter whose count does not reach zero keeps the item",
    "[counter][storage][pop]"){
    GIVEN("a counter holding two lasagnas as one stacked item"){
        testing::ecs_test_game game;
        auto counter_id = game.create_food_counter(counter_spot);
        push(counter_id, entity_config::foods::lasagna);
        push(counter_id, entity_config::foods::lasagna);
        REQUIRE(storage_of(counter_id)->head().get_count() == 2);

        WHEN("one is taken"){
            auto taken = pop(counter_id);

            THEN("the take reports the lasagna"){
                REQUIRE(taken.has_value());
                REQUIRE(taken.value() == entity_config::foods::lasagna);
            }
            THEN("the item survives with a decremented count"){
                REQUIRE_FALSE(storage_of(counter_id)->empty());
                REQUIRE(storage_of(counter_id)->size() == 1);
                REQUIRE(storage_of(counter_id)->head().get_count() == 1);
            }
            THEN("the food slot is kept, still rendering lasagna"){
                REQUIRE(slot_count(counter_id) == 2);
                REQUIRE(food_slot(counter_id)->get_sprite_index()
                    == entity_config::foods::lasagna);
            }
        }
    }
}

SCENARIO("popping the last of an item drops to the item beneath it",
    "[counter][storage][pop]"){
    GIVEN("a counter holding a lasagna with a coffee on top"){
        testing::ecs_test_game game;
        auto counter_id = game.create_food_counter(counter_spot);
        push(counter_id, entity_config::foods::lasagna);
        push(counter_id, entity_config::foods::coffee);
        REQUIRE(storage_of(counter_id)->size() == 2);

        WHEN("the coffee is taken"){
            auto taken = pop(counter_id);

            THEN("the take reports the coffee"){
                REQUIRE(taken.has_value());
                REQUIRE(taken.value() == entity_config::foods::coffee);
            }
            THEN("the coffee item is popped, leaving the lasagna"){
                REQUIRE(storage_of(counter_id)->size() == 1);
                REQUIRE(storage_of(counter_id)->head().get_id() == entity_config::foods::lasagna);
            }
            THEN("the food slot is kept and swaps back to lasagna"){
                REQUIRE(slot_count(counter_id) == 2);
                REQUIRE(food_slot(counter_id)->get_sprite_index()
                    == entity_config::foods::lasagna);
            }
        }
    }
}

SCENARIO("popping a counter empty removes the food slot", "[counter][storage][pop]"){
    GIVEN("a counter holding a single lasagna"){
        testing::ecs_test_game game;
        auto counter_id = game.create_food_counter(counter_spot);
        push(counter_id, entity_config::foods::lasagna);
        REQUIRE(slot_count(counter_id) == 2);

        WHEN("it is taken"){
            auto taken = pop(counter_id);

            THEN("the take reports the lasagna"){
                REQUIRE(taken.has_value());
                REQUIRE(taken.value() == entity_config::foods::lasagna);
            }
            THEN("the storage is empty"){
                REQUIRE(storage_of(counter_id)->empty());
                REQUIRE(storage_of(counter_id)->size() == 0);
            }
            THEN("the food slot is removed, leaving the body slot"){
                REQUIRE(slot_count(counter_id) == 1);
                REQUIRE(food_slot(counter_id) == nullptr);
            }
            AND_WHEN("food is pushed again"){
                push(counter_id, entity_config::foods::coffee);

                THEN("the slot is rebuilt at counter_food"){
                    REQUIRE(slot_count(counter_id) == 2);
                    REQUIRE(food_slot(counter_id) != nullptr);
                    REQUIRE(food_slot(counter_id)->get_sprite_index()
                        == entity_config::foods::coffee);
                }
                THEN("the rebuilt sprite still carries the draw offset"){
                    auto offset = food_slot(counter_id)->get_sprite().get_draw_position_offset();
                    REQUIRE(offset.x == entity_config::food_draw_offset.x);
                    REQUIRE(offset.y == entity_config::food_draw_offset.y);
                }
            }
        }
    }
}

SCENARIO("popping an empty counter reports nothing and changes nothing",
    "[counter][storage][pop]"){
    GIVEN("an empty counter"){
        testing::ecs_test_game game;
        auto counter_id = game.create_food_counter(counter_spot);
        REQUIRE(storage_of(counter_id)->empty());

        WHEN("a take is attempted"){
            auto taken = pop(counter_id);

            THEN("nothing is reported"){
                REQUIRE_FALSE(taken.has_value());
            }
            THEN("the storage is still empty"){
                REQUIRE(storage_of(counter_id)->empty());
                REQUIRE(storage_of(counter_id)->size() == 0);
            }
            THEN("no food slot appears"){
                REQUIRE(slot_count(counter_id) == 1);
                REQUIRE(food_slot(counter_id) == nullptr);
            }
        }
    }
}

SCENARIO("a moved counter carries its stored food with it", "[counter][storage][movement]"){
    GIVEN("a counter holding a lasagna"){
        testing::ecs_test_game game;
        auto counter_id = game.create_food_counter(counter_spot);
        push(counter_id, entity_config::foods::lasagna);

        auto* position = component_managers::positional_manager_.get_component(counter_id);
        REQUIRE(position != nullptr);
        REQUIRE(position->get_position().x == counter_spot.x);

        const Vector2 destination{level_config::edge_weight * 14, level_config::edge_weight * 11};

        WHEN("the counter is moved"){
            game.move_entity(counter_id, destination);

            THEN("the position component follows"){
                REQUIRE(position->get_position().x == destination.x);
                REQUIRE(position->get_position().y == destination.y);
            }
            THEN("the hitbox follows"){
                auto box = game.hitbox_of(counter_id);
                REQUIRE(box.x == destination.x);
                REQUIRE(box.y == destination.y);
            }
            THEN("the food slot is still present and unchanged"){
                REQUIRE(slot_count(counter_id) == 2);
                REQUIRE(food_slot(counter_id)->get_sprite_index()
                    == entity_config::foods::lasagna);
            }
            THEN("the food's draw offset is unchanged, so it draws relative to the new position"){
                auto offset = food_slot(counter_id)->get_sprite().get_draw_position_offset();
                REQUIRE(offset.x == entity_config::food_draw_offset.x);
                REQUIRE(offset.y == entity_config::food_draw_offset.y);
            }
        }
    }
}

SCENARIO("an item id with no sprite is stored but not drawn", "[counter][storage][push]"){
    GIVEN("an empty counter"){
        testing::ecs_test_game game;
        auto counter_id = game.create_food_counter(counter_spot);

        WHEN("an item id past the end of the food sprite list is pushed"){
            push(counter_id, entity_config::foods::foods_size + 5);

            THEN("the item is still stored"){
                REQUIRE_FALSE(storage_of(counter_id)->empty());
                REQUIRE(storage_of(counter_id)->size() == 1);
            }
            THEN("no food slot is created"){
                REQUIRE(slot_count(counter_id) == 1);
                REQUIRE(food_slot(counter_id) == nullptr);
            }
            AND_WHEN("a real food is pushed on top"){
                push(counter_id, entity_config::foods::coffee);

                THEN("the slot appears at counter_food and renders coffee"){
                    REQUIRE(slot_count(counter_id) == 2);
                    REQUIRE(food_slot(counter_id) != nullptr);
                    REQUIRE(food_slot(counter_id)->get_sprite_index()
                        == entity_config::foods::coffee);
                }
            }
        }
    }
}
