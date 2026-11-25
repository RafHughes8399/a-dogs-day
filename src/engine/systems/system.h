#ifndef SYSTEM_H
#define SYSTEM_H
#include "entity_systems/entity_systems.hpp"
#include "hud_systems/hud_systems.hpp"
#include "interface_systems/interface_systems.hpp"

namespace systems{
    // singletons outlive a test scenario, so their own storage has to be wiped
    // alongside the component managers
    inline void clear_all_systems(){
        entity_lifespan_system::get_instance().clear();
        spatial_system::get_instance().clear();
        rendering_system::get_instance().clear();
        movement_system::get_instance().clear();
        control_input_system::get_instance().clear();
        selection_system::get_instance().clear();
        npc_system::get_instance().clear();
        interaction_system::get_instance().clear();
        animation_system::get_instance().clear();
        state_machine_system::get_instance().clear();
        hud_systems::hud_spatial_system::get_instance().clear();
        menu_navigation_system::get_instance().clear();
    }
}

#endif
