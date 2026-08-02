#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "component.h"
#include "events.h"
#include "events_interface.h"

// this should replace most of the logic required by the level and render layers
namespace systems{
    // storage system [quadtree managemet]
    // moovemnet sytem [ posiitons, pathfinding logic etc]
    // rendering system
    // menu system ? 
    // hud system ? 
    // * systems are non-copyable and non-movable, not defaulted like the
    // * components. Each one subscribes its event handlers in its constructor
    // * and unsubscribes in its destructor, so a copy would double-subscribe
    // * and a move would leave the dispatcher holding a handler whose owner
    // * has gone. level and game delete all four for exactly this reason;
    // * tree::quadtree keeps a copy constructor and has to manually re-subscribe
    // * in it (quadtree.h) - avoid needing that here.
    class movement_system{
        // the level graph and pathfinding
        public:
            ~movement_system() = default;
            movement_system() = default;
            movement_system(const movement_system& other) = delete;
            movement_system(movement_system&& other) = delete;

            movement_system& operator=(const movement_system& other) = delete;
            movement_system& operator=(movement_system&& other) = delete;
    };
    class rendering_system{
        // rendering layers
        // the backdrop
        // and the viewframe
        public:
            ~rendering_system() = default;
            rendering_system() = default;
            rendering_system(const rendering_system& other) = delete;
            rendering_system(rendering_system&& other) = delete;

            rendering_system& operator=(const rendering_system& other) = delete;
            rendering_system& operator=(rendering_system&& other) = delete;
    };
    // * entity storage
    class spatial_system{
        // has the quadtree, holding entities in a more effiecint spatial system so collision and interaciton
        // checks can be performed at O(n log n) instead of

        public:
            ~spatial_system() = default;
            spatial_system() = default;
            spatial_system(const spatial_system& other) = delete;
            spatial_system(spatial_system&& other) = delete;

            spatial_system& operator=(const spatial_system& other) = delete;
            spatial_system& operator=(spatial_system&& other) = delete;
    };
    class entity_lifecycle_system{
        // responsible for managing entity creation and destruction
        public:
            ~entity_lifecycle_system() = default;
            entity_lifecycle_system() = default;
            entity_lifecycle_system(const entity_lifecycle_system& other) = delete;
            entity_lifecycle_system(entity_lifecycle_system&& other) = delete;

            entity_lifecycle_system& operator=(const entity_lifecycle_system& other) = delete;
            entity_lifecycle_system& operator=(entity_lifecycle_system&& other) = delete;
    };
    class collision_system{
        // for physics based collisions
        public:
            ~collision_system() = default;
            collision_system() = default;
            collision_system(const collision_system& other) = delete;
            collision_system(collision_system&& other) = delete;

            collision_system& operator=(const collision_system& other) = delete;
            collision_system& operator=(collision_system&& other) = delete;
    };
    class interaction_system{
        // for behavioural interactions
        public:
            ~interaction_system() = default;
            interaction_system() = default;
            interaction_system(const interaction_system& other) = delete;
            interaction_system(interaction_system&& other) = delete;

            interaction_system& operator=(const interaction_system& other) = delete;
            interaction_system& operator=(interaction_system&& other) = delete;
    };
    class key_input_system{
        // for player input and control
        public:
            ~key_input_system() = default;
            key_input_system() = default;
            key_input_system(const key_input_system& other) = delete;
            key_input_system(key_input_system&& other) = delete;

            key_input_system& operator=(const key_input_system& other) = delete;
            key_input_system& operator=(key_input_system&& other) = delete;
    };
    class npc_system{
        // uses the expediter and the maitre d to orchestrate
        // customer arrivals and departures
        // and waiter serving and clearing
        public:
            ~npc_system() = default;
            npc_system() = default;
            npc_system(const npc_system& other) = delete;
            npc_system(npc_system&& other) = delete;

            npc_system& operator=(const npc_system& other) = delete;
            npc_system& operator=(npc_system&& other) = delete;
    };

    // hold a refernece to the glboal managers that they need to process things
    // and the events that they need ot process 
}

#endif