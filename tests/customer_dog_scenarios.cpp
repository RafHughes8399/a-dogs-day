// Scenarios for the customer dog's own state machine and the maitre d' flow
// that drives it: arrival -> queued -> walking_to_table -> seated -> eating ->
// leaving -> harvested, plus table availability and in-flight table changes.
//
// These are integration scenarios by nature - a customer only changes state in
// response to the maitre d' seating it and the expediter serving it, so the
// whole cafe runs.
#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <string>
#include <vector>

#include "test_game.h"
#include "events.h"
#include "events_interface.h"
#include "raymath.h" // Vector2Distance

using testing::test_game;

namespace{
    // Records each distinct state a dog passes through so a scenario can assert
    // the whole journey and its ordering, rather than sampling one instant and
    // hoping the tick count landed there.
    class state_trace{
        public:
            void sample(const std::string& name){
                if(names_.empty() || names_.back() != name){
                    names_.push_back(name);
                }
            }
            bool saw(const std::string& name) const{
                return std::find(names_.begin(), names_.end(), name) != names_.end();
            }
            // Position in visit order, or -1 if never seen - lets a test assert
            // "a happened before b" instead of only "both happened".
            int index_of(const std::string& name) const{
                auto it = std::find(names_.begin(), names_.end(), name);
                return it == names_.end() ? -1 : static_cast<int>(std::distance(names_.begin(), it));
            }
        private:
            std::vector<std::string> names_;
    };

    // The main level's two tables, in build order (level::build_main_level).
    constexpr int first_table_id = 3;
    constexpr int second_table_id = 4;
    constexpr float frame = 1.0f / 60.0f;
}

SCENARIO("an arriving customer is tracked and queued", "[customer][maitre_d][arrival]"){
    test_game game;
    game.tick(frame); // drain the main level's registrations

    GIVEN("no customers yet"){
        REQUIRE(game.num_tracked_customers() == 0);

        WHEN("a customer arrives"){
            game.customer_arrives();
            // The arrival queues build_customer_dog; the level builds the entity
            // and fires customer_dog_created, so it takes a tick to exist.
            const bool spawned = game.tick_until([&]{ return game.first_customer() != nullptr; }, 60);

            THEN("the maitre d' tracks it and puts it in the queue"){
                REQUIRE(spawned);
                REQUIRE(game.num_tracked_customers() == 1);
                REQUIRE(game.num_customers() == 1);
            }

            THEN("it starts in its default (unseated) state"){
                auto* customer = game.first_customer();
                REQUIRE(customer != nullptr);
                REQUIRE(customer->get_state_name() == "default_state");
            }
        }
    }
}

SCENARIO("a customer runs the full seat-eat-leave cycle", "[customer][lifecycle][integration]"){
    // The whole journey end to end. The main level supplies the tables, counter,
    // food and waiter, so this also exercises the serving chain incidentally.
    test_game game;
    game.tick(frame);

    GIVEN("a customer arrives at an empty cafe"){
        game.customer_arrives();
        REQUIRE(game.tick_until([&]{ return game.first_customer() != nullptr; }, 60));
        auto* customer = game.first_customer();
        REQUIRE(customer != nullptr);
        const int customer_id = customer->get_id();

        WHEN("the cafe runs until the customer is gone"){
            state_trace trace;
            // Tracked via the maitre d's pointer, not find_entity: the level's
            // id map keeps a dangling entry for a self-removing entity (see the
            // "level forgets" scenario below), so reading through it would
            // dereference freed memory once the customer is harvested.
            const bool departed = game.tick_until([&]{
                auto* live = game.first_customer();
                if(live == nullptr){
                    return true; // harvested after leaving
                }
                trace.sample(live->get_state_name());
                return false;
            }, 6000);
            (void)customer_id;

            THEN("it was seated, ate, and left"){
                REQUIRE(departed);
                REQUIRE(trace.saw("walking_to_table"));
                REQUIRE(trace.saw("seated"));
                REQUIRE(trace.saw("eating"));
                REQUIRE(trace.saw("leaving"));
            }

            THEN("it passed through those states in order"){
                REQUIRE(trace.index_of("walking_to_table") < trace.index_of("seated"));
                REQUIRE(trace.index_of("seated") < trace.index_of("eating"));
                REQUIRE(trace.index_of("eating") < trace.index_of("leaving"));
            }

            THEN("the maitre d' stopped tracking it"){
                REQUIRE(game.num_tracked_customers() == 0);
            }
        }
    }
}

SCENARIO("the level forgets an entity that removes itself",
         "[customer][level][lifetime][!shouldfail]"){
    // NOT implemented, and a use-after-free rather than a cosmetic gap.
    // level::update harvests entities that return status_codes::dead via the
    // quadtree graveyard and cleans the render layers, but never erases them
    // from id_entity_map_ - only on_removed_entity (the explicit remove_entity
    // path) does that, at level.cpp:324. A customer dog removes ITSELF at the
    // end of `leaving`, so its raw pointer outlives the object in the map that
    // level::get_entity reads. on_send_dog_to_position, on_send_dog_to_station
    // and on_order_served all resolve dogs through that map.
    //
    // Asserted by pointer comparison only - dereferencing the stale entry is
    // what crashes.
    test_game game;
    game.tick(frame);

    GIVEN("a customer that has run its whole cycle and left"){
        game.customer_arrives();
        REQUIRE(game.tick_until([&]{ return game.first_customer() != nullptr; }, 60));
        const int customer_id = game.first_customer()->get_id();

        const bool departed = game.tick_until([&]{
            return game.first_customer() == nullptr;
        }, 6000);
        REQUIRE(departed);

        THEN("the level no longer resolves its id"){
            REQUIRE(game.find_entity(customer_id) == nullptr);
        }
    }
}

SCENARIO("a customer is only seated when a table is free", "[customer][maitre_d][tables]"){
    test_game game;
    game.tick(frame);

    GIVEN("no tables at all"){
        game.remove_entity(first_table_id);
        game.remove_entity(second_table_id);
        REQUIRE(game.num_tables() == 0);

        WHEN("a customer arrives"){
            game.customer_arrives();
            REQUIRE(game.tick_until([&]{ return game.first_customer() != nullptr; }, 60));
            game.tick(frame, 300);

            THEN("it stays queued and unseated"){
                auto* customer = game.first_customer();
                REQUIRE(customer != nullptr);
                REQUIRE(game.num_customers() == 1);
                REQUIRE(customer->get_state_name() == "default_state");
            }
        }
    }

    GIVEN("exactly one table"){
        game.remove_entity(second_table_id);
        REQUIRE(game.num_tables() == 1);

        WHEN("a customer arrives"){
            game.customer_arrives();
            REQUIRE(game.tick_until([&]{ return game.first_customer() != nullptr; }, 60));
            auto* customer = game.first_customer();
            REQUIRE(customer != nullptr);
            const int customer_id = customer->get_id();

            const bool seated = game.tick_until([&]{
                return game.find_entity(customer_id) != nullptr
                    && game.get_customer_dog(customer_id).get_state_name() == "seated";
            }, 3000);

            THEN("it is seated at the remaining table and leaves the queue"){
                REQUIRE(seated);
                REQUIRE(game.num_customers() == 0);
                auto* table = game.find_table(first_table_id);
                REQUIRE(table != nullptr);
                // The dog enters `seated` on the frame it fires the queued
                // dog_reached_station; the maitre d' occupies the table when
                // that event is processed, one tick later.
                REQUIRE(game.tick_until([&]{ return table->is_interacting(); }, 10));
            }
        }
    }

    GIVEN("two tables and two customers"){
        REQUIRE(game.num_tables() == 2);

        WHEN("both customers arrive"){
            game.customer_arrives();
            REQUIRE(game.tick_until([&]{ return game.num_tracked_customers() == 1; }, 60));
            game.customer_arrives();
            REQUIRE(game.tick_until([&]{ return game.num_tracked_customers() == 2; }, 60));

            const bool both_seated = game.tick_until([&]{
                auto* first = game.find_table(first_table_id);
                auto* second = game.find_table(second_table_id);
                return first != nullptr && second != nullptr
                    && first->is_interacting() && second->is_interacting();
            }, 4000);

            THEN("both are seated, one per table, and the queue drains"){
                REQUIRE(both_seated);
                REQUIRE(game.num_customers() == 0);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// In-flight station changes. Both of these describe behaviour that is NOT
// implemented yet - a path is computed once at dispatch and never revisited
// (filed as issue #40), and nothing tells a walking customer its table is gone.
// They are written as real assertions rather than skips so the gap is visible
// and they flip to green the moment the behaviour lands.
// ---------------------------------------------------------------------------

SCENARIO("a customer re-routes when its table moves mid-journey",
         "[customer][recalibration][!shouldfail]"){
    test_game game;
    game.tick(frame);
    game.remove_entity(second_table_id); // force a known table

    GIVEN("a customer walking to its assigned table"){
        game.customer_arrives();
        REQUIRE(game.tick_until([&]{ return game.first_customer() != nullptr; }, 60));
        auto* customer = game.first_customer();
        REQUIRE(customer != nullptr);
        const int customer_id = customer->get_id();

        const bool walking = game.tick_until([&]{
            return game.find_entity(customer_id) != nullptr
                && game.get_customer_dog(customer_id).get_state_name() == "walking_to_table";
        }, 3000);
        REQUIRE(walking);

        WHEN("the table is moved while it is still walking"){
            const Vector2 destination{level_config::edge_weight * 16,
                                      level_config::edge_weight * 4};
            game.move_entity(first_table_id, destination);
            game.tick(frame);

            const bool seated = game.tick_until([&]{
                return game.find_entity(customer_id) != nullptr
                    && game.get_customer_dog(customer_id).get_state_name() == "seated";
            }, 3000);

            THEN("it arrives at the table's new position"){
                REQUIRE(seated);
                auto* table = game.find_table(first_table_id);
                REQUIRE(table != nullptr);
                // Seated means physically at the table, so the customer should
                // be within an interaction node of wherever the table now is.
                const float distance = Vector2Distance(
                    game.get_customer_dog(customer_id).get_position(), table->get_position());
                REQUIRE(distance < level_config::edge_weight * 2.0f);
            }
        }
    }
}

SCENARIO("a customer gives up when its table is removed mid-journey",
         "[customer][failure][!shouldfail]"){
    test_game game;
    game.tick(frame);
    game.remove_entity(second_table_id);

    GIVEN("a customer walking to its assigned table"){
        game.customer_arrives();
        REQUIRE(game.tick_until([&]{ return game.first_customer() != nullptr; }, 60));
        auto* customer = game.first_customer();
        REQUIRE(customer != nullptr);
        const int customer_id = customer->get_id();

        const bool walking = game.tick_until([&]{
            return game.find_entity(customer_id) != nullptr
                && game.get_customer_dog(customer_id).get_state_name() == "walking_to_table";
        }, 3000);
        REQUIRE(walking);

        WHEN("the table is removed out from under it"){
            game.remove_entity(first_table_id);
            REQUIRE(game.num_tables() == 0);

            THEN("it turns around and leaves instead of walking to nothing"){
                const bool left = game.tick_until([&]{
                    auto* live = game.find_entity(customer_id);
                    return live == nullptr
                        || game.get_customer_dog(customer_id).get_state_name() == "leaving";
                }, 3000);
                REQUIRE(left);
            }
        }
    }
}
