#include "hud_systems.hpp"
#include "debug_log_interface.h"
#include "system.h"
#include <string>

void hud_systems::hud_rendering_system::render(int frame){
    auto view_frame = systems::rendering_system::get_instance().get_view_frame();
    debug::log("[hud_rendering_system::render] hud components: " + std::to_string(component_managers::hud_manager_.size())
        + ", view_frame: (" + std::to_string(view_frame.x) + ", " + std::to_string(view_frame.y) + ")");
    for(auto it = component_managers::hud_manager_.begin(); it != component_managers::hud_manager_.end(); ++it){
        auto id = it->first;
        auto& hud_component = it->second;
        debug::log("[hud_rendering_system::render, entity] id: " + std::to_string(id));

        // check selected, if so, draw the component
        auto selectable_component = component_managers::selectable_manager_.get_component(id);
        if(not selectable_component){
            debug::log("[hud_rendering_system::render, skipped] id: " + std::to_string(id) + ", no selectable component");
            continue;
        }
        debug::log("[hud_rendering_system::render, selectable] id: " + std::to_string(id)
            + ", is_selected: " + std::to_string(selectable_component->is_selected()));
        
        if(selectable_component->is_selected()){
            auto position_component = component_managers::positional_manager_.get_component(id);
            if(not position_component){
                debug::log("[hud_rendering_system::render, skipped] id: " + std::to_string(id) + ", no position component");
                continue;
            }
            auto position = Vector2Subtract(
                Vector2Add(position_component->get_position(), hud_component.get_offset()),
                Vector2{view_frame.x, view_frame.y});
            auto world_position = position_component->get_position();
            auto offset = hud_component.get_offset();
            debug::log("[hud_rendering_system::render, drawing] id: " + std::to_string(id)
                + ", world: (" + std::to_string(world_position.x) + ", " + std::to_string(world_position.y)
                + "), offset: (" + std::to_string(offset.x) + ", " + std::to_string(offset.y)
                + "), screen: (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
            hud_component.get_base().render(position, frame);
        }
    }
}