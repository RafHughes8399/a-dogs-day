#include "dog_factory.hpp"
#include "events.h"
#include "events_interface.h"
#include <random>
#include <algorithm>
size_t dog_factory::dog_factory::pick_route(){
    // TODO make it a proper random thing
    return 0;
}
size_t dog_factory::dog_factory::pick_dog(){
    if(index_ >= dogs_.size()){
        refresh_dogs();
    }
    size_t dog = dogs_[index_];
    ++index_;
    return dog; 
}

void dog_factory::dog_factory::refresh_dogs(){
    dogs_.clear();
    for(int d = customers::tex; d < customers::customers_size; d++){
        for(int c = 0; c < CUSTOMERS; c++){
            dogs_.push_back(static_cast<size_t>(d));
        }
    }
    for(int d = special_customers::garfield; d < special_customers::cumulative_customers_size; d++){
        for(int c = 0; c < SPECIAL_CUSTOMERS; c++){
            dogs_.push_back(static_cast<size_t>(d));
        }
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(dogs_.begin(), dogs_.end(), g);

    index_ = 0;
}

// it does need an id and a position 
void dog_factory::dog_factory::build_customer_dog(size_t id){
    auto dog = pick_dog();
    auto route = pick_route();
    auto builder = builders_[dog];
    builder(id, spawn_positions_[route]);

    events::create_path_to create_path_event{id, destination_positions_[route], path::replace};
    event_interface::execute_event(create_path_event);

    return;
}