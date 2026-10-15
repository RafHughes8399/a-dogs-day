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
        auto position = GetWorldToScreen2D(
            Vector2Add(position_component->get_position(), hud_component.get_offset()),
            systems::rendering_system::get_instance().get_camera());
        hud_component.get_base().render(position, frame);
        for(auto element : hud_component.get_elements()){
            auto* element_position = component_managers::hud_positional_manager_.get_component(element);
            auto* element_renderable = component_managers::hud_renderable_manager_.get_component(element);
            if(element_position == nullptr or element_renderable == nullptr){ continue; }
            auto element_screen = GetWorldToScreen2D(element_position->get_position(),
                systems::rendering_system::get_instance().get_camera());
            for(auto& body : element_renderable->get_layers()){
                body.get_active_sprite().render(element_screen, frame);
            }
        }
    }
}
