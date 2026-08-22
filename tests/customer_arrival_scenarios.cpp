#include <catch2/catch_test_macros.hpp>

#include <algorithm>

#include "component.h"
#include "config.h"
#include "ecs_test_game.h"
#include "raylib.h"
#include "system.h"

namespace{
    // A table's hitbox is 128x128 (entity_config::table_attributes) and
    // entity_config::station_reach is 16, so its interaction box spans 160x160
    // from (x-16, y-16). A dog occupies a table when its own interaction box
    // overlaps that; k_clear_gap puts one far enough away that it does not.
    constexpr float k_clear_gap = 400.0f;
    // close enough that the two tables' interaction boxes overlap - which no
    // longer means anything, and that is the point of the scenario using it
    constexpr float k_neighbour_gap = 100.0f;

    Vector2 in_cafe(float x, float y){
        return Vector2{level_config::cafe_x + x, level_config::cafe_y + y};
    }

    bool tracks(const std::vector<size_t>& ids, size_t id){
        return std::find(ids.begin(), ids.end(), id) != ids.end();
    }
}

SCENARIO("the customer arrival system tracks the customers registered with it",
        "[ecs][npc][customer_arrival]"){
    GIVEN("a fresh ecs world and an arrival system"){
        testing::ecs_test_game game;
        systems::npc_system::customer_arrival_system arrival;

        THEN("it starts holding no customers"){
            REQUIRE(arrival.get_customers().empty());
        }

        WHEN("a customer dog is registered"){
            auto customer_id = game.create_customer_dog(in_cafe(320.0f, 320.0f));
            arrival.register_customer(customer_id);

            THEN("the system tracks it"){
                REQUIRE(arrival.get_customers().size() == 1);
                REQUIRE(tracks(arrival.get_customers(), customer_id));
            }

            AND_WHEN("it is unregistered"){
                arrival.unregister_customer(customer_id);

                THEN("the system no longer tracks it"){
                    REQUIRE(arrival.get_customers().empty());
                }
                THEN("the entity itself is untouched - unregistering is not a destroy"){
                    REQUIRE(game.is_tracked(customer_id));
                    REQUIRE(game.has_position(customer_id));
                }
            }
        }

        WHEN("two customers are registered and only the first is unregistered"){
            auto first_id = game.create_customer_dog(in_cafe(320.0f, 320.0f));
            auto second_id = game.create_customer_dog(in_cafe(320.0f, 320.0f + k_clear_gap));
            arrival.register_customer(first_id);
            arrival.register_customer(second_id);
            arrival.unregister_customer(first_id);

            THEN("the second is still tracked and the first is not"){
                REQUIRE(arrival.get_customers().size() == 1);
                REQUIRE_FALSE(tracks(arrival.get_customers(), first_id));
                REQUIRE(tracks(arrival.get_customers(), second_id));
            }
        }

        WHEN("an id the system never held is unregistered"){
            auto customer_id = game.create_customer_dog(in_cafe(320.0f, 320.0f));
            arrival.register_customer(customer_id);
            arrival.unregister_customer(customer_id + 99);

            THEN("nothing is dropped"){
                REQUIRE(arrival.get_customers().size() == 1);
                REQUIRE(tracks(arrival.get_customers(), customer_id));
            }
        }
    }
}

SCENARIO("the customer arrival system tracks the tables registered with it",
        "[ecs][npc][customer_arrival]"){
    GIVEN("a fresh ecs world and an arrival system"){
        testing::ecs_test_game game;
        systems::npc_system::customer_arrival_system arrival;

        THEN("it starts holding no tables"){
            REQUIRE(arrival.get_tables().empty());
        }

        WHEN("a table is registered"){
            auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
            arrival.register_table(table_id);

            THEN("the system tracks it"){
                REQUIRE(arrival.get_tables().size() == 1);
                REQUIRE(tracks(arrival.get_tables(), table_id));
            }
            THEN("it is reachable through pick_table"){
                REQUIRE(arrival.pick_table() == static_cast<int>(table_id));
            }

            AND_WHEN("it is unregistered"){
                arrival.unregister_table(table_id);

                THEN("the system no longer tracks it"){
                    REQUIRE(arrival.get_tables().empty());
                }
                THEN("pick_table can no longer reach it"){
                    REQUIRE(arrival.pick_table() == game_config::empty_entity);
                }
                THEN("the entity itself is untouched - unregistering is not a destroy"){
                    REQUIRE(game.is_tracked(table_id));
                    REQUIRE(game.has_interactable(table_id));
                }
            }
        }

        WHEN("two tables are registered and only the first is unregistered"){
            auto first_id = game.create_table(in_cafe(320.0f, 320.0f));
            auto second_id = game.create_table(in_cafe(320.0f, 320.0f + k_clear_gap));
            arrival.register_table(first_id);
            arrival.register_table(second_id);
            arrival.unregister_table(first_id);

            THEN("the second is still tracked and the first is not"){
                REQUIRE(arrival.get_tables().size() == 1);
                REQUIRE_FALSE(tracks(arrival.get_tables(), first_id));
                REQUIRE(tracks(arrival.get_tables(), second_id));
            }
            THEN("pick_table returns the survivor"){
                REQUIRE(arrival.pick_table() == static_cast<int>(second_id));
            }
        }

        WHEN("an id the system never held is unregistered"){
            auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
            arrival.register_table(table_id);
            arrival.unregister_table(table_id + 99);

            THEN("nothing is dropped"){
                REQUIRE(arrival.get_tables().size() == 1);
                REQUIRE(tracks(arrival.get_tables(), table_id));
            }
        }

        WHEN("customers and tables are registered together"){
            auto customer_id = game.create_customer_dog(in_cafe(320.0f, 320.0f));
            auto table_id = game.create_table(in_cafe(320.0f, 320.0f + k_clear_gap));
            arrival.register_customer(customer_id);
            arrival.register_table(table_id);

            THEN("the two lists stay separate"){
                REQUIRE(arrival.get_customers().size() == 1);
                REQUIRE(arrival.get_tables().size() == 1);
                REQUIRE(tracks(arrival.get_customers(), customer_id));
                REQUIRE(tracks(arrival.get_tables(), table_id));
            }

            AND_WHEN("the customer is unregistered"){
                arrival.unregister_customer(customer_id);

                THEN("the table is untouched"){
                    REQUIRE(arrival.get_customers().empty());
                    REQUIRE(arrival.get_tables().size() == 1);
                }
            }
        }
    }
}

SCENARIO("clearing the arrival system drops both registers", "[ecs][npc][customer_arrival]"){
    GIVEN("an arrival system holding a customer and a table"){
        testing::ecs_test_game game;
        systems::npc_system::customer_arrival_system arrival;

        auto customer_id = game.create_customer_dog(in_cafe(320.0f, 320.0f));
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f + k_clear_gap));
        arrival.register_customer(customer_id);
        arrival.register_table(table_id);

        WHEN("it is cleared"){
            arrival.clear();

            THEN("neither register survives"){
                REQUIRE(arrival.get_customers().empty());
                REQUIRE(arrival.get_tables().empty());
            }
            THEN("pick_table has nothing left to reach"){
                REQUIRE(arrival.pick_table() == game_config::empty_entity);
            }
        }
    }
}

SCENARIO("picking a table with a single registered table", "[ecs][npc][customer_arrival][pick_table]"){
    GIVEN("a fresh ecs world and an arrival system"){
        testing::ecs_test_game game;
        systems::npc_system::customer_arrival_system arrival;

        WHEN("no table is registered at all"){
            THEN("nothing is picked"){
                REQUIRE(arrival.pick_table() == game_config::empty_entity);
                REQUIRE_FALSE(arrival.free_tables());
            }
        }

        WHEN("the one registered table has no dog near it"){
            auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
            arrival.register_table(table_id);

            THEN("it is picked"){
                REQUIRE(arrival.pick_table() == static_cast<int>(table_id));
                REQUIRE(arrival.free_tables());
            }
        }

        WHEN("a dog stands inside the one registered table's interaction box"){
            auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
            game.create_customer_dog(in_cafe(320.0f, 320.0f));
            arrival.register_table(table_id);

            THEN("nothing is picked"){
                REQUIRE(arrival.pick_table() == game_config::empty_entity);
                REQUIRE_FALSE(arrival.free_tables());
            }
        }

        WHEN("a dog stands well clear of the one registered table"){
            auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
            game.create_customer_dog(in_cafe(320.0f, 320.0f + k_clear_gap));
            arrival.register_table(table_id);

            THEN("it is still picked"){
                REQUIRE(arrival.pick_table() == static_cast<int>(table_id));
                REQUIRE(arrival.free_tables());
            }
        }

        WHEN("another station sits inside the one registered table's interaction box"){
            auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
            game.create_food_counter(in_cafe(320.0f + k_neighbour_gap, 320.0f));
            arrival.register_table(table_id);

            THEN("it is still picked - stations do not occupy one another"){
                REQUIRE(arrival.pick_table() == static_cast<int>(table_id));
                REQUIRE(arrival.free_tables());
            }
        }

        WHEN("the one registered id carries no interactable component"){
            auto decoration_id = game.create_test_decoration(in_cafe(320.0f, 320.0f));
            arrival.register_table(decoration_id);

            THEN("it is skipped rather than picked"){
                REQUIRE_FALSE(game.has_interactable(decoration_id));
                REQUIRE(arrival.pick_table() == game_config::empty_entity);
                REQUIRE_FALSE(arrival.free_tables());
            }
        }
    }
}

SCENARIO("picking a table with several registered tables", "[ecs][npc][customer_arrival][pick_table]"){
    GIVEN("a fresh ecs world and an arrival system"){
        testing::ecs_test_game game;
        systems::npc_system::customer_arrival_system arrival;

        WHEN("no registered table has a dog near it"){
            auto first_id = game.create_table(in_cafe(320.0f, 320.0f));
            auto second_id = game.create_table(in_cafe(320.0f, 320.0f + k_clear_gap));
            auto third_id = game.create_table(in_cafe(320.0f, 320.0f + 2.0f * k_clear_gap));
            arrival.register_table(first_id);
            arrival.register_table(second_id);
            arrival.register_table(third_id);

            THEN("the first one registered is picked"){
                REQUIRE(arrival.pick_table() == static_cast<int>(first_id));
                REQUIRE(arrival.free_tables());
            }

            AND_WHEN("the first is unregistered"){
                arrival.unregister_table(first_id);

                THEN("the next in registration order is picked"){
                    REQUIRE(arrival.pick_table() == static_cast<int>(second_id));
                }
            }
        }

        WHEN("every registered table has a dog on it"){
            auto first_id = game.create_table(in_cafe(320.0f, 320.0f));
            auto second_id = game.create_table(in_cafe(320.0f, 320.0f + k_clear_gap));
            auto third_id = game.create_table(in_cafe(320.0f, 320.0f + 2.0f * k_clear_gap));
            game.create_customer_dog(in_cafe(320.0f, 320.0f));
            game.create_customer_dog(in_cafe(320.0f, 320.0f + k_clear_gap));
            game.create_customer_dog(in_cafe(320.0f, 320.0f + 2.0f * k_clear_gap));
            arrival.register_table(first_id);
            arrival.register_table(second_id);
            arrival.register_table(third_id);

            THEN("nothing is picked"){
                REQUIRE(arrival.pick_table() == game_config::empty_entity);
                REQUIRE_FALSE(arrival.free_tables());
            }
        }

        WHEN("exactly one registered table is clear and it was registered last"){
            auto first_id = game.create_table(in_cafe(320.0f, 320.0f));
            auto second_id = game.create_table(in_cafe(320.0f, 320.0f + k_clear_gap));
            auto clear_id = game.create_table(in_cafe(320.0f, 320.0f + 2.0f * k_clear_gap));
            game.create_customer_dog(in_cafe(320.0f, 320.0f));
            game.create_customer_dog(in_cafe(320.0f, 320.0f + k_clear_gap));
            arrival.register_table(first_id);
            arrival.register_table(second_id);
            arrival.register_table(clear_id);

            THEN("the occupied pair is skipped and the clear table is picked"){
                REQUIRE(arrival.pick_table() == static_cast<int>(clear_id));
                REQUIRE(arrival.free_tables());
            }

            AND_WHEN("the clear table is unregistered too"){
                arrival.unregister_table(clear_id);

                THEN("nothing is left to pick"){
                    REQUIRE(arrival.pick_table() == game_config::empty_entity);
                    REQUIRE_FALSE(arrival.free_tables());
                }
            }
        }

        WHEN("an occupying dog is removed from the world"){
            auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
            auto dog_id = game.create_customer_dog(in_cafe(320.0f, 320.0f));
            arrival.register_table(table_id);
            REQUIRE(arrival.pick_table() == game_config::empty_entity);

            game.remove(dog_id);

            THEN("the table becomes pickable again"){
                REQUIRE(arrival.pick_table() == static_cast<int>(table_id));
                REQUIRE(arrival.free_tables());
            }
        }

        WHEN("an occupying dog walks out of the table's interaction box"){
            auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
            auto dog_id = game.create_customer_dog(in_cafe(320.0f, 320.0f));
            arrival.register_table(table_id);
            REQUIRE(arrival.pick_table() == game_config::empty_entity);

            game.move_entity(dog_id, in_cafe(320.0f, 320.0f + k_clear_gap));

            THEN("the table frees up"){
                REQUIRE(arrival.pick_table() == static_cast<int>(table_id));
                REQUIRE(arrival.free_tables());
            }
        }
    }
}

SCENARIO("any dog occupies a table, not just a customer", "[ecs][npc][customer_arrival][pick_table]"){
    GIVEN("one registered table with a waiter dog standing on it"){
        testing::ecs_test_game game;
        systems::npc_system::customer_arrival_system arrival;

        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto waiter_id = game.create_waiter_dog(in_cafe(320.0f, 320.0f));
        arrival.register_table(table_id);

        THEN("both entities are in the spatial index"){
            REQUIRE(game.is_tracked(table_id));
            REQUIRE(game.is_tracked(waiter_id));
        }
        THEN("the waiter carries an interactor, not an interactable"){
            REQUIRE(game.has_interactor(waiter_id));
            REQUIRE_FALSE(game.has_interactable(waiter_id));
        }
        // occupancy is spatial and blind to why the dog is there - a waiter
        // serving the table reads the same as a customer sitting at it
        THEN("the table is not picked"){
            REQUIRE(arrival.pick_table() == game_config::empty_entity);
            REQUIRE_FALSE(arrival.free_tables());
        }
    }
}
