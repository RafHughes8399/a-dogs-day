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
            class interaction_state{
                public:
                    virtual ~interaction_state() = default;
                    interaction_state(){};
                    interaction_state(const interaction_state& other) = default;
                    interaction_state(interaction_state&& other) = default;
                    
                    interaction_state& operator=(const interaction_state& other) = default;
                    interaction_state& operator=(interaction_state&& other) = default;

                    virtual void interact(cursor& cursor, entity& other) = 0;
                private:
            };
            class left_click_state : public interaction_state{
                public:
                    left_click_state()
                    : interaction_state() {};
                    left_click_state(const left_click_state& other) = default;
                    left_click_state(left_click_state&& other) = default;
                    
                    left_click_state& operator=(const left_click_state& other) = default;
                    left_click_state& operator=(left_click_state&& other) = default;
                    
                    void interact(cursor& cursor, entity& other) override;
                    
                    private:
                };
                
                class right_click_state : public interaction_state{
                    public:
                    right_click_state()
                    : interaction_state() {};
                    right_click_state(const right_click_state& other) = default;
                    right_click_state(right_click_state&& other) = default;
                    
                    right_click_state& operator=(const right_click_state& other) = default;
                    right_click_state& operator=(right_click_state&& other) = default;

                    void interact(cursor& cursor, entity& other) override;
                    private:
                };

            class default_state : public interaction_state{
                public:
                    default_state()
                    : interaction_state() {};
                    default_state(const default_state& other) = default;
                    default_state(default_state&& other) = default;
                        
                    default_state& operator=(const default_state& other) = default;
                    default_state& operator=(default_state&& other) = default;
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
                interaction_state_(std::make_unique<default_state>()){
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

                std::unique_ptr<interaction_state> interaction_state_;
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
            player_dog(sprite::sprite sprite, hitbox::hitbox hitbox, Vector2 position, int id, Vector2 move_speed)
            : entity(sprite, hitbox, position, id), move_speed_(move_speed), is_selected_(false), cosmetics_({}),
            right_mouse_click_handler_([this](const events::right_mouse_click& event) -> void {on_right_click_event(event);}){
                    event_interface::subscribe<events::right_mouse_click>(right_mouse_click_handler_);

            };
            player_dog(const player_dog& other) = default;
            player_dog(player_dog&& other) = default;

            player_dog& operator=(const player_dog& other) = default;
            player_dog& operator=(player_dog&& other) = default;

            int update(float delta) override;
            void interact(entity& other) override;

            void on_right_click_event(const events::right_mouse_click& event);

            // something for cosmetics

        private:
            const Vector2 move_speed_;

            bool is_selected_;
            events::event_handler<events::right_mouse_click> right_mouse_click_handler_;
            std::vector<sprite::sprite> cosmetics_;

            std::vector<Vector2> move_path_;

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