#include "hud_systems/hud_systems.hpp"
#include "debug_log_interface.h"
#include "system.h"
#include <string>
#include "component_managers.hpp"

void hud_systems::hud_rendering_system::render(int frame){
    auto view_frame = systems::rendering_system::get_instance().get_view_frame();
    debug::log("[hud_rendering_system::render] hud components: " + std::to_string(component_managers::hud_manager_.size())
        + ", view_frame: (" + std::to_string(view_frame.x) + ", " + std::to_string(view_frame.y) + ")");
    for(auto it = component_managers::hud_manager_.begin(); it != component_managers::hud_manager_.end(); ++it){
        auto id = it->first;
        auto& hud_component = it->second;
        debug::log("[hud_rendering_system::render, entity] id: " + std::to_string(id));

        debug::log("[hud_rendering_system::render, active] id: " + std::to_string(id)
            + ", is_active: " + std::to_string(hud_component.is_active()));

        if(hud_component.is_active()){
            auto position_component = component_managers::entity_positional_manager_.get_component(id);
            if(not position_component){
                debug::log("[hud_rendering_system::render, skipped] id: " + std::to_string(id) + ", no position component");
                continue;
            }
            auto position = GetWorldToScreen2D(
                Vector2Add(position_component->get_position(), hud_component.get_offset()),
                systems::rendering_system::get_instance().get_camera());
            auto world_position = position_component->get_position();
            auto offset = hud_component.get_offset();
            debug::log("[hud_rendering_system::render, drawing] id: " + std::to_string(id)
                + ", world: (" + std::to_string(world_position.x) + ", " + std::to_string(world_position.y)
                + "), offset: (" + std::to_string(offset.x) + ", " + std::to_string(offset.y)
                + "), screen: (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
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
}