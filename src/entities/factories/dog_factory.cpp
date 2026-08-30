#include "factories.hpp"
#include "events.h"
#include "events_interface.h"
#include <random>
#include <algorithm>
size_t factories::dog_factory::pick_route(){
    // TODO (25 / 8 / 26) make it a proper random thing
    return 0;
}
size_t factories::dog_factory::pick_dog(){
    if(index_ >= customer_dogs_.size()){
        refresh_dogs();
    }
    size_t dog = customer_dogs_[index_];
    ++index_;
    return dog; 
}

void factories::dog_factory::refresh_dogs(){
    customer_dogs_.clear();
    for(int d = entity_config::customers::tex; d < entity_config::customers::customers_size; d++){
        for(int c = 0; c < CUSTOMERS; c++){
            customer_dogs_.push_back(static_cast<size_t>(d));
        }
    }
    for(int d = entity_config::special_customers::garfield; d < entity_config::special_customers::cumulative_customers_size; d++){
        for(int c = 0; c < SPECIAL_CUSTOMERS; c++){
            customer_dogs_.push_back(static_cast<size_t>(d));
        }
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(customer_dogs_.begin(), customer_dogs_.end(), g);

    index_ = 0;
}

// it does need an id and a position 
void factories::dog_factory::build_customer_dog(size_t id){
    auto dog = pick_dog();
    auto route = pick_route();
    auto builder = customer_builders_[dog];
    builder(id, customer_spawn_positions_[route]);
    debug::log("[dog_factory::build_customer_dog, built customer] id: "
        + std::to_string(id));
    events::create_path_to create_path_event{id, customer_destination_positions_[route], path::replace};
    event_interface::execute_event(create_path_event);
    return;
}

void factories::dog_factory::build_waiter_dog(size_t waiter, size_t entity_id, Vector2 position){
    auto waiter_builder = waiter_builders_[waiter];
    waiter_builder(entity_id, position);
}