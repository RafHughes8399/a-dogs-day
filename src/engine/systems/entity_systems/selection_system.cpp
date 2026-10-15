#include "component.h"
#include "component_managers.hpp"
#include "system.h"

// ---------------- event handlers ----------------
void systems::selection_system::on_destroyed_entity(const events::remove_entity& event){
    if(static_cast<int>(event.get_id()) == selected_){
        selected_ = game_config::empty_entity;
    }
}

// ---------------- frame update ----------------
// TODO (25 / 8 / 26) stub - selection is event driven, nothing to do per frame yet
void systems::selection_system::update(float delta){
    (void) delta;
}

// ---------------- selection ----------------
void systems::selection_system::select(size_t entity_id){
    auto* selectable = component_managers::entity_selectable_manager_.get_component(entity_id);
    if(selectable == nullptr){ return; }

    deselect();
    selectable->select();
    selected_ = static_cast<int>(entity_id);
    events::entity_selected selected{entity_id};
    event_interface::execute_event(selected);
}

void systems::selection_system::deselect(){
    if(selected_ == game_config::empty_entity){ return; }
    auto deselected_id = static_cast<size_t>(selected_);
    if(auto* selectable = component_managers::entity_selectable_manager_.get_component(deselected_id)){
        selectable->unselect();
    }
    selected_ = game_config::empty_entity;
    events::entity_deselected deselected{deselected_id};
    event_interface::execute_event(deselected);
}
