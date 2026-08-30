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
void systems::entity_lifespan_system::destroy(size_t entity_id){
    // executed before the components go, so listeners can still read them
    events::remove_entity removed{entity_id};
    event_interface::execute_event(removed);

    component_helpers::unregister_all_components(entity_id);
    recycled_ids_.push(entity_id);
}

// TODO (25 / 8 / 26) stub - the loop calls this every frame, nothing to do yet
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
void systems::entity_lifespan_system::destroy_customer_dog(size_t entity_id){
    destroy(entity_id);
    npc_system::get_instance().unregister_customer(entity_id);
}
size_t systems::entity_lifespan_system::create_waiter_dog(size_t waiter, Vector2 position){
    auto id = create([this](size_t waiter_type, size_t entity_id, Vector2 position) -> void{
        dog_factory_.build_waiter_dog(waiter_type, entity_id, position); },
        waiter, position, level_config::draw_layers::dogs);
    npc_system::get_instance().register_waiter(id);
    return id;
    }
    void systems::entity_lifespan_system::destroy_waiter_dog(size_t entity_id){
        destroy(entity_id);
        npc_system::get_instance().unregister_waiter(entity_id);
}
size_t systems::entity_lifespan_system::create_table(size_t entity, Vector2 position){
    auto id = create([this](size_t id, size_t entity, Vector2 position) -> void{ station_factory_.build_table(id, entity, position); },
        entity, position, level_config::draw_layers::stations);
    npc_system::get_instance().register_table(id);
    return id;
}
void systems::entity_lifespan_system::destroy_table(size_t id){
    destroy(id);
    npc_system::get_instance().unregister_table(id);
}

size_t systems::entity_lifespan_system::create_counter(size_t counter, Vector2 position){
    auto id = create([this](size_t id, size_t counter, Vector2 position) -> void{ station_factory_.build_counter(id, counter, position); },
        counter, position, level_config::draw_layers::stations);
    return id;
}
void systems::entity_lifespan_system::destroy_counter(size_t id){
    destroy(id);
}