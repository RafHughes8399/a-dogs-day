#ifndef SYSTEMS_H
#define SYSTEMS_H
#include "component.h"
#include "config.h"
#include "entity.h"
#include "events.h"
#include "dog_factory.hpp"
#include "quadtree.h"
#include "events_interface.h"
#include "render_layer.h"
#include <array>
#include <functional>
#include <set>
#include <utility>
#include <raylib.h>
#include "graph.h"
#include "path.h"
namespace systems{
    // storage system [quadtree managemet]
    // moovemnet sytem [ posiitons, pathfinding logic etc]
    // rendering system
    // menu system ?
    // hud system ?
    // systems are non-copyable and non-movable - they subscribe handlers in the
    // constructor and unsubscribe in the destructor, so a copy double-subscribes

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

            void update(float delta);
        private:
            collision_system() = default;

    };
    class control_input_system{
        // for player input and control, maps the control input to a function.
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
            void left_click(size_t id);
            void move_view_frame(Vector2 direction_scalar, float delta);
            void open_inventory();
            void open_map();
            void open_menu();
            void open_quests();
            void open_shop();
            void queue_key_press(int key);
            void right_click(size_t id);
            void select_dog();
            void switch_dog();
            void toggle_debug_logger();

            // * keyed on {key_, action_}, not the key alone - KEY_E is both
            // * key_hold_actions::edit_mode and key_press_actions::exit_edit, and
            // * MOUSE_BUTTON_LEFT is 0, which is also a legal KEY_* value, so a
            // * lookup on the raw int would alias all three.
            std::map<std::pair<int, int>, std::function<void(size_t, float)>> control_function_map_;
            // mirrors player::selected_dog_ - switch_dog flips it, right_click carries it
            size_t selected_dog_;
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
            size_t create_customer_dog();
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

            dog_factory::dog_factory dog_factory_;
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
    // -> movement system assigning, calculating, processing and updating paths
    // -> the movment system is where the level_graph should be stored so it can in house process and check paths
    class movement_system{
        // the level graph and pathfinding
        public:
            static movement_system& get_instance(){
                static movement_system instance;
                return instance;
            }
            ~movement_system(){
                event_interface::unsubscribe<events::create_entity>(create_entity_handler_);
                event_interface::unsubscribe<events::move_entity>(move_entity_handler_);
                event_interface::unsubscribe<events::remove_entity>(remove_entity_handler_);
                event_interface::unsubscribe<events::create_path_to>(create_path_to_handler_);
                event_interface::unsubscribe<events::create_path_to_entity>(create_path_to_entity_handler_);
            }
            movement_system(const movement_system& other) = delete;
            movement_system(movement_system&& other) = delete;

            movement_system& operator=(const movement_system& other) = delete;
            movement_system& operator=(movement_system&& other) = delete;
            void update(float delta);
            void update_position(size_t id, Vector2 position);


            void on_created_entity(const events::create_entity& event);
            void on_moved_entity(const events::move_entity& event);
            void on_destroyed_entity(const events::remove_entity& event);
            void on_create_path_to_event(const events::create_path_to& event);
            void on_create_path_to_entity_event(const events::create_path_to_entity& event);
            // * the graph is private to this system, so walkability questions come
            // * through here - slot selection needs it in shipped builds, not only
            // * under DOG_DAYS_TESTING

            void create_path();
            int graph_occupant_at(Vector2 position){
                return resolve_graph(position).occupant_at(position);
            }
            bool is_walkable(Vector2 position){
                return resolve_graph(position).occupant_at(position) == graph_config::empty_node;
            }
            graph::level_graph::node* node_at(Vector2 position){
                return resolve_graph(position).node_at(position);
            }
            void clear(){
                cafe_.reset();
                footpath_.reset();
            }
            void render_graph(Rectangle frame){
                cafe_.render(frame);
                footpath_.render(frame);
            }
#ifdef DOG_DAYS_TESTING
            size_t graph_occupied_node_count(){
                std::set<std::pair<float, float>> cells;
                for(auto* graph : graphs()){
                    for(auto position : graph->occupied_node_positions()){
                        cells.insert({position.x, position.y});
                    }
                }
                return cells.size();
            }
            int graph_cell_at_index(Vector2 position){
                return resolve_graph(position).cell_at_index(position);
            }
            int graph_nearest_node_index(Vector2 position){
                return resolve_graph(position).nearest_node_index(position);
            }
#endif
            // set a path for an entity
            // queue a path for an entity
            // within update, process movement [can use the existing dog logic]

            // should listen for when an entity is created, moved and destroyed so it can
            // update the graph nodes
        private:
            movement_system()
            : create_entity_handler_([this](const events::create_entity& event) -> void{on_created_entity(event);}),
            move_entity_handler_([this](const events::move_entity& event) -> void{on_moved_entity(event);}),
            remove_entity_handler_([this](const events::remove_entity& event) -> void{on_destroyed_entity(event);}),
            create_path_to_handler_([this](const events::create_path_to& event) -> void{on_create_path_to_event(event);}),
            create_path_to_entity_handler_([this](const events::create_path_to_entity& event) -> void{on_create_path_to_entity_event(event);}),

            cafe_(Rectangle{level_config::cafe_x, level_config::cafe_y, level_config::cafe_width, level_config::cafe_height}, false),
            footpath_(Rectangle{level_config::footpath_x, level_config::footpath_y, level_config::footpath_width, level_config::footpath_height}, false){

                event_interface::subscribe<events::create_entity>(create_entity_handler_);
                event_interface::subscribe<events::move_entity>(move_entity_handler_);
                event_interface::subscribe<events::remove_entity>(remove_entity_handler_);
                event_interface::subscribe<events::create_path_to>(create_path_to_handler_);
                event_interface::subscribe<events::create_path_to_entity>(create_path_to_entity_handler_);
            }

            std::optional<path::path> create_path(Vector2 source, Vector2 direction, Vector2 destination,
                std::optional<size_t> destination_entity = std::nullopt);
            // * planning against a named graph is what lets a leg end in the
            // * overlap band - resolve_graph always answers cafe there, so a
            // * leg that re-derived its graph could never cross out of the footpath
            std::optional<path::path> create_path(graph::level_graph& graph, Vector2 source,
                Vector2 direction, Vector2 destination,
                std::optional<size_t> destination_entity = std::nullopt);
            void create_path_to(size_t entity_id, Vector2 destination,
                const std::vector<Vector2>& checkpoints, path::assignment mode);
            void create_path_to_entity(size_t entity_id, size_t destination_entity,
                const std::vector<Vector2>& checkpoints, path::assignment mode);
            bool build_legs(Vector2 source, Vector2 direction, Vector2 destination,
                std::optional<size_t> destination_entity,
                const std::vector<Vector2>& checkpoints, std::vector<path::path>& legs);
            bool build_leg(Vector2 source, Vector2 direction, Vector2 destination,
                std::optional<size_t> destination_entity, std::vector<path::path>& legs);
            void commit_route(size_t entity_id, components::movement_component& movement,
                components::position_component& position, path::assignment mode,
                std::vector<path::path> legs);
            void determine_direction(size_t id, components::movement_component& movement,
                Vector2 position, Vector2 target);


            graph::level_graph& resolve_graph(Vector2 position);
            std::array<graph::level_graph*, 2> graphs();
            events::event_handler<events::create_entity> create_entity_handler_;
            events::event_handler<events::move_entity> move_entity_handler_;
            events::event_handler<events::remove_entity> remove_entity_handler_;
            events::event_handler<events::create_path_to> create_path_to_handler_;
            events::event_handler<events::create_path_to_entity> create_path_to_entity_handler_;

            graph::level_graph cafe_;
            graph::level_graph footpath_;
    };
    class npc_system{
        public:
        // uses the expediter and the maitre d to orchestrate
        // customer arrivals and departures
        // and waiter serving and clearing
        // comprised of teh following subsystmes
                // * customer arrival - manages the footpath and picking dogs to actually enter the cafe
                // * table_allocation_ - managers assigning customers to tables and general table availability
                // * serving system - manages serving food to customers
                // * clearing system  - managers clearing tables after customers have left
            class customer_arrival_system{
                public:
                    // TODO must listen to table construction and deletion, can create a new event for it and update teh builders 
                    // TODO and destroyers to emit those events 
                    ~customer_arrival_system() = default;
                    customer_arrival_system() = default;

                    customer_arrival_system(const customer_arrival_system& other) = default;
                    customer_arrival_system(customer_arrival_system&& other) = default;

                    customer_arrival_system& operator=(const customer_arrival_system& other) = default;
                    customer_arrival_system& operator=(customer_arrival_system&& other) = default;
                    
                    // create_dog
                    // destroy_dog


                    void update(float delta);
                    void create_customer_dog();
                    void destroy_customer_dog(size_t id);

                    void register_customer(size_t id);
                    void unregister_customer(size_t id);
                    void register_table(size_t id);
                    void unregister_table(size_t id);
                    
                    
                    bool free_tables();
                    int pick_table();
                    int pick_customer();
                    void customer_cleanup();
                    void send_customer_to_table();
#ifdef DOG_DAYS_TESTING
                    const std::vector<size_t>& get_customers() const{
                        return customers_;
                    }
                    const std::vector<size_t>& get_tables() const{
                        return tables_;
                    }
#endif
                    // teardown between test scenarios - the singleton outlives them
                    void clear(){
                        customers_.clear();
                        tables_.clear();
                        time_since_dog_ = 0.0f;
                    }
                    // check dog enter cafe
                    //
                private:
                    // const Rectangle cafe_entrace_;
                    float time_since_dog_ = 0.0f;
                    std::vector<size_t> customers_;
                    std::vector<size_t> tables_;
            };
            class table_allocation_system{
                public:
                private:
            };
            class serving_system{
                public:
                private:
            };
            class clearing_system{
                public:
                private:
            };
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

            void register_customer(size_t id);
            void unregister_customer(size_t id);
            void clear(){
                customer_arrival_.clear();
            }
        private:
            npc_system() = default;
            customer_arrival_system customer_arrival_;
        public:
            void update(float delta);
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
            void move_frame(Vector2 move_delta);
            // teardown between test scenarios - the singleton outlives them
            void clear();
#ifdef DOG_DAYS_TESTING
            render_layer::ecs_layer& get_layer(size_t layer){
                return render_layers_[layer];
            }
            Rectangle get_view_frame(){
                return view_frame_;
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
    class selection_system{
        // owns which entity is currently selected
        public:
            static selection_system& get_instance(){
                static selection_system instance;
                return instance;
            }
            ~selection_system(){
                event_interface::unsubscribe<events::remove_entity>(remove_entity_handler_);
            }
        private:
            selection_system()
            : remove_entity_handler_([this](const events::remove_entity& event) -> void{on_destroyed_entity(event);}),
            selected_(game_config::empty_entity){
                event_interface::subscribe<events::remove_entity>(remove_entity_handler_);
            }
        public:
            selection_system(const selection_system& other) = delete;
            selection_system(selection_system&& other) = delete;

            selection_system& operator=(const selection_system& other) = delete;
            selection_system& operator=(selection_system&& other) = delete;

            void update(float delta);
            void on_destroyed_entity(const events::remove_entity& event);

            void select(size_t entity_id);
            void deselect();
            int selected() const{
                return selected_;
            }
            // teardown between test scenarios - the singleton outlives them
            void clear(){
                selected_ = game_config::empty_entity;
            }
        private:
            events::event_handler<events::remove_entity> remove_entity_handler_;
            int selected_;
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
                Vector2{level_config::graph_x, level_config::graph_y},
                Vector2{level_config::graph_x + level_config::graph_width,
                    level_config::footpath_y + level_config::footpath_height}})
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
            int check_collision_with(size_t id, Vector2 position);
            int check_collision_with(size_t id, Rectangle box);
            int check_interactions_with(size_t id, Rectangle box);
        private:
            // an entity with no collision component has no bounds and is not indexed
            hitbox::hitbox* bounds_for(size_t entity_id);

            events::event_handler<events::create_entity> create_entity_handler_;
            events::event_handler<events::move_entity> move_entity_handler_;
            events::event_handler<events::remove_entity> remove_entity_handler_;

            tree::ecs_quadtree entities_;
    };

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
    }

    // hold a refernece to the glboal managers that they need to process things
    // and the events that they need ot process
}

#endif
