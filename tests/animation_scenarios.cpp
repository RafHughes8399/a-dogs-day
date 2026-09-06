#include <catch2/catch_test_macros.hpp>

#include "ecs_test_game.h"
#include "component.h"
#include "config.h"
#include "events.h"
#include "events_interface.h"
#include "system.h"

// -----------------------------------------------------------------------------
// animation_system - starting, stopping, and hearing that a one-shot ended.
//
// the countdown runs in update rather than off animation::playing(), because
// advance() only steps while the entity is being rendered and these scenarios
// never render. that is the same reason the real game needs it: a one-shot
// started off screen would otherwise never finish.
//
// a dog carries one layer of two direction sprites today, and its sheet declares
// a single frame, so a one-shot here completes on the first tick after the play.
// -----------------------------------------------------------------------------

namespace {

    // RAII, because REQUIRE throws - an unsubscribe after the assertion would be
    // skipped on failure and leak the handler into the next scenario
    template<typename E>
    class listener{
        public:
            listener(std::function<void(const E&)> on_event)
            : handler_(std::move(on_event)){
                event_interface::subscribe<E>(handler_);
            }
            ~listener(){
                event_interface::unsubscribe<E>(handler_);
            }
            listener(const listener&) = delete;
            listener(listener&&) = delete;
            listener& operator=(const listener&) = delete;
            listener& operator=(listener&&) = delete;
        private:
            events::event_handler<E> handler_;
    };

    class finished_recorder{
        public:
            finished_recorder()
            : listener_([this](const events::animation_finished& event) -> void{
                count_++;
                last_id_ = event.get_id();
                last_slot_ = event.get_sprite_slot();
                last_animation_ = event.get_animation_index();
            }){}
            int count_ = 0;
            size_t last_id_ = 0;
            size_t last_slot_ = 0;
            size_t last_animation_ = 0;
        private:
            listener<events::animation_finished> listener_;
    };

    // animation_system::update queues, so the fact is not observable until the
    // dispatcher is pumped - which tick does on its way in
    void flush(){
        events::global_dispatcher_.process_events(0.0f);
    }

    const size_t slot = entity_config::dog_sprite_slots::dog_head;
}

SCENARIO("a one-shot animation announces that it finished", "[animation]"){
    GIVEN("a dog playing a non-repeating animation"){
        testing::ecs_test_game game;
        finished_recorder recorder;
        auto khiri_id = game.create_khiri();

        systems::animation_system::get_instance().play(khiri_id,
            {slot, animation_config::head::eating, false});

        THEN("it is tracked until it ends"){
            REQUIRE(game.in_flight_animation_count() == 1);
            REQUIRE(recorder.count_ == 0);
        }

        WHEN("the countdown runs out"){
            game.tick(0.016f);
            flush();

            THEN("the fact is raised once, naming the entity, slot and animation"){
                REQUIRE(recorder.count_ == 1);
                REQUIRE(recorder.last_id_ == khiri_id);
                REQUIRE(recorder.last_slot_ == slot);
                REQUIRE(recorder.last_animation_ == animation_config::head::eating);
            }
            THEN("it is no longer tracked, and does not fire again"){
                REQUIRE(game.in_flight_animation_count() == 0);
                game.tick(0.016f);
                flush();
                REQUIRE(recorder.count_ == 1);
            }
        }
    }
}

SCENARIO("a repeating animation never announces an end", "[animation]"){
    GIVEN("a dog playing a repeating animation"){
        testing::ecs_test_game game;
        finished_recorder recorder;
        auto khiri_id = game.create_khiri();

        systems::animation_system::get_instance().play(khiri_id,
            {slot, animation_config::head::idle, true});

        THEN("nothing is tracked - a loop ends when its owner stops it"){
            REQUIRE(game.in_flight_animation_count() == 0);
        }

        WHEN("many frames pass"){
            game.tick_until([](){ return false; }, 30, 0.016f);
            flush();

            THEN("no fact is raised"){
                REQUIRE(recorder.count_ == 0);
            }
        }
    }
}

SCENARIO("stopping a one-shot early cancels its announcement", "[animation]"){
    GIVEN("a dog playing a non-repeating animation"){
        testing::ecs_test_game game;
        finished_recorder recorder;
        auto khiri_id = game.create_khiri();

        systems::animation_system::get_instance().play(khiri_id,
            {slot, animation_config::head::eating, false});
        REQUIRE(game.in_flight_animation_count() == 1);

        WHEN("it is stopped before the countdown runs out"){
            systems::animation_system::get_instance().stop(khiri_id, slot);

            THEN("it stops being tracked"){
                REQUIRE(game.in_flight_animation_count() == 0);
            }
            THEN("no fact is raised"){
                game.tick(0.016f);
                flush();
                REQUIRE(recorder.count_ == 0);
            }
        }

        WHEN("the whole entity is stopped"){
            systems::animation_system::get_instance().stop(khiri_id);

            THEN("it stops being tracked"){
                REQUIRE(game.in_flight_animation_count() == 0);
            }
        }
    }
}

SCENARIO("a destroyed entity drops its pending animation", "[animation]"){
    GIVEN("a dog playing a non-repeating animation"){
        testing::ecs_test_game game;
        finished_recorder recorder;
        auto khiri_id = game.create_khiri();

        systems::animation_system::get_instance().play(khiri_id,
            {slot, animation_config::head::eating, false});
        REQUIRE(game.in_flight_animation_count() == 1);

        WHEN("the entity is destroyed"){
            game.remove(khiri_id);

            THEN("its pending animation goes with it"){
                REQUIRE(game.in_flight_animation_count() == 0);
            }
            THEN("no fact is raised for a dead entity"){
                game.tick(0.016f);
                flush();
                REQUIRE(recorder.count_ == 0);
            }
        }
    }
}

SCENARIO("replaying a slot supersedes what was counting down on it", "[animation]"){
    GIVEN("a dog playing a non-repeating animation"){
        testing::ecs_test_game game;
        finished_recorder recorder;
        auto khiri_id = game.create_khiri();

        systems::animation_system::get_instance().play(khiri_id,
            {slot, animation_config::head::eating, false});

        WHEN("a second one-shot starts on the same slot"){
            systems::animation_system::get_instance().play(khiri_id,
                {slot, animation_config::head::bouncing, false});

            THEN("only the second is tracked"){
                REQUIRE(game.in_flight_animation_count() == 1);
            }
            THEN("only the second is announced"){
                game.tick(0.016f);
                flush();
                REQUIRE(recorder.count_ == 1);
                REQUIRE(recorder.last_animation_ == animation_config::head::bouncing);
            }
        }

        WHEN("a repeating animation replaces it on the same slot"){
            systems::animation_system::get_instance().play(khiri_id,
                {slot, animation_config::head::idle, true});

            THEN("nothing is left counting down"){
                REQUIRE(game.in_flight_animation_count() == 0);
            }
        }
    }
}
