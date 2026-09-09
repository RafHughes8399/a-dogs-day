#include "config.h"
#include "state.h"

std::unique_ptr<state::state> state_builders::build_player_idle_state(){
    return std::make_unique<state::idle_state>(dog_config::player_idle,
        animation_config::shared::idle);
}
std::unique_ptr<state::state> state_builders::build_player_walking_state(){
    return std::make_unique<state::walking_state>(dog_config::player_walking,
        animation_config::shared::walking);
}
std::unique_ptr<state::state> state_builders::build_player_interacting_state(){
    return std::make_unique<state::interacting_state>(dog_config::player_interacting,
        animation_config::shared::interacting);
}

std::unique_ptr<state::state> state_builders::build_customer_walking_state(){
    return std::make_unique<state::walking_state>(dog_config::customer_walking,
        animation_config::shared::walking);
}
std::unique_ptr<state::state> state_builders::build_customer_sitting_state(){
    return std::make_unique<state::sitting_state>(dog_config::customer_sitting,
        animation_config::shared::sitting);
}
std::unique_ptr<state::state> state_builders::build_customer_eating_state(){
    return std::make_unique<state::eating_state>(dog_config::customer_eating,
        animation_config::shared::eating);
}

std::unique_ptr<state::state> state_builders::build_waiter_stationary_state(){
    return std::make_unique<state::stationary_state>(dog_config::waiter_stationary,
        animation_config::shared::idle);
}
std::unique_ptr<state::state> state_builders::build_waiter_idle_state(){
    return std::make_unique<state::idle_state>(dog_config::waiter_idle,
        animation_config::shared::walking);
}
std::unique_ptr<state::state> state_builders::build_waiter_interacting_state(){
    return std::make_unique<state::interacting_state>(dog_config::waiter_interacting,
        animation_config::shared::interacting);
}
std::unique_ptr<state::state> state_builders::build_waiter_carrying_state(){
    return std::make_unique<state::carrying_state>(dog_config::waiter_carrying,
        animation_config::shared::carrying);
}
