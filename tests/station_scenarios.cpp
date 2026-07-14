// Scenarios for station's generic interacting-dog tracking (enter/leave/
// is_interacting/capacity) and the level/maitre_d wiring that drives it on
// dog arrival.
#include <catch2/catch_test_macros.hpp>

#include "test_game.h"
#include "events.h"
#include "events_interface.h"

using testing::test_game;

SCENARIO("a station enforces its interacting-dog capacity", "[station][capacity]"){
    GIVEN("a table (capacity 1) built and inserted"){
        test_game game;
        const int table_id = 220;
        const Vector2 position{level_config::edge_weight * 8, level_config::edge_weight * 8};
        game.insert_entity(game.build_table(table_id, position), level_config::draw_layers::stations);
        auto* station = dynamic_cast<entities::station*>(game.find_entity(table_id));
        REQUIRE(station != nullptr);
        REQUIRE(station->capacity() == 1);

        WHEN("a dog enters"){
            REQUIRE(station->enter(1));
            THEN("the station is interacting"){
                REQUIRE(station->is_interacting());
            }
            AND_WHEN("a second dog tries to enter while the first is still present"){
                THEN("it is rejected"){
                    REQUIRE_FALSE(station->enter(2));
                }
            }
        }
    }
}
SCENARIO("enter/leave toggle a station's interacting state", "[station][enter][leave]"){
    GIVEN("a table built and inserted"){
        test_game game;
        const int table_id = 221;
        const Vector2 position{level_config::edge_weight * 8, level_config::edge_weight * 8};
        game.insert_entity(game.build_table(table_id, position), level_config::draw_layers::stations);
        auto* station = dynamic_cast<entities::station*>(game.find_entity(table_id));
        REQUIRE(station != nullptr);
        REQUIRE_FALSE(station->is_interacting());

        WHEN("a dog enters then leaves"){
            station->enter(5);
            REQUIRE(station->is_interacting());
            station->leave(5);
            THEN("the station is no longer interacting"){
                REQUIRE_FALSE(station->is_interacting());
            }
        }

        WHEN("leave is called for a dog that was never present"){
            THEN("it is a safe no-op"){
                station->leave(999);
                station->leave(999);
                REQUIRE_FALSE(station->is_interacting());
            }
        }
    }
}

SCENARIO("the level calls enter() on the station a dog_reached_station names",
        "[station][level][arrival]"){
    // dog_reached_station already carries the table_id (customer_dog's own
    // traveling state emits it on arrival) - level resolves it by id, no
    // position scanning over every station in the level.
    GIVEN("a table registered with the level"){
        test_game game;
        const int table_id = 223;
        const Vector2 position{level_config::edge_weight * 8, level_config::edge_weight * 8};
        game.insert_entity(game.build_table(table_id, position), level_config::draw_layers::stations);
        game.tick(1.0f / 60.0f); // drain registered_table
        auto* station = dynamic_cast<entities::station*>(game.find_entity(table_id));
        REQUIRE(station != nullptr);
        REQUIRE_FALSE(station->is_interacting());

        WHEN("a dog_reached_station event names this table's id"){
            events::dog_reached_station reached{7, static_cast<size_t>(table_id), position};
            event_interface::execute_event(reached);

            THEN("the station becomes interacting"){
                REQUIRE(station->is_interacting());
            }
        }

        WHEN("a dog_reached_station event names a table id that doesn't exist"){
            events::dog_reached_station reached{8, 99999, position};
            event_interface::execute_event(reached);

            THEN("the station stays non-interacting"){
                REQUIRE_FALSE(station->is_interacting());
            }
        }
    }
}

SCENARIO("the maitre d' occupies a table when its reserved dog reaches it",
        "[station][maitre_d][occupy][arrival]"){
    GIVEN("a table registered with the maitre d' and reserved for a dog"){
        test_game game;
        const int table_id = 224;
        const int dog_id = 9;
        const Vector2 position{level_config::edge_weight * 8, level_config::edge_weight * 8};
        game.insert_entity(game.build_table(table_id, position), level_config::draw_layers::stations);
        game.tick(1.0f / 60.0f); // drain registered_table so the maitre d' tracks it
        auto* table = dynamic_cast<entities::table*>(game.find_entity(table_id));
        REQUIRE(table != nullptr);
        REQUIRE(table->reserve_for(dog_id));
        REQUIRE(table->get_state() == entities::table::table_state::reserved);

        WHEN("a dog_reached_station event names the reserved dog and this table"){
            events::dog_reached_station reached{
                static_cast<size_t>(dog_id), static_cast<size_t>(table_id), position};
            event_interface::execute_event(reached);

            THEN("the table transitions to occupied"){
                REQUIRE(table->get_state() == entities::table::table_state::occupied);
            }
        }

        WHEN("a dog_reached_station event names a different dog than the one reserved"){
            events::dog_reached_station reached{
                static_cast<size_t>(dog_id) + 1, static_cast<size_t>(table_id), position};
            event_interface::execute_event(reached);

            THEN("the table stays reserved"){
                REQUIRE(table->get_state() == entities::table::table_state::reserved);
            }
        }
    }
}

SCENARIO("table::occupy() and table::clear() drive the station's interacting state",
        "[station][table][occupy]"){
    GIVEN("a table reserved for a dog"){
        test_game game;
        const int table_id = 222;
        const Vector2 position{level_config::edge_weight * 8, level_config::edge_weight * 8};
        game.insert_entity(game.build_table(table_id, position), level_config::draw_layers::stations);
        auto* table = dynamic_cast<entities::table*>(game.find_entity(table_id));
        REQUIRE(table != nullptr);
        REQUIRE(table->reserve_for(42));

        WHEN("the table is occupied"){
            table->occupy();
            THEN("its state is occupied and the station is interacting"){
                REQUIRE(table->get_state() == entities::table::table_state::occupied);
                REQUIRE(dynamic_cast<entities::station*>(table)->is_interacting());
            }

            AND_WHEN("the table is cleared"){
                table->clear();
                THEN("its state is available and the station is no longer interacting"){
                    REQUIRE(table->get_state() == entities::table::table_state::available);
                    REQUIRE_FALSE(dynamic_cast<entities::station*>(table)->is_interacting());
                }
            }
        }
    }
}
