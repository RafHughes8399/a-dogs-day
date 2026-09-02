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

// TODO (25 / 8 / 26) on moved entity
systems::interaction_system::interaction systems::interaction_system::create_interaction(size_t interactor, size_t interactee){
    return interaction();
}
void systems::interaction_system::add_interaction(interaction& interaction){
    interactions_to_process_.push_back(std::move(interaction));
}
void systems::interaction_system::remove_interaction(size_t entity_id){
    // TODO: find an interaction where either the interactor or the interactee uses this 
    // TODO enitty and then remove uit 

    //* for cleanup upon entity removal
}