/**
 * dog entity hierarchy.
 *
 * There are multiple kinds of dogs:
 *  -> the player dog (K and M): moves around responding to cursor events, also
 *     has cosmetics (hat, shirt, paw clothes)
 *  -> helper dogs (waiters, cooks, etc)
 *  -> customer dogs
 */
#ifndef DOGS_H
#define DOGS_H

#include "entity.h"

namespace entities{
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
    class food; // waiter_dog holds a unique_ptr<food> while serving

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
}
#endif
