#include <catch2/catch_test_macros.hpp>

#include "ecs_test_game.h"
#include "component.h"

SCENARIO("creating an entity allocates an id and announces it", "[ecs][lifespan]"){
    GIVEN("a fresh ecs world"){
        testing::ecs_test_game game;

        THEN("it starts with no components and empty layers"){
            REQUIRE(game.total_components() == 0);
            REQUIRE(game.layer_size(level_config::draw_layers::dogs) == 0);
        }

        WHEN("an entity is created"){
            auto entity_id = game.create_empty(level_config::draw_layers::dogs);

            THEN("it lands in the layer it was created on"){
                REQUIRE(game.layer_size(level_config::draw_layers::dogs) == 1);
                REQUIRE(game.layer_contains(level_config::draw_layers::dogs, entity_id));
            }
            THEN("no other layer picked it up"){
                REQUIRE(game.layer_size(level_config::draw_layers::stations) == 0);
                REQUIRE(game.layer_size(level_config::draw_layers::hud) == 0);
            }
        }

        WHEN("several entities are created"){
            auto first = game.create_empty(level_config::draw_layers::dogs);
            auto second = game.create_empty(level_config::draw_layers::dogs);
            auto third = game.create_empty(level_config::draw_layers::stations);

            THEN("each gets a distinct id"){
                REQUIRE(first != second);
                REQUIRE(second != third);
                REQUIRE(first != third);
            }
            THEN("they are distributed across the layers they asked for"){
                REQUIRE(game.layer_size(level_config::draw_layers::dogs) == 2);
                REQUIRE(game.layer_size(level_config::draw_layers::stations) == 1);
            }
        }
    }
}

SCENARIO("building an entity registers its components", "[ecs][components]"){
    GIVEN("a fresh ecs world"){
        testing::ecs_test_game game;

        WHEN("a renderable entity is built"){
            auto entity_id = game.create_renderable(level_config::draw_layers::decoration,
                                                    Vector2{64.0f, 128.0f});

            THEN("only the components its builder registered are present"){
                REQUIRE(game.has_position(entity_id));
                REQUIRE(game.has_renderable(entity_id));
                REQUIRE(game.num_components(entity_id) == 2);
            }
            THEN("the position component holds what it was built with"){
                auto* position = component_managers::positional_manager_.get_component(entity_id);
                REQUIRE(position != nullptr);
                REQUIRE(position->get_position().x == 64.0f);
                REQUIRE(position->get_position().y == 128.0f);
            }
        }

        WHEN("an entity is built with no components"){
            auto entity_id = game.create_empty(level_config::draw_layers::dogs);

            THEN("nothing is registered for it"){
                REQUIRE(game.num_components(entity_id) == 0);
                REQUIRE(game.total_components() == 0);
            }
        }

        WHEN("the player is built"){
            auto cursor_id = game.create_empty(level_config::draw_layers::cursor);
            auto player_id = game.create_player(cursor_id);

            THEN("it carries its controls component"){
                REQUIRE(game.has_controls(player_id));
            }
        }
    }
}

SCENARIO("removing an entity clears its components and drops it from the layers", "[ecs][lifespan]"){
    GIVEN("a world with a renderable entity"){
        testing::ecs_test_game game;
        auto entity_id = game.create_renderable(level_config::draw_layers::decoration);

        REQUIRE(game.num_components(entity_id) == 2);
        REQUIRE(game.layer_contains(level_config::draw_layers::decoration, entity_id));

        WHEN("it is removed"){
            game.remove(entity_id);

            THEN("every one of its components is gone"){
                REQUIRE(game.num_components(entity_id) == 0);
                REQUIRE(game.total_components() == 0);
            }
            THEN("it is gone from the render layer"){
                REQUIRE_FALSE(game.layer_contains(level_config::draw_layers::decoration, entity_id));
                REQUIRE(game.layer_size(level_config::draw_layers::decoration) == 0);
            }
        }
    }

    GIVEN("a world with several entities on one layer"){
        testing::ecs_test_game game;
        auto first = game.create_renderable(level_config::draw_layers::dogs);
        auto second = game.create_renderable(level_config::draw_layers::dogs);
        auto third = game.create_renderable(level_config::draw_layers::dogs);

        WHEN("the middle one is removed"){
            game.remove(second);

            THEN("only that one leaves the layer"){
                REQUIRE(game.layer_size(level_config::draw_layers::dogs) == 2);
                REQUIRE(game.layer_contains(level_config::draw_layers::dogs, first));
                REQUIRE_FALSE(game.layer_contains(level_config::draw_layers::dogs, second));
                REQUIRE(game.layer_contains(level_config::draw_layers::dogs, third));
            }
            THEN("the others keep their components"){
                REQUIRE(game.num_components(first) == 2);
                REQUIRE(game.num_components(third) == 2);
            }
        }
    }

    GIVEN("a world where an id has been recycled"){
        testing::ecs_test_game game;
        auto first = game.create_empty(level_config::draw_layers::dogs);
        game.remove(first);

        WHEN("another entity is created"){
            auto second = game.create_renderable(level_config::draw_layers::dogs);

            THEN("the freed id is reused"){
                REQUIRE(second == first);
            }
            THEN("it carries only its own components, not the dead entity's"){
                REQUIRE(game.num_components(second) == 2);
            }
        }
    }
}

SCENARIO("the ecs world starts clean for every scenario", "[ecs][harness]"){
    GIVEN("a world that had entities in a previous scenario"){
        testing::ecs_test_game game;

        THEN("no components or layer entries survived"){
            REQUIRE(game.total_components() == 0);
            for(size_t layer = 0; layer < level_config::draw_layers::size; ++layer){
                REQUIRE(game.layer_size(layer) == 0);
            }
        }
    }
}
