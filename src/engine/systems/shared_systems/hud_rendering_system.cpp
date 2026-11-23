#include "events.h"
#include "game_config.h"
#include "hud_systems/hud_systems.hpp"
#include "system.h"
#include "component_managers.hpp"

void hud_systems::hud_rendering_system::render(int frame){
    for(auto it = component_managers::hud_manager_.begin(); it != component_managers::hud_manager_.end(); ++it){
        auto id = it->first;
        auto& hud_component = it->second;
        if(not hud_component.is_active()){ continue; }

        auto position_component = component_managers::entity_positional_manager_.get_component(id);
        if(not position_component){ continue; }
        for(auto& panel : hud_component.get_panels()){
            auto position = GetWorldToScreen2D(
                Vector2Add(position_component->get_position(), panel.get_offset()),
                systems::rendering_system::get_instance().get_camera());
            panel.get_base().render(position, frame);
        }
        for(auto element : hud_component.get_elements()){
            auto* element_position = component_managers::hud_positional_manager_.get_component(element);
            auto* element_renderable = component_managers::hud_renderable_manager_.get_component(element);
            if(element_position == nullptr or element_renderable == nullptr){ continue; }
            auto element_screen = GetWorldToScreen2D(element_position->get_position(),
                systems::rendering_system::get_instance().get_camera());
            for(auto& body : element_renderable->get_layers()){
                body.get_active_sprite().render(Vector2Add(element_screen, body.get_offset()), frame, body.get_scale());
            }
        }
    }
}

void hud_systems::hud_rendering_system::on_hover_event(const events::hover_hud_entity& event){
    auto previous = event.get_previous_hovered();
    auto current = event.get_current_hovered();
    if(previous != game_config::empty_entity){
        auto* previous_renderable = component_managers::hud_renderable_manager_.get_component(static_cast<size_t>(previous));
        if(previous_renderable != nullptr){
            previous_renderable->reset_scale();
        }
    }
    if(current == game_config::empty_entity){ return; }
    auto* current_renderable = component_managers::hud_renderable_manager_.get_component(static_cast<size_t>(current));
    auto* hoverable = component_managers::hud_hoverable_manager_.get_component(static_cast<size_t>(current));
    if(current_renderable == nullptr or hoverable == nullptr){ return; }
    current_renderable->scale_by(hoverable->get_scale());
}