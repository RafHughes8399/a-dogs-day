#include "component.h"
#include "system.h"
#include <raylib.h>
#include <raymath.h>

// TODO stub - the loop calls this every frame, nothing to do yet
void systems::control_input_system::update(float delta){
    (void) delta;
    // TODO: ok when i new control component is regsitered, check the control map
    // we define ae control map based on the controls defined in the config
    // the control map maps a key / mouse input to a function.

    // or  it could either be trmpalted like the builder pattern. no but we need some form 
    // of ownership to attach the function to the exact key press

    // so the input is used as the lookup key to call the function
    // * check key components
    for(auto it = component_managers::control_manager_.begin(); it != component_managers::control_manager_.end(); ++it){
        // do
        
    }
    // * check mouse components

    // * and check mouse delta, this retursn an int, figure out what that int means 
    // * 1 is true, 0 is false
    for(auto it = component_managers::mouse_input_manager_.begin(); it != component_managers::mouse_input_manager_.end(); ++it){
        size_t id = static_cast<size_t>(*&it->first);
        if(not Vector2Equals(GetMouseDelta(), Vector2Zero())){
            auto current_position = component_managers::positional_manager_.get_component(id)->get_position();
            systems::movement_system::get_instance().update_position(id, Vector2Add(current_position, GetMouseDelta()));
        }
        // * check for mouse input also 
    }
}

