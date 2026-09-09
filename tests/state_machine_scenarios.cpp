#include <catch2/catch_test_macros.hpp>

#include "component.h"
#include "config.h"
#include "ecs_test_game.h"
#include "events.h"
#include "events_interface.h"
#include "state_machine.h"
#include "system.h"

// -----------------------------------------------------------------------------
// state_machine - the graph itself, and the system that drives it off events.
//
// the graph scenarios build a machine directly so a transition can be asserted
// without a world around it; the system scenarios go through a real dog, whose
// machine is assigned by its builder.
// -----------------------------------------------------------------------------

namespace {

    systems::state_machine_system& machines(){
        return systems::state_machine_system::get_instance();
    }

    void flush(){
        events::global_dispatcher_.process_events(0.0f);
    }

    template<typename E, typename... Args>
    void raise(Args&&... args){
        std::unique_ptr<events::event> event = std::make_unique<E>(std::forward<Args>(args)...);
        event_interface::queue_event(event);
        flush();
    }
}

SCENARIO("a machine follows the edges out of its current state", "[state_machine]"){
    GIVEN("a player machine parked in its start state"){
        auto machine = state_machine_builders::build_player_state_machine();

        THEN("it starts idle"){
            REQUIRE(machine.current() == dog_config::player_idle);
            REQUIRE(machine.num_states() == dog_config::player_states_size);
        }

        WHEN("a path is created"){
            machine.transition(0, dog_config::path_created);

            THEN("it walks"){
                REQUIRE(machine.current() == dog_config::player_walking);
            }
            THEN("finishing the path puts it back to idle"){
                machine.transition(0, dog_config::path_finished);
                REQUIRE(machine.current() == dog_config::player_idle);
            }
            THEN("an interaction takes it to interacting"){
                machine.transition(0, dog_config::interaction_started);
                REQUIRE(machine.current() == dog_config::player_interacting);
            }
        }

        WHEN("a transition with no edge out of this state arrives"){
            machine.transition(0, dog_config::order_served);

            THEN("the machine stays where it was"){
                REQUIRE_FALSE(machine.can_transition(dog_config::order_served));
                REQUIRE(machine.current() == dog_config::player_idle);
            }
        }
    }
}

SCENARIO("the customer machine walks its service cycle", "[state_machine]"){
    GIVEN("a customer machine"){
        auto machine = state_machine_builders::build_customer_state_machine();

        THEN("it starts walking"){
            REQUIRE(machine.current() == dog_config::customer_walking);
        }

        WHEN("it is seated, served, and finishes its meal"){
            machine.transition(0, dog_config::interaction_started);
            REQUIRE(machine.current() == dog_config::customer_sitting);

            machine.transition(0, dog_config::order_served);
            REQUIRE(machine.current() == dog_config::customer_eating);

            machine.transition(0, dog_config::meal_finished);

            THEN("it is sitting again, and leaving walks it out"){
                REQUIRE(machine.current() == dog_config::customer_sitting);
                machine.transition(0, dog_config::customer_leaving);
                REQUIRE(machine.current() == dog_config::customer_walking);
            }
        }
    }
}

SCENARIO("the waiter machine splits collecting from serving", "[state_machine]"){
    GIVEN("a waiter machine at its post"){
        auto machine = state_machine_builders::build_waiter_state_machine();

        THEN("it starts stationary"){
            REQUIRE(machine.current() == dog_config::waiter_stationary);
        }

        WHEN("it wanders, reaches a station, and collects food"){
            machine.transition(0, dog_config::path_created);
            REQUIRE(machine.current() == dog_config::waiter_idle);

            machine.transition(0, dog_config::interaction_started);
            REQUIRE(machine.current() == dog_config::waiter_interacting);

            machine.transition(0, dog_config::food_collected, 42);

            THEN("it carries what it was handed"){
                REQUIRE(machine.current() == dog_config::waiter_carrying);
                auto* carrying = dynamic_cast<state::carrying_state*>(machine.get_current_state());
                REQUIRE(carrying != nullptr);
                REQUIRE(carrying->get_carried_item().has_value());
                REQUIRE(carrying->get_carried_item().value() == 42);
            }
            THEN("reaching the table interacts again, and serving returns it to idle"){
                machine.transition(0, dog_config::interaction_started);
                REQUIRE(machine.current() == dog_config::waiter_interacting);

                machine.transition(0, dog_config::order_served);
                REQUIRE(machine.current() == dog_config::waiter_idle);
            }
        }

        WHEN("it leaves the carrying state"){
            machine.transition(0, dog_config::path_created);
            machine.transition(0, dog_config::interaction_started);
            machine.transition(0, dog_config::food_collected, 42);
            machine.transition(0, dog_config::interaction_started);

            THEN("the carried item is let go"){
                machine.transition(0, dog_config::food_collected, 7);
                auto* carrying = dynamic_cast<state::carrying_state*>(machine.get_current_state());
                REQUIRE(carrying != nullptr);
                REQUIRE(carrying->get_carried_item().value() == 7);
            }
        }
    }
}

SCENARIO("a dog is built with the machine for its kind", "[state_machine]"){
    GIVEN("one of each dog"){
        testing::ecs_test_game game;
        auto khiri_id = game.create_khiri();
        auto customer_id = game.create_customer_dog(Vector2{200.0f, 200.0f});
        auto waiter_id = game.create_waiter_dog(Vector2{300.0f, 300.0f});

        THEN("each carries a machine, parked in its own start state"){
            REQUIRE(game.has_state_machine(khiri_id));
            REQUIRE(game.has_state_machine(customer_id));
            REQUIRE(game.has_state_machine(waiter_id));

            REQUIRE(game.state_of(khiri_id).value() == dog_config::player_idle);
            REQUIRE(game.state_of(customer_id).value() == dog_config::customer_walking);
            REQUIRE(game.state_of(waiter_id).value() == dog_config::waiter_stationary);
        }

        WHEN("the dog is destroyed"){
            game.remove(khiri_id);

            THEN("its machine goes with it"){
                REQUIRE_FALSE(game.has_state_machine(khiri_id));
            }
        }
    }
}

SCENARIO("the system transitions the entity the event names", "[state_machine]"){
    GIVEN("a player dog and a customer dog"){
        testing::ecs_test_game game;
        auto khiri_id = game.create_khiri();
        auto customer_id = game.create_customer_dog(Vector2{200.0f, 200.0f});

        WHEN("the player is given a path"){
            raise<events::dog_started_path>(khiri_id);

            THEN("only the player walks"){
                REQUIRE(game.state_of(khiri_id).value() == dog_config::player_walking);
                REQUIRE(game.state_of(customer_id).value() == dog_config::customer_walking);
            }
            THEN("completing the path idles it again"){
                raise<events::dog_completed_path>(khiri_id, Vector2{0.0f, 0.0f});
                REQUIRE(game.state_of(khiri_id).value() == dog_config::player_idle);
            }
        }

        WHEN("an interaction starts for the customer"){
            raise<events::interaction_started>(customer_id, khiri_id);

            THEN("the interactor is the one that transitions"){
                REQUIRE(game.state_of(customer_id).value() == dog_config::customer_sitting);
                REQUIRE(game.state_of(khiri_id).value() == dog_config::player_idle);
            }
        }

        WHEN("an event names an entity with no machine"){
            raise<events::dog_started_path>(9999);

            THEN("nothing moves"){
                REQUIRE(game.state_of(khiri_id).value() == dog_config::player_idle);
            }
        }
    }
}

SCENARIO("a seated customer eats for as long as the config says", "[state_machine]"){
    GIVEN("a customer sitting at a table"){
        testing::ecs_test_game game;
        auto customer_id = game.create_customer_dog(Vector2{200.0f, 200.0f});
        auto waiter_id = game.create_waiter_dog(Vector2{300.0f, 300.0f});

        raise<events::interaction_started>(customer_id, waiter_id);
        REQUIRE(game.state_of(customer_id).value() == dog_config::customer_sitting);

        WHEN("its order is served"){
            raise<events::order_served>(0, waiter_id, customer_id, 0, Vector2{0.0f, 0.0f});

            THEN("it eats"){
                REQUIRE(game.state_of(customer_id).value() == dog_config::customer_eating);
            }
            THEN("it is still eating a tick before the meal is done"){
                game.tick_until([](){ return false; }, dog_config::eating_duration - 1);
                flush();
                REQUIRE(game.state_of(customer_id).value() == dog_config::customer_eating);
            }
            THEN("it sits again once the meal is done"){
                game.tick_until([](){ return false; }, dog_config::eating_duration);
                flush();
                REQUIRE(game.state_of(customer_id).value() == dog_config::customer_sitting);
            }
        }
    }
}

SCENARIO("a waiter carries the food the event handed it", "[state_machine]"){
    GIVEN("a waiter interacting with a counter"){
        testing::ecs_test_game game;
        auto waiter_id = game.create_waiter_dog(Vector2{300.0f, 300.0f});
        auto counter_id = game.create_food_counter(Vector2{400.0f, 400.0f});

        raise<events::dog_started_path>(waiter_id);
        raise<events::interaction_started>(waiter_id, counter_id);
        REQUIRE(game.state_of(waiter_id).value() == dog_config::waiter_interacting);

        WHEN("it collects a named food entity"){
            raise<events::waiter_collected_food>(waiter_id, std::optional<size_t>(counter_id));

            THEN("the carrying state holds that entity"){
                REQUIRE(game.state_of(waiter_id).value() == dog_config::waiter_carrying);
                REQUIRE(game.carried_item_of(waiter_id).value() == counter_id);
            }
        }

        WHEN("it collects food with nothing named"){
            raise<events::waiter_collected_food>(waiter_id);

            THEN("it carries nothing in particular"){
                REQUIRE(game.state_of(waiter_id).value() == dog_config::waiter_carrying);
                REQUIRE_FALSE(game.carried_item_of(waiter_id).has_value());
            }
        }
    }
}
