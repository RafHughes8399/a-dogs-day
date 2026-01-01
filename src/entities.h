/**
 * header file that defines entitiy class hierarchy
 */
#ifndef ENTITIES_H
#define ENTITIES_H


#include <iostream>
#include "config.h"
#include "events.h"
#include "events_interface.h"
#include "hitbox.h"
#include "queries.h"
#include "query_interface.h"
#include "raglib.h"
#include "sprite.h"
#include "texture.h"
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
            entity(sprite::sprite sprite, hitbox::hitbox hitbox, Vector2 position, int id)
            : hitbox_(hitbox), sprite_(sprite), position_(position), id_(id){

            };
            entity(const entity& other) = default;

            entity(entity&& other) = default;
            entity& operator=(const entity& other) = default;
            entity& operator=(entity&& other) = default;
            bool operator==(entity& other){
                return id_ == other.id_;
            }

            bool check_collision(const hitbox::hitbox other);
            hitbox::hitbox& get_hitbox();
            sprite::sprite&  get_sprite();
            Vector2 get_position();
            int get_id();

            void render();

            virtual int update(float delta){
                (void) delta;
                return status_codes::nothing;
            }
            virtual void interact(entity& other){
                (void) other;
                return;
            }

        protected:
            const int id_;
            
            hitbox::hitbox hitbox_;
            sprite::sprite sprite_;
            Vector2 position_;

    };

    class cursor : public entity{
        public:
            class interaction_strategy{
                public:
                    virtual ~interaction_strategy() = default;
                    interaction_strategy(){};
                    interaction_strategy(const interaction_strategy& other) = default;
                    interaction_strategy(interaction_strategy&& other) = default;
                    
                    interaction_strategy& operator=(const interaction_strategy& other) = default;
                    interaction_strategy& operator=(interaction_strategy&& other) = default;

                    virtual void interact(cursor& cursor, entity& other) = 0;
                private:
            };
            class left_click_strategy : public interaction_strategy{
                public:
                    left_click_strategy()
                    : interaction_strategy() {};
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
                    : interaction_strategy() {};
                    right_click_strategy(const right_click_strategy& other) = default;
                    right_click_strategy(right_click_strategy&& other) = default;
                    
                    right_click_strategy& operator=(const right_click_strategy& other) = default;
                    right_click_strategy& operator=(right_click_strategy&& other) = default;

                    void interact(cursor& cursor, entity& other) override;
                    private:
                };

            class default_strategy : public interaction_strategy{
                public:
                    default_strategy()
                    : interaction_strategy() {};
                    default_strategy(const default_strategy& other) = default;
                    default_strategy(default_strategy&& other) = default;
                        
                    default_strategy& operator=(const default_strategy& other) = default;
                    default_strategy& operator=(default_strategy&& other) = default;
                    void interact(cursor& cursor, entity& other) override;
                private:
            };
                ~cursor() {
                    event_interface::unsubscribe<events::left_mouse_click>(left_mouse_click_handler_);
                    event_interface::unsubscribe<events::left_mouse_down>(left_mouse_down_handler_);
                    event_interface::unsubscribe<events::right_mouse_click>(right_mouse_click_handler_);
                }
                cursor(sprite::sprite sprite, hitbox::hitbox hitbox, Vector2 position, int id)
                : entity(sprite, hitbox, position, id), 
                left_mouse_click_handler_([this](const events::left_mouse_click& event) -> void{on_left_mouse_click_event(event);}),
                left_mouse_down_handler_([this](const events::left_mouse_down& event) -> void{on_left_mouse_down_event(event);}),
                right_mouse_click_handler_([this](const events::right_mouse_click& event) -> void{on_right_mouse_click_event(event);}),
                interaction_strategy_(std::make_unique<default_strategy>()){
                    event_interface::subscribe<events::left_mouse_click>(left_mouse_click_handler_);
                    event_interface::subscribe<events::left_mouse_down>(left_mouse_down_handler_);
                    event_interface::subscribe<events::right_mouse_click>(right_mouse_click_handler_);
                };
                cursor(const cursor& other) = default;
                cursor(cursor&& other) = default;
                    
                cursor& operator=(const cursor& other) = default;
                cursor& operator=(cursor&& other)  = default;
                
                
                int update(float delta) override;
                
                void interact(entity& other) override;            
                void on_left_mouse_click_event(const events::left_mouse_click& event);
                void on_left_mouse_down_event(const events::left_mouse_down& event);
                void on_right_mouse_click_event(const events::right_mouse_click& event);                
            private:
                enum animation_tags{
                        base = 0,
                        hover = 1
                };
                events::event_handler<events::left_mouse_click> left_mouse_click_handler_;
                events::event_handler<events::left_mouse_down> left_mouse_down_handler_;
                events::event_handler<events::right_mouse_click> right_mouse_click_handler_;

                std::unique_ptr<interaction_strategy> interaction_strategy_;
        };
        
        class paw_mark : public entity{
        public:
        ~paw_mark() = default;
        paw_mark(sprite::sprite sprite, hitbox::hitbox hitbox, Vector2 position, int id)
        : entity(sprite, hitbox, position, id){};
            paw_mark(const paw_mark& other) = default;
            paw_mark(paw_mark&& other) = default;

            paw_mark& operator=(const paw_mark& other) = default;
            paw_mark& operator=(paw_mark&& other) = default;

            int update(float delta) override;
            void interact(entity& other) override;

        private:
    };

    /**
     * there would be multiple kinds of dogs
     * -> the player dog (K and M )
     *      -> the player dog moves around, responding to cursor events 
     *      -> also has cosmetics (hat, shirt, paw clothes)
     *      
     * -> hepler dogs (waiters, cooks, etc)
     * -> customer dogs 
     */
    class player_dog : public entity{
        public:
            ~player_dog(){
                event_interface::unsubscribe<events::right_mouse_click>(right_mouse_click_handler_);
            }
            player_dog(sprite::sprite sprite, hitbox::hitbox hitbox, Vector2 position, int id)
            : entity(sprite, hitbox, position, id), cosmetics_({}), 
            direction_scalar_(level_config::direction_scalars[level_config::directions::right]),
            right_mouse_click_handler_([this](const events::right_mouse_click& event) -> void {on_right_click_event(event);}){
                    event_interface::subscribe<events::right_mouse_click>(right_mouse_click_handler_);

            };
            player_dog(const player_dog& other) = default;
            player_dog(player_dog&& other) = default;

            player_dog& operator=(const player_dog& other) = default;
            player_dog& operator=(player_dog&& other) = default;

            int update(float delta) override;
            Vector2 get_direction_scalar();

            void interact(entity& other) override;
            void on_right_click_event(const events::right_mouse_click& event);
            void set_path(std::vector<Vector2>& path);
            // something for cosmetics
            
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
            void draw_path();

            events::event_handler<events::right_mouse_click> right_mouse_click_handler_;
            std::vector<sprite::sprite> cosmetics_;
            std::vector<Vector2> move_path_; // the prev array from the path algorithm
            
            const Vector2 move_speed_ = assets_config::dog_move_speed; // TODO: specify move speed, in config file (28 . 12), in terms of a factor of edge weight (one tenth ? )
            Vector2 direction_scalar_;


    };
    // ------------------ entity builder ------------------ //
    class entity_builder{
        public:
            std::unique_ptr<entity> build_cursor(Vector2 position, int id);
            std::unique_ptr<entity> build_mack(Vector2 position, int id);
            std::unique_ptr<entity> build_khiri(Vector2 position, int id);
            std::unique_ptr<entity> build_paw_mark(Vector2 position, int id);
            ~entity_builder() = default;
            entity_builder() {};
            entity_builder(const entity_builder& other) = default;
            entity_builder(entity_builder&& other) = default;

            entity_builder& operator=(const entity_builder& other) = default;
            entity_builder& operator=(entity_builder&& other) = default;

    };
    extern entity_builder e_builder;
}
#endif