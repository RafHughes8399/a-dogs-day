#include <catch2/catch_test_macros.hpp>

#include <algorithm>

#include "component.h"
#include "config.h"
#include "ecs_test_game.h"
#include "raylib.h"
#include "raymath.h"
#include "system.h"

namespace{
    constexpr float k_clear_gap = 400.0f;
    // close enough that the two stations' interaction boxes overlap, which no
    // longer means anything - that is the point of the scenario using it
    constexpr float k_neighbour_gap = 100.0f;

    Vector2 in_cafe(float x, float y){
        return Vector2{level_config::cafe_x + x, level_config::cafe_y + y};
    }

    bool tracks(const std::vector<size_t>& ids, size_t id){
        return std::find(ids.begin(), ids.end(), id) != ids.end();
    }

    // both halves of the handshake, the way a real claim would land
    void seat(size_t dog_id, size_t table_id){
        auto* interactable = component_managers::interactable_manager_.get_component(table_id);
        auto* interactor = component_managers::interactor_manager_.get_component(dog_id);
        REQUIRE(interactable != nullptr);
        REQUIRE(interactor != nullptr);
        REQUIRE(interactable->claim(dog_id));
        interactor->interact_with(table_id);
    }

    // build_table opens a left and a right slot, so a table seats two
    void fill(testing::ecs_test_game& game, size_t table_id){
        seat(game.create_customer_dog(in_cafe(2000.0f, 2000.0f)), table_id);
        seat(game.create_customer_dog(in_cafe(2000.0f, 2000.0f)), table_id);
    }

    bool claimed_by(size_t table_id, size_t dog_id){
        auto* interactable = component_managers::interactable_manager_.get_component(table_id);
        if(interactable == nullptr){ return false; }
        for(auto slot : interactable->get_interactors()){
            if(slot.has_value() and slot.value() == dog_id){ return true; }
        }
        return false;
    }

    std::optional<size_t> target_of(size_t dog_id){
        auto* interactor = component_managers::interactor_manager_.get_component(dog_id);
        return interactor == nullptr ? std::nullopt : interactor->get_target();
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

SCENARIO("a table takes two claims to fill", "[ecs][npc][customer_arrival][pick_table]"){
    GIVEN("one registered table"){
        testing::ecs_test_game game;
        systems::npc_system::customer_arrival_system arrival;

        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        arrival.register_table(table_id);
        auto* interactable = component_managers::interactable_manager_.get_component(table_id);

        THEN("build_table opened a left and a right slot and nothing else"){
            REQUIRE(interactable->get_slot_offset(level_config::directions::left).has_value());
            REQUIRE(interactable->get_slot_offset(level_config::directions::right).has_value());
            REQUIRE_FALSE(interactable->get_slot_offset(level_config::directions::up).has_value());
            REQUIRE_FALSE(interactable->get_slot_offset(level_config::directions::down).has_value());
        }

        WHEN("one dog claims it"){
            auto first_id = game.create_customer_dog(in_cafe(320.0f, 320.0f + k_clear_gap));
            seat(first_id, table_id);

            THEN("the second slot is still open"){
                REQUIRE(interactable->can_accept_interactor());
                REQUIRE(arrival.pick_table() == static_cast<int>(table_id));
            }

            AND_WHEN("a second dog claims it"){
                auto second_id = game.create_customer_dog(in_cafe(320.0f, 320.0f + k_clear_gap));
                seat(second_id, table_id);

                THEN("the table is full"){
                    REQUIRE_FALSE(interactable->can_accept_interactor());
                    REQUIRE(arrival.pick_table() == game_config::empty_entity);
                }
                THEN("a third dog is turned away"){
                    auto third_id = game.create_customer_dog(in_cafe(320.0f, 320.0f + k_clear_gap));
                    REQUIRE_FALSE(interactable->claim(third_id));
                }
            }

            AND_WHEN("the same dog claims again"){
                THEN("the duplicate is rejected and no second slot is spent"){
                    REQUIRE_FALSE(interactable->claim(first_id));
                    REQUIRE(interactable->can_accept_interactor());
                }
            }

            AND_WHEN("it releases"){
                interactable->release(first_id);

                THEN("the slot comes back"){
                    REQUIRE_FALSE(claimed_by(table_id, first_id));
                    REQUIRE(interactable->can_accept_interactor());
                }
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

        WHEN("the one registered table is unclaimed"){
            auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
            arrival.register_table(table_id);

            THEN("it is picked"){
                REQUIRE(arrival.pick_table() == static_cast<int>(table_id));
                REQUIRE(arrival.free_tables());
            }
        }

        WHEN("both of the one registered table's slots are claimed"){
            auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
            arrival.register_table(table_id);
            fill(game, table_id);

            THEN("nothing is picked"){
                REQUIRE(arrival.pick_table() == game_config::empty_entity);
                REQUIRE_FALSE(arrival.free_tables());
            }
        }

        WHEN("a dog stands on the one registered table without claiming it"){
            auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
            game.create_customer_dog(in_cafe(320.0f, 320.0f));
            arrival.register_table(table_id);

            THEN("it is still picked - occupancy is a claim, not proximity"){
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

SCENARIO("checking free tables with exactly two registered tables", "[ecs][npc][customer_arrival][pick_table]"){
    GIVEN("two registered tables"){
        testing::ecs_test_game game;
        systems::npc_system::customer_arrival_system arrival;

        auto first_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto second_id = game.create_table(in_cafe(320.0f, 320.0f + k_clear_gap));
        arrival.register_table(first_id);
        arrival.register_table(second_id);

        WHEN("both are unclaimed"){
            THEN("a table is free and the first registered is picked"){
                REQUIRE(arrival.free_tables());
                REQUIRE(arrival.pick_table() == static_cast<int>(first_id));
            }
        }

        WHEN("the first is full and the second is unclaimed"){
            fill(game, first_id);

            THEN("a table is still free and the second is picked"){
                REQUIRE(arrival.free_tables());
                REQUIRE(arrival.pick_table() == static_cast<int>(second_id));
            }
        }

        WHEN("the first is unclaimed and the second is full"){
            fill(game, second_id);

            THEN("a table is still free and the first is picked"){
                REQUIRE(arrival.free_tables());
                REQUIRE(arrival.pick_table() == static_cast<int>(first_id));
            }
        }

        WHEN("the first is only half claimed and the second is full"){
            auto lone_dog = game.create_customer_dog(in_cafe(320.0f, 320.0f + 2.0f * k_clear_gap));
            seat(lone_dog, first_id);
            fill(game, second_id);

            THEN("the half-claimed table still counts as free"){
                REQUIRE(arrival.free_tables());
                REQUIRE(arrival.pick_table() == static_cast<int>(first_id));
            }
        }

        WHEN("both are full"){
            fill(game, first_id);
            fill(game, second_id);

            THEN("no table is free"){
                REQUIRE_FALSE(arrival.free_tables());
                REQUIRE(arrival.pick_table() == game_config::empty_entity);
            }
        }
    }
}

SCENARIO("picking a table with several registered tables", "[ecs][npc][customer_arrival][pick_table]"){
    GIVEN("a fresh ecs world and an arrival system"){
        testing::ecs_test_game game;
        systems::npc_system::customer_arrival_system arrival;

        WHEN("no registered table is claimed"){
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

        WHEN("every registered table is full"){
            auto first_id = game.create_table(in_cafe(320.0f, 320.0f));
            auto second_id = game.create_table(in_cafe(320.0f, 320.0f + k_clear_gap));
            auto third_id = game.create_table(in_cafe(320.0f, 320.0f + 2.0f * k_clear_gap));
            arrival.register_table(first_id);
            arrival.register_table(second_id);
            arrival.register_table(third_id);
            fill(game, first_id);
            fill(game, second_id);
            fill(game, third_id);

            THEN("nothing is picked"){
                REQUIRE(arrival.pick_table() == game_config::empty_entity);
                REQUIRE_FALSE(arrival.free_tables());
            }
        }

        WHEN("exactly one registered table has room and it was registered last"){
            auto first_id = game.create_table(in_cafe(320.0f, 320.0f));
            auto second_id = game.create_table(in_cafe(320.0f, 320.0f + k_clear_gap));
            auto free_id = game.create_table(in_cafe(320.0f, 320.0f + 2.0f * k_clear_gap));
            arrival.register_table(first_id);
            arrival.register_table(second_id);
            arrival.register_table(free_id);
            fill(game, first_id);
            fill(game, second_id);

            THEN("the full pair is skipped and the free table is picked"){
                REQUIRE(arrival.pick_table() == static_cast<int>(free_id));
                REQUIRE(arrival.free_tables());
            }

            AND_WHEN("the free table is unregistered too"){
                arrival.unregister_table(free_id);

                THEN("nothing is left to pick"){
                    REQUIRE(arrival.pick_table() == game_config::empty_entity);
                    REQUIRE_FALSE(arrival.free_tables());
                }
            }
        }

        WHEN("a claiming dog walks away without releasing"){
            auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
            arrival.register_table(table_id);
            auto first_id = game.create_customer_dog(in_cafe(320.0f, 320.0f));
            auto second_id = game.create_customer_dog(in_cafe(320.0f, 320.0f));
            seat(first_id, table_id);
            seat(second_id, table_id);
            REQUIRE(arrival.pick_table() == game_config::empty_entity);

            game.move_entity(first_id, in_cafe(320.0f, 320.0f + 2.0f * k_clear_gap));

            THEN("the table stays taken - this is what reservation means"){
                REQUIRE(arrival.pick_table() == game_config::empty_entity);
                REQUIRE(claimed_by(table_id, first_id));
            }
        }
    }
}

SCENARIO("removing an entity undoes both halves of the claim",
        "[ecs][npc][customer_arrival][lifespan]"){
    GIVEN("a full table with both seats claimed"){
        testing::ecs_test_game game;
        systems::npc_system::customer_arrival_system arrival;

        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        arrival.register_table(table_id);
        auto first_id = game.create_customer_dog(in_cafe(320.0f, 320.0f + k_clear_gap));
        auto second_id = game.create_customer_dog(in_cafe(320.0f, 320.0f + k_clear_gap));
        seat(first_id, table_id);
        seat(second_id, table_id);

        REQUIRE(arrival.pick_table() == game_config::empty_entity);
        REQUIRE(claimed_by(table_id, first_id));
        REQUIRE(target_of(first_id) == table_id);

        WHEN("one of the dogs is removed"){
            game.remove(first_id);

            THEN("its slot is released and the table opens up again"){
                REQUIRE_FALSE(claimed_by(table_id, first_id));
                REQUIRE(arrival.pick_table() == static_cast<int>(table_id));
            }
            THEN("the other dog keeps its own claim"){
                REQUIRE(claimed_by(table_id, second_id));
                REQUIRE(target_of(second_id) == table_id);
            }
        }

        WHEN("both dogs are removed"){
            game.remove(first_id);
            game.remove(second_id);

            THEN("the table is empty"){
                auto* interactable = component_managers::interactable_manager_.get_component(table_id);
                for(auto slot : interactable->get_interactors()){
                    REQUIRE_FALSE(slot.has_value());
                }
                REQUIRE(arrival.pick_table() == static_cast<int>(table_id));
            }
        }

        WHEN("the table is removed instead"){
            game.remove(table_id);

            THEN("neither dog is left pointing at a dead id"){
                REQUIRE_FALSE(target_of(first_id).has_value());
                REQUIRE_FALSE(target_of(second_id).has_value());
            }
            THEN("the dogs themselves survive"){
                REQUIRE(game.is_tracked(first_id));
                REQUIRE(game.is_tracked(second_id));
                REQUIRE(game.has_interactor(first_id));
            }
        }
    }
}

SCENARIO("the cafe entrance sits on the seam both graphs share",
        "[ecs][npc][customer_arrival][entrance]"){
    GIVEN("the configured entrance"){
        THEN("it is halfway up the cafe"){
            REQUIRE(cafe_config::cafe_entrance.y
                == level_config::cafe_y + level_config::cafe_height * 0.5f);
        }
        THEN("it is inside the overlap band, so both zones can reach it"){
            REQUIRE(cafe_config::cafe_entrance.x >= level_config::cafe_x);
            REQUIRE(cafe_config::cafe_entrance.x
                < level_config::footpath_x + level_config::footpath_width);
        }
    }
}

SCENARIO("sending a customer to a table routes it through the entrance",
        "[ecs][npc][customer_arrival][entrance]"){
    GIVEN("a customer on the footpath and a free table in the cafe"){
        testing::ecs_test_game game;
        systems::npc_system::customer_arrival_system arrival;

        auto customer_id = game.create_customer_dog(Vector2{96.0f, 512.0f});
        auto table_id = game.create_table(Vector2{1600.0f, 1024.0f});
        arrival.register_customer(customer_id);
        arrival.register_table(table_id);

        WHEN("it is sent to a table"){
            arrival.send_customer_to_table();

            THEN("the table is claimed before the walk starts"){
                REQUIRE(claimed_by(table_id, customer_id));
                REQUIRE(target_of(customer_id) == table_id);
            }
            THEN("the route is two legs - the entrance checkpoint is the crossing, so no extra split"){
                REQUIRE(game.queued_path_count(customer_id) == 2);
            }
            THEN("the first leg ends at the entrance"){
                auto destinations = game.path_destinations(customer_id);
                REQUIRE(Vector2Equals(destinations.front(), cafe_config::cafe_entrance));
            }
            THEN("the last leg ends at an interaction slot, not the table centre"){
                auto destinations = game.path_destinations(customer_id);
                REQUIRE_FALSE(Vector2Equals(destinations.back(), Vector2{1600.0f, 1024.0f}));
                REQUIRE(destinations.back().x >= level_config::cafe_x);
            }
        }

        WHEN("a second customer is sent while the first holds a slot"){
            arrival.send_customer_to_table();
            auto second_id = game.create_customer_dog(Vector2{96.0f, 640.0f});
            arrival.register_customer(second_id);
            arrival.send_customer_to_table();

            THEN("it takes the table's other slot, not the first one"){
                REQUIRE(claimed_by(table_id, second_id));
                REQUIRE(claimed_by(table_id, customer_id));
                REQUIRE(target_of(second_id) == table_id);
            }
            THEN("the table is now full"){
                REQUIRE(arrival.pick_table() == game_config::empty_entity);
            }

            AND_WHEN("a third customer is sent"){
                auto third_id = game.create_customer_dog(Vector2{96.0f, 768.0f});
                arrival.register_customer(third_id);
                arrival.send_customer_to_table();

                THEN("it gets nothing - no table left to claim"){
                    REQUIRE_FALSE(target_of(third_id).has_value());
                    REQUIRE(game.queued_path_count(third_id) == 0);
                }
            }
        }

        WHEN("a customer already holding a table is passed over"){
            arrival.send_customer_to_table();
            auto second_table = game.create_table(Vector2{1600.0f, 400.0f});
            arrival.register_table(second_table);
            arrival.send_customer_to_table();

            THEN("the seated customer is not re-sent"){
                REQUIRE(target_of(customer_id) == table_id);
                REQUIRE_FALSE(claimed_by(second_table, customer_id));
            }
        }
    }
}

SCENARIO("sending a customer to a table respects table occupancy",
        "[ecs][npc][customer_arrival][send_customer_to_table]"){
    GIVEN("a customer waiting on the footpath"){
        testing::ecs_test_game game;
        systems::npc_system::customer_arrival_system arrival;

        auto customer_id = game.create_customer_dog(Vector2{96.0f, 512.0f});
        arrival.register_customer(customer_id);

        WHEN("no table is registered"){
            arrival.send_customer_to_table();

            THEN("the customer is left unclaimed and unrouted"){
                REQUIRE_FALSE(target_of(customer_id).has_value());
                REQUIRE(game.queued_path_count(customer_id) == 0);
            }
        }

        WHEN("the one registered table is free"){
            auto table_id = game.create_table(Vector2{1600.0f, 1024.0f});
            arrival.register_table(table_id);
            arrival.send_customer_to_table();

            THEN("the customer claims it and is routed via the entrance"){
                REQUIRE(claimed_by(table_id, customer_id));
                REQUIRE(target_of(customer_id) == table_id);
                REQUIRE(game.queued_path_count(customer_id) == 2);
            }
        }

        WHEN("the one registered table is full"){
            auto table_id = game.create_table(Vector2{1600.0f, 1024.0f});
            arrival.register_table(table_id);
            fill(game, table_id);
            arrival.send_customer_to_table();

            THEN("the customer is left unclaimed and unrouted"){
                REQUIRE_FALSE(target_of(customer_id).has_value());
                REQUIRE(game.queued_path_count(customer_id) == 0);
            }
        }

        WHEN("of two registered tables, the first is full and the second is free"){
            auto first_id = game.create_table(Vector2{1600.0f, 1024.0f});
            auto second_id = game.create_table(Vector2{1600.0f, 1024.0f + k_clear_gap});
            arrival.register_table(first_id);
            arrival.register_table(second_id);
            fill(game, first_id);
            arrival.send_customer_to_table();

            THEN("the customer claims the free second table, not the full first"){
                REQUIRE(claimed_by(second_id, customer_id));
                REQUIRE_FALSE(claimed_by(first_id, customer_id));
                REQUIRE(target_of(customer_id) == second_id);
            }
        }

        WHEN("both of the two registered tables are full"){
            auto first_id = game.create_table(Vector2{1600.0f, 1024.0f});
            auto second_id = game.create_table(Vector2{1600.0f, 1024.0f + k_clear_gap});
            arrival.register_table(first_id);
            arrival.register_table(second_id);
            fill(game, first_id);
            fill(game, second_id);
            arrival.send_customer_to_table();

            THEN("the customer is left unclaimed and unrouted"){
                REQUIRE_FALSE(target_of(customer_id).has_value());
                REQUIRE(game.queued_path_count(customer_id) == 0);
            }
        }
    }
}

SCENARIO("a seated customer survives the departure sweep",
        "[ecs][npc][customer_arrival][entrance]"){
    GIVEN("a customer that has walked to its table"){
        testing::ecs_test_game game;
        systems::npc_system::customer_arrival_system arrival;

        auto customer_id = game.create_customer_dog(Vector2{96.0f, 512.0f});
        auto table_id = game.create_table(Vector2{1600.0f, 1024.0f});
        arrival.register_customer(customer_id);
        arrival.register_table(table_id);
        arrival.send_customer_to_table();

        REQUIRE(game.tick_until([&](){
            return game.queued_path_count(customer_id) == 0;
        }, 8000));

        WHEN("the cleanup sweep runs with its path queue empty"){
            arrival.customer_cleanup();

            THEN("it is not destroyed - an empty queue means seated, not finished"){
                REQUIRE(tracks(arrival.get_customers(), customer_id));
                REQUIRE(game.is_tracked(customer_id));
                REQUIRE(claimed_by(table_id, customer_id));
            }
        }
    }

    GIVEN("a customer holding no table with an empty path queue"){
        testing::ecs_test_game game;
        systems::npc_system::customer_arrival_system arrival;

        auto customer_id = game.create_customer_dog(Vector2{96.0f, 512.0f});
        arrival.register_customer(customer_id);

        WHEN("the cleanup sweep runs"){
            arrival.customer_cleanup();

            THEN("it is destroyed as a departure"){
                REQUIRE(arrival.get_customers().empty());
                REQUIRE_FALSE(game.is_tracked(customer_id));
            }
        }
    }
}
