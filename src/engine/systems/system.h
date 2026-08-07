#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "component.h"
#include "entity.h"
#include "events.h"
#include "quadtree.h"
#include "events_interface.h"
#include "render_layer.h"
#include <functional>

// this should replace most of the logic required by the level and render layers
namespace systems{
    // storage system [quadtree managemet]
    // moovemnet sytem [ posiitons, pathfinding logic etc]
    // rendering system
    // menu system ? 
    // hud system ? 
    // systems are non-copyable and non-movable - they subscribe handlers in the
    // constructor and unsubscribe in the destructor, so a copy double-subscribes
    class movement_system{
        // the level graph and pathfinding
        public:
            static movement_system& get_instance(){
                static movement_system instance;
                return instance;
            }
            ~movement_system() = default;
            movement_system(const movement_system& other) = delete;
            movement_system(movement_system&& other) = delete;

            movement_system& operator=(const movement_system& other) = delete;
            movement_system& operator=(movement_system&& other) = delete;
        private:
            movement_system() = default;
        public:

            void update(float delta);
            void update_position(size_t id, Vector2 position);
    };
    class rendering_system{
        // rendering layers
        // the backdrop
        // and the viewframe
        public:
            static rendering_system& get_instance(){
                static rendering_system instance;
                return instance;
            }
            ~rendering_system(){
                event_interface::unsubscribe<events::create_entity>(create_entity_handler_);
                event_interface::unsubscribe<events::remove_entity>(remove_entity_handler_);
            }
            rendering_system(const rendering_system& other) = delete;
            rendering_system(rendering_system&& other) = delete;

            rendering_system& operator=(const rendering_system& other) = delete;
            rendering_system& operator=(rendering_system&& other) = delete;

            void render(int frame);
            void on_created_entity(const events::create_entity& event);
            void on_destroyed_entity(const events::remove_entity& event);
            // teardown between test scenarios - the singleton outlives them
            void clear();
#ifdef DOG_DAYS_TESTING
            render_layer::ecs_layer& get_layer(size_t layer){
                return render_layers_[layer];
            }
#endif
        private:
            // the background is an entity on the background layer now, so nothing
            // here needs raylib running at construction
            rendering_system()
                : create_entity_handler_([this](const events::create_entity& event) -> void{on_created_entity(event);}),
                remove_entity_handler_([this](const events::remove_entity& event) -> void{on_destroyed_entity(event);}),
                view_frame_(Rectangle{0.0f, 0.0f, level_config::screen_width, level_config::screen_height}),
                render_layers_(){
                    event_interface::subscribe<events::create_entity>(create_entity_handler_);
                    event_interface::subscribe<events::remove_entity>(remove_entity_handler_);
                }

            bool is_entity_in_frame(size_t id, Rectangle view_frame);

            events::event_handler<events::create_entity> create_entity_handler_;
            events::event_handler<events::remove_entity> remove_entity_handler_;

            // the render layer is a 2d list layering the entities  so you can draw in layers
            // refferencing like [j][k] is drawing entity k on layer j

            Rectangle view_frame_;
            render_layer::ecs_layer render_layers_[level_config::draw_layers::size];
    };
    // * entity storage
    class spatial_system{
        // has the quadtree, holding entities in a more effiecint spatial system so collision and interaciton
        // checks can be performed at O(n log n) instead of

        public:
            static spatial_system& get_instance(){
                static spatial_system instance;
                return instance;
            }
            ~spatial_system(){
                event_interface::unsubscribe<events::create_entity>(create_entity_handler_);
                event_interface::unsubscribe<events::move_entity>(move_entity_handler_);
                event_interface::unsubscribe<events::remove_entity>(remove_entity_handler_);
            }
        private:
            spatial_system(raglib::bounding_box_2 world_bounds = raglib::bounding_box_2{
                Vector2{0.0f, 0.0f}, Vector2{level_config::world_x, level_config::world_y}})
                : create_entity_handler_([this](const events::create_entity& event) -> void{on_created_entity(event);}),
                move_entity_handler_([this](const events::move_entity& event) -> void{on_moved_entity(event);}),
                remove_entity_handler_([this](const events::remove_entity& event) -> void{on_destroyed_entity(event);}),
                entities_(world_bounds){
                    event_interface::subscribe<events::create_entity>(create_entity_handler_);
                    event_interface::subscribe<events::move_entity>(move_entity_handler_);
                    event_interface::subscribe<events::remove_entity>(remove_entity_handler_);
                }
        public:
            spatial_system(const spatial_system& other) = delete;
            spatial_system(spatial_system&& other) = delete;

            spatial_system& operator=(const spatial_system& other) = delete;
            spatial_system& operator=(spatial_system&& other) = delete;

            void update(float delta);
            void on_created_entity(const events::create_entity& event);
            void on_moved_entity(const events::move_entity& event);
            void on_destroyed_entity(const events::remove_entity& event);

            bool is_tracked(size_t entity_id){
                return entities_.contains(entity_id);
            }
            size_t tracked_count(){
                return entities_.size();
            }
            int node_depth_of(size_t entity_id){
                return entities_.depth_of(entity_id);
            }
            bool node_bounds_of(size_t entity_id, raglib::bounding_box_2& bounds){
                return entities_.node_bounds_of(entity_id, bounds);
            }
            // teardown between test scenarios - the singleton outlives them
            void clear(){
                entities_.clear();
            }
        private:
            // an entity with no collision component has no bounds and is not indexed
            hitbox::hitbox* bounds_for(size_t entity_id);

            events::event_handler<events::create_entity> create_entity_handler_;
            events::event_handler<events::move_entity> move_entity_handler_;
            events::event_handler<events::remove_entity> remove_entity_handler_;

            tree::ecs_quadtree entities_;
    };
    class entity_lifespan_system{
        // responsible for managing entity creation and destruction
        public:
            static entity_lifespan_system& get_instance(){
                static entity_lifespan_system instance;
                return instance;
            }
            ~entity_lifespan_system() = default;
            entity_lifespan_system(const entity_lifespan_system& other) = delete;
            entity_lifespan_system(entity_lifespan_system&& other) = delete;

            entity_lifespan_system& operator=(const entity_lifespan_system& other) = delete;
            entity_lifespan_system& operator=(entity_lifespan_system&& other) = delete;
        private:
            entity_lifespan_system() = default;
        public:

            // allocate -> build -> announce, so nothing can be built without
            // reaching the spatial index and a render layer
            template<typename Builder>
            size_t create(Builder build, size_t layer){
                auto entity_id = next_id();
                build(entity_id);
                // executed, never queued - listeners must see the entity this frame
                events::create_entity created{entity_id, layer};
                event_interface::execute_event(created);
                return entity_id;
            }
            void remove(size_t entity_id);
            void update(float delta);
            // teardown between test scenarios - the singleton outlives them
            void clear(){
                recycled_ids_ = {};
                fresh_id_ = 0;
            }
        private:
            size_t next_id();
            std::queue<size_t> recycled_ids_;
            size_t fresh_id_ = 0;
    };
    class collision_system{
        // for physics based collisions
        public:
            static collision_system& get_instance(){
                static collision_system instance;
                return instance;
            }
            ~collision_system() = default;
            collision_system(const collision_system& other) = delete;
            collision_system(collision_system&& other) = delete;

            collision_system& operator=(const collision_system& other) = delete;
            collision_system& operator=(collision_system&& other) = delete;
        private:
            collision_system() = default;
        public:

            void update(float delta);
    };
    class interaction_system{
        // for behavioural interactions
        public:
            static interaction_system& get_instance(){
                static interaction_system instance;
                return instance;
            }
            ~interaction_system() = default;
            interaction_system(const interaction_system& other) = delete;
            interaction_system(interaction_system&& other) = delete;

            interaction_system& operator=(const interaction_system& other) = delete;
            interaction_system& operator=(interaction_system&& other) = delete;
        private:
            interaction_system() = default;
        public:

            void update(float delta);
    };
    class control_input_system{
        // for player input and control, maps the control input to a function.
        // * owns both input components - key_input_component (keyboard) and
        // * mouse_input_component (buttons) - rather than splitting a mouse
        // * system out, because they are two modalities of one job: turn device
        // * state into world changes. it is also the only place that may call
        // * raylib's input functions; the components hold bindings, not state.
        // TODO update() runs two loops: the mouse pass syncs position from the
        // TODO device for everything in mouse_input_manager_ (that is what makes
        // TODO the cursor follow the pointer - see the note above
        // TODO ecs_entities::build_cursor) then dispatches button bindings; the
        // TODO keyboard pass dispatches key bindings. blocked on component_manager
        // TODO gaining iteration and movement_system gaining move_to.
        public:
            static control_input_system& get_instance(){
                static control_input_system instance;
                return instance;
            }
            ~control_input_system() = default;
            control_input_system(const control_input_system& other) = delete;
            control_input_system(control_input_system&& other) = delete;

            control_input_system& operator=(const control_input_system& other) = delete;
            control_input_system& operator=(control_input_system&& other) = delete;

            void update(float delta);
            // teardown between test scenarios - the singleton outlives them
            void clear(){
                selected_dog_ = level_config::mack_id;
            }
#ifdef DOG_DAYS_TESTING
            // * raylib 5.5 has no way to inject key or button state - IsKeyPressed
            // * and friends read state only PollInputEvents writes - so the tests
            // * enter one step below the device. this is exactly what check_inputs
            // * calls once a binding has fired, so the lookup and the action are
            // * the real path; only the "did the device do it" question is faked.
            void simulate_input(const game_config::input& input, size_t id, float delta = 0.0f){
                dispatch(input.key_, input.action_, id, delta);
            }
            bool is_bound(int key, int action){
                return control_function_map_.find({key, action}) != control_function_map_.end();
            }
            size_t bound_count(){
                return control_function_map_.size();
            }
            size_t get_selected_dog(){
                return selected_dog_;
            }
#endif
        private:
            control_input_system()
            : control_function_map_({}), selected_dog_(level_config::mack_id){
                build_control_map();
            }

            void build_control_map();
            void check_inputs(size_t id, std::vector<game_config::input>& controls, float delta);
            void dispatch(int key, int action, size_t id, float delta);

            // the actions, mirroring player::controls' default scheme
            void back();
            void left_click();
            void move_view_frame(Vector2 direction_scalar, float delta);
            void open_inventory();
            void open_map();
            void open_menu();
            void open_quests();
            void open_shop();
            void queue_key_press(int key);
            void right_click();
            void select_dog();
            void switch_dog();

            // * keyed on {key_, action_}, not the key alone - KEY_E is both
            // * key_hold_actions::edit_mode and key_press_actions::exit_edit, and
            // * MOUSE_BUTTON_LEFT is 0, which is also a legal KEY_* value, so a
            // * lookup on the raw int would alias all three.
            std::map<std::pair<int, int>, std::function<void(size_t, float)>> control_function_map_;
            // mirrors player::selected_dog_ - switch_dog flips it, right_click carries it
            size_t selected_dog_;
    };
    class npc_system{
        // uses the expediter and the maitre d to orchestrate
        // customer arrivals and departures
        // and waiter serving and clearing
        public:
            static npc_system& get_instance(){
                static npc_system instance;
                return instance;
            }
            ~npc_system() = default;
            npc_system(const npc_system& other) = delete;
            npc_system(npc_system&& other) = delete;

            npc_system& operator=(const npc_system& other) = delete;
            npc_system& operator=(npc_system&& other) = delete;
        private:
            npc_system() = default;
        public:

            void update(float delta);
    };

    // singletons outlive a test scenario, so their own storage has to be wiped
    // alongside the component managers
    inline void clear_all_systems(){
        entity_lifespan_system::get_instance().clear();
        spatial_system::get_instance().clear();
        rendering_system::get_instance().clear();
        control_input_system::get_instance().clear();
    }

    // hold a refernece to the glboal managers that they need to process things
    // and the events that they need ot process 
}

#endif