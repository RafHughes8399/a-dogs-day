#ifndef COMPONENT_H
#define COMPONENT_H


#include <concepts>
#include <queue>
#include <stddef.h>
#include <vector>

#include "event_core.h"
#include "events_interface.h"
#include "raylib.h"
#include "raymath.h"
#include "sprite.h"
namespace components {

    class position_component {
        public:
            ~position_component() = default;
            position_component(size_t entity_id, Vector2 position, Vector2 direction_scalar)
            : entity_id_(entity_id), position_(position), direction_scalar_(direction_scalar){};

        private:
            const size_t entity_id_;
            Vector2 position_;
            Vector2 direction_scalar_;
    };
    class movement_component {
        using path = std::vector<Vector2>;
        public:
            ~movement_component() = default;
            movement_component(size_t entity_id, Vector2 move_speed, std::queue<path> paths = {})
            : entity_id_(entity_id), paths_(paths), move_speed_(move_speed){};
        private:
        const size_t entity_id_;
        std::queue<path> paths_;
        const Vector2 move_speed_;
    }; 
    
    class sprite_component {
        // body_
        public:
            ~sprite_component() = default;
            // TODO fix magic number 
            sprite_component(size_t entity_id, std::vector<sprite::sprite>& sprites, size_t index = 0)
            :entity_id_(entity_id), sprites_(sprites), sprite_index_(index){};
        private:
            const size_t entity_id_;
            std::vector<sprite::sprite> sprites_;
            size_t sprite_index_;
    };


    // * structurally identical to the sprite compoennt, so just give the entity mulitple, 
    // * it is a one to many 
    // class outline_component {
    //     // outline

    // };
        
    // class cosmetic_component {
    //     // cosmetic slots and the sprites attached 
    // };
    // TODO at a later point, we aren't up to collision and interaction yet even currently 
    class collision_component  {
        // defines collision behaviour 
    };
    class interaction_component  {
        // supports interaction
    };
    
    
    // * again a one to many, because you cant template over an abstract class 
    template <std::derived_from<events::event> E> // E for event
    class event_handling_component  {
        //holds a list of event handlers and their on event behaviour 
        ~event_handling_component(){
            event_interface::unsubscribe<E>(event_handler_);
        }
        event_handling_component(std::function<void(const E&)> on_event)
            : event_handler_(on_event){
                event_interface::subscribe(event_handler_);
            }
        public:
        private:
        events::event_handler<E> event_handler_;
    };
    class state_machine_component  {
        // build a state machien for an npc dog
        // what is a state machine if 
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
    // extern component_manager<outline_component> outline_manager;
    // extern component_manager<cosmetic_component> cosmetic_manager_;
    extern component_manager<collision_component> collision_manager_;
    extern component_manager<interaction_component> interaction_manager_;
    extern component_manager<event_handling_component> event_handling_manager_;
    extern component_manager<state_machine_component> state_machine_manager_;
    extern component_manager<storage_component> storage_manager_;
}
#endif