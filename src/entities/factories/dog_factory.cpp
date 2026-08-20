#include "dog_factory.hpp"
#include <random>
#include <algorithm>
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
void dog_factory::dog_factory::build_dog(size_t id, Vector2 position){
    auto dog = pick_dog();
    auto builder = builders_[dog];
    builder(id, position);
    return;
}