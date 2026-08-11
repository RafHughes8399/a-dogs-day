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

SCENARIO("building the cursor registers its components", "[ecs][components][cursor]"){
    GIVEN("a fresh ecs world"){
        testing::ecs_test_game game;

        WHEN("the cursor is built"){
            auto cursor_id = game.create_cursor();

            THEN("it carries exactly the four its builder registers"){
                REQUIRE(game.has_position(cursor_id));
                REQUIRE(game.has_mouse_input(cursor_id));
                REQUIRE(game.has_renderable(cursor_id));
                REQUIRE(game.has_collision(cursor_id));
                REQUIRE(game.num_components(cursor_id) == 4);
            }
            THEN("it takes no movement and no keyboard controls"){
                REQUIRE_FALSE(game.has_movement(cursor_id));
                REQUIRE_FALSE(game.has_controls(cursor_id));
            }
            THEN("it lands on the cursor layer"){
                REQUIRE(game.layer_contains(level_config::draw_layers::cursor, cursor_id));
            }
            THEN("it has one sprite slot and one hitbox variant, both at index 0"){
                auto* renderable = component_managers::renderable_manager_.get_component(cursor_id);
                REQUIRE(renderable != nullptr);
                REQUIRE(renderable->get_sprites().size() == 1);
                REQUIRE(renderable->get_sprites()[0].get_sprite_index() == 0);

                auto* collision = component_managers::collision_manager_.get_component(cursor_id);
                REQUIRE(collision != nullptr);
                REQUIRE(collision->get_hitbox_component().get_hitboxes().size() == 1);
                REQUIRE(collision->get_hitbox_component().get_hitbox_index() == 0);
            }
            THEN("its hitbox is the size cursor_attributes asks for"){
                auto* collision = component_managers::collision_manager_.get_component(cursor_id);
                auto box = collision->get_hitbox_component().get_hitbox().get_box();
                REQUIRE(box.width == entity_config::cursor_attributes[entity_config::attributes::frame_width]);
                REQUIRE(box.height == entity_config::cursor_attributes[entity_config::attributes::frame_height]);
            }
            THEN("it binds both mouse buttons"){
                auto* mouse = component_managers::mouse_input_manager_.get_component(cursor_id);
                REQUIRE(mouse != nullptr);
                REQUIRE(mouse->get_inputs().size() == game_config::cursor_controls.size());
            }
        }
    }

    GIVEN("a world where the player built the cursor"){
        testing::ecs_test_game game;
        auto cursor_id = game.create_empty(level_config::draw_layers::cursor);
        auto player_id = game.create_player(cursor_id);

        THEN("the cursor got its components through the player builder"){
            REQUIRE(cursor_id != player_id);
            REQUIRE(game.num_components(cursor_id) == 4);
            REQUIRE(game.has_mouse_input(cursor_id));
        }
        THEN("the keyboard controls went to the player, not the cursor"){
            REQUIRE(game.has_controls(player_id));
            REQUIRE_FALSE(game.has_controls(cursor_id));
        }
    }
}

SCENARIO("removing the cursor unregisters every component it held", "[ecs][lifespan][cursor]"){
    GIVEN("a world with a cursor"){
        testing::ecs_test_game game;
        auto cursor_id = game.create_cursor();

        REQUIRE(game.num_components(cursor_id) == 4);
        REQUIRE(game.total_components() == 4);

        WHEN("it is removed"){
            game.remove(cursor_id);

            THEN("all four managers dropped it"){
                REQUIRE_FALSE(game.has_position(cursor_id));
                REQUIRE_FALSE(game.has_mouse_input(cursor_id));
                REQUIRE_FALSE(game.has_renderable(cursor_id));
                REQUIRE_FALSE(game.has_collision(cursor_id));
                REQUIRE(game.num_components(cursor_id) == 0);
                REQUIRE(game.total_components() == 0);
            }
            THEN("it leaves the cursor layer"){
                REQUIRE_FALSE(game.layer_contains(level_config::draw_layers::cursor, cursor_id));
                REQUIRE(game.layer_size(level_config::draw_layers::cursor) == 0);
            }
        }
    }

    GIVEN("a world with a cursor and another entity"){
        testing::ecs_test_game game;
        auto cursor_id = game.create_cursor();
        auto other_id = game.create_renderable(level_config::draw_layers::decoration);

        WHEN("the cursor is removed"){
            game.remove(cursor_id);

            THEN("only the cursor's components go"){
                REQUIRE(game.num_components(cursor_id) == 0);
                REQUIRE(game.num_components(other_id) == 2);
                REQUIRE(game.total_components() == 2);
            }
        }
    }

    GIVEN("a world where the cursor's id has been recycled"){
        testing::ecs_test_game game;
        auto cursor_id = game.create_cursor();
        game.remove(cursor_id);

        WHEN("a bare entity takes the freed id"){
            auto reused_id = game.create_empty(level_config::draw_layers::dogs);

            THEN("it inherits none of the cursor's components"){
                REQUIRE(reused_id == cursor_id);
                REQUIRE(game.num_components(reused_id) == 0);
                REQUIRE(game.total_components() == 0);
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

SCENARIO("a facing with no matching sprite variant is ignored", "[ecs][components][facing]"){
    GIVEN("a player dog, which carries left and right variants only"){
        testing::ecs_test_game game;
        auto khiri_id = game.create_khiri();

        auto* renderable = component_managers::renderable_manager_.get_component(khiri_id);
        auto* collision = component_managers::collision_manager_.get_component(khiri_id);
        REQUIRE(renderable != nullptr);
        REQUIRE(collision != nullptr);
        REQUIRE(renderable->get_sprites().front().num_sprites() == 2);
        REQUIRE(collision->get_hitbox_component().num_hitboxes() == 2);

        component_helpers::set_facing_index(khiri_id, level_config::directions::left);
        REQUIRE(renderable->get_sprites().front().get_sprite_index()
            == level_config::directions::left);

        WHEN("it is told to face up, which it has no sprite for"){
            component_helpers::set_facing_index(khiri_id, level_config::directions::up);

            THEN("the sprite index is left where it was"){
                REQUIRE(renderable->get_sprites().front().get_sprite_index()
                    == level_config::directions::left);
            }
            THEN("the hitbox index is left where it was"){
                REQUIRE(collision->get_hitbox_component().get_hitbox_index()
                    == level_config::directions::left);
            }
            THEN("reading the sprite and hitbox stays in range"){
                REQUIRE(renderable->get_sprites().front().get_sprite_index()
                    < renderable->get_sprites().front().num_sprites());
                REQUIRE(collision->get_hitbox_component().get_hitbox_index()
                    < collision->get_hitbox_component().num_hitboxes());
            }
        }

        WHEN("it is told to face down, which it has no sprite for"){
            component_helpers::set_facing_index(khiri_id, level_config::directions::down);

            THEN("the facing is unchanged and still in range"){
                REQUIRE(renderable->get_sprites().front().get_sprite_index()
                    == level_config::directions::left);
                REQUIRE(collision->get_hitbox_component().get_hitbox_index()
                    == level_config::directions::left);
            }
        }

        WHEN("it is told to face right, which it does have"){
            component_helpers::set_facing_index(khiri_id, level_config::directions::right);

            THEN("the facing changes"){
                REQUIRE(renderable->get_sprites().front().get_sprite_index()
                    == level_config::directions::right);
                REQUIRE(collision->get_hitbox_component().get_hitbox_index()
                    == level_config::directions::right);
            }
        }
    }
}
