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

    void nudge(testing::ecs_test_game& game, size_t entity_id){
        auto box = game.hitbox_of(entity_id);
        game.move_entity(entity_id, Vector2{box.x + 1.0f, box.y});
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
            auto built = systems::interaction_system::get_instance()
                .create_interaction(customer_id, table_id);

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

SCENARIO("moving a claimed interactor raises an interaction for it and its target",
        "[ecs][interaction][detection]"){
    GIVEN("a claimed table and a customer"){
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto customer_id = game.create_customer_dog(in_cafe(640.0f, 640.0f));
        game.claim(customer_id, table_id);

        THEN("nothing is held before anything moves"){
            REQUIRE(game.interaction_count() == 0);
        }

        WHEN("the customer moves"){
            nudge(game, customer_id);

            THEN("one interaction is held, naming the customer and the table"){
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
        }
    }
    GIVEN("a claimed table and a waiter"){
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto waiter_id = game.create_waiter_dog(in_cafe(640.0f, 640.0f));
        game.claim(waiter_id, table_id);

        WHEN("the waiter moves"){
            nudge(game, waiter_id);

            THEN("one interaction is held, naming the waiter and the table"){
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
    GIVEN("a customer that has claimed nothing"){
        testing::ecs_test_game game;
        game.create_table(in_cafe(320.0f, 320.0f));
        auto customer_id = game.create_customer_dog(in_cafe(640.0f, 640.0f));

        WHEN("it moves"){
            nudge(game, customer_id);

            THEN("no interaction is raised, the guard on the target holding"){
                REQUIRE(game.interaction_count() == 0);
            }
        }
    }
    GIVEN("a customer targeting an entity with no interactable component"){
        testing::ecs_test_game game;
        auto decoration_id = game.create_test_decoration(in_cafe(320.0f, 320.0f));
        auto customer_id = game.create_customer_dog(in_cafe(640.0f, 640.0f));
        component_managers::interactor_manager_.get_component(customer_id)
            ->interact_with(decoration_id);

        WHEN("it moves"){
            nudge(game, customer_id);

            THEN("no interaction is raised, the guard on the interactable holding"){
                REQUIRE(game.interaction_count() == 0);
            }
        }
    }
    GIVEN("an entity with no interactor component"){
        testing::ecs_test_game game;
        auto decoration_id = game.create_test_decoration(in_cafe(320.0f, 320.0f));

        WHEN("it moves"){
            nudge(game, decoration_id);

            THEN("no interaction is raised, the guard on the interactor holding"){
                REQUIRE(game.interaction_count() == 0);
            }
        }
    }
    GIVEN("a claimed table and a customer nowhere near it"){
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto customer_id = game.create_customer_dog(in_cafe(2048.0f, 2048.0f));
        game.claim(customer_id, table_id);

        WHEN("the customer moves"){
            nudge(game, customer_id);

            THEN("an interaction is still raised - nothing tests the boxes yet"){
                REQUIRE(game.interaction_count() == 1);
                REQUIRE(game.has_interaction(customer_id, table_id));
            }
        }
        WHEN("the customer moves repeatedly"){
            nudge(game, customer_id);
            nudge(game, customer_id);
            nudge(game, customer_id);

            THEN("one interaction is raised per move - nothing dedupes yet"){
                REQUIRE(game.interaction_count() == 3);
            }
        }
    }
}

SCENARIO("processing dispatches every performable index with the pair's ids and the frame delta",
        "[ecs][interaction][processing]"){
    GIVEN("a recorder on both behaviours and a customer holding a table"){
        recorder sit;
        recorder serve;
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto customer_id = game.create_customer_dog(in_cafe(640.0f, 640.0f));
        game.claim(customer_id, table_id);
        nudge(game, customer_id);
        REQUIRE(game.interaction_count() == 1);

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
    GIVEN("a recorder on both behaviours and a waiter holding a table"){
        recorder sit;
        recorder serve;
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto waiter_id = game.create_waiter_dog(in_cafe(640.0f, 640.0f));
        game.claim(waiter_id, table_id);
        nudge(game, waiter_id);

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
        recorder sit;
        recorder serve;
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto customer_id = game.create_customer_dog(in_cafe(640.0f, 640.0f));
        auto waiter_id = game.create_waiter_dog(in_cafe(768.0f, 640.0f));
        game.claim(customer_id, table_id);
        game.claim(waiter_id, table_id);
        nudge(game, customer_id);
        nudge(game, waiter_id);

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

SCENARIO("destroying an entity cancels the interactions naming it",
        "[ecs][interaction][cleanup]"){
    GIVEN("a customer holding an interaction with a table"){
        testing::ecs_test_game game;
        auto table_id = game.create_table(in_cafe(320.0f, 320.0f));
        auto customer_id = game.create_customer_dog(in_cafe(640.0f, 640.0f));
        game.claim(customer_id, table_id);
        nudge(game, customer_id);
        REQUIRE(game.interaction_count() == 1);

        WHEN("the customer is destroyed"){
            game.remove(customer_id);

            THEN("the pending interaction is dropped without waiting for a tick"){
                REQUIRE(game.interaction_count() == 0);
            }
            THEN("no behaviour runs for it"){
                recorder sit;
                game.set_interaction_behaviour(interaction_config::customer_table_sit, record_into(sit));
                game.tick(0.016f);
                REQUIRE(sit.calls() == 0);
                game.restore_interaction_behaviours();
            }
        }
        WHEN("the table is destroyed"){
            game.remove(table_id);

            THEN("the pending interaction is dropped"){
                REQUIRE(game.interaction_count() == 0);
            }
        }
    }
    GIVEN("two customers, each holding an interaction with its own table"){
        testing::ecs_test_game game;
        auto first_table = game.create_table(in_cafe(320.0f, 320.0f));
        auto second_table = game.create_table(in_cafe(640.0f, 320.0f));
        auto first_customer = game.create_customer_dog(in_cafe(320.0f, 640.0f));
        auto second_customer = game.create_customer_dog(in_cafe(640.0f, 640.0f));
        game.claim(first_customer, first_table);
        game.claim(second_customer, second_table);
        nudge(game, first_customer);
        nudge(game, second_customer);
        REQUIRE(game.interaction_count() == 2);

        WHEN("the first customer is destroyed"){
            game.remove(first_customer);

            THEN("only its own interaction goes"){
                REQUIRE(game.interaction_count() == 1);
                REQUIRE(game.has_interaction(second_customer, second_table));
            }
        }
    }
}
