#ifndef COMPONENT_H
#define COMPONENT_H


#include <stddef.h>
#include <vector>

#include "raylib.h"
#include "raymath.h"
namespace components {

    class position_component {
        public:
        private:
            size_t entity_id_;
            Vector2 position_;
            int direction_;
            Vector2 direction_scalar_;
    };
    class movement_component {
        public:
        private:
            // list of paths
    }; 
    
    class sprite_component {
        // body_
    };
        
    class outline_component {
        // outline
    };
        
    class cosmetic_component {
        // cosmetic slots and the sprites attached 
    };
    class collision_component  {
        // defines collision behaviour 
    };
    class interaction_component  {
        // supports interaction
    };
    class event_handling_component  {
        //holds a list of event handlers and their on event behaviour 
    };
    class state_machine_component  {
        // build a state machien for an npc dog
    };
    
    class storage_component  {
        // ? current idea for the storage component is for stations to store food, 
        // ? tables to store foood 
        // ? and dishwasher to store plates 
    };
    template<typename C> // C for component
    class component_manager{
        public:

        private:
            // * entity_indices_[entity_id] holds the index where the 
            // * entity's component is stored in components, enabling constant time lookup
            // ? might need to transition to a one to many, but that shouldn't be too bad, 
            // ? just a list of size_t 
            std::vector<size_t> entity_indices_; 
            std::vector<C> components_;
    };
    // TODO: define the component managers for each component type, like the query handlers 
    extern component_manager<position_component> positional_manager_;
    extern component_manager<movement_component> movment_manager_;
    extern component_manager<sprite_component> sprite_manager_;
    extern component_manager<outline_component> outline_manager;
    extern component_manager<cosmetic_component> cosmetic_manager_;
    extern component_manager<collision_component> collision_manager_;
    extern component_manager<interaction_component> interaction_manager_;
    extern component_manager<event_handling_component> event_handling_manager_;
    extern component_manager<state_machine_component> state_machine_manager_;
    extern component_manager<storage_component> storage_manager_;
}
#endif