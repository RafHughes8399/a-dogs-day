#include "component.h"
#include "config.h"
#include "debug_log_interface.h"
#include "raglib.h"
#include "system.h"
#include <algorithm>
#include <string>

namespace {
    std::string position_of(size_t id){
        auto* position = component_managers::positional_manager_.get_component(id);
        if(position == nullptr){ return "no position component"; }
        return raglib::vector_to_string(position->get_position());
    }
}

void systems::npc_system::customer_arrival_system::register_table(size_t id){
    (void) id;
}
void systems::npc_system::customer_arrival_system::unregister_table(size_t id){
    (void) id;
}             
bool systems::npc_system::customer_arrival_system::free_tables(){
    return true;
}
size_t systems::npc_system::customer_arrival_system::pick_table(){
    return 1;
}

void systems::npc_system::customer_arrival_system::register_customer(size_t id){
    customers_.push_back(id);
}
void systems::npc_system::customer_arrival_system::create_customer_dog(){
    auto id = entity_lifespan_system::get_instance().create_customer_dog();
    debug::log("[customer_arrival_system::create_customer_dog, built customer] id: "
        + std::to_string(id)
        + ", spawn: " + position_of(id)
        + ", tracked customers: " + std::to_string(customers_.size()));
}
void systems::npc_system::customer_arrival_system::destroy_customer_dog(size_t id){
    // ? would i need a more specic remove, or should the generic be fine ?
    debug::log("[customer_arrival_system::destroy_customer_dog, removing customer] id: "
        + std::to_string(id)
        + ", last position: " + position_of(id)
        + ", tracked customers: " + std::to_string(customers_.size()));
    entity_lifespan_system::get_instance().remove(id);
    std::erase_if(customers_,  [id](auto customer) -> bool {return customer == id;});
}
void systems::npc_system::customer_arrival_system::update(float delta){
    time_since_dog_ += delta;
    // check for creating customers
    if(time_since_dog_ >= dog_config::customer_spawn_interval){
        create_customer_dog();
        time_since_dog_ = 0.0f;
    }
    // check existing customers
    std::vector<size_t> departed;
    for(auto customer : customers_){
        auto movement = component_managers::movement_manager_.get_component(customer);
        if(movement and movement->get_paths().empty()){
            departed.push_back(customer);
        }
    }
    for(auto id : departed){
        destroy_customer_dog(id);
    }
}

void systems::npc_system::update(float delta){
    customer_arrival_.update(delta);

}

void systems::npc_system::register_customer(size_t id){
    customer_arrival_.register_customer(id);
}