#include "events.h"
#include "events_interface.h"
#include "game_config.h"
#include "hud_systems.hpp"

void hud_systems::hover_system::hover(size_t id){
    if(static_cast<int>(id) == hovered_element_id_){ return; }
    events::hover_hud_entity hovered{hovered_element_id_, static_cast<int>(id)};
    hovered_element_id_ = static_cast<int>(id);
    event_interface::execute_event(hovered);
}
void hud_systems::hover_system::unhover(){
    if(hovered_element_id_ == game_config::empty_entity){ return; }
    events::hover_hud_entity hovered{hovered_element_id_, game_config::empty_entity};
    hovered_element_id_ = game_config::empty_entity;
    event_interface::execute_event(hovered);
}
void hud_systems::hover_system::on_removed_hud_element(const events::remove_hud_element& event){
    if(static_cast<int>(event.get_id()) == hovered_element_id_){
        unhover();
    }
}
