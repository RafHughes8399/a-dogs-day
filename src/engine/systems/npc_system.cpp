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

void systems::npc_system::update(float delta){
    customer_arrival_.update(delta);
    waiter_idling_.update(delta);
}

void systems::npc_system::register_customer(size_t id){
    customer_arrival_.register_customer(id);
}
void systems::npc_system::unregister_customer(size_t id){
    customer_arrival_.unregister_customer(id);
}

void systems::npc_system::register_waiter(size_t id){
    waiter_idling_.register_waiter(id);
}
void systems::npc_system::unregister_waiter(size_t id){
    waiter_idling_.unregister_waiter(id);
}
void systems::npc_system::register_table(size_t id){
    customer_arrival_.register_table(id);
}
void systems::npc_system::unregister_table(size_t id){
    customer_arrival_.unregister_table(id);
}

