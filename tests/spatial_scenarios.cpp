#include <catch2/catch_test_macros.hpp>

#include "ecs_test_game.h"
#include "component.h"
#include "raymath.h"

// the world is 2048x2048 over MAX_DEPTH 4, so the nodes holding a 25x25 cursor
// hitbox are 128x128. positions below are chosen against that grid:
//   (100,100) -> depth 4, node [0,0]-[128,128]
//   (102,102) -> same node, no reindex needed
//   (150,50)  -> depth 4, node [128,0]-[256,128]   sibling
//   (120,120) -> depth 3, node [0,0]-[256,256]     straddles x=128 and y=128
//   (1000,1000) -> depth 0, straddles the world centre
namespace {
    bool same_node(const raglib::bounding_box_2& a, const raglib::bounding_box_2& b){
        return Vector2Equals(a.min, b.min) and Vector2Equals(a.max, b.max);
    }
    raglib::bounding_box_2 node_of(testing::ecs_test_game& game, size_t entity_id){
        raglib::bounding_box_2 bounds{};
        REQUIRE(game.node_bounds_of(entity_id, bounds));
        return bounds;
    }
}

SCENARIO("the spatial index follows an entity's lifetime", "[ecs][spatial]"){
    GIVEN("a fresh ecs world"){
        testing::ecs_test_game game;

        THEN("the tree starts empty"){
            REQUIRE(game.tracked_count() == 0);
        }

        WHEN("a cursor is created"){
            auto cursor_id = game.create_cursor();

            THEN("the create event put it in the tree"){
                REQUIRE(game.is_tracked(cursor_id));
                REQUIRE(game.tracked_count() == 1);
            }
            THEN("it sits in a real node"){
                REQUIRE(game.node_depth_of(cursor_id) >= 0);
            }
        }

        WHEN("an entity with no collision component is created"){
            auto entity_id = game.create_renderable(level_config::draw_layers::decoration);

            THEN("it is not spatial, so the tree ignores it"){
                REQUIRE_FALSE(game.is_tracked(entity_id));
                REQUIRE(game.tracked_count() == 0);
                REQUIRE(game.node_depth_of(entity_id) == -1);
            }
        }
    }

    GIVEN("a world with a tracked cursor"){
        testing::ecs_test_game game;
        auto cursor_id = game.create_cursor();
        REQUIRE(game.is_tracked(cursor_id));

        WHEN("it is removed"){
            game.remove(cursor_id);

            THEN("the remove event took it out of the tree"){
                REQUIRE_FALSE(game.is_tracked(cursor_id));
                REQUIRE(game.tracked_count() == 0);
                REQUIRE(game.node_depth_of(cursor_id) == -1);
            }
        }
    }
}

SCENARIO("update_position carries the hitbox with the position", "[ecs][spatial][movement]"){
    GIVEN("a world with a cursor"){
        testing::ecs_test_game game;
        auto cursor_id = game.create_cursor();

        auto original = game.hitbox_of(cursor_id);

        WHEN("its position is updated"){
            game.move_entity(cursor_id, Vector2{300.0f, 300.0f});

            THEN("the hitbox origin followed the position"){
                auto box = game.hitbox_of(cursor_id);
                REQUIRE(box.x == 300.0f);
                REQUIRE(box.y == 300.0f);
            }
            THEN("only the origin moved, not the extent"){
                auto box = game.hitbox_of(cursor_id);
                REQUIRE(box.width == original.width);
                REQUIRE(box.height == original.height);
            }
            THEN("the position component agrees with the hitbox"){
                auto* position = component_managers::positional_manager_.get_component(cursor_id);
                REQUIRE(position->get_position().x == 300.0f);
                REQUIRE(position->get_position().y == 300.0f);
            }
        }
    }
}

SCENARIO("a move keeps the entity in one node, reindexing only when it must", "[ecs][spatial]"){
    GIVEN("a cursor sitting in the [0,0]-[128,128] node at depth 4"){
        testing::ecs_test_game game;
        auto cursor_id = game.create_cursor();
        game.move_entity(cursor_id, Vector2{100.0f, 100.0f});

        REQUIRE(game.node_depth_of(cursor_id) == 4);
        auto start_node = node_of(game, cursor_id);

        WHEN("it moves but stays inside the same node"){
            game.move_entity(cursor_id, Vector2{102.0f, 102.0f});

            THEN("it did not change node"){
                REQUIRE(game.node_depth_of(cursor_id) == 4);
                REQUIRE(same_node(node_of(game, cursor_id), start_node));
            }
            THEN("it is still held exactly once"){
                REQUIRE(game.tracked_count() == 1);
            }
        }

        WHEN("it moves to a sibling node at the same depth"){
            game.move_entity(cursor_id, Vector2{150.0f, 50.0f});

            THEN("the depth is unchanged but the node is not"){
                REQUIRE(game.node_depth_of(cursor_id) == 4);
                REQUIRE_FALSE(same_node(node_of(game, cursor_id), start_node));
            }
            THEN("it is still held exactly once"){
                REQUIRE(game.tracked_count() == 1);
            }
        }

        WHEN("it moves onto a node boundary it now straddles"){
            game.move_entity(cursor_id, Vector2{120.0f, 120.0f});

            THEN("it is promoted to the shallower node that still contains it"){
                REQUIRE(game.node_depth_of(cursor_id) == 3);
                REQUIRE_FALSE(same_node(node_of(game, cursor_id), start_node));
            }
            THEN("it is still held exactly once"){
                REQUIRE(game.tracked_count() == 1);
            }
        }

        WHEN("it moves onto the world centre"){
            game.move_entity(cursor_id, Vector2{1000.0f, 1000.0f});

            THEN("straddling the root's centre parks it at the root"){
                REQUIRE(game.node_depth_of(cursor_id) == 0);
                REQUIRE(game.tracked_count() == 1);
            }
        }
    }

    GIVEN("a cursor promoted to depth 3 by straddling a boundary"){
        testing::ecs_test_game game;
        auto cursor_id = game.create_cursor();
        game.move_entity(cursor_id, Vector2{120.0f, 120.0f});

        REQUIRE(game.node_depth_of(cursor_id) == 3);

        WHEN("it moves clear of the boundary"){
            game.move_entity(cursor_id, Vector2{100.0f, 100.0f});

            THEN("it sinks to the deepest node that contains it"){
                REQUIRE(game.node_depth_of(cursor_id) == 4);
            }
            THEN("it is still held exactly once"){
                REQUIRE(game.tracked_count() == 1);
            }
        }
    }
}
