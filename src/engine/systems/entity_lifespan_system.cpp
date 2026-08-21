#include "system.h"

// ---------------- id allocation ----------------
size_t systems::entity_lifespan_system::next_id(){
    if(recycled_ids_.empty()){
        return fresh_id_++;
    }
    auto entity_id = recycled_ids_.front();
    recycled_ids_.pop();
    return entity_id;
}

// create() is a template - defined in system.h.

// ---------------- lifecycle ----------------
void systems::entity_lifespan_system::remove(size_t entity_id){
    // executed before the components go, so listeners can still read them
    events::remove_entity removed{entity_id};
    event_interface::execute_event(removed);

    component_helpers::unregister_all_components(entity_id);
    recycled_ids_.push(entity_id);
}

// TODO stub - the loop calls this every frame, nothing to do yet
void systems::entity_lifespan_system::update(float delta){
    (void) delta;
    return;
}
size_t systems::entity_lifespan_system::create_customer_dog(){
    auto id = create([this](size_t id) -> void{ dog_factory_.build_customer_dog(id); },
        level_config::draw_layers::dogs);
    npc_system::get_instance().register_customer(id);
    return id;
}