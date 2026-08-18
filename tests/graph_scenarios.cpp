#include <catch2/catch_test_macros.hpp>

#include "ecs_test_game.h"
#include "component.h"

SCENARIO("the level graph follows an entity's lifetime", "[ecs][graph][movement]"){
    GIVEN("a fresh ecs world"){
        testing::ecs_test_game game;

        THEN("no node is occupied"){
            REQUIRE(game.graph_occupied_node_count() == 0);
        }

        WHEN("a dog is created"){
            auto dog_id = game.create_khiri();
            auto footprint = game.hitbox_of(dog_id);

            THEN("the create event marked every node under its hitbox"){
                REQUIRE(game.graph_marks(dog_id, footprint));
            }
            THEN("it marks exactly the two nodes the hitbox spans"){
                REQUIRE(game.graph_occupied_node_count() == 2);
            }
            THEN("a node away from the dog is still empty"){
                REQUIRE(game.graph_occupant_at(Vector2{1024.0f, 1024.0f}) == graph_config::empty_node);
            }
        }

        WHEN("an entity with no collision component is created"){
            auto entity_id = game.create_renderable(level_config::draw_layers::decoration);

            THEN("it has no footprint, so it marks nothing"){
                REQUIRE(game.graph_occupied_node_count() == 0);
                REQUIRE_FALSE(game.has_collision(entity_id));
            }
        }
    }

    GIVEN("a world with one dog"){
        testing::ecs_test_game game;
        auto dog_id = game.create_khiri();
        auto spawn_footprint = game.hitbox_of(dog_id);
        REQUIRE(game.graph_marks(dog_id, spawn_footprint));

        WHEN("the dog moves clear of its old nodes"){
            // eight cells right, so the pre- and post-move footprints do not touch
            Vector2 destination{spawn_footprint.x + (level_config::edge_weight * 8.0f), spawn_footprint.y};
            game.move_entity(dog_id, destination);

            THEN("the nodes it left read empty"){
                REQUIRE(game.graph_occupant_at(Vector2{spawn_footprint.x, spawn_footprint.y}) == graph_config::empty_node);
            }
            THEN("the nodes it arrived at carry its id"){
                REQUIRE(game.graph_marks(dog_id, game.hitbox_of(dog_id)));
            }
            THEN("it occupies no more nodes than it did before"){
                REQUIRE(game.graph_occupied_node_count() == 2);
            }
        }

        WHEN("the dog moves less than one cell, so the footprints overlap"){
            Vector2 destination{spawn_footprint.x + (level_config::edge_weight * 0.5f), spawn_footprint.y};
            game.move_entity(dog_id, destination);

            THEN("the overlapping node is still marked - the clear ran before the mark"){
                REQUIRE(game.graph_marks(dog_id, game.hitbox_of(dog_id)));
            }
        }

        WHEN("the dog is destroyed"){
            game.remove(dog_id);

            THEN("every node it held is released"){
                REQUIRE(game.graph_occupied_node_count() == 0);
                REQUIRE(game.graph_occupant_at(Vector2{spawn_footprint.x, spawn_footprint.y}) == graph_config::empty_node);
            }
        }
    }

    GIVEN("a world with two dogs"){
        testing::ecs_test_game game;
        auto khiri_id = game.create_khiri();
        auto mack_id = game.create_mack();

        THEN("each dog's nodes carry its own id"){
            REQUIRE(game.graph_marks(khiri_id, game.hitbox_of(khiri_id)));
            REQUIRE(game.graph_marks(mack_id, game.hitbox_of(mack_id)));
        }

        WHEN("one is destroyed"){
            auto mack_footprint = game.hitbox_of(mack_id);
            game.remove(mack_id);

            THEN("only its nodes are released"){
                REQUIRE(game.graph_occupant_at(Vector2{mack_footprint.x, mack_footprint.y}) == graph_config::empty_node);
                REQUIRE(game.graph_marks(khiri_id, game.hitbox_of(khiri_id)));
            }
        }
    }
}

SCENARIO("the cursor is spatial but never occupies the graph", "[ecs][graph][movement]"){
    GIVEN("a fresh ecs world"){
        testing::ecs_test_game game;

        WHEN("a cursor is created"){
            auto cursor_id = game.create_cursor();

            THEN("it has a hitbox and the quadtree tracks it"){
                REQUIRE(game.has_collision(cursor_id));
                REQUIRE(game.is_tracked(cursor_id));
            }
            THEN("it holds a mouse input component, which is what exempts it"){
                REQUIRE(game.has_mouse_input(cursor_id));
            }
            THEN("it marks no node"){
                REQUIRE(game.graph_occupied_node_count() == 0);
            }

            AND_WHEN("it moves"){
                game.move_entity(cursor_id, Vector2{512.0f, 512.0f});

                THEN("it still marks no node"){
                    REQUIRE(game.graph_occupied_node_count() == 0);
                }
                THEN("the quadtree still followed it"){
                    REQUIRE(game.is_tracked(cursor_id));
                }
            }

            AND_WHEN("it is destroyed"){
                game.remove(cursor_id);

                THEN("nothing was left behind in the graph"){
                    REQUIRE(game.graph_occupied_node_count() == 0);
                }
            }
        }

        WHEN("a cursor and a dog share the world"){
            auto cursor_id = game.create_cursor();
            auto dog_id = game.create_khiri();

            THEN("only the dog is in the graph"){
                REQUIRE(game.graph_occupied_node_count() == 2);
                REQUIRE(game.graph_marks(dog_id, game.hitbox_of(dog_id)));
            }
            THEN("both are in the quadtree"){
                REQUIRE(game.is_tracked(cursor_id));
                REQUIRE(game.is_tracked(dog_id));
            }
        }
    }
}

SCENARIO("clear_all_systems empties the graph between scenarios", "[ecs][graph][movement]"){
    GIVEN("a world that had a dog in it"){
        {
            testing::ecs_test_game game;
            auto dog_id = game.create_khiri();
            REQUIRE(game.graph_occupied_node_count() == 2);
            (void) dog_id;
        }

        WHEN("a new world is built"){
            testing::ecs_test_game game;

            THEN("the graph starts empty"){
                REQUIRE(game.graph_occupied_node_count() == 0);
            }
        }
    }
}

SCENARIO("the graph rejects positions outside the world", "[ecs][graph][movement]"){
    GIVEN("a world with one dog"){
        testing::ecs_test_game game;
        auto dog_id = game.create_mack();
        REQUIRE(game.graph_occupied_node_count() == 2);

        WHEN("the dog is moved clear of the right edge"){
            game.move_entity(dog_id, Vector2{level_config::world_x, 0.0f});

            THEN("it marks nothing"){
                REQUIRE(game.graph_occupied_node_count() == 0);
            }
            THEN("its column does not fold into the next row down"){
                REQUIRE(game.graph_occupant_at(Vector2{0.0f, level_config::edge_weight})
                    == graph_config::empty_node);
                REQUIRE(game.graph_occupant_at(Vector2{level_config::edge_weight, level_config::edge_weight})
                    == graph_config::empty_node);
            }
        }

        WHEN("the dog straddles the right edge"){
            game.move_entity(dog_id, Vector2{level_config::world_x - level_config::edge_weight, 0.0f});

            THEN("only the cell still on the map is marked"){
                REQUIRE(game.graph_occupied_node_count() == 1);
                REQUIRE(game.graph_occupant_at(Vector2{level_config::world_x - level_config::edge_weight, 0.0f})
                    == static_cast<int>(dog_id));
            }
        }

        WHEN("the dog is moved clear of the bottom edge"){
            game.move_entity(dog_id, Vector2{0.0f, level_config::world_y});

            THEN("it marks nothing"){
                REQUIRE(game.graph_occupied_node_count() == 0);
            }
        }

        WHEN("the dog is moved to a negative position"){
            game.move_entity(dog_id, Vector2{-level_config::edge_weight * 4.0f,
                                             -level_config::edge_weight * 4.0f});

            THEN("it marks nothing rather than folding onto the origin"){
                REQUIRE(game.graph_occupied_node_count() == 0);
                REQUIRE(game.graph_occupant_at(Vector2{0.0f, 0.0f}) == graph_config::empty_node);
            }
        }
    }
}

SCENARIO("a path destination resolves to the nearest node", "[ecs][graph][movement]"){
    GIVEN("a dog and an off-grid destination nearer the row above"){
        testing::ecs_test_game game;
        auto dog_id = game.create_mack();

        Vector2 destination{level_config::edge_weight * 10.2f, level_config::edge_weight * 3.4f};
        Vector2 nearest{level_config::edge_weight * 10.0f, level_config::edge_weight * 3.0f};

        WHEN("a right-facing dog is sent there"){
            game.path_to(dog_id, destination);
            REQUIRE(game.tick_until([&]{ return not game.has_path(dog_id); }, 4000));

            THEN("it finishes on the nearest node, not the cell corner"){
                REQUIRE(game.hitbox_of(dog_id).x == nearest.x);
                REQUIRE(game.hitbox_of(dog_id).y == nearest.y);
            }
        }

        WHEN("a left-facing dog is sent there"){
            game.path_to(dog_id, Vector2{0.0f, 0.0f});
            REQUIRE(game.tick_until([&]{ return not game.has_path(dog_id); }, 4000));
            REQUIRE(game.facing_x_of(dog_id) < 0.0f);

            game.path_to(dog_id, destination);
            REQUIRE(game.tick_until([&]{ return not game.has_path(dog_id); }, 4000));

            THEN("it finishes on the same node - facing no longer moves the target"){
                REQUIRE(game.hitbox_of(dog_id).x == nearest.x);
                REQUIRE(game.hitbox_of(dog_id).y == nearest.y);
            }
        }
    }

    GIVEN("a dog and a destination exactly between two rows"){
        testing::ecs_test_game game;
        auto dog_id = game.create_mack();

        WHEN("it is sent to a position on the tie"){
            game.path_to(dog_id, Vector2{level_config::edge_weight * 10.0f,
                                         level_config::edge_weight * 3.5f});
            REQUIRE(game.tick_until([&]{ return not game.has_path(dog_id); }, 4000));

            THEN("the tie breaks away from zero, onto the row below"){
                REQUIRE(game.hitbox_of(dog_id).y == level_config::edge_weight * 4.0f);
            }
        }
    }

    GIVEN("a dog and a destination in the last half cell of the world"){
        testing::ecs_test_game game;
        auto dog_id = game.create_mack();

        WHEN("it is sent to a position that rounds past the far edge"){
            game.path_to(dog_id, Vector2{level_config::world_x - 1.0f, level_config::edge_weight * 4.0f});
            REQUIRE(game.tick_until([&]{ return not game.has_path(dog_id); }, 8000));

            THEN("the round is clamped back onto the map rather than rejected"){
                REQUIRE(game.hitbox_of(dog_id).x == level_config::world_x - level_config::edge_weight);
            }
        }

        WHEN("it is sent to a position genuinely off the map"){
            game.path_to(dog_id, Vector2{level_config::world_x * 2.0f, 0.0f});

            THEN("no path is created"){
                REQUIRE_FALSE(game.has_path(dog_id));
            }
        }
    }
}

SCENARIO("a path created with a destination entity resolves to one of its interaction offsets",
        "[ecs][graph][movement][interaction]"){
    GIVEN("a dog approaching a table from the left"){
        testing::ecs_test_game game;
        Vector2 table_position{level_config::edge_weight * 12.0f, level_config::edge_weight * 12.0f};
        auto table_id = game.create_table(table_position);
        auto dog_id = game.create_mack(); // mack_start is left of the table on x

        WHEN("the dog is sent to the table by entity id"){
            game.path_to(dog_id, table_position, table_id);
            REQUIRE(game.tick_until([&]{ return not game.has_path(dog_id); }, 4000));

            THEN("it finishes on the table's left interaction offset, not the table's own position"){
                auto final_footprint = game.hitbox_of(dog_id);
                auto left = Vector2Add(table_position, entity_config::station_slot_left);
                REQUIRE(final_footprint.x == left.x);
                REQUIRE(final_footprint.y == left.y);
            }
        }
    }

    GIVEN("a dog and a destination entity with no interactable component"){
        testing::ecs_test_game game;
        Vector2 decoration_position{level_config::edge_weight * 12.0f, level_config::edge_weight * 12.0f};
        auto decoration_id = game.create_test_decoration(decoration_position);
        auto dog_id = game.create_mack();

        WHEN("the dog is sent to it by entity id"){
            game.path_to(dog_id, decoration_position, decoration_id);

            THEN("no path is created - the decoration's own footprint blocks the destination node, "
                 "and it offers no offset to route around it"){
                REQUIRE_FALSE(game.has_path(dog_id));
            }
        }
    }
}
