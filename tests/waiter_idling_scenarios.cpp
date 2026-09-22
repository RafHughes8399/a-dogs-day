#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <vector>

#include "component.h"
#include "config.h"
#include "dog_behavioural_systems.h"
#include "ecs_test_game.h"
#include "raylib.h"
#include "raymath.h"
#include "system.h"

namespace{
    constexpr float frame = 1.0f / 60.0f;
    const float idle_reach = level_config::edge_weight * dog_config::waiter_idle_bounds_edges;

    Vector2 in_cafe(float x, float y){
        return Vector2{level_config::cafe_x + x, level_config::cafe_y + y};
    }

    bool tracks(const std::vector<dbs::idle_waiter>& waiters, size_t id){
        return std::find_if(waiters.begin(), waiters.end(),
            [id](const dbs::idle_waiter& waiter) -> bool { return waiter.id() == id; }) != waiters.end();
    }

    void clear_paths(size_t id){
        auto* movement = component_managers::movement_manager_.get_component(id);
        REQUIRE(movement != nullptr);
        movement->clear_paths();
    }
}

SCENARIO("the waiter idling system tracks the waiters registered with it",
        "[ecs][npc][waiter_idling]"){
    GIVEN("a fresh ecs world and an idling system"){
        testing::ecs_test_game game;
        dbs::waiter_idling_system idling;

        THEN("it starts holding no waiters"){
            REQUIRE(idling.get_waiters().empty());
        }

        WHEN("a waiter dog is registered"){
            auto waiter_id = game.create_waiter_dog(in_cafe(640.0f, 640.0f));
            idling.register_waiter(waiter_id);

            THEN("it is tracked"){
                REQUIRE(idling.get_waiters().size() == 1);
                REQUIRE(tracks(idling.get_waiters(), waiter_id));
            }

            AND_WHEN("it is unregistered"){
                idling.unregister_waiter(waiter_id);

                THEN("it is no longer tracked"){
                    REQUIRE(idling.get_waiters().empty());
                }
            }
        }
    }
}

SCENARIO("a waiter reads as idle only when it is neither walking nor interacting",
        "[ecs][npc][waiter_idling]"){
    GIVEN("a registered waiter standing still"){
        testing::ecs_test_game game;
        dbs::waiter_idling_system idling;
        auto waiter_id = game.create_waiter_dog(in_cafe(640.0f, 640.0f));
        idling.register_waiter(waiter_id);

        THEN("it is idle"){
            REQUIRE(idling.is_idle(waiter_id));
        }

        WHEN("it is given a path"){
            game.path_to(waiter_id, in_cafe(896.0f, 640.0f));
            REQUIRE(game.queued_path_count(waiter_id) > 0);

            THEN("it is no longer idle"){
                REQUIRE_FALSE(idling.is_idle(waiter_id));
            }
        }

        WHEN("it is interacting with a table"){
            auto table_id = game.create_table(in_cafe(1280.0f, 1280.0f));
            auto* interactor = component_managers::interactor_manager_.get_component(waiter_id);
            REQUIRE(interactor != nullptr);
            interactor->interact_with(table_id);

            THEN("it is not idle even with an empty path queue"){
                REQUIRE(game.queued_path_count(waiter_id) == 0);
                REQUIRE_FALSE(idling.is_idle(waiter_id));
            }
        }
    }
}

SCENARIO("idle bounds are extents around the waiter, clamped to the cafe",
        "[ecs][npc][waiter_idling]"){
    testing::ecs_test_game game;
    dbs::waiter_idling_system idling;

    GIVEN("a waiter well inside the cafe"){
        auto position = in_cafe(1280.0f, 1280.0f);
        auto waiter_id = game.create_waiter_dog(position);
        idling.register_waiter(waiter_id);

        THEN("the bounds span twice the reach on each axis"){
            auto bounds = idling.determine_idle_bounds(waiter_id);
            REQUIRE(bounds.has_value());
            REQUIRE(bounds->x == position.x - idle_reach);
            REQUIRE(bounds->y == position.y - idle_reach);
            REQUIRE(bounds->width == idle_reach * 2.0f);
            REQUIRE(bounds->height == idle_reach * 2.0f);
        }
    }

    GIVEN("a waiter against the cafe's top left corner"){
        auto waiter_id = game.create_waiter_dog(in_cafe(0.0f, 0.0f));
        idling.register_waiter(waiter_id);

        THEN("the bounds clamp to the cafe rather than running off the graph"){
            auto bounds = idling.determine_idle_bounds(waiter_id);
            REQUIRE(bounds.has_value());
            REQUIRE(bounds->x == level_config::cafe_x);
            REQUIRE(bounds->y == level_config::cafe_y);
            REQUIRE(bounds->width == idle_reach);
            REQUIRE(bounds->height == idle_reach);
        }
    }
}

SCENARIO("the bounds sweep returns only walkable nodes on the graph",
        "[ecs][npc][waiter_idling]"){
    GIVEN("a waiter with a decoration inside its idle bounds"){
        testing::ecs_test_game game;
        dbs::waiter_idling_system idling;
        auto position = in_cafe(1280.0f, 1280.0f);
        auto waiter_id = game.create_waiter_dog(position);
        idling.register_waiter(waiter_id);

        auto decoration_position = Vector2{position.x + level_config::edge_weight * 2.0f,
                                           position.y};
        game.create_test_decoration(decoration_position);

        auto bounds = idling.determine_idle_bounds(waiter_id);
        REQUIRE(bounds.has_value());
        auto positions = idling.walkable_positions(bounds.value());

        THEN("the sweep finds candidates"){
            REQUIRE_FALSE(positions.empty());
        }

        THEN("every candidate is an unoccupied node inside the bounds"){
            for(auto candidate : positions){
                REQUIRE(game.graph_occupant_at(candidate) == graph_config::empty_node);
                REQUIRE(Vector2Equals(game.graph_node_position_at(candidate), candidate));
                REQUIRE(candidate.x >= bounds->x);
                REQUIRE(candidate.y >= bounds->y);
                REQUIRE(candidate.x < bounds->x + bounds->width);
                REQUIRE(candidate.y < bounds->y + bounds->height);
            }
        }

        THEN("the decoration's own cell is not a candidate"){
            auto blocked = game.graph_node_position_at(decoration_position);
            REQUIRE(std::none_of(positions.begin(), positions.end(),
                [blocked](Vector2 candidate) -> bool { return Vector2Equals(candidate, blocked); }));
        }
    }
}

SCENARIO("bounds holding no walkable node leave the waiter where it is",
        "[ecs][npc][waiter_idling]"){
    GIVEN("a registered waiter"){
        testing::ecs_test_game game;
        dbs::waiter_idling_system idling;
        auto waiter_id = game.create_waiter_dog(in_cafe(1280.0f, 1280.0f));
        idling.register_waiter(waiter_id);

        WHEN("paths are built against bounds that sit off the graph"){
            Rectangle off_graph{level_config::world_x + level_config::edge_weight,
                                level_config::world_y + level_config::edge_weight,
                                level_config::edge_weight * 4.0f,
                                level_config::edge_weight * 4.0f};

            THEN("the sweep finds nothing and no route is committed"){
                REQUIRE(idling.walkable_positions(off_graph).empty());
                REQUIRE_FALSE(idling.build_paths(waiter_id, dog_config::waiter_idle_max_points, off_graph));
                REQUIRE(game.queued_path_count(waiter_id) == 0);
            }
        }
    }
}

SCENARIO("a route built over open floor holds one leg per picked point",
        "[ecs][npc][waiter_idling]"){
    GIVEN("a registered waiter in open floor"){
        testing::ecs_test_game game;
        dbs::waiter_idling_system idling;
        auto waiter_id = game.create_waiter_dog(in_cafe(1280.0f, 1280.0f));
        idling.register_waiter(waiter_id);
        REQUIRE(game.queued_path_count(waiter_id) == 0);

        auto bounds = idling.determine_idle_bounds(waiter_id);
        REQUIRE(bounds.has_value());

        WHEN("a route is built for the maximum number of points"){
            REQUIRE(idling.build_paths(waiter_id, dog_config::waiter_idle_max_points,
                bounds.value()));

            THEN("it holds one leg per picked point"){
                REQUIRE(game.queued_path_count(waiter_id) == dog_config::waiter_idle_max_points);
            }

            THEN("every leg ends on a node the graph holds"){
                for(auto destination : game.path_destinations(waiter_id)){
                    REQUIRE(Vector2Equals(game.graph_node_position_at(destination), destination));
                }
            }

            THEN("it walks the route out and comes to rest"){
                REQUIRE(game.tick_until([&game, waiter_id]() -> bool {
                    return game.queued_path_count(waiter_id) == 0;
                }, 3000, frame));
            }
        }
    }
}

SCENARIO("idling is gated on the waiter standing still in its machine",
        "[ecs][npc][waiter_idling][state_machine]"){
    GIVEN("a registered waiter in open floor"){
        testing::ecs_test_game game;
        dbs::waiter_idling_system idling;
        auto waiter_id = game.create_waiter_dog(in_cafe(1280.0f, 1280.0f));
        idling.register_waiter(waiter_id);

        THEN("it starts stationary and idle"){
            REQUIRE(game.state_of(waiter_id).value() == dog_config::waiter_stationary);
            REQUIRE(idling.is_idle(waiter_id));
        }

        WHEN("it wanders"){
            auto bounds = idling.determine_idle_bounds(waiter_id);
            REQUIRE(bounds.has_value());
            REQUIRE(idling.build_paths(waiter_id, dog_config::waiter_idle_min_points, bounds.value()));
            game.tick(frame);

            THEN("it is walking idle, and not eligible to wander again"){
                REQUIRE(game.state_of(waiter_id).value() == dog_config::waiter_idle);
                REQUIRE_FALSE(idling.is_idle(waiter_id));
            }

            AND_WHEN("it walks the route out"){
                REQUIRE(game.tick_until([&game, waiter_id]() -> bool {
                    return game.queued_path_count(waiter_id) == 0;
                }, 3000, frame));
                game.tick(frame);

                THEN("it is stationary again and eligible to wander"){
                    REQUIRE(game.state_of(waiter_id).value() == dog_config::waiter_stationary);
                    REQUIRE(idling.is_idle(waiter_id));
                }
            }
        }

        WHEN("its path queue empties but the machine still reads walking"){
            systems::state_machine_system::get_instance().transition(waiter_id, dog_config::path_created);
            REQUIRE(game.queued_path_count(waiter_id) == 0);

            THEN("it is not idle - the state decides, not the queue alone"){
                REQUIRE_FALSE(idling.is_idle(waiter_id));
            }
        }

        WHEN("it is carrying food and standing still with nothing claimed"){
            systems::state_machine_system::get_instance().transition(waiter_id, dog_config::food_collected);
            REQUIRE(game.state_of(waiter_id).value() == dog_config::waiter_carrying);
            REQUIRE(game.queued_path_count(waiter_id) == 0);

            THEN("it is not idle, so it never wanders off with the food"){
                REQUIRE_FALSE(idling.is_idle(waiter_id));
            }
            THEN("a second of updates commits no route"){
                for(int tick = 0; tick < 200; ++tick){
                    idling.update(frame, tick * game_config::frames);
                }
                REQUIRE(game.queued_path_count(waiter_id) == 0);
            }
        }

        WHEN("it is standing still and the update rolls many times"){
            bool wandered = false;
            for(int tick = 0; tick < 200 and not wandered; ++tick){
                idling.update(frame, tick * game_config::frames);
                wandered = game.queued_path_count(waiter_id) > 0;
            }

            THEN("it eventually wanders"){
                REQUIRE(wandered);
            }
        }
    }

    GIVEN("a registered id with no state machine"){
        testing::ecs_test_game game;
        dbs::waiter_idling_system idling;
        auto decoration_id = game.create_test_decoration(in_cafe(1280.0f, 1280.0f));
        idling.register_waiter(decoration_id);

        THEN("it is never idle"){
            REQUIRE_FALSE(game.has_state_machine(decoration_id));
            REQUIRE_FALSE(idling.is_idle(decoration_id));
        }
    }
}
