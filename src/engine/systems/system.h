#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "component.h"
#include "events.h"
#include "events_interface.h"
#include "render_layer.h"

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
            // * no default constructor - sprite::sprite has none, so background_
            // * cannot be default initialised.
            rendering_system(sprite::sprite background, Rectangle view_frame)
                : background_(background), view_frame_(view_frame), render_layers_(){}
            rendering_system(const rendering_system& other) = delete;
            rendering_system(rendering_system&& other) = delete;

            rendering_system& operator=(const rendering_system& other) = delete;
            rendering_system& operator=(rendering_system&& other) = delete;

            void render(int frame);
            // TODO listen for entity creations and removals, to add and remove from the render layer 
            void on_created_entity();
            void on_destroyed_entity();
        private:
            // on entity creation [and deletion ] so on_entity_created, on_entity _removed
            // the render layer is a 2d list layering the entities  so you can draw in layers
            // * refferencing like [j][k] is drawing entity k on layer j - ecs_layer
            // * is the inner list, and owns the per-entity component lookup and draw.
            sprite::sprite background_;
            Rectangle view_frame_;
            render_layer::ecs_layer render_layers_[level_config::draw_layers::size];
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

            void build_entity();
        private:
            std::queue<size_t> recycled_ids_;
            size_t fresh_id_;



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
    class control_input_system{
        // for player input and control, maps the control input to a function
        public:
            
            ~control_input_system() = default;
            control_input_system() = default;
            control_input_system(const control_input_system& other) = delete;
            control_input_system(control_input_system&& other) = delete;

            control_input_system& operator=(const control_input_system& other) = delete;
            control_input_system& operator=(control_input_system&& other) = delete;

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