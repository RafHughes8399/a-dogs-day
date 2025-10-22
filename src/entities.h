/**
 * header file that defines entitiy class hierarchy
 */
#ifndef ENTITIES_H
#define ENTITIES_h

#include "raglib.h"
#include "sprite.h"
namespace entities{
    enum status_codes{
        nothing = 0,
        moved = 1
    };
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
            Vector2 get_position();
            int get_id();

            virtual int update(float delta) = 0;
            void render();
            virtual void interact(entity& other) = 0;

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
     */
    class cursor : public entity{
        public:
            ~cursor() = default;
            cursor(sprite::sprite sprite, raglib::bounding_box_2 bounds, Vector2 position, int id)
                : entity(sprite, bounds, position, id){

            };
            cursor(const cursor& other) = default;
            cursor(cursor&& other) = default;

            cursor& operator=(const cursor& other) = default;
            cursor& operator=(cursor&& other) = default;

            int update(float delta) override;
            void interact(entity& other) override;

        private:
    };
    
}
#endif