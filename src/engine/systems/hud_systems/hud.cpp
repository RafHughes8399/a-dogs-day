#include "hud_systems/hud_systems.hpp"
#include "system.h"

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
queries::query_result hud_systems::hud::on_create_hud_button(const queries::create_hud_button_query& query){
    auto kind = query.get_kind();
    auto position = query.get_position();
    auto subject = query.get_subject();
    auto id = systems::entity_lifespan_system::get_instance().create_hud_element(query.get_owner(),
        [this, kind, position, subject](size_t button) -> void { buttons_.build_button(kind, button, position, subject); });
    return queries::query_result{std::in_place_type<size_t>, id};
}
