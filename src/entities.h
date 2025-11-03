/**
 * header file that defines entitiy class hierarchy
 */
#ifndef ENTITIES_H
#define ENTITIES_h


#include <iostream>
#include "config.h"
#include "events.h"
#include "events_interface.h"
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
            entity(sprite::sprite sprite, raglib::bounding_box_2 bounds, Vector2 position, int id)
            : bounds_(bounds), sprite_(sprite), position_(position), id_(id){

            };
            entity(const entity& other) = default;

            entity(entity&& other) = default;
            entity& operator=(const entity& other) = default;
            entity& operator=(entity&& other) = default;
            bool operator==(entity& other){
                return id_ == other.id_;
            }

            raglib::bounding_box_2& get_bounds();
            sprite::sprite&  get_sprite();
            Vector2 get_position();
            int get_id();

            void update_bounds(Vector2 delta);
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
            
            raglib::bounding_box_2 bounds_;
            sprite::sprite sprite_;
            Vector2 position_;

    };
    // the cursor for the player 
    /**
     * shaped as a paw, changes based on interactable behaviour, like a regular cursor, 
     * if you consider it, a cursor is an entity, because it will interact with other entities
     * 
     * they would differ by their interactions ? 
     *  instead of making a class you could do entity, entity builder and then interact strategy and assign it that one ? 
     * i like this idea 
     * 
     */
    class cursor : public entity{
        public:
            ~cursor() {
                 event_interface::unsubscribe<events::left_mouse_down>(left_mouse_handler_);

            }
            cursor(sprite::sprite sprite, raglib::bounding_box_2 bounds, Vector2 position, int id)
            : entity(sprite, bounds, position, id), left_mouse_handler_([this](const events::left_mouse_down& event) -> void{on_left_mouse_event(event);}){
                event_interface::subscribe<events::left_mouse_down>(left_mouse_handler_);
                
            };
            cursor(const cursor& other)
            : entity(other), left_mouse_handler_(other.left_mouse_handler_){
                event_interface::subscribe<events::left_mouse_down>(left_mouse_handler_);
            }
            cursor(cursor&& other)
            : entity(other), left_mouse_handler_(std::move(other.left_mouse_handler_)){
                event_interface::subscribe<events::left_mouse_down>(left_mouse_handler_);
                std::cout << "move construct cursor" << std::endl;
            }
            
            cursor& operator=(const cursor& other) = default;
            cursor& operator=(cursor&& other)  = default;
            int update(float delta) override;
            void interact(entity& other) override;
            
            void on_left_mouse_event(const events::left_mouse_down& event);
            

        private:
            events::event_handler<events::left_mouse_down> left_mouse_handler_;
    };
    class paw_mark : public entity{
        public:
            ~paw_mark() = default;
            paw_mark(sprite::sprite sprite, raglib::bounding_box_2 bounds, Vector2 position, int id)
            : entity(sprite, bounds, position, id){};
            paw_mark(const paw_mark& other) = default;
            paw_mark(paw_mark&& other) = default;

            paw_mark& operator=(const paw_mark& other) = default;
            paw_mark& operator=(paw_mark&& other) = default;

            int update(float delta) override;
            void interact(entity& other) override;

        private:
    };

    // ------------------ entity builder ------------------ //
    class entity_builder{
        public:
            std::unique_ptr<entity> build_cursor(Vector2 position, int id);
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