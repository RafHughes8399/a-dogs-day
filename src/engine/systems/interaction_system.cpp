#include "component.h"
#include "config.h"
#include "debug_log_interface.h"
#include "debug_logger.h"
#include "dog_events.h"
#include "event_core.h"
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
void systems::interaction_system::waiter_table_serve(size_t waiter, size_t table, float delta){
    (void) waiter;
    (void) table;
    (void) delta;
    debug::log("waiter table serve interaction attempt");
    // ! again the waiter must be carrying to serve
    auto waiter_state = component_managers::state_machine_manager_.get_component(waiter);
    if(not waiter_state or waiter_state->get_machine().get_current_state()->get_state_id() != dog_config::waiter_carrying){
        debug::log("[waiter table serve attempt: ] - state is not carrying, cannot serve");
        debug::log("[waiter table serve attempt: ] - state is " + std::to_string(waiter_state->get_machine().get_current_state()->get_state_id()));
        debug::log("[waiter table serve attempt: ] - state should be " + std::to_string(dog_config::waiter_carrying));
        return;
    }
    
    // get the table
    auto table_position = component_managers::positional_manager_.get_component(table);
    if(not table_position) {
        debug::log("[waiter table serve attempt: ] - no table position");
        return;
    }
    
    auto food_place_position = Vector2Add(table_position->get_position(), entity_config::food_draw_offset);
    
    auto waiter_carrier = component_managers::carrier_manager_.get_component(waiter);
    if(not waiter_carrier or not waiter_carrier->is_carrying()){ 
        debug::log("[waiter table serve attempt: ] - no waiter carrier, or waiter is not carrying");
        return;
    }
    auto food = waiter_carrier->get_carried_entity().value();
    waiter_carrier->drop();
    std::unique_ptr<events::event> served = std::make_unique<events::order_served>(waiter, table);
    event_interface::queue_event(served);
    auto carried_after_drop = waiter_carrier->get_carried_entity();
    debug::log("[waiter table serve attempt: ] - post drop, waiter: " + std::to_string(waiter)
        + ", table: " + std::to_string(table)
        + ", dropped food: " + std::to_string(food)
        + ", carrier carrying: " + std::string(waiter_carrier->is_carrying() ? "yes" : "no")
        + ", carried entity: " + (carried_after_drop.has_value() ? std::to_string(carried_after_drop.value()) : std::string("none"))
        + ", state: " + std::to_string(waiter_state->get_machine().get_current_state()->get_state_id()));
    // !!!! and transition the state 
    // place the food at the table position + config_table_food_offset
    auto food_position = component_managers::positional_manager_.get_component(food);
    if(not food_position) {return;}
    food_position->set_position(food_place_position);
    // and emit a served evenmt
}   

Vector2 get_dog_mouth_offset(size_t dog){
    auto hitbox = component_managers::collision_manager_.get_component(dog);
    if(not hitbox) {return Vector2Zero();}
    auto dog_box = hitbox->get_hitbox_component().get_hitbox().get_box();
    auto direction = level_config::direction_scalars[level_config::directions::right];
    if(auto* movement = component_managers::movement_manager_.get_component(dog)){
        direction = movement->get_direction_scalar();
    }
    auto half_width = dog_box.width * 0.5f;
    return Vector2{
        half_width + (half_width * direction.x) - (entity_config::food_width * 0.5f),
        (dog_box.height * 0.2f) - (entity_config::food_height * 0.5f)};
}
void systems::interaction_system::waiter_counter_pickup(size_t waiter, size_t counter, float delta){
    debug::log("waiter counter pickup interaction attempt");
    // two things to do:
    // * 1. switch the dog state and play the animation 
    // *.2 perform the taking from the counter, the building of the food and the assinging to the dog
    // but should be done in reverse order, the dog animation should only play if the pickup was successful
    
    // * 3. maybe the dog needs a carrier component, or maybe the food entity needs a "movement" component ?
    // * no, no ,no in the dog state, the dog will hold a food id, then the state update will take the dog position,
    // * and the update the food position's accordingly
    // ! first state check guard, only perform this if not carrying !!!!!!!!
    // ! cannot pick up if carrying ! must place  down instead !
    auto waiter_state = component_managers::state_machine_manager_.get_component(waiter);
    if(not waiter_state){
        debug::log("[waiter counter pickup interaction] no waiter state");
        return;
    }
    if(waiter_state->get_machine().get_current_state()->get_state_id() == dog_config::waiter_carrying){
        debug::log("[waiter counter pickup interaction] waiter is in the carrying state, cannot pickup");
        return; 
    }
    auto* waiter_position = component_managers::positional_manager_.get_component(waiter);
    if(not waiter_position){ 
        debug::log("[waiter counter pickup interaction] no waiter position ");
        return; 
    }

    auto food_opt = item_system::get_instance().take_item(counter);
    if(not food_opt.has_value()){ 
        debug::log("[waiter counter pickup interaction] no food on the counter ");
        return; 
    }

    auto dog_mouth_position = Vector2Add(waiter_position->get_position(), get_dog_mouth_offset(waiter));
    auto food_id = entity_lifespan_system::get_instance().create_food(food_opt.value().get_id(), dog_mouth_position);


    // * update the carrier component
    auto waiter_carrier = component_managers::carrier_manager_.get_component(waiter);
    if(waiter_carrier){
        waiter_carrier->set_carried_entity(food_id);
        waiter_carrier->set_previous_position(waiter_position->get_position());
    }
    // * and the state [for animations]
    // ? maybe in the future may have to override movement speed to 0 for some delay to ensure that hte 
    // ? waiter cannot run away while still playing the interaction animation 
    std::unique_ptr<events::event> collected = std::make_unique<events::waiter_collected_food>(waiter, food_id);
    event_interface::queue_event(collected);
}
void systems::interaction_system::waiter_counter_place_down(size_t waiter, size_t counter, float delta){
    // ! first guard ! check that is carrying ! cannot put down if not carrying ! 
    debug::log("waiter-counter place down interaction attempt");
    auto waiter_state = component_managers::state_machine_manager_.get_component(waiter);
    if(not waiter_state){
        return;
    }
    if(waiter_state->get_machine().get_current_state()->get_state_id() != dog_config::waiter_carrying){
        return;
    }
    // * 1. get the carried item
    auto waiter_carrier = component_managers::carrier_manager_.get_component(waiter);
    if(not waiter_carrier) {return;}
    auto food_entity_opt = waiter_carrier->get_carried_entity();
    if(not food_entity_opt) { return;}
    auto food_entity_id = food_entity_opt.value();
    // * 2. get the counter storage item
    auto counter_storage = component_managers::storage_manager_.get_component(counter);
    if(not counter_storage){
        return;
    }
    auto food_item_id = 1; // food_manager.get_componet(food_entity_id)
    // * 3. place the item oin the stroage component [requires food to know what item it is, pending that component implementation]
    counter_storage->place(food_item_id);
    // * 4. clean up the carrier component
    waiter_carrier->drop();
}

// * ----------------------------------------------------- INTERACTIONS ------------------------------------------------------- * // 
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
    // ! it is this that is fucking us up. this event being called at the wrong spot, all it does is call
    interactions_to_process_.push_back(std::move(interaction));
    std::unique_ptr<events::event> started = std::make_unique<events::interaction_started>(
        interaction.get_interactor(), interaction.get_interactee());
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