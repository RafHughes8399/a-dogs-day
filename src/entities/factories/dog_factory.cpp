#include "dog_factory.hpp"
#include "events.h"
#include "events_interface.h"
#include <random>
#include <algorithm>
Vector2 dog_factory::dog_factory::pick_spawn(){
    // TODO make it a proper random thing
    return spawn_positions_[0];
}
Vector2 dog_factory::dog_factory::pick_destination(){
    // TODO make it a proper random thing
    return destination_positions_[0];
}
int dog_factory::dog_factory::pick_dog(){
    if(index_ == dogs_.size() - 1){
        refresh_dogs();
        
    }
    size_t dog = dogs_[index_];
    ++index_;
    return dog; 
}

void dog_factory::dog_factory::refresh_dogs(){
    for(int d = customers::tex; d < customers::customers_size; d++){
        for(int c = 0; c < CUSTOMERS; c++){
            dogs_.push_back(d);
        }
    }
    for(int d = special_customers::garfield; d < special_customers::cumulative_customers_size; d++){
        for(int c = 0; c < SPECIAL_CUSTOMERS; c++){
            dogs_.push_back(d);
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
    auto spawn =  pick_spawn();
    auto destination = pick_destination();
    auto builder = builders_[dog];
    builder(id, spawn);

    std::unique_ptr<events::event> create_path_event = std::make_unique<events::create_path_to>(id, destination, path::replace);
    event_interface::queue_event(create_path_event);

    return;
}