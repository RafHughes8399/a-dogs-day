/**
 * header file that defines entitiy class hierarchy
 */
#ifndef ENTITIES_H
#define ENTITIES_h

#include "raglib.h"
namespace entities{
    enum status_codes{
        nothing = 0,
        moved = 1
    };
    class entity {
        public:

            bool operator==(entity& other){
                (void) other;
                return true;
            }
            raglib::bounding_box_2& get_bounds();
            Vector2 get_position();
            int get_id();

            int update(float delta);
            void render();
            void interact(entity& other);
        private:
            Vector2 position_;
            raglib::bounding_box_2 bounds_;
            const int id_;
    };
    
}
#endif