#include "system.h"


size_t systems::interaction_system::interaction::get_interactor(){
    return interactor_;
}
size_t systems::interaction_system::interaction::get_interactee(){
    return interactee_;
}
std::vector<size_t> systems::interaction_system::interaction::get_performable_interactions(){
    return performable_interactions_;
}
std::vector<size_t> systems::interaction_system::interaction::determine_performable_interactions(){
    std::vector<size_t> interactions = {};
    auto interactor = component_managers::interactor_manager_.get_component(interactor_);
    auto interactee = component_managers::interactable_manager_.get_component(interactee_);
    if(interactor and interactee){
        for(auto interactor_interaction : interactor->get_interactions()){
            for(auto interactee_interaction : interactee->get_interactions()){
               if(interactor_interaction == interactee_interaction){
                    interactions.push_back(interactor_interaction);
               } 
            }
        }
    }
    return interactions;
}


// TODO (25 / 8 / 26) stub - the loop calls this every frame, nothing to do yet
void systems::interaction_system::update(float delta){
    process_interactions(delta);
}

void systems::interaction_system::process_interactions(float delta){
    std::for_each(interactions_to_process_.begin(), interactions_to_process_.end(), [this, delta](auto& i) -> void {
        process_interaction(i, delta);
    });
}
void systems::interaction_system::process_interaction(interaction& interaction, float delta){
    // from the interaction, get the list of interaction indices that should be performed
    auto interactions = interaction.get_performable_interactions();
    auto interactor = interaction.get_interactor();
    auto interactee = interaction.get_interactee();
    // and then perform them
    std::for_each(interactions.begin(), interactions.end(), [this, interactions, interactor, interactee, delta](auto& interaction_index) -> void {
        defined_interactions_[interaction_index](interactor, interactee, delta); // will need delta and the two ids
    });
}

void systems::interaction_system::on_moved_entity(const events::move_entity& event){
    auto interactor_id = event.get_id();
    auto* interactor = component_managers::interactor_manager_.get_component(interactor_id);
    if(interactor == nullptr){ return; }

    auto target = interactor->get_target();
    if(not target.has_value()){ return; }

    auto interactee_id = target.value();
    if(component_managers::interactable_manager_.get_component(interactee_id) == nullptr){ return; }

    auto built = create_interaction(interactor_id, interactee_id);
    add_interaction(built);
}
void systems::interaction_system::on_destroyed_entity(const events::remove_entity& event){
    remove_interaction(event.get_id());
}

systems::interaction_system::interaction systems::interaction_system::create_interaction(size_t interactor, size_t interactee){
    return interaction(interactor, interactee);
}
void systems::interaction_system::add_interaction(interaction& interaction){
    interactions_to_process_.push_back(std::move(interaction));
}
void systems::interaction_system::remove_interaction(size_t entity_id){
    std::erase_if(interactions_to_process_, [entity_id](auto& interaction) -> bool {
        return interaction.get_interactee() == entity_id or interaction.get_interactor() == entity_id;
    });
    //* for cleanup upon entity removal
}