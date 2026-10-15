#include "hud_systems/hud_systems.hpp"

void hud_systems::hud::init(){

}
void hud_systems::hud::update(float delta_time){
    (void) delta_time;
}
void hud_systems::hud::render(int frame){
    hud_rendering_.render(frame);
}
void hud_systems::hud::on_entity_selected(const events::entity_selected& event){
    if(auto* hud = component_managers::hud_manager_.get_component(event.get_id())){
        hud->set_active(true);
    }
}
void hud_systems::hud::on_entity_deselected(const events::entity_deselected& event){
    if(auto* hud = component_managers::hud_manager_.get_component(event.get_id())){
        hud->set_active(false);
    }
}
