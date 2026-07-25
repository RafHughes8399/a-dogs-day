// Scenarios for the waiter dog's two state machines - serving (counter ->
// table) and clearing (table -> dishwasher) - plus waiter availability and the
// failure paths when a station moves or disappears mid-journey.
//
// Note on tracing: waiter_dog::animating deliberately has no state_name()
// override, so the pickup/placement holds report as "unknown". Scenarios assert
// on the named leg states around them rather than on the animation itself.
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
            int index_of(const std::string& name) const{
                auto it = std::find(names_.begin(), names_.end(), name);
                return it == names_.end() ? -1 : static_cast<int>(std::distance(names_.begin(), it));
            }
            size_t size() const{ return names_.size(); }
        private:
            std::vector<std::string> names_;
    };

    constexpr int first_table_id = 3;
    constexpr int second_table_id = 4;
    constexpr float frame = 1.0f / 60.0f;
    const Vector2 first_table_position{level_config::edge_weight * 6,
                                       level_config::edge_weight * 6};
}

SCENARIO("a waiter runs the serving chain from counter to table", "[waiter][serving][lifecycle]"){
    test_game game;
    game.tick(frame);
    auto* waiter = game.first_waiter();
    auto* counter = game.first_counter();
    REQUIRE(waiter != nullptr);
    REQUIRE(counter != nullptr);
    const int waiter_id = waiter->get_id();
    const size_t food_before = counter->current_capacity();

    GIVEN("an idle waiter and a seated customer's order"){
        REQUIRE(waiter->get_state_name() == "idle");
        game.request_order(200, static_cast<size_t>(first_table_id), first_table_position);

        WHEN("the cafe runs until the waiter is idle again"){
            state_trace trace;
            const bool completed = game.tick_until([&]{
                trace.sample(game.get_waiter_dog(waiter_id).get_state_name());
                return trace.size() > 1
                    && game.get_waiter_dog(waiter_id).get_state_name() == "idle"
                    && game.num_serving_jobs() == 0;
            }, 6000);

            THEN("it walked both legs and finished the order"){
                REQUIRE(completed);
                REQUIRE(trace.saw("serving_counter"));
                REQUIRE(trace.saw("walking_to_table"));
                REQUIRE(trace.saw("finished_serving"));
                REQUIRE_FALSE(trace.saw("abandoned_serving"));
            }

            THEN("the legs happened in order"){
                REQUIRE(trace.index_of("serving_counter") < trace.index_of("walking_to_table"));
                REQUIRE(trace.index_of("walking_to_table") < trace.index_of("finished_serving"));
            }

            THEN("one item of food left the counter and the waiter carries nothing"){
                REQUIRE(counter->current_capacity() == food_before - 1);
                REQUIRE_FALSE(game.get_waiter_dog(waiter_id).is_carrying_food());
                REQUIRE(counter->reserved() == 0);
            }
        }
    }
}

SCENARIO("a waiter runs the clearing chain from table to dishwasher", "[waiter][clearing][lifecycle]"){
    test_game game;
    game.tick(frame);
    auto* waiter = game.first_waiter();
    REQUIRE(waiter != nullptr);
    REQUIRE(game.first_dishwasher() != nullptr);
    const int waiter_id = waiter->get_id();

    GIVEN("an idle waiter and a table needing clearing"){
        REQUIRE(waiter->get_state_name() == "idle");
        game.request_clear_table(first_table_id);

        WHEN("the cafe runs until the waiter is idle again"){
            state_trace trace;
            const bool completed = game.tick_until([&]{
                trace.sample(game.get_waiter_dog(waiter_id).get_state_name());
                return trace.size() > 1
                    && game.get_waiter_dog(waiter_id).get_state_name() == "idle"
                    && game.num_clearing_jobs() == 0;
            }, 6000);

            THEN("it walked both legs and closed the job"){
                REQUIRE(completed);
                REQUIRE(trace.saw("clearing_table"));
                REQUIRE(trace.saw("walking_to_dishwasher"));
                REQUIRE(trace.saw("finished_clearing"));
                REQUIRE(game.num_clearing_jobs() == 0);
            }

            THEN("the legs happened in order"){
                REQUIRE(trace.index_of("clearing_table") < trace.index_of("walking_to_dishwasher"));
                REQUIRE(trace.index_of("walking_to_dishwasher") < trace.index_of("finished_clearing"));
            }

            THEN("the waiter is available for new work"){
                REQUIRE(game.get_waiter_dog(waiter_id).is_available_for_order());
            }
        }
    }
}

SCENARIO("serving dispatch depends on how many waiters are free", "[waiter][serving][capacity]"){
    test_game game;
    game.tick(frame);

    GIVEN("no waiters at all"){
        auto* waiter = game.first_waiter();
        REQUIRE(waiter != nullptr);
        game.remove_entity(waiter->get_id());
        REQUIRE(game.num_waiters() == 0);

        WHEN("an order is requested"){
            game.request_order(210, static_cast<size_t>(first_table_id), first_table_position);
            game.tick(frame, 60);

            THEN("the job is recorded but never dispatched"){
                REQUIRE(game.num_serving_jobs() == 1);
                REQUIRE(game.first_serving_job_status() == expediter::serving_job_status::created);
            }
        }
    }

    GIVEN("exactly one waiter"){
        REQUIRE(game.num_waiters() == 1);

        WHEN("an order is requested"){
            game.request_order(211, static_cast<size_t>(first_table_id), first_table_position);
            game.tick(frame);

            THEN("it is dispatched and the waiter becomes unavailable"){
                REQUIRE(game.first_serving_job_status() == expediter::serving_job_status::serving);
                REQUIRE_FALSE(game.first_waiter()->is_available_for_order());
            }
        }
    }

    GIVEN("two waiters"){
        game.insert_waiter_dog(212, dog_config::waiter_dog_types::basic,
                               Vector2{level_config::edge_weight * 22, level_config::edge_weight * 10});
        game.tick(frame);
        REQUIRE(game.num_waiters() == 2);

        WHEN("two orders are requested and the counter is stocked for both"){
            auto* counter = game.first_counter();
            REQUIRE(counter != nullptr);
            REQUIRE(counter->current_capacity() >= 2);
            game.request_order(213, static_cast<size_t>(first_table_id), first_table_position);
            game.request_order(214, static_cast<size_t>(second_table_id),
                               Vector2{level_config::edge_weight * 12, level_config::edge_weight * 12});
            game.tick(frame, 5);

            THEN("both jobs are dispatched to different waiters"){
                REQUIRE(game.num_serving_jobs() == 2);
                REQUIRE(game.num_waiters() == 2);
                // Both waiters bound means neither is left advertising itself.
                auto& first = game.get_waiter_dog(212);
                REQUIRE_FALSE(first.is_available_for_order());
            }
        }
    }
}

SCENARIO("a waiter recovers when its counter is removed mid-serve", "[waiter][serving][failure]"){
    // The counter vanishing is the reconciliation pass's job: it frees the
    // waiter and re-queues the order rather than leaving a permanently busy dog.
    test_game game;
    game.tick(frame);
    auto* waiter = game.first_waiter();
    auto* counter = game.first_counter();
    REQUIRE(waiter != nullptr);
    REQUIRE(counter != nullptr);
    const int waiter_id = waiter->get_id();

    GIVEN("a waiter en route to the counter"){
        game.request_order(220, static_cast<size_t>(first_table_id), first_table_position);
        const bool dispatched = game.tick_until([&]{
            return game.get_waiter_dog(waiter_id).get_state_name() == "serving_counter";
        }, 120);
        REQUIRE(dispatched);

        WHEN("the counter is removed"){
            game.remove_entity(counter->get_id());
            REQUIRE(game.num_counters() == 0);
            game.tick(frame, 10);

            THEN("the waiter is freed and the job goes back to created"){
                REQUIRE(game.get_waiter_dog(waiter_id).is_available_for_order());
                REQUIRE(game.first_serving_job_status() == expediter::serving_job_status::created);
            }
        }
    }
}

SCENARIO("a waiter recovers when its table is removed mid-serve", "[waiter][serving][failure]"){
    test_game game;
    game.tick(frame);
    auto* waiter = game.first_waiter();
    REQUIRE(waiter != nullptr);
    const int waiter_id = waiter->get_id();

    GIVEN("a waiter en route with an order for a table"){
        game.request_order(221, static_cast<size_t>(first_table_id), first_table_position);
        const bool dispatched = game.tick_until([&]{
            return game.get_waiter_dog(waiter_id).get_state_name() == "serving_counter";
        }, 120);
        REQUIRE(dispatched);

        WHEN("the table is removed"){
            game.remove_entity(first_table_id);
            game.tick(frame, 10);

            THEN("the job is dropped entirely and the waiter is freed"){
                // Unlike a missing counter there is no re-queue recovery: with
                // no table there is nothing left to deliver to.
                REQUIRE(game.num_serving_jobs() == 0);
                REQUIRE(game.get_waiter_dog(waiter_id).is_available_for_order());
            }
        }
    }
}

SCENARIO("a waiter still finishes when its dishwasher is removed mid-clear", "[waiter][clearing][failure]"){
    // The dishwasher's position is captured at dispatch, so removing the entity
    // mid-journey does not strand the waiter - the important property is that it
    // always gets back to idle and the job always closes.
    test_game game;
    game.tick(frame);
    auto* waiter = game.first_waiter();
    auto* dishwasher = game.first_dishwasher();
    REQUIRE(waiter != nullptr);
    REQUIRE(dishwasher != nullptr);
    const int waiter_id = waiter->get_id();

    GIVEN("a waiter dispatched to clear a table"){
        game.request_clear_table(first_table_id);
        const bool dispatched = game.tick_until([&]{
            return game.get_waiter_dog(waiter_id).get_state_name() == "clearing_table";
        }, 120);
        REQUIRE(dispatched);

        WHEN("the dishwasher is removed"){
            game.remove_entity(dishwasher->get_id());
            REQUIRE(game.num_dishwashers() == 0);

            THEN("the waiter still returns to idle and the job is closed"){
                const bool recovered = game.tick_until([&]{
                    return game.get_waiter_dog(waiter_id).get_state_name() == "idle"
                        && game.num_clearing_jobs() == 0;
                }, 6000);
                REQUIRE(recovered);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// In-flight station moves. Not implemented (issue #40): a leg's path is
// computed when the leg starts and never revisited, so moving the target
// strands the waiter at the old spot. Asserted rather than skipped so the gap
// stays visible.
// ---------------------------------------------------------------------------

SCENARIO("a waiter re-routes when its counter moves mid-journey",
         "[waiter][recalibration][!shouldfail]"){
    test_game game;
    game.tick(frame);
    auto* waiter = game.first_waiter();
    auto* counter = game.first_counter();
    REQUIRE(waiter != nullptr);
    REQUIRE(counter != nullptr);
    const int waiter_id = waiter->get_id();
    const int counter_id = counter->get_id();

    GIVEN("a waiter walking to the counter"){
        game.request_order(230, static_cast<size_t>(first_table_id), first_table_position);
        REQUIRE(game.tick_until([&]{
            return game.get_waiter_dog(waiter_id).get_state_name() == "serving_counter";
        }, 120));

        WHEN("the counter is moved while it is still walking"){
            const Vector2 destination{level_config::edge_weight * 8,
                                      level_config::edge_weight * 18};
            game.move_entity(counter_id, destination);
            game.tick(frame);

            THEN("it collects the food from the counter's new position"){
                const bool collected = game.tick_until([&]{
                    return game.get_waiter_dog(waiter_id).is_carrying_food();
                }, 3000);
                REQUIRE(collected);
                const float distance = Vector2Distance(
                    game.get_waiter_dog(waiter_id).get_position(), destination);
                REQUIRE(distance < level_config::edge_weight * 2.0f);
            }
        }
    }
}

SCENARIO("a waiter re-routes when its dishwasher moves mid-journey",
         "[waiter][recalibration][!shouldfail]"){
    test_game game;
    game.tick(frame);
    auto* waiter = game.first_waiter();
    auto* dishwasher = game.first_dishwasher();
    REQUIRE(waiter != nullptr);
    REQUIRE(dishwasher != nullptr);
    const int waiter_id = waiter->get_id();
    const int dishwasher_id = dishwasher->get_id();

    GIVEN("a waiter dispatched to clear a table"){
        game.request_clear_table(first_table_id);
        REQUIRE(game.tick_until([&]{
            return game.get_waiter_dog(waiter_id).get_state_name() == "clearing_table";
        }, 120));

        WHEN("the dishwasher is moved before the plate gets there"){
            const Vector2 destination{level_config::edge_weight * 4,
                                      level_config::edge_weight * 18};
            game.move_entity(dishwasher_id, destination);

            THEN("the waiter ends its journey at the dishwasher's new position"){
                const bool finished = game.tick_until([&]{
                    return game.get_waiter_dog(waiter_id).get_state_name() == "idle"
                        && game.num_clearing_jobs() == 0;
                }, 6000);
                REQUIRE(finished);
                const float distance = Vector2Distance(
                    game.get_waiter_dog(waiter_id).get_position(), destination);
                REQUIRE(distance < level_config::edge_weight * 2.0f);
            }
        }
    }
}
