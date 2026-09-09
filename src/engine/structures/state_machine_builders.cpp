#include "config.h"
#include "state_machine.h"

state_machine::state_machine state_machine_builders::build_player_state_machine(){
    state_machine::graph graph;

    graph.push_back(state_machine::node(state_builders::build_player_idle_state(),
        {state_machine::edge(dog_config::path_created, dog_config::player_walking)}));

    graph.push_back(state_machine::node(state_builders::build_player_walking_state(),
        {state_machine::edge(dog_config::path_finished, dog_config::player_idle),
         state_machine::edge(dog_config::interaction_started, dog_config::player_interacting)}));

    graph.push_back(state_machine::node(state_builders::build_player_interacting_state(),
        {state_machine::edge(dog_config::path_created, dog_config::player_walking)}));

    return state_machine::state_machine(std::move(graph), dog_config::player_idle);
}

state_machine::state_machine state_machine_builders::build_customer_state_machine(){
    state_machine::graph graph;

    graph.push_back(state_machine::node(state_builders::build_customer_walking_state(),
        {state_machine::edge(dog_config::interaction_started, dog_config::customer_sitting)}));

    graph.push_back(state_machine::node(state_builders::build_customer_sitting_state(),
        {state_machine::edge(dog_config::order_served, dog_config::customer_eating),
         state_machine::edge(dog_config::customer_leaving, dog_config::customer_walking)}));

    graph.push_back(state_machine::node(state_builders::build_customer_eating_state(),
        {state_machine::edge(dog_config::meal_finished, dog_config::customer_sitting)}));

    return state_machine::state_machine(std::move(graph), dog_config::customer_walking);
}

state_machine::state_machine state_machine_builders::build_waiter_state_machine(){
    state_machine::graph graph;

    graph.push_back(state_machine::node(state_builders::build_waiter_stationary_state(),
        {state_machine::edge(dog_config::path_created, dog_config::waiter_idle)}));

    graph.push_back(state_machine::node(state_builders::build_waiter_idle_state(),
        {state_machine::edge(dog_config::interaction_started, dog_config::waiter_interacting)}));

    graph.push_back(state_machine::node(state_builders::build_waiter_interacting_state(),
        {state_machine::edge(dog_config::food_collected, dog_config::waiter_carrying),
         state_machine::edge(dog_config::order_served, dog_config::waiter_idle)}));

    graph.push_back(state_machine::node(state_builders::build_waiter_carrying_state(),
        {state_machine::edge(dog_config::interaction_started, dog_config::waiter_interacting)}));

    return state_machine::state_machine(std::move(graph), dog_config::waiter_stationary);
}
