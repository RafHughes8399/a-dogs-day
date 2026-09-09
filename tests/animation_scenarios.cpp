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

    const float part_width = 20.0f;
    const float part_height = 24.0f;

    size_t build_part_entity(testing::ecs_test_game& game){
        auto id = game.create_empty(level_config::draw_layers::dogs);
        std::vector<sprite::sprite> sprites;
        sprites.push_back(sprite::sprite(Texture2D{},
            animation_builders::build_dog_head_animations(part_width, part_height)));
        std::vector<components::renderable_component::sprite_layer> layers = {
            component_builders::build_sprite_layer(sprites, 0)};
        component_helpers::add_renderable_component(id, layers);
        return id;
    }
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

SCENARIO("a dog part carries one animation per row of its sheet", "[animation]"){
    GIVEN("a head sprite built from the head animation builders"){
        auto head = sprite::sprite(Texture2D{},
            animation_builders::build_dog_head_animations(part_width, part_height));

        THEN("every row the head declares has an animation, and idle is active"){
            REQUIRE(head.num_animations() == animation_config::head::size);
            REQUIRE(head.get_animation_index() == animation_config::head::idle);
            REQUIRE(head.get_animation().num_frames() == animation_config::idle_frames);
            REQUIRE(head.get_animation().get_play_speed() == animation_config::idle_play_speed);
        }

        WHEN("a longer row is made active"){
            head.set_animation(animation_config::head::walking);

            THEN("it brings its own frame count and play speed"){
                REQUIRE(head.get_animation().num_frames() == animation_config::walking_frames);
                REQUIRE(head.get_animation().get_play_speed() == animation_config::walking_play_speed);
            }
            THEN("it samples its own row of the sheet"){
                REQUIRE(head.get_animation().get_frame().y
                    == part_height * static_cast<float>(animation_config::head::walking));
            }
        }

        WHEN("a part specific row is made active"){
            head.set_animation(animation_config::head::pinned_back);

            THEN("the part row is shorter than the shared ones"){
                REQUIRE(head.get_animation().num_frames() == animation_config::head_pinned_back_frames);
                REQUIRE(head.get_animation().num_frames() < animation_config::walking_frames);
            }
        }

        WHEN("a row the sheet does not have is asked for"){
            head.set_animation(animation_config::head::size + 3);

            THEN("the active animation is left alone"){
                REQUIRE(head.get_animation_index() == animation_config::head::idle);
            }
        }
    }
}

SCENARIO("the parts of one dog agree on the shared rows", "[animation]"){
    GIVEN("every part of a dog"){
        auto head = sprite::sprite(Texture2D{},
            animation_builders::build_dog_head_animations(part_width, part_height));
        auto face = sprite::sprite(Texture2D{},
            animation_builders::build_dog_face_animations(part_width, part_height));
        auto body = sprite::sprite(Texture2D{},
            animation_builders::build_dog_body_animations(part_width, part_height));
        auto tail = sprite::sprite(Texture2D{},
            animation_builders::build_dog_tail_animations(part_width, part_height));

        THEN("each declares as many animations as its own enum"){
            REQUIRE(head.num_animations() == animation_config::head::size);
            REQUIRE(face.num_animations() == animation_config::face::size);
            REQUIRE(body.num_animations() == animation_config::body::size);
            REQUIRE(tail.num_animations() == animation_config::tail::size);
        }

        WHEN("a whole body action plays with one index"){
            head.set_animation(animation_config::shared::eating);
            face.set_animation(animation_config::shared::eating);
            body.set_animation(animation_config::shared::eating);
            tail.set_animation(animation_config::shared::eating);

            THEN("all four parts run the same length at the same speed"){
                REQUIRE(head.get_animation().num_frames() == animation_config::eating_frames);
                REQUIRE(face.get_animation().num_frames() == animation_config::eating_frames);
                REQUIRE(body.get_animation().num_frames() == animation_config::eating_frames);
                REQUIRE(tail.get_animation().num_frames() == animation_config::eating_frames);

                REQUIRE(head.get_animation().get_play_speed() == animation_config::eating_play_speed);
                REQUIRE(tail.get_animation().get_play_speed() == animation_config::eating_play_speed);
            }
        }
    }
}

SCENARIO("a one-shot runs for its own frames and play speed", "[animation]"){
    GIVEN("an entity whose slot holds a ragged sheet"){
        testing::ecs_test_game game;
        finished_recorder recorder;
        auto id = build_part_entity(game);

        WHEN("a part specific row plays once"){
            systems::animation_system::get_instance().play(id,
                {0, animation_config::head::bouncing, false});

            const int ticks = animation_config::head_bouncing_frames
                * animation_config::head_bouncing_play_speed;

            THEN("it is still counting down a tick before its frames have played"){
                game.tick_until([](){ return false; }, ticks - 1, 0.016f);
                flush();

                REQUIRE(recorder.count_ == 0);
                REQUIRE(game.in_flight_animation_count() == 1);
            }
            THEN("it ends after frames times play speed ticks"){
                game.tick_until([](){ return false; }, ticks, 0.016f);
                flush();

                REQUIRE(recorder.count_ == 1);
                REQUIRE(recorder.last_id_ == id);
                REQUIRE(recorder.last_animation_ == animation_config::head::bouncing);
            }
        }

        WHEN("a shorter row plays once"){
            systems::animation_system::get_instance().play(id,
                {0, animation_config::head::pinned_back, false});

            THEN("its countdown is its own, not the sheet's longest"){
                const int ticks = animation_config::head_pinned_back_frames
                    * animation_config::head_pinned_back_play_speed;
                game.tick_until([](){ return false; }, ticks, 0.016f);
                flush();

                REQUIRE(recorder.count_ == 1);
                REQUIRE(recorder.last_animation_ == animation_config::head::pinned_back);
            }
        }
    }
}
