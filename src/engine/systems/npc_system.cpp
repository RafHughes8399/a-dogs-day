#include "system.h"

void systems::npc_system::update(float delta){
    customer_arrival_.update(delta);

}

void systems::npc_system::register_customer(size_t id){
    customer_arrival_.register_customer(id);
}
void systems::npc_system::unregister_customer(size_t id){
    customer_arrival_.unregister_customer(id);
}
void systems::npc_system::register_table(size_t id){
    customer_arrival_.register_table(id);
}
void systems::npc_system::unregister_table(size_t id){
    customer_arrival_.unregister_table(id);
}

