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

void systems::npc_system::customer_arrival_system::register_customer(size_t id){
    customers_.push_back(id);
}
void systems::npc_system::customer_arrival_system::unregister_customer(size_t id){
    customers_.erase(std::remove(customers_.begin(), customers_.end(), id), customers_.end());
    
}
void systems::npc_system::customer_arrival_system::register_table(size_t id){
    tables_.push_back(id);
}
void systems::npc_system::customer_arrival_system::unregister_table(size_t id){
    tables_.erase(std::remove(tables_.begin(), tables_.end(), id), tables_.end());
}             
bool systems::npc_system::customer_arrival_system::free_tables(){
    return pick_table() != game_config::empty_entity;
}
int systems::npc_system::customer_arrival_system::pick_customer(){
    for(auto customer: customers_){

    }
    return game_config::empty_entity;
}
int systems::npc_system::customer_arrival_system::pick_table(){
    for(auto table : tables_){
        // get the interactable component
        // check its status
        auto interactable = component_managers::interactable_manager_.get_component(table);
        if(interactable and interactable->can_accept_interactor(table)){
            return table;
        }
    }
    return game_config::empty_entity;
}

void systems::npc_system::customer_arrival_system::create_customer_dog(){
    if(time_since_dog_ >= dog_config::customer_spawn_interval){
        time_since_dog_ = 0.0f;
        auto id = entity_lifespan_system::get_instance().create_customer_dog();
        debug::log("[customer_arrival_system::create_customer_dog, built customer] id: "
            + std::to_string(id)
            + ", spawn: " + position_of(id)
            + ", tracked customers: " + std::to_string(customers_.size()));
    }
}
void systems::npc_system::customer_arrival_system::destroy_customer_dog(size_t id){
    // ? would i need a more specific remove, or should the generic be fine ?
    debug::log("[customer_arrival_system::destroy_customer_dog, removing customer] id: "
        + std::to_string(id)
        + ", last position: " + position_of(id)
        + ", tracked customers: " + std::to_string(customers_.size()));
    entity_lifespan_system::get_instance().remove(id);
    std::erase_if(customers_,  [id](auto customer) -> bool {return customer == id;});
}

void systems::npc_system::customer_arrival_system::send_customer_to_table(){
    auto table = pick_table();
    auto customer = pick_customer();
    if(table != game_config::empty_entity){
        // * send customer to table,
        Vector2 entrance;
        // path to the entrance
        // TODO need to flesh out a pathfinding system ? 
        // TODO i need a helper that can create paths with checkpoints
        //create_paths(customer, table, entrance);
        events::create_path_to create_path_event = events::create_path_to(customer, entrance, path::replace);
        event_interface::execute_event(create_path_event);

        // path from the entrance to the table
        

    }
}

void systems::npc_system::customer_arrival_system::customer_cleanup(){
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

void systems::npc_system::customer_arrival_system::update(float delta){
    time_since_dog_ += delta;
    create_customer_dog();
     
    if(not customers_.empty() and not tables_.empty()){
        send_customer_to_table();
    }
    customer_cleanup();

}

void systems::npc_system::update(float delta){
    customer_arrival_.update(delta);

}

void systems::npc_system::register_customer(size_t id){
    customer_arrival_.register_customer(id);
}
void systems::npc_system::unregister_customer(size_t id){
    customer_arrival_.unregister_customer(id);
}