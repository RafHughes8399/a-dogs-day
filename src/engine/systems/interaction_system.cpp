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

// ---------------- geometry ----------------
std::optional<Rectangle> systems::interaction_system::interactor_box(size_t entity_id){
    auto* interactor = component_managers::interactor_manager_.get_component(entity_id);
    auto* collision = component_managers::collision_manager_.get_component(entity_id);
    if(interactor == nullptr or collision == nullptr){ return std::nullopt; }
    return interactor->get_interaction_box(collision->get_hitbox_component().get_hitbox().get_box());
}
std::optional<Rectangle> systems::interaction_system::interactable_box(size_t entity_id){
    auto* interactable = component_managers::interactable_manager_.get_component(entity_id);
    auto* collision = component_managers::collision_manager_.get_component(entity_id);
    if(interactable == nullptr or collision == nullptr){ return std::nullopt; }
    return interactable->get_interaction_box(collision->get_hitbox_component().get_hitbox().get_box());
}

// ---------------- pair bookkeeping ----------------
bool systems::interaction_system::is_paired(size_t interactor, size_t interactee){
    return std::any_of(interactions_to_process_.begin(), interactions_to_process_.end(),
        [interactor, interactee](auto& interaction) -> bool {
            return interaction.get_interactor() == interactor
                and interaction.get_interactee() == interactee;
        });
}
bool systems::interaction_system::overlapping(size_t interactor, size_t interactee){
    auto actor_box = interactor_box(interactor);
    auto target_box = interactable_box(interactee);
    if(not actor_box.has_value() or not target_box.has_value()){ return false; }
    return CheckCollisionRecs(actor_box.value(), target_box.value());
}
void systems::interaction_system::erase_interaction(size_t interactor, size_t interactee){
    std::erase_if(interactions_to_process_, [interactor, interactee](auto& interaction) -> bool {
        return interaction.get_interactor() == interactor
            and interaction.get_interactee() == interactee;
    });
}
bool systems::interaction_system::is_live(interaction& interaction){
    auto interactor_id = interaction.get_interactor();
    auto interactee_id = interaction.get_interactee();
    auto* interactor = component_managers::interactor_manager_.get_component(interactor_id);
    auto* interactable = component_managers::interactable_manager_.get_component(interactee_id);
    if(interactor == nullptr or interactable == nullptr){ return false; }
    if(interactor->get_target() != interactee_id){ return false; }
    for(auto slot : interactable->get_interactors()){
        if(slot.has_value() and slot.value() == interactor_id){ return true; }
    }
    return false;
}

void systems::interaction_system::update(float delta){
    drop_dead_interactions();
    process_interactions(delta);
}

// * pairing hangs off movement, the way collision indexing does - overlap can
// * only change when something moves, so there is no reason to sweep every
// * interactor every frame. a pair still only forms where an arbitrated claim
// * already exists: the boxes decide when it starts, never who it is with
void systems::interaction_system::on_moved_entity(const events::move_entity& event){
    auto moved_id = event.get_id();
    reconcile(moved_id);

    // the mover can be the station rather than the dog, and then every actor
    // aimed at it has to be re-tested
    if(component_managers::interactable_manager_.get_component(moved_id) == nullptr){ return; }
    std::vector<size_t> aimed_here;
    for(auto& [interactor_id, interactor] : component_managers::interactor_manager_){
        auto target = interactor.get_target();
        if(target.has_value() and target.value() == moved_id){
            aimed_here.push_back(interactor_id);
        }
    }
    for(auto actor_id : aimed_here){
        reconcile(actor_id);
    }
}

void systems::interaction_system::reconcile(size_t interactor_id){
    auto* interactor = component_managers::interactor_manager_.get_component(interactor_id);
    if(interactor == nullptr){ return; }
    auto target = interactor->get_target();
    if(not target.has_value()){ return; }
    auto interactee_id = target.value();
    if(component_managers::interactable_manager_.get_component(interactee_id) == nullptr){ return; }

    auto paired = is_paired(interactor_id, interactee_id);
    auto touching = overlapping(interactor_id, interactee_id);

    if(touching and not paired){
        auto built = create_interaction(interactor_id, interactee_id);
        add_interaction(built);
    }
    else if(paired and not touching){
        erase_interaction(interactor_id, interactee_id);
    }
}

void systems::interaction_system::drop_dead_interactions(){
    std::erase_if(interactions_to_process_, [this](auto& interaction) -> bool {
        return not is_live(interaction);
    });
}

// * index based, never an iterator - a behaviour can destroy an entity, whose
// * remove_entity handler erases from the very vector being walked
void systems::interaction_system::process_interactions(float delta){
    for(size_t i = 0; i < interactions_to_process_.size(); ++i){
        process_interaction(interactions_to_process_[i], delta);
    }
}
void systems::interaction_system::process_interaction(interaction& interaction, float delta){
    // from the interaction, get the list of interaction indices that should be performed
    auto interactions = interaction.get_performable_interactions();
    auto interactor = interaction.get_interactor();
    auto interactee = interaction.get_interactee();
    // and then perform them
    std::for_each(interactions.begin(), interactions.end(), [this, interactor, interactee, delta](auto& interaction_index) -> void {
        if(interaction_index >= defined_interactions_.size()){ return; }
        auto& behaviour = defined_interactions_[interaction_index];
        if(not behaviour){ return; }
        behaviour(interactor, interactee, delta); // will need delta and the two ids
    });
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
// * remove_entity executes before the components go, but a pair naming a dead
// * entity has to be gone before anything else reads it - drop it here rather
// * than waiting for the next drop pass
void systems::interaction_system::on_destroyed_entity(const events::remove_entity& event){
    remove_interaction(event.get_id());
}
