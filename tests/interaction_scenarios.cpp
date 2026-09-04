#include <catch2/catch_test_macros.hpp>

#include <vector>

#include "component.h"
#include "config.h"
#include "ecs_test_game.h"
#include "raylib.h"
#include "system.h"

namespace{
    Vector2 in_cafe(float x, float y){
        return Vector2{level_config::cafe_x + x, level_config::cafe_y + y};
    }

    class recorder{
    public:
        std::vector<size_t> interactors_;
        std::vector<size_t> interactees_;
        std::vector<float> deltas_;
        size_t calls() const{ return interactors_.size(); }
    };

    std::function<void(size_t, size_t, float)> record_into(recorder& log){
        return [&log](size_t interactor, size_t interactee, float delta) -> void{
            log.interactors_.push_back(interactor);
            log.interactees_.push_back(interactee);
            log.deltas_.push_back(delta);
        };
    }

    bool walk_to(testing::ecs_test_game& game, size_t dog_id, size_t station_id){
        game.path_to(dog_id, Vector2Zero(), station_id);
        return game.tick_until([&game, dog_id](){
            return game.queued_path_count(dog_id) == 0;
        }, 2000);
    }
}

SCENARIO("an interaction pairs the ids it was built from",
        "[ecs][interaction]"){
    GIVEN("a table and a customer that has claimed it"){
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto customer_id = game.create_customer_dog(in_cafe(640.0f, 640.0f));
        game.claim(customer_id, table_id);

        WHEN("an interaction is built for the pair"){
            auto& system = systems::interaction_system::get_instance();
            auto built = system.create_interaction(customer_id, table_id);

            THEN("it carries the interactor and the interactee the right way round"){
                REQUIRE(built.get_interactor() == customer_id);
                REQUIRE(built.get_interactee() == table_id);
            }
            THEN("its performable list is the intersection of the two participation lists"){
                auto performable = built.get_performable_interactions();
                REQUIRE(performable.size() == 1);
                REQUIRE(performable.front()
                    == static_cast<size_t>(interaction_config::customer_table_sit));
            }
        }
    }
    GIVEN("a table and a waiter that has claimed it"){
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto waiter_id = game.create_waiter_dog(in_cafe(640.0f, 640.0f));
        game.claim(waiter_id, table_id);

        WHEN("an interaction is built for the pair"){
            auto built = systems::interaction_system::get_instance()
                .create_interaction(waiter_id, table_id);

            THEN("only the serve interaction is shared"){
                auto performable = built.get_performable_interactions();
                REQUIRE(performable.size() == 1);
                REQUIRE(performable.front()
                    == static_cast<size_t>(interaction_config::waiter_table_serve));
            }
        }
    }
    GIVEN("a station carrying no participation list"){
        testing::ecs_test_game game;
        auto counter_id = game.create_food_counter(in_cafe(320.0f, 320.0f));
        auto customer_id = game.create_customer_dog(in_cafe(640.0f, 640.0f));

        WHEN("an interaction is built against it"){
            auto built = systems::interaction_system::get_instance()
                .create_interaction(customer_id, counter_id);

            THEN("nothing is performable"){
                REQUIRE(built.get_performable_interactions().empty());
            }
        }
    }
}

SCENARIO("a customer walking to its claimed table raises an interaction on overlap",
        "[ecs][interaction][customer]"){
    GIVEN("a claimed table and a customer standing well clear of it"){
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto customer_id = game.create_customer_dog(in_cafe(1024.0f, 1024.0f));
        game.claim(customer_id, table_id);

        THEN("the boxes do not overlap yet and no interaction exists"){
            REQUIRE_FALSE(game.interaction_boxes_overlap(customer_id, table_id));
            game.tick(0.016f);
            REQUIRE(game.interaction_count() == 0);
        }

        WHEN("it walks to the table"){
            REQUIRE(walk_to(game, customer_id, table_id));

            THEN("the interaction boxes overlap"){
                REQUIRE(game.interaction_boxes_overlap(customer_id, table_id));
            }
            THEN("exactly one interaction is held, naming the two entities"){
                REQUIRE(game.interaction_count() == 1);
                REQUIRE(game.has_interaction(customer_id, table_id));
                REQUIRE_FALSE(game.has_interaction(table_id, customer_id));
            }
            THEN("its performable list holds only customer_table_sit"){
                auto performable = game.performable_interactions_of(customer_id, table_id);
                REQUIRE(performable.size() == 1);
                REQUIRE(performable.front()
                    == static_cast<size_t>(interaction_config::customer_table_sit));
            }
            THEN("further frames do not raise a second interaction for the same pair"){
                for(int frame = 0; frame < 10; ++frame){ game.tick(0.016f); }
                REQUIRE(game.interaction_count() == 1);
            }
            THEN("further moves inside range do not raise a second interaction"){
                auto seat = game.hitbox_of(customer_id);
                for(int nudge = 0; nudge < 5; ++nudge){
                    game.move_entity(customer_id, Vector2{seat.x + static_cast<float>(nudge),
                        seat.y});
                }
                REQUIRE(game.interaction_boxes_overlap(customer_id, table_id));
                REQUIRE(game.interaction_count() == 1);
            }
        }
    }
    GIVEN("a claimed table and a customer already standing on it"){
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto customer_id = game.create_customer_dog(in_cafe(320.0f, 320.0f));
        game.claim(customer_id, table_id);

        // * pairing hangs off move_entity, so a claim taken while both entities
        // * are already stationary and overlapping raises nothing - the trigger
        // * for that case is a started_interacting event that does not exist yet
        THEN("the boxes overlap but no move has fired, so no interaction forms"){
            REQUIRE(game.interaction_boxes_overlap(customer_id, table_id));
            for(int frame = 0; frame < 10; ++frame){ game.tick(0.016f); }
            REQUIRE(game.interaction_count() == 0);
        }

        WHEN("anything moves it, even by a pixel"){
            auto box = game.hitbox_of(customer_id);
            game.move_entity(customer_id, Vector2{box.x + 1.0f, box.y});

            THEN("the pair is picked up"){
                REQUIRE(game.interaction_count() == 1);
                REQUIRE(game.has_interaction(customer_id, table_id));
            }
        }
    }
    GIVEN("a table the customer never claimed"){
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto customer_id = game.create_customer_dog(in_cafe(1024.0f, 1024.0f));

        WHEN("it is moved on top of the table anyway"){
            game.move_entity(customer_id, in_cafe(320.0f, 320.0f));
            game.tick(0.016f);

            THEN("the boxes overlap but no interaction forms"){
                REQUIRE(game.interaction_boxes_overlap(customer_id, table_id));
                REQUIRE(game.interaction_count() == 0);
            }
        }
    }
}

SCENARIO("a waiter walking to its claimed table raises an interaction on overlap",
        "[ecs][interaction][waiter]"){
    GIVEN("a claimed table and a waiter standing well clear of it"){
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto waiter_id = game.create_waiter_dog(in_cafe(1024.0f, 1024.0f));
        game.claim(waiter_id, table_id);

        WHEN("it walks to the table"){
            REQUIRE(walk_to(game, waiter_id, table_id));

            THEN("one interaction is held, naming the waiter and the table"){
                REQUIRE(game.interaction_boxes_overlap(waiter_id, table_id));
                REQUIRE(game.interaction_count() == 1);
                REQUIRE(game.has_interaction(waiter_id, table_id));
            }
            THEN("its performable list holds only waiter_table_serve"){
                auto performable = game.performable_interactions_of(waiter_id, table_id);
                REQUIRE(performable.size() == 1);
                REQUIRE(performable.front()
                    == static_cast<size_t>(interaction_config::waiter_table_serve));
            }
        }
    }
}

SCENARIO("processing dispatches every performable index with the pair's ids and the frame delta",
        "[ecs][interaction][processing]"){
    GIVEN("a seated customer and a recorder on both behaviours"){
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto customer_id = game.create_customer_dog(in_cafe(1024.0f, 1024.0f));
        game.claim(customer_id, table_id);
        REQUIRE(walk_to(game, customer_id, table_id));

        recorder sit;
        recorder serve;
        game.set_interaction_behaviour(interaction_config::customer_table_sit, record_into(sit));
        game.set_interaction_behaviour(interaction_config::waiter_table_serve, record_into(serve));

        WHEN("a single frame is ticked"){
            game.tick(0.5f);

            THEN("customer_table_sit runs once with the interactor, the interactee and the delta"){
                REQUIRE(sit.calls() == 1);
                REQUIRE(sit.interactors_.front() == customer_id);
                REQUIRE(sit.interactees_.front() == table_id);
                REQUIRE(sit.deltas_.front() == 0.5f);
            }
            THEN("waiter_table_serve is never reached"){
                REQUIRE(serve.calls() == 0);
            }
        }
        WHEN("three frames are ticked"){
            game.tick(0.25f);
            game.tick(0.25f);
            game.tick(0.25f);

            THEN("the behaviour runs once per frame, every call naming the same pair"){
                REQUIRE(sit.calls() == 3);
                for(size_t call = 0; call < sit.calls(); ++call){
                    REQUIRE(sit.interactors_[call] == customer_id);
                    REQUIRE(sit.interactees_[call] == table_id);
                    REQUIRE(sit.deltas_[call] == 0.25f);
                }
            }
        }
    }
    GIVEN("a seated waiter and a recorder on both behaviours"){
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto waiter_id = game.create_waiter_dog(in_cafe(1024.0f, 1024.0f));
        game.claim(waiter_id, table_id);
        REQUIRE(walk_to(game, waiter_id, table_id));

        recorder sit;
        recorder serve;
        game.set_interaction_behaviour(interaction_config::customer_table_sit, record_into(sit));
        game.set_interaction_behaviour(interaction_config::waiter_table_serve, record_into(serve));

        WHEN("a single frame is ticked"){
            game.tick(0.5f);

            THEN("waiter_table_serve runs once with the waiter, the table and the delta"){
                REQUIRE(serve.calls() == 1);
                REQUIRE(serve.interactors_.front() == waiter_id);
                REQUIRE(serve.interactees_.front() == table_id);
                REQUIRE(serve.deltas_.front() == 0.5f);
            }
            THEN("customer_table_sit is never reached"){
                REQUIRE(sit.calls() == 0);
            }
        }
    }
    GIVEN("a customer and a waiter sharing one table"){
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto customer_id = game.create_customer_dog(in_cafe(1024.0f, 1024.0f));
        auto waiter_id = game.create_waiter_dog(in_cafe(1024.0f, 1216.0f));
        game.claim(customer_id, table_id);
        game.claim(waiter_id, table_id);
        REQUIRE(walk_to(game, customer_id, table_id));
        REQUIRE(walk_to(game, waiter_id, table_id));

        recorder sit;
        recorder serve;
        game.set_interaction_behaviour(interaction_config::customer_table_sit, record_into(sit));
        game.set_interaction_behaviour(interaction_config::waiter_table_serve, record_into(serve));

        WHEN("a frame is ticked"){
            game.tick(0.5f);

            THEN("two interactions are held, one per dog"){
                REQUIRE(game.interaction_count() == 2);
                REQUIRE(game.has_interaction(customer_id, table_id));
                REQUIRE(game.has_interaction(waiter_id, table_id));
            }
            THEN("each behaviour fires once, for its own dog only"){
                REQUIRE(sit.calls() == 1);
                REQUIRE(sit.interactors_.front() == customer_id);
                REQUIRE(serve.calls() == 1);
                REQUIRE(serve.interactors_.front() == waiter_id);
            }
        }
    }
}

SCENARIO("an interaction is cancelled when either half stops being true",
        "[ecs][interaction][cleanup]"){
    GIVEN("a seated customer holding an interaction"){
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto customer_id = game.create_customer_dog(in_cafe(1024.0f, 1024.0f));
        game.claim(customer_id, table_id);
        REQUIRE(walk_to(game, customer_id, table_id));
        REQUIRE(game.interaction_count() == 1);

        WHEN("the customer is destroyed"){
            game.remove(customer_id);

            THEN("the pending interaction is dropped without waiting for a tick"){
                REQUIRE(game.interaction_count() == 0);
            }
        }
        WHEN("the table is destroyed"){
            game.remove(table_id);

            THEN("the pending interaction is dropped"){
                REQUIRE(game.interaction_count() == 0);
            }
        }
        WHEN("the claim is released"){
            game.unclaim(customer_id, table_id);
            game.tick(0.016f);

            THEN("the interaction is dropped on the next frame"){
                REQUIRE(game.interaction_count() == 0);
            }
            THEN("no behaviour runs for it"){
                recorder sit;
                game.set_interaction_behaviour(interaction_config::customer_table_sit, record_into(sit));
                game.tick(0.016f);
                REQUIRE(sit.calls() == 0);
            }
        }
        WHEN("the customer is moved out of range"){
            auto seat = game.hitbox_of(customer_id);
            game.move_entity(customer_id, in_cafe(1024.0f, 1024.0f));

            THEN("the pair is dropped on the move itself, without waiting for a tick"){
                REQUIRE_FALSE(game.interaction_boxes_overlap(customer_id, table_id));
                REQUIRE(game.interaction_count() == 0);
            }
            THEN("no behaviour runs for it"){
                recorder sit;
                game.set_interaction_behaviour(interaction_config::customer_table_sit, record_into(sit));
                game.tick(0.016f);
                REQUIRE(sit.calls() == 0);
            }
            THEN("walking back into range raises it again, the claim never having moved"){
                game.move_entity(customer_id, Vector2{seat.x, seat.y});
                REQUIRE(game.interaction_count() == 1);
                REQUIRE(game.has_interaction(customer_id, table_id));
            }
        }
        WHEN("the table is moved out from under the customer"){
            game.move_entity(table_id, in_cafe(1024.0f, 1024.0f));

            THEN("the pair is dropped, the mover being the interactable"){
                REQUIRE_FALSE(game.interaction_boxes_overlap(customer_id, table_id));
                REQUIRE(game.interaction_count() == 0);
            }
        }
        WHEN("an unrelated entity moves"){
            auto bystander_id = game.create_customer_dog(in_cafe(1024.0f, 1024.0f));
            game.move_entity(bystander_id, in_cafe(1088.0f, 1024.0f));

            THEN("the pair is untouched"){
                REQUIRE(game.interaction_count() == 1);
                REQUIRE(game.has_interaction(customer_id, table_id));
            }
        }
    }
    GIVEN("a destroyed entity that never had an interaction"){
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto customer_id = game.create_customer_dog(in_cafe(1024.0f, 1024.0f));
        game.claim(customer_id, table_id);
        auto bystander_id = game.create_customer_dog(in_cafe(1216.0f, 1024.0f));
        REQUIRE(walk_to(game, customer_id, table_id));
        REQUIRE(game.interaction_count() == 1);

        WHEN("the bystander is destroyed"){
            game.remove(bystander_id);

            THEN("the unrelated interaction is untouched"){
                REQUIRE(game.interaction_count() == 1);
                REQUIRE(game.has_interaction(customer_id, table_id));
            }
        }
    }
}
