/**
 * header file that defines entitiy class hierarchy
 */
#ifndef ENTITIES_H
#define ENTITIES_H


#include "config.h"
#include "events.h"
#include "events_interface.h"
#include "hitbox.h"
#include "sprite.h"
#include "raylib.h"
#include "body.h"
#include "debug_log_interface.h"
#include "query_interface.h"
#include "queries.h"
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <queue>

namespace entities{
    enum status_codes{
        nothing = 0,
        moved = 1,
        dead = 2
    };
    // ------------------------- entities ------------------------- // 
    class entity {
        public:
            virtual ~entity() = default;
            entity(body::body body, Vector2 position, int id, std::string debug_id)
            : id_(id), body_(body), position_(position), debug_id_(std::move(debug_id)){}
            entity(const entity& other) = default;
            entity(entity&& other) = default;

            entity& operator=(const entity& other) = delete;
            entity& operator=(entity&& other) = delete;

            bool operator==(entity& other){
                return id_ == other.id_;
            }

            bool check_collision(const hitbox::hitbox other);
            body::body& get_body();
            int get_id();
            const std::string& get_debug_id();
            hitbox::hitbox& get_hitbox();
            sprite::sprite& get_sprite();

            Vector2 get_position();
            void move(Vector2 new_postion);
            void move_without_event(Vector2 new_position);

            virtual int update(float delta, int frame){
                (void) delta;
                (void) frame;
                return status_codes::nothing;
            }

            virtual void render(Vector2 draw_position, int frame);

            virtual void interact(entity& other){
                (void) other;
                return;
            }

        protected:
            const int id_;    
            body::body body_;  
            Vector2 position_;
            const std::string debug_id_;

    };
    class cursor : public entity{
        public:
            class state {
                public:
                    virtual ~state() = default;
                    state(){}
                    state(const state& other) = default;
                    state(state&& other) = default;
                    
                    state& operator=(const state& other) = default;
                    state& operator=(state&& other) = default;
                    
                    virtual void create_move_event(cursor& cursor);
                    virtual void left_click(cursor& cursor, entity& other);
                    virtual void right_click(cursor& cursor, entity& other);
            };
            class in_menus : public state{
                // to be implemented
                in_menus()
                : state() {}
            };
            class editing : public state{
                public:
                    editing()
                    : state(){}
                    editing(const editing& other) = default;
                    editing(editing&& other) = default;
                    
                    editing& operator=(const editing& other) = default;
                    editing& operator=(editing&& other) = default;
                    
                    void left_click(cursor& cursor, entity& other) override;
                    void right_click(cursor& cursor, entity& other) override;
                    
                };
                class carrying_decoration : public editing {
                    public:
                    carrying_decoration(entity* carried)
                    : editing(), carried_decoration_(carried){}
                    carrying_decoration(const carrying_decoration& other) = default;
                    carrying_decoration(carrying_decoration&& other) = default;
                    
                    carrying_decoration& operator=(const carrying_decoration& other) = default;
                    carrying_decoration& operator=(carrying_decoration&& other) = default;
                    
                    void left_click(cursor& cursor, entity& other) override;
                    void create_move_event(cursor& cursor) override;
                    private:
                    entity* carried_decoration_;
            };
            class interaction_strategy{
                public:
                    virtual ~interaction_strategy() = default;
                    interaction_strategy(){}
                    interaction_strategy(const interaction_strategy& other) = default;
                    interaction_strategy(interaction_strategy&& other) = default;
                    
                    interaction_strategy& operator=(const interaction_strategy& other) = default;
                    interaction_strategy& operator=(interaction_strategy&& other) = default;

                    virtual void interact(cursor& cursor, entity& other) = 0;
                private:
            };
            class default_strategy : public interaction_strategy{
                public:
                    default_strategy()
                    : interaction_strategy() {}
                    default_strategy(const default_strategy& other) = default;
                    default_strategy(default_strategy&& other) = default;
                        
                    default_strategy& operator=(const default_strategy& other) = default;
                    default_strategy& operator=(default_strategy&& other) = default;
                    void interact(cursor& cursor, entity& other) override;
                private:
                    
            };
            class left_click_strategy : public interaction_strategy{
                public:
                    left_click_strategy()
                    : interaction_strategy() {}
                    left_click_strategy(const left_click_strategy& other) = default;
                    left_click_strategy(left_click_strategy&& other) = default;
                    
                    left_click_strategy& operator=(const left_click_strategy& other) = default;
                    left_click_strategy& operator=(left_click_strategy&& other) = default;
                    
                    void interact(cursor& cursor, entity& other) override;
                    
                    private:
                };
                
                class right_click_strategy : public interaction_strategy{
                    public:
                    right_click_strategy()
                    : interaction_strategy() {}
                    right_click_strategy(const right_click_strategy& other) = default;
                    right_click_strategy(right_click_strategy&& other) = default;
                    
                    right_click_strategy& operator=(const right_click_strategy& other) = default;
                    right_click_strategy& operator=(right_click_strategy&& other) = default;

                    void interact(cursor& cursor, entity& other) override;
                    private:
                };

                ~cursor() override {
                    event_interface::unsubscribe<events::enter_edit_mode>(enter_edit_mode_handler_);
                    event_interface::unsubscribe<events::exit_edit_mode>(exit_edit_mode_handler_);
                    event_interface::unsubscribe<events::left_mouse_click>(left_mouse_click_handler_);
                    event_interface::unsubscribe<events::move_view_frame>(move_view_frame_handler_);
                    event_interface::unsubscribe<events::right_mouse_click>(right_mouse_click_handler_);
                }
                cursor(body::body body, Vector2 position, int id, std::string debug_id)
                : entity(body, position, id, std::move(debug_id)), 
                enter_edit_mode_handler_([this](const events::enter_edit_mode& event) -> void{on_enter_edit_mode_event(event);}),
                exit_edit_mode_handler_([this](const events::exit_edit_mode& event) -> void{on_exit_edit_mode_event(event);}),
                left_mouse_click_handler_([this](const events::left_mouse_click& event) -> void{on_left_mouse_click_event(event);}),
                move_view_frame_handler_([this](const events::move_view_frame& event) -> void{on_move_view_frame_event(event);}),
                right_mouse_click_handler_([this](const events::right_mouse_click& event) -> void{on_right_mouse_click_event(event);}),
                interaction_strategy_(std::make_unique<default_strategy>()),
                state_(std::make_unique<state>()){
                    event_interface::subscribe<events::enter_edit_mode>(enter_edit_mode_handler_);
                    event_interface::subscribe<events::exit_edit_mode>(exit_edit_mode_handler_);
                    event_interface::subscribe<events::left_mouse_click>(left_mouse_click_handler_);
                    event_interface::subscribe<events::move_view_frame>(move_view_frame_handler_);
                    event_interface::subscribe<events::right_mouse_click>(right_mouse_click_handler_);
                }

                cursor(const cursor& other) = delete;
                cursor(cursor&& other) = default;
                    
                cursor& operator=(const cursor& other) = delete;
                cursor& operator=(cursor&& other)  = delete;
                

                int update(float delta, int frame) override;
                void create_move_event();
                void interact(entity& other) override;   

                void on_enter_edit_mode_event(const events::enter_edit_mode& event);       
                void on_exit_edit_mode_event(const events::exit_edit_mode& event);       
                void on_left_mouse_click_event(const events::left_mouse_click& event);
                void on_move_view_frame_event(const events::move_view_frame& event);
                void on_right_mouse_click_event(const events::right_mouse_click& event);                

            private:
                
                enum animation_tags{
                        base = 0,
                        hover = 1
                };

                events::event_handler<events::enter_edit_mode> enter_edit_mode_handler_;
                events::event_handler<events::exit_edit_mode> exit_edit_mode_handler_;
                events::event_handler<events::left_mouse_click> left_mouse_click_handler_;
                events::event_handler<events::move_view_frame> move_view_frame_handler_;
                events::event_handler<events::right_mouse_click> right_mouse_click_handler_;
                
                std::unique_ptr<interaction_strategy> interaction_strategy_;
                std::unique_ptr<state> state_;
        };
        
        class paw_mark : public entity{
        public:
        paw_mark(body::body body, Vector2 position, int id, std::string debug_id)
        : entity(body, position, id, std::move(debug_id)){}
            paw_mark(const paw_mark& other) = default;
            paw_mark(paw_mark&& other) = default;

            paw_mark& operator=(const paw_mark& other) = delete;
            paw_mark& operator=(paw_mark&& other) = delete;

            int update(float delta, int frame) override;
            void interact(entity& other) override;

        private:
    };
    /**
     * there would be multiple kinds of dogs
     * 
    
     * -> the player dog (K and M )
     *      -> the player dog moves around, responding to cursor events 
     *      -> also has cosmetics (hat, shirt, paw clothes)
     *      
     * -> hepler dogs (waiters, cooks, etc)
     * -> customer dogs 
     */
    class dog : public entity{
        using path = std::vector<Vector2>;
        public:
            dog(body::body body, body::body head, Vector2 position, int id, std::string debug_id,
            int direction = level_config::directions::right)
            : entity(body, position, id, std::move(debug_id)), head_(head),
            direction_scalar_(level_config::direction_scalars[direction]){
                body_.set_index(static_cast<size_t>(direction));
                head_.set_index(static_cast<size_t>(direction));
            }
            dog(const dog& other) = delete;
            dog(dog&& other) = default;

            dog& operator=(const dog& other) = delete;
            dog& operator=(dog&& other) = delete;

            int update(float delta, int frame) override;
            Vector2 get_direction_scalar();
            void render(Vector2 draw_position, int frame) override;
            virtual void set_path(const std::vector<Vector2>& path);
            virtual void set_path(const std::vector<Vector2>& path, int furniture_id, Vector2 furniture_position);
            // Final waypoint of the current path (where the dog is ultimately
            // headed), or {-1,-1} when it has no path. For tests/inspection.
            Vector2 peek_destination() const {
                return current_path_.empty() ? Vector2{-1.0f, -1.0f} : current_path_.back();
            }

        protected:
            bool reached_position(Vector2 target);
            void update_path(float delta);
            void move_toward_current_waypoint(float delta);
            void start_next_path();
            virtual void on_path_finished(Vector2 destination);
	        void determine_direction(Vector2 target);
	        void set_direction_index(size_t direction);

            body::body head_;
            const Vector2 move_speed_ = entity_config::dog_move_speed;
            Vector2 direction_scalar_;
            // current path is the current path that the dog is walking
            // move-paths are the next paths 9not that curent_path and move_path head are not the same
            // the move_path head is the next path
            std::queue<path> move_paths_;
            path current_path_;
    };    
    class player_dog : public dog{
        public:
            class state{
                public:
                    virtual ~state() = default;
                    state() {}
                    state(const state& other) = default;
                    state(state&& other) = default;

                    state& operator=(const state& other) = default;
                    state& operator=(state&& other) = default;

                    virtual void render(player_dog& dog, Vector2 draw_position, int frame) = 0;

            };
            class selected : public state {
                public:
                
                    selected() {}
                    selected(const selected& other) = default;
                    selected(selected&& other) = default;

                    selected& operator=(const selected& other) = default;
                    selected& operator=(selected&& other) = default;

                    void render(player_dog& dog, Vector2 draw_position, int frame) override;

            }; 
            class unselected : public state{
                public:
                    unselected() {}
                    unselected(const unselected& other) = default;
                    unselected(unselected&& other) = default;

                    unselected& operator=(const unselected& other) = default;
                    unselected& operator=(unselected&& other) = default;

                    void render(player_dog& dog, Vector2 draw_position, int frame) override;
            };
            
            ~player_dog() override{
                event_interface::unsubscribe<events::right_mouse_click>(right_mouse_click_handler_);
                event_interface::unsubscribe<events::selected_dog>(selected_dog_handler_);
            }
            player_dog(body::body body, body::body head, std::vector<sprite::sprite> outlines, Vector2 position, int id,
            std::string debug_id, int direction = level_config::directions::right, std::unique_ptr<player_dog::state> state = std::make_unique<unselected>())
            : dog(body, head, position, id, std::move(debug_id), direction), selected_state_(std::move(state)),
            outlines_(outlines), cosmetics_(), 
            right_mouse_click_handler_([this](const events::right_mouse_click& event) -> void {on_right_click_event(event);}),
            selected_dog_handler_([this](const events::selected_dog& event)->void {on_dog_select_event(event);}){
                event_interface::subscribe<events::right_mouse_click>(right_mouse_click_handler_);
                event_interface::subscribe<events::selected_dog>(selected_dog_handler_);
            }
            player_dog(const player_dog& other) = delete;
            player_dog(player_dog&& other) = default;

            player_dog& operator=(const player_dog& other) = delete;
            player_dog& operator=(player_dog&& other) = delete;

            void interact(entity& other) override;
            void select();
            void unselect();
            void render(Vector2 draw_position, int frame) override;
            void on_dog_select_event(const events::selected_dog& event);
            void on_right_click_event(const events::right_mouse_click& event);

            
        private:
            std::unique_ptr<state> selected_state_;
            std::vector<sprite::sprite> outlines_;
            std::vector<sprite::sprite> cosmetics_;
            

            events::event_handler<events::right_mouse_click> right_mouse_click_handler_;
            events::event_handler<events::selected_dog> selected_dog_handler_;

    };

    class npc_dog : public dog{
        public:
            // default constructor
            npc_dog(body::body body, body::body head, Vector2 position, int id, std::string debug_id,
            int direction = level_config::directions::right)
            : dog(body, head, position, id, std::move(debug_id), direction){}

            // constructor that specifes a path destintion
            npc_dog(body::body body, body::body head, Vector2 position, Vector2 path_dst, int id, std::string debug_id, int direction = level_config::directions::right)
            : dog(body, head, position, id, std::move(debug_id), direction){
                // upon creating an npc dog with a path destination, immediately query the graph for a path and set it
                debug::log(
                    "[npc_dog::npc_dog, querying path] "
                    "dog_id: " + std::to_string(id)
                    + ", source: {" + std::to_string(position.x) + ", " + std::to_string(position.y) + "}"
                    + ", destination: {" + std::to_string(path_dst.x) + ", " + std::to_string(path_dst.y) + "}"
                    + ", direction: {" + std::to_string(get_direction_scalar().x) + ", " + std::to_string(get_direction_scalar().y) + "}");
                std::unique_ptr<queries::query> path_query = std::make_unique<queries::path_query>(position, path_dst, get_direction_scalar());
                auto path = query_interface::execute_query(queries::path_executor_, *path_query);
                debug::log(
                    "[npc_dog::npc_dog, path query complete] "
                    "dog_id: " + std::to_string(id)
                    + ", path_size: " + std::to_string(path.size()));
                set_path(path);
            }
            npc_dog(const npc_dog& other) = delete;
            npc_dog(npc_dog&& other) = default;

            npc_dog& operator=(const npc_dog& other) = delete;
            npc_dog& operator=(npc_dog&& other) = delete;

            int update(float delta, int frame) override;
    };

    // Shared glue between customer_dog and waiter_dog: owns the state pointer and
    // forwards update/set_path/on_path_finished to it, casting to the concrete dog
    // type so state classes always operate on their own dog type (no downcasting).
    template <typename Derived, typename StateBase>
    class stateful_npc_dog : public npc_dog{
        public:
            template <typename... Args>
            explicit stateful_npc_dog(Args&&... args) : npc_dog(std::forward<Args>(args)...) {}

            int update(float delta, int frame) override{
                auto status = npc_dog::update(delta, frame);
                state_->update(static_cast<Derived&>(*this), delta, frame);
                return status;
            }
            void set_path(const std::vector<Vector2>& path) override{
                state_->set_path(static_cast<Derived&>(*this), path);
            }
            void set_path(const std::vector<Vector2>& path, int furniture_id, Vector2 furniture_position) override{
                state_->set_path(static_cast<Derived&>(*this), path, furniture_id, furniture_position);
            }
            void set_state(std::unique_ptr<StateBase> state){
                state_ = std::move(state);
            }

        protected:
            void on_path_finished(Vector2 destination) override{
                dog::on_path_finished(destination);
                state_->on_path_finished(static_cast<Derived&>(*this), destination);
            }

            std::unique_ptr<StateBase> state_;
    };

    class customer_dog;
    class waiter_dog;
    class food; // defined below; waiter_dog holds a unique_ptr<food> while serving

    class customer_dog_state{
        public:
            virtual ~customer_dog_state() = default;
            customer_dog_state() = default;
            customer_dog_state(const customer_dog_state& other) = default;
            customer_dog_state(customer_dog_state&& other) = default;

            customer_dog_state& operator=(const customer_dog_state& other) = default;
            customer_dog_state& operator=(customer_dog_state&& other) = default;

            virtual void update(customer_dog& dog, float delta, int frame) = 0;
            virtual void set_path(customer_dog& dog, const std::vector<Vector2>& path);
            virtual void set_path(customer_dog& dog, const std::vector<Vector2>& path, int furniture_id, Vector2 furniture_position);
            virtual void on_path_finished(customer_dog& dog, Vector2 destination);
            // Human-readable state name for inspection/tests.
            virtual std::string state_name() const { return "unknown"; }
    };

    class customer_dog_traveling_state : public customer_dog_state{
        public:
            explicit customer_dog_traveling_state(Vector2 destination) : destination_(destination) {}

            void on_path_finished(customer_dog& dog, Vector2 destination) final;

        protected:
            virtual void on_arrived(customer_dog& dog) = 0;

        private:
            Vector2 destination_;
    };

    class customer_dog : public stateful_npc_dog<customer_dog, customer_dog_state>{
        using base = stateful_npc_dog<customer_dog, customer_dog_state>;
        public:
            class default_state : public customer_dog_state{
                public:
                    void update(customer_dog& dog, float delta, int frame) override;
                    std::string state_name() const override { return "default_state"; }
            };

            class walking_to_table : public customer_dog_traveling_state{
                public:
                    walking_to_table(size_t table_id, Vector2 table_position, Vector2 interaction_position)
                    : customer_dog_traveling_state(interaction_position),
                    table_id_(table_id), table_position_(table_position){}

                    void update(customer_dog& dog, float delta, int frame) override;
                    std::string state_name() const override { return "walking_to_table"; }

                protected:
                    void on_arrived(customer_dog& dog) override;

                private:
                    size_t table_id_;
                    Vector2 table_position_;
            };

            class seated : public customer_dog_state{
                public:
                    void update(customer_dog& dog, float delta, int frame) override;
                    std::string state_name() const override { return "seated"; }
            };

            class eating : public customer_dog_state{
                public:
                    eating(size_t order_id, size_t table_id, Vector2 table_position)
                    : order_id_(order_id), table_id_(table_id), table_position_(table_position){}

                    void update(customer_dog& dog, float delta, int frame) override;
                    std::string state_name() const override { return "eating"; }
                private:
                    size_t order_id_;
                    size_t table_id_;
                    Vector2 table_position_;
                    float elapsed_ = 0.0f;
            };

            class leaving : public customer_dog_state{
                public:
                    void update(customer_dog& dog, float delta, int frame) override;
                    std::string state_name() const override { return "leaving"; }
            };

            customer_dog(body::body body, body::body head, Vector2 position, int id, std::string debug_id,
            int direction = level_config::directions::right,
            std::unique_ptr<customer_dog_state> state = std::make_unique<default_state>())
            : base(body, head, position, id, std::move(debug_id), direction),
            give_dog_path_handler_([this](const events::give_dog_path& event) -> void {on_give_dog_path_event(event);}){
                state_ = std::move(state);
                event_interface::subscribe<events::give_dog_path>(give_dog_path_handler_);
            }

            customer_dog(body::body body, body::body head, Vector2 position, Vector2 path_dst, int id, std::string debug_id,
            int direction = level_config::directions::right,
            std::unique_ptr<customer_dog_state> state = std::make_unique<default_state>())
            : base(body, head, position, path_dst, id, std::move(debug_id), direction),
            give_dog_path_handler_([this](const events::give_dog_path& event) -> void {on_give_dog_path_event(event);}){
                state_ = std::move(state);
                event_interface::subscribe<events::give_dog_path>(give_dog_path_handler_);
            }
            ~customer_dog() override{
                event_interface::unsubscribe<events::give_dog_path>(give_dog_path_handler_);
            }
            customer_dog(const customer_dog& other) = delete;
            customer_dog(customer_dog&& other) = default;

            customer_dog& operator=(const customer_dog& other) = delete;
            customer_dog& operator=(customer_dog&& other) = delete;

            void set_walking_to_table(size_t table_id, Vector2 table_position, Vector2 interaction_position);
            void set_eating(size_t order_id, size_t table_id, Vector2 table_position);
            void on_give_dog_path_event(const events::give_dog_path& event);
            std::string get_state_name() const { return state_->state_name(); }

        private:
            // Customer behaviour state belongs to the dog entity. The maitre d'
            // only tracks queue/table allocation by id and emits commands that
            // cause the level or dog to move between these states.
            events::event_handler<events::give_dog_path> give_dog_path_handler_;
    };

    class waiter_dog_state{
        public:
            virtual ~waiter_dog_state() = default;
            waiter_dog_state() = default;
            waiter_dog_state(const waiter_dog_state& other) = default;
            waiter_dog_state(waiter_dog_state&& other) = default;

            waiter_dog_state& operator=(const waiter_dog_state& other) = default;
            waiter_dog_state& operator=(waiter_dog_state&& other) = default;

            virtual void update(waiter_dog& dog, float delta, int frame) = 0;
            virtual bool is_available_for_order() = 0;
            virtual void set_path(waiter_dog& dog, const std::vector<Vector2>& path);
            virtual void set_path(waiter_dog& dog, const std::vector<Vector2>& path, int furniture_id, Vector2 furniture_position);
            virtual void on_path_finished(waiter_dog& dog, Vector2 destination);
            virtual std::string state_name() const { return "unknown"; }
    };

    class waiter_dog_traveling_state : public waiter_dog_state{
        public:
            explicit waiter_dog_traveling_state(Vector2 destination) : destination_(destination) {}

            void on_path_finished(waiter_dog& dog, Vector2 destination) final;
            bool is_available_for_order() override;
        protected:
            virtual void on_arrived(waiter_dog& dog) = 0;

        private:
            Vector2 destination_;
    };

    class waiter_dog : public stateful_npc_dog<waiter_dog, waiter_dog_state>{
        using base = stateful_npc_dog<waiter_dog, waiter_dog_state>;
        public:
            class idle : public waiter_dog_state{
                public:
                    idle()
                    : waiter_dog_state(){}
                    void update(waiter_dog& dog, float delta, int frame) override;
                    bool is_available_for_order() override;
                    std::string state_name() const override { return "idle"; }
                };
            // Busy marker: a serving waiter is not available for another order.
            // The counter -> table journey is orchestrated by the expediter off
            // dog_completed_path, so serving needs no per-leg travel logic itself.
            class serving : public waiter_dog_state{
                public:
                    void update(waiter_dog& dog, float delta, int frame) override;
                    bool is_available_for_order() override;
                    std::string state_name() const override { return "serving"; }
            };
            waiter_dog(body::body body, body::body head, Vector2 position, int id, std::string debug_id,
            int direction = level_config::directions::right,
            std::unique_ptr<waiter_dog_state> state = std::make_unique<idle>())
            : base(body, head, position, id, std::move(debug_id), direction){
                state_ = std::move(state);
            }
            waiter_dog(const waiter_dog& other) = delete;
            waiter_dog(waiter_dog&& other) = default;
            // Out of line so the unique_ptr<food> member can destruct where food
            // is a complete type (food is only forward-declared here).
            ~waiter_dog() override;

            waiter_dog& operator=(const waiter_dog& other) = delete;
            waiter_dog& operator=(waiter_dog&& other) = delete;

            bool is_available_for_order();
            std::string get_state_name() const { return state_->state_name(); }
            void set_serving();
            void set_idle();
            // Carry food between the counter and the table. hold_food takes
            // ownership; release_food hands it off (e.g. delivered to the table).
            void hold_food(std::unique_ptr<food> item);
            std::unique_ptr<food> release_food();
            bool is_carrying_food() const;

        private:
            std::unique_ptr<food> held_food_;
    };

    // body behaves slightly differently for decorations, it will have the variants for the decoration (probably should be called deocraiotn)
    class decoration : public entity {
        public:
            decoration(body::body body, Vector2 position, int id, std::string debug_id)
            : entity(body, position, id, std::move(debug_id)),
            moved_cursor_handler([this](const events::moved_cursor& event) -> void { on_moved_cursor(event);} ),
            pre_move_position_(position_), post_move_position_(position_){
                // upon creating a decoration, let the graph know where it was placed with the event
                auto rectangle = body_.get_hitbox().get_box();
                std::unique_ptr<events::event> place_decoration = std::make_unique<events::placed_decoration>(rectangle, id_);
                event_interface::execute_event(*place_decoration);
            }
            decoration(const decoration& other) = default;
            decoration(decoration&& other) = default;

            decoration& operator=(const decoration& other) = delete;
            decoration& operator=(decoration&& other) = delete;
            
            void on_moved_cursor(const events::moved_cursor& event);
            bool can_place_down();
            virtual void place_down();
            void pick_up();
            void subscribe_to_cursor();
            void unsubscribe_from_cursor();

        private:
            Vector2 round_position();
            events::event_handler<events::moved_cursor> moved_cursor_handler;
            Vector2 pre_move_position_;
            Vector2 post_move_position_;
        
    };

    class station : public decoration {
        public:
            struct interaction_positions{
                Vector2 left;
                Vector2 right;
            };
            enum station_type{
                table_station = 0,
                food_counter_station = 1
            };

            station(body::body body, Vector2 position, int id, std::string debug_id, station_type type)
            : decoration(body, position, id, std::move(debug_id)), interaction_positions_{}, type_(type){
                update_interaction_positions();
            }
            station(const station& other) = default;
            station(station&& other) = default;

            station& operator=(const station& other) = delete;
            station& operator=(station&& other) = delete;

            station_type get_station_type();
            void interact(entity& other) override;

            // Interaction positions are the walkable nodes flanking the station
            // (left and right) that a dog paths to in order to interact with it.
            // Centralised here so every station type (table, food counter) shares
            // one implementation instead of each recomputing its own.
            interaction_positions get_interaction_positions() const;

        protected:
            void update_interaction_positions();
            interaction_positions interaction_positions_; // TODO ! refactor this type, why the fuck is it under events 

        private:
            station_type type_;
    };

    class table : public station {
        public:
            enum table_state{
                available = 0,
                reserved = 1,
                occupied = 2
            };

            table(body::body body, Vector2 position, int id, std::string debug_id)
            : station(body, position, id, std::move(debug_id), station_type::table_station),
            state_(table_state::available), assigned_dog_id_(level_config::empty_node){}
            table(const table& other) = default;
            table(table&& other) = default;

            table& operator=(const table& other) = delete;
            table& operator=(table&& other) = delete;

            bool can_accept_dog();
            bool reserve_for(int dog_id);
            void occupy();
            void clear();
            table_state get_state();
            int get_assigned_dog_id();
            void place_down() override;

        private:
            table_state state_;
            int assigned_dog_id_;
    };

    // food is a generic world entity. Different foods are produced by different
    // builders (see build_test_food); subclasses are introduced only as-needed.
    class food : public entity {
        public:
            food(body::body body, Vector2 position, int id, std::string debug_id)
            : entity(body, position, id, std::move(debug_id)){}
            food(const food& other) = default;
            food(food&& other) = default;

            food& operator=(const food& other) = delete;
            food& operator=(food&& other) = delete;
    };

    // A food_counter stores food as a FILO stack, up to a fixed capacity. Producers
    // store() food onto it; waiters take() the top for service (ownership moves out).
    class food_counter : public station {
        public:
            enum counter_status{
                empty = 0,
                has_food = 1,
                full = 2
            };

            food_counter(body::body body, Vector2 position, int id, std::string debug_id)
            : station(body, position, id, std::move(debug_id), station_type::food_counter_station),
            max_capacity_(entity_config::food_counter_capacity), stored_food_(){}
            food_counter(const food_counter& other) = delete;
            food_counter(food_counter&& other) = default;

            food_counter& operator=(const food_counter& other) = delete;
            food_counter& operator=(food_counter&& other) = delete;

            // store: push food onto the stack, rejects (returns false) when full.
            bool store(std::unique_ptr<food> item);
            // take: pop and move out the top of the stack. Precondition: !is_empty().
            std::unique_ptr<food> take();
            bool is_empty() const;
            size_t current_capacity() const;
            size_t max_capacity() const;
            counter_status status() const;
            // Reservations: food promised to an in-flight order that hasn't been
            // collected yet. available_capacity() = stored - reserved, so a second
            // order can't claim the same item before the first waiter picks it up.
            void reserve();
            void release_reservation();
            size_t reserved() const;
            size_t available_capacity() const;
            bool has_available_food() const;
            void render(Vector2 draw_position, int frame) override;
            void place_down() override;

        private:
            size_t max_capacity_;
            std::vector<std::unique_ptr<food>> stored_food_;
            size_t reserved_ = 0;
    };
    // ------------------ entity builder ------------------ //
    class entity_builder{
        public:
            std::unique_ptr<entity> build_cursor(Vector2 position, int id);
            std::unique_ptr<entity> build_mack(Vector2 position, int id);
            std::unique_ptr<entity> build_khiri(Vector2 position, int id);
            // NPC dog sprite art/config pending.
            std::unique_ptr<entity> build_customer_dog(int id, int dog_type, Vector2 position, std::optional<Vector2> destination);
            std::unique_ptr<entity> build_waiter_dog(int id, int dog_type, Vector2 position, std::optional<Vector2> destination);
            std::unique_ptr<entity> build_paw_mark(Vector2 position, int id);

            std::unique_ptr<entity> build_test_decoration(Vector2 position, int id);
            std::unique_ptr<entity> build_gargoyle(Vector2 position, int id);
            std::unique_ptr<entity> build_table(Vector2 position, int id);
            std::unique_ptr<entity> build_food_counter(Vector2 position, int id);
            // basic/first food builder; producers stocking a counter pass
            // counter.get_position() + entity_config::food_draw_offset as the position.
            std::unique_ptr<food> build_test_food(Vector2 position, int id);
            ~entity_builder() = default;
            entity_builder() : debug_id_counts_() {}
            entity_builder(const entity_builder& other) = default;
            entity_builder(entity_builder&& other) = default;

            entity_builder& operator=(const entity_builder& other) = default;
            entity_builder& operator=(entity_builder&& other) = default;

        private:
            std::string next_debug_id(const std::string& prefix);
            std::map<std::string, size_t> debug_id_counts_;

    };
    extern entity_builder e_builder;
}
#endif
