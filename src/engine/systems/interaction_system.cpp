#include "debug_log_interface.h"
#include "debug_logger.h"
#include "system.h"
#include <string>


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
        debug::log("[interaciotn system, determine performable interactions]: interactor interactions " + std::to_string(interactor->get_interactions().size()));
        debug::log("[interaciotn system, determine performable interactions]: interactee interactions " + std::to_string(interactee->get_interactions().size()));
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

bool systems::interaction_system::establish_handhsake(size_t interactor_id, size_t interactable_id){
    auto interactor = component_managers::interactor_manager_.get_component(interactor_id);
    auto interactable = component_managers::interactable_manager_.get_component(interactable_id);
    if(not (interactor and interactable)) {
        return false;
    }
    if(not interactable->can_accept_interactor()){return false;}
    if(not interactable->claim(interactor_id)){return false;}
    interactor->interact_with(interactable_id);
    return true;
}
void systems::interaction_system::teardown_handshake(size_t interactor_id, size_t interactable_id){
    auto interactor = component_managers::interactor_manager_.get_component(interactor_id);
    auto interactable = component_managers::interactable_manager_.get_component(interactable_id);
    if(interactable){ interactable->release(interactor_id); }
    if(interactor){ interactor->stop_interacting(); }
}

// * ----------------------------------------------------- INTERACTIONS ------------------------------------------------------- * // 

void systems::interaction_system::customer_table_sit(size_t interactor, size_t interactee, float delta){
    (void) interactor;
    (void) interactee;
    (void) delta;
    debug::log("customer table sit interaction attempt");
    // TODO: update the customer state to sitting
}
void systems::interaction_system::waiter_table_serve(size_t interactor, size_t interactee, float delta){
    (void) interactor;
    (void) interactee;
    (void) delta;
}
void systems::interaction_system::waiter_counter_pickup(size_t waiter, size_t counter, float delta){
    (void) waiter;
    (void) counter;
    (void) delta;
    debug::log("waiter counter pickup interaction attempt");
    // two things to do:
    // * 1. switch the dog state and play the animation 
    // *.2 perform the taking from the counter, the building of the food and the assinging to the dog
    // but should be done in reverse order, the dog animation should only play if the pickup was successful
    
    // * 3. maybe the dog needs a carrier component, or maybe the food entity needs a "movement" component ?
    // * no, no ,no in the dog state, the dog will hold a food id, then the state update will take the dog position,
    // * and the update the food position's accordingly
    auto food_opt = item_system::get_instance().take_item(counter);
    if(not food_opt.has_value()){
        return;
    }
    auto food = food_opt.value();
    auto dog_mouth = Vector2Zero(); // get the dog mouth offset
    // lifespan system create.
    entity_lifespan_system::get_instance().create_food(food.get_id(), dog_mouth);
    // dog transition to  carrying 
    // the dog state should handle the animation through on transition   
}

// TODO (25 / 8 / 26) stub - the loop calls this every frame, nothing to do yet
void systems::interaction_system::update(float delta){
    process_interactions(delta);
}

void systems::interaction_system::process_interactions(float delta){
    std::for_each(interactions_to_process_.begin(), interactions_to_process_.end(), [this, delta](auto& i) -> void {
        process_interaction(i, delta);
    });
    //  ? empty the list if processed ?, yeah should be a one and done. for now, can introduce some more extensive checking later down the line should we need it
    interactions_to_process_.clear();
}
void systems::interaction_system::process_interaction(interaction& interaction, float delta){
    // from the interaction, get the list of interaction indices that should be performed
    auto interactions = interaction.get_performable_interactions();
    auto interactor = interaction.get_interactor();
    auto interactee = interaction.get_interactee();
    // and then perform them
    debug::log("[interaction system - process interaction] - interactor " + std::to_string(interactor)
            + " interactee: " + std::to_string(interactee) + " interactions to check: " + std::to_string(interactions.size()));
        
    std::for_each(interactions.begin(), interactions.end(), [this, interactions, interactor, interactee, delta](auto& interaction_index) -> void {
        debug::log("[interaction system - process interaction] - perform interaction " + std::to_string((interaction_index)));
        defined_interactions_[interaction_index](interactor, interactee, delta); // will need delta and the two ids
    });
}

void systems::interaction_system::on_moved_entity(const events::move_entity& event){
    auto interactor_id = event.get_id();
    auto* interactor = component_managers::interactor_manager_.get_component(interactor_id);
    if(interactor == nullptr){ return; }

    auto target = interactor->get_target();
    if(not target.has_value()){ return; }
    auto target_id = target.value();

    auto* interactor_collision = component_managers::collision_manager_.get_component(interactor_id);
    auto* target_collision = component_managers::collision_manager_.get_component(target_id);
    if(interactor_collision == nullptr or target_collision == nullptr){ return; }

    Rectangle interactor_box = interactor->get_interaction_box(
        interactor_collision->get_hitbox_component().get_hitbox().get_box());
    Rectangle target_box = target_collision->get_hitbox_component().get_hitbox().get_box();

    bool overlapping = CheckCollisionRecs(interactor_box, target_box);
    if(not overlapping) {
        // * drop the interaction, clean up both halves of the claim -
        // * interactor->stop_interacting() and interactable->release() - and
        teardown_handshake(interactor_id, target_id);
        // TODO: process the state transition for the interaction having ended
    }
}
void systems::interaction_system::on_path_finished(const events::dog_completed_path& event){
    auto dog_id = event.get_id();
    auto* interactor = component_managers::interactor_manager_.get_component(dog_id);
    if(interactor == nullptr){
        debug::log("[interaction_system::on_path_finished] dog: " + std::to_string(dog_id)
            + " has no interactor component - no interaction");
        return;
    }

    auto target = event.get_destination_entity();
    if(not target.has_value()){
        debug::log("[interaction_system::on_path_finished] dog: " + std::to_string(dog_id)
            + " arrived with no destination entity and no interactor target - no interaction");
        return;
    }
    auto target_id = target.value();

    auto* interactor_collision = component_managers::collision_manager_.get_component(dog_id);
    auto* target_collision = component_managers::collision_manager_.get_component(target_id);
    if(interactor_collision == nullptr or target_collision == nullptr){ return; }

    Rectangle interactor_box = interactor->get_interaction_box(
        interactor_collision->get_hitbox_component().get_hitbox().get_box());
    Rectangle target_box = target_collision->get_hitbox_component().get_hitbox().get_box();

    bool overlapping = CheckCollisionRecs(interactor_box, target_box);
    debug::log("[interaction_system::on_path_finished] dog: " + std::to_string(dog_id)
        + ", target: " + std::to_string(target_id)
        + ", interactor box: " + std::to_string(interactor_box.x) + "," + std::to_string(interactor_box.y)
            + " " + std::to_string(interactor_box.width) + "x" + std::to_string(interactor_box.height)
        + ", target box: " + std::to_string(target_box.x) + "," + std::to_string(target_box.y)
            + " " + std::to_string(target_box.width) + "x" + std::to_string(target_box.height)
        + ", overlapping: " + std::string(overlapping ? "yes" : "no"));
    if(overlapping and establish_handhsake(dog_id, target_id)){
        auto interaction = create_interaction(dog_id, target_id);
        add_interaction(interaction);
    }
}
void systems::interaction_system::on_destroyed_entity(const events::remove_entity& event){
    remove_interaction(event.get_id());
}

systems::interaction_system::interaction systems::interaction_system::create_interaction(size_t interactor, size_t interactee){
    return interaction(interactor, interactee);
}
void systems::interaction_system::add_interaction(interaction& interaction){
    std::unique_ptr<events::event> started = std::make_unique<events::interaction_started>(
        interaction.get_interactor(), interaction.get_interactee());
    interactions_to_process_.push_back(std::move(interaction));
    debug::log("add interaction to process");
    event_interface::queue_event(started);
}
void systems::interaction_system::remove_interaction(size_t entity_id){
    for(auto& interaction : interactions_to_process_){
        if(interaction.get_interactee() != entity_id and interaction.get_interactor() != entity_id){ continue; }

        std::unique_ptr<events::event> finished = std::make_unique<events::interaction_finished>(
            interaction.get_interactor(), interaction.get_interactee());
        event_interface::queue_event(finished);
    }
    std::erase_if(interactions_to_process_, [entity_id](auto& interaction) -> bool {
        return interaction.get_interactee() == entity_id or interaction.get_interactor() == entity_id;
    });
    //* for cleanup upon entity removal
}