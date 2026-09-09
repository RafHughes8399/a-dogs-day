#include "system.h"

void systems::state_machine_system::update(float delta){
    for(auto& [entity, component] : component_managers::state_machine_manager_){
        component.get_machine().update(entity, delta);
    }
}

void systems::state_machine_system::transition(size_t entity, int transition,
    std::optional<size_t> payload){
    auto* component = component_managers::state_machine_manager_.get_component(entity);
    if(component == nullptr){ return; }

    auto& machine = component->get_machine();
    if(not machine.can_transition(transition)){ return; }

    auto previous = machine.current();
    machine.transition(entity, transition, payload);
    debug::log("[state_machine_system::transition] entity: " + std::to_string(entity)
        + ", transition: " + std::to_string(transition)
        + ", state: " + std::to_string(previous) + " -> " + std::to_string(machine.current()));
    play_state_animation(entity);
}

void systems::state_machine_system::play_state_animation(size_t entity){
    auto* component = component_managers::state_machine_manager_.get_component(entity);
    if(component == nullptr){ return; }

    auto* current = component->get_machine().get_current_state();
    if(current == nullptr){ return; }

    std::vector<animation_system::sprite_animation> animations;
    for(size_t slot = 0; slot < entity_config::dog_sprite_slots_size; ++slot){
        animations.push_back(animation_system::sprite_animation{slot, current->get_animation(), true});
    }
    animation_system::get_instance().play(entity, animations);
}

void systems::state_machine_system::on_started_path(const events::dog_started_path& event){
    transition(event.get_id(), dog_config::path_created);
}
void systems::state_machine_system::on_completed_path(const events::dog_completed_path& event){
    transition(event.get_id(), dog_config::path_finished);
}
void systems::state_machine_system::on_interaction_started(const events::interaction_started& event){
    transition(event.get_interactor_id(), dog_config::interaction_started);
}
void systems::state_machine_system::on_interaction_finished(const events::interaction_finished& event){
    transition(event.get_interactor_id(), dog_config::interaction_finished);
}
void systems::state_machine_system::on_order_served(const events::order_served& event){
    transition(event.get_customer_id(), dog_config::order_served);
    transition(event.get_waiter_id(), dog_config::order_served);
}
void systems::state_machine_system::on_collected_food(const events::waiter_collected_food& event){
    transition(event.get_waiter_id(), dog_config::food_collected, event.get_food_id());
}
void systems::state_machine_system::on_finished_meal(const events::customer_finished_meal& event){
    transition(event.get_customer_id(), dog_config::meal_finished);
}
void systems::state_machine_system::on_customer_left(const events::customer_dog_left& event){
    transition(event.get_customer_id(), dog_config::customer_leaving);
}
