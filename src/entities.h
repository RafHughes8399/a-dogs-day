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
            entity(body::body body, Vector2 position, int id)
            : body_(body), position_(position), id_(id){}
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
            hitbox::hitbox& get_hitbox();
            sprite::sprite& get_sprite();

            Vector2 get_position();
            void move(Vector2 new_postion);
            
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
                cursor(body::body body, Vector2 position, int id)
                : entity(body, position, id), 
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
        paw_mark(body::body body, Vector2 position, int id)
        : entity(body, position, id){}
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
    class player_dog : public entity{
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
            int direction = level_config::directions::right, std::unique_ptr<player_dog::state> state = std::make_unique<unselected>())
            : entity(body, position, id), head_(head), outlines_(outlines), cosmetics_(), 
            direction_scalar_(level_config::direction_scalars[direction]), selected_state_(std::move(state)),
            right_mouse_click_handler_([this](const events::right_mouse_click& event) -> void {on_right_click_event(event);}),
            selected_dog_handler_([this](const events::selected_dog& event)->void {on_dog_select_event(event);}){
                event_interface::subscribe<events::right_mouse_click>(right_mouse_click_handler_);
                event_interface::subscribe<events::selected_dog>(selected_dog_handler_);
                body_.set_index(static_cast<size_t>(direction));
            }
            player_dog(const player_dog& other) = delete;
            player_dog(player_dog&& other) = default;

            player_dog& operator=(const player_dog& other) = delete;
            player_dog& operator=(player_dog&& other) = delete;

            int update(float delta, int frame) override;
            Vector2 get_direction_scalar();

            void interact(entity& other) override;
            void select();
            void unselect();
            void render(Vector2 draw_position, int frame) override;
            void on_dog_select_event(const events::selected_dog& event);
            void on_right_click_event(const events::right_mouse_click& event);
            void set_path(std::vector<Vector2>& path);

            
        private:
            /**
             * setup a direction map to a scalar vector
             * so up = {0, -1}
             * so right = {1, 0}
             * 
             * so left = {-1, 0}
             */
            bool reached_position(Vector2 target);
            void determine_direction(Vector2 target);
            
            body::body head_;
            
            const Vector2 move_speed_ = entity_config::dog_move_speed;
            Vector2 direction_scalar_;

            std::unique_ptr<state> selected_state_;
            std::vector<sprite::sprite> outlines_;
            std::vector<sprite::sprite> cosmetics_;
            std::vector<Vector2> move_path_; // the prev array from the path algorithm
            

            events::event_handler<events::right_mouse_click> right_mouse_click_handler_;
            events::event_handler<events::selected_dog> selected_dog_handler_;

    };
    // body behaves slightly differently for decorations, it will have the variants for the decoration (probably should be called deocraiotn)
    class decoration : public entity {
        public:
            decoration(body::body body, Vector2 position, int id)
            : entity(body, position, id), pre_move_position_(position_), post_move_position_(position_),
            moved_cursor_handler([this](const events::moved_cursor& event) -> void { on_moved_cursor(event);} ){
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
            void place_down();
            void pick_up();
            void subscribe_to_cursor();
            void unsubscribe_from_cursor();

        private:
            Vector2 round_position();
            events::event_handler<events::moved_cursor> moved_cursor_handler;
            Vector2 pre_move_position_;
            Vector2 post_move_position_;
        
    };
    // ------------------ entity builder ------------------ //
    class entity_builder{
        public:
            std::unique_ptr<entity> build_cursor(Vector2 position, int id);
            std::unique_ptr<entity> build_mack(Vector2 position, int id);
            std::unique_ptr<entity> build_khiri(Vector2 position, int id);
            std::unique_ptr<entity> build_paw_mark(Vector2 position, int id);

            std::unique_ptr<entity> build_test_decoration(Vector2 position, int id);
            std::unique_ptr<entity> build_gargoyle(Vector2 position, int id);
            ~entity_builder() = default;
            entity_builder() {}
            entity_builder(const entity_builder& other) = default;
            entity_builder(entity_builder&& other) = default;

            entity_builder& operator=(const entity_builder& other) = default;
            entity_builder& operator=(entity_builder&& other) = default;

    };
    extern entity_builder e_builder;
}
#endif