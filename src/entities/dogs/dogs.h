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
            : entity(body, position, id, std::move(debug_id)), direction_scalar_(level_config::direction_scalars[direction]),
            head_(head){
                body_.set_index(static_cast<size_t>(direction));
                head_.set_index(static_cast<size_t>(direction));
            }
            dog(const dog& other) = delete;
            dog(dog&& other) = default;

            dog& operator=(const dog& other) = delete;
            dog& operator=(dog&& other) = delete;

            Vector2 get_direction_scalar();
            // Final waypoint of the current path (where the dog is ultimately
            // headed), or {-1,-1} when it has no path. For tests/inspection.
            Vector2 peek_destination() const {
                return current_path_.empty() ? Vector2{-1.0f, -1.0f} : current_path_.back();
            }
            void render(Vector2 draw_position, int frame) override;
            virtual void set_path(const std::vector<Vector2>& path);
            virtual void set_path(const std::vector<Vector2>& path, int station_id, Vector2 station_position);
            int update(float delta, int frame) override;

        protected:
	        void determine_direction(Vector2 target);
            void move_toward_current_waypoint(float delta);
            // Fires the dog_completed_path fact so position-tracking systems
            // (maitre_d's queue bookkeeping) can react. Nothing overrides this
            // any more: dog behaviour reacts to update_path's completed_path
            // return value inside the owning dog's own state, not to a
            // callback fanned out through the entity hierarchy.
            void on_path_finished(Vector2 destination);
            bool reached_position(Vector2 target);
	        void set_direction_index(size_t direction);
            void start_next_path();
            int update_path(float delta);

            // current path is the current path that the dog is walking
            // move-paths are the next paths 9not that curent_path and move_path head are not the same
            // the move_path head is the next path
            path current_path_;
            Vector2 direction_scalar_;
            body::body head_;
            std::queue<path> move_paths_;
            const Vector2 move_speed_ = entity_config::dog_move_speed;
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
            : dog(body, head, position, id, std::move(debug_id), direction), cosmetics_(),
            outlines_(outlines),
            right_mouse_click_handler_([this](const events::right_mouse_click& event) -> void {on_right_click_event(event);}),
            selected_dog_handler_([this](const events::selected_dog& event)->void {on_dog_select_event(event);}),
            selected_state_(std::move(state)){
                event_interface::subscribe<events::right_mouse_click>(right_mouse_click_handler_);
                event_interface::subscribe<events::selected_dog>(selected_dog_handler_);
            }
            player_dog(const player_dog& other) = delete;
            player_dog(player_dog&& other) = default;

            player_dog& operator=(const player_dog& other) = delete;
            player_dog& operator=(player_dog&& other) = delete;

            void interact(entity& other) override;
            void on_dog_select_event(const events::selected_dog& event);
            void on_right_click_event(const events::right_mouse_click& event);
            void render(Vector2 draw_position, int frame) override;
            void select();
            void unselect();


        private:
            std::vector<sprite::sprite> cosmetics_;
            std::vector<sprite::sprite> outlines_;
            events::event_handler<events::right_mouse_click> right_mouse_click_handler_;
            events::event_handler<events::selected_dog> selected_dog_handler_;
            std::unique_ptr<state> selected_state_;

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

            // Human-readable state name for inspection/tests.
            virtual std::string state_name() const { return "unknown"; }
            // `status` is whatever the dog's own movement update returned this
            // frame; status_codes::completed_path means a path leg finished on
            // this frame. States react to that instead of receiving a separate
            // arrival callback, so the dog answers "did I arrive" and the state
            // answers "what happens now". Note that by the time a state sees
            // completed_path the dog has already started the next queued leg
            // (dog::update_path calls start_next_path before returning), so
            // "the whole journey is over" is current_path_.empty() &&
            // move_paths_.empty(), not completed_path on its own.
            virtual int update(customer_dog& dog, float delta, int frame, int status) = 0;
    };

    class customer_dog : public npc_dog{
        public:
            // The customer's own counterpart to waiter_dog::animation: picking
            // up the food the waiter placed on the table (start of eating) and
            // placing the empty plate back down (end of eating, ready for a
            // waiter to collect - see clear_table/expediter::dispatch_clearing_job).
            // `size` tracks the element count (same pattern as events::ids,
            // events/event_core.h).
            enum animation{
                resting = 0,
                picking_up_food,
                placing_plate,
                size
            };
            class default_state : public customer_dog_state{
                public:
                    std::string state_name() const override { return "default_state"; }
                    int update(customer_dog& dog, float delta, int frame, int status) override;
            };

            // Walks to the table it was given and seats itself on arrival. The
            // interaction position it is walking to isn't stored: the dog owns
            // the path, so "a leg finished and nothing is left to walk" is a
            // complete arrival test on its own.
            class walking_to_table : public customer_dog_state{
                public:
                    walking_to_table(size_t table_id, Vector2 table_position)
                    : table_id_(table_id), table_position_(table_position){}

                    std::string state_name() const override { return "walking_to_table"; }
                    int update(customer_dog& dog, float delta, int frame, int status) override;

                private:
                    size_t table_id_;
                    Vector2 table_position_;
            };

            class seated : public customer_dog_state{
                public:
                    std::string state_name() const override { return "seated"; }
                    int update(customer_dog& dog, float delta, int frame, int status) override;
            };

            class eating : public customer_dog_state{
                public:
                    eating(size_t order_id, size_t table_id, Vector2 table_position)
                    : order_id_(order_id), table_id_(table_id), table_position_(table_position){}

                    std::string state_name() const override { return "eating"; }
                    int update(customer_dog& dog, float delta, int frame, int status) override;
                private:
                    float elapsed_ = 0.0f;
                    size_t order_id_;
                    size_t table_id_;
                    Vector2 table_position_;
            };

            class leaving : public customer_dog_state{
                public:
                    std::string state_name() const override { return "leaving"; }
                    int update(customer_dog& dog, float delta, int frame, int status) override;
            };

            customer_dog(body::body body, body::body head, Vector2 position, int id, std::string debug_id,
            int direction = level_config::directions::right,
            std::unique_ptr<customer_dog_state> state = std::make_unique<default_state>())
            : npc_dog(body, head, position, id, std::move(debug_id), direction),
            give_dog_path_handler_([this](const events::give_dog_path& event) -> void {on_give_dog_path_event(event);}),
            state_(std::move(state)){
                event_interface::subscribe<events::give_dog_path>(give_dog_path_handler_);
            }

            customer_dog(body::body body, body::body head, Vector2 position, Vector2 path_dst, int id, std::string debug_id,
            int direction = level_config::directions::right,
            std::unique_ptr<customer_dog_state> state = std::make_unique<default_state>())
            : npc_dog(body, head, position, path_dst, id, std::move(debug_id), direction),
            give_dog_path_handler_([this](const events::give_dog_path& event) -> void {on_give_dog_path_event(event);}),
            state_(std::move(state)){
                event_interface::subscribe<events::give_dog_path>(give_dog_path_handler_);
            }
            ~customer_dog() override{
                event_interface::unsubscribe<events::give_dog_path>(give_dog_path_handler_);
            }
            customer_dog(const customer_dog& other) = delete;
            customer_dog(customer_dog&& other) = default;

            customer_dog& operator=(const customer_dog& other) = delete;
            customer_dog& operator=(customer_dog&& other) = delete;

            std::string get_state_name() const { return state_->state_name(); }
            void on_give_dog_path_event(const events::give_dog_path& event);
            // A station-targeted path always means "go sit at this table", so
            // the transition happens here rather than round-tripping an event.
            using dog::set_path;
            void set_path(const std::vector<Vector2>& path, int station_id, Vector2 station_position) override;
            void set_eating(size_t order_id, size_t table_id, Vector2 table_position);
            void set_state(std::unique_ptr<customer_dog_state> state){
                state_ = std::move(state);
            }
            void set_walking_to_table(size_t table_id, Vector2 table_position);
            void leave();
            int update(float delta, int frame) override;
        private:
            // Customer behaviour state belongs to the dog entity. The maitre d'
            // only tracks queue/table allocation by id and emits commands that
            // cause the level or dog to move between these states.
            events::event_handler<events::give_dog_path> give_dog_path_handler_;
            std::unique_ptr<customer_dog_state> state_;
    };

    class waiter_dog_state{
        public:
            virtual ~waiter_dog_state() = default;
            waiter_dog_state() = default;
            waiter_dog_state(const waiter_dog_state& other) = default;
            waiter_dog_state(waiter_dog_state&& other) = default;

            waiter_dog_state& operator=(const waiter_dog_state& other) = default;
            waiter_dog_state& operator=(waiter_dog_state&& other) = default;

            virtual bool is_available_for_order() = 0;
            virtual std::string state_name() const { return "unknown"; }
            // Same contract as customer_dog_state::update - `status` is the
            // dog's own movement result for this frame, and completed_path is
            // how a state learns a leg finished.
            virtual int update(waiter_dog& dog, float delta, int frame, int status) = 0;
    };

    class waiter_dog : public npc_dog{
        public:
            // Pickup/placement animations the waiter plays while serving and
            // clearing. `size` tracks the element count (same pattern as
            // events::ids, events/event_core.h) - not tied to state
            // transitions directly; see the TODOs on serving/clearing
            // update() and expediter's dog_completed_path handling for where
            // each of these needs to gate on playback time.
            enum animation{
                resting = 0,
                picking_up_food,
                placing_food,
                picking_up_plate,
                placing_plate,
                size
            };
            class idle : public waiter_dog_state{
                public:
                    idle()
                    : waiter_dog_state(){}
                    bool is_available_for_order() override;
                    std::string state_name() const override { return "idle"; }
                    int update(waiter_dog& dog, float delta, int frame, int status) override;
                };
            // TODO: [waiter_dog::serving] [class shape] change from [one
            // waiter_dog_state busy-marker; counter-vs-table leg told apart
            // reactively in expediter via is_carrying_food()] to [split into
            // two waiter_dog_state subclasses, one per leg, mirroring
            // customer_dog::walking_to_table's new shape:
            //   class serving_counter : public waiter_dog_state{ ... };
            //   class serving_table : public waiter_dog_state{ ... };
            // Each one's update() does its arrival work when status ==
            // status_codes::completed_path, then set_state()s to the next leg
            // (serving_counter -> serving_table) or set_idle()s
            // (serving_table, after firing order_served). Neither needs to
            // store a destination: the expediter queues both legs onto the
            // dog at dispatch, so completed_path IS the leg boundary.
            // The leg IS the class now - no more incidental-flag guessing,
            // per project convention (CLAUDE.md: "prefer adding a state class
            // over adding boolean flags").
            class serving : public waiter_dog_state{
                public:
                    bool is_available_for_order() override;
                    std::string state_name() const override { return "serving"; }
                    int update(waiter_dog& dog, float delta, int frame, int status) override;
            };
            // TODO: [waiter_dog::clearing] [class shape] change from [one
            // waiter_dog_state busy-marker; table-vs-dishwasher leg told
            // apart reactively in expediter via job.dishwasher_id==empty_id]
            // to [split into clearing_table / clearing_dishwasher, same
            // per-leg shape as serving_counter/serving_table above.
            // clearing_table's completed_path branch picks up the plate and
            // set_state()s to clearing_dishwasher; clearing_dishwasher's
            // places the plate, fires a new events::waiter_finished_clearing
            // fact (add via the event-wire skill) so the expediter erases the
            // clearing_job, then set_idle()s itself - mirroring
            // customer_dog::eating calling dog.leave() as its last act.
            // clearing_dishwasher carries table_id_/dishwasher_id_ only
            // because the completion event needs them, not for routing.]
            class clearing : public waiter_dog_state{
                public:
                    bool is_available_for_order() override;
                    std::string state_name() const override { return "clearing"; }
                    int update(waiter_dog& dog, float delta, int frame, int status) override;
            };
            waiter_dog(body::body body, body::body head, Vector2 position, int id, std::string debug_id,
            int direction = level_config::directions::right,
            std::unique_ptr<waiter_dog_state> state = std::make_unique<idle>())
            : npc_dog(body, head, position, id, std::move(debug_id), direction),
            state_(std::move(state)){}
            waiter_dog(const waiter_dog& other) = delete;
            waiter_dog(waiter_dog&& other) = default;
            // Out of line so the unique_ptr<food> member can destruct where food
            // is a complete type (food is only forward-declared here).
            ~waiter_dog() override;

            waiter_dog& operator=(const waiter_dog& other) = delete;
            waiter_dog& operator=(waiter_dog&& other) = delete;

            std::string get_state_name() const { return state_->state_name(); }
            // Carry food between the counter and the table. hold_food takes
            // ownership; release_food hands it off (e.g. delivered to the table).
            void hold_food(std::unique_ptr<food> item);
            bool is_available_for_order();
            bool is_carrying_food() const;
            std::unique_ptr<food> release_food();
            void set_idle();
            void set_state(std::unique_ptr<waiter_dog_state> state){
                state_ = std::move(state);
            }
            // TODO: [waiter_dog::set_serving/set_clearing] [signature] change
            // from [set_serving(), set_clearing() - one entry point per job
            // kind] to [set_serving_counter(), set_clearing_table() - naming
            // the *first leg* rather than the job, once the states split.
            // Still no parameters: the destination lives in the path the
            // expediter queues onto the dog, not in the state. The later legs
            // (serving_table, clearing_dishwasher) are never set externally -
            // they're only entered by the previous leg's state transitioning
            // on completed_path - so they need no set_* entry point at all.]
            void set_serving();
            void set_clearing();
            int update(float delta, int frame) override;

        private:
            std::unique_ptr<food> held_food_;
            std::unique_ptr<waiter_dog_state> state_;
    };
    class dishwasher_dog : public npc_dog{
        public:
            dishwasher_dog(body::body body, body::body head, Vector2 position, int id, std::string debug_id,
            int direction = level_config::directions::right)
            : npc_dog(body, head, position, id, std::move(debug_id), direction){
            }
            dishwasher_dog(const dishwasher_dog& other) = delete;
            dishwasher_dog(dishwasher_dog&& other) = default;
            ~dishwasher_dog() override = default;

            dishwasher_dog& operator=(const dishwasher_dog& other) = delete;
            dishwasher_dog& operator=(dishwasher_dog&& other) = delete;
    };
}
#endif
