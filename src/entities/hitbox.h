#ifndef HITBOX_H
#define HITBOX_H


#include <vector>
#include "config.h"

#include "raylib.h"
namespace hitbox{
    class hitbox{
        public:
            ~hitbox() = default;
            hitbox(Rectangle box)
            : box_(box), sub_boxes_({}){}
            hitbox(Rectangle box, std::vector<Rectangle> sub_boxes)
            : box_(box), sub_boxes_(sub_boxes){}

            hitbox(const hitbox& other) = default;
            hitbox(hitbox&& other) = default;

            hitbox& operator=(const hitbox& other) = default;
            hitbox& operator=(hitbox&& other) = default;

            bool check_collision(const hitbox& other);
        
            const Rectangle& get_box() const;
            Rectangle get_sub_box(size_t index);
        
            std::vector<Rectangle> get_sub_boxes();
            void update(Vector2 new_position);    
        
            private:

            bool check_collision_box_sub_boxes(const Rectangle& box, const std::vector<Rectangle>& other_boxes);
            bool check_collision_sub_boxes(const std::vector<Rectangle>& boxes, const std::vector<Rectangle> other_boxes);
            Rectangle box_;
            std::vector<Rectangle> sub_boxes_;
    };

    class hitbox_builder{
        public:
            ~hitbox_builder() = default;
            hitbox_builder() {}
            hitbox_builder(const hitbox_builder& other) = default;
            hitbox_builder(hitbox_builder&& other) = default;

            hitbox_builder& operator=(const hitbox_builder& other) = default;
            hitbox_builder& operator=(hitbox_builder&& other) = default;

            hitbox build_cursor_hitbox(Vector2 position);
            hitbox build_paw_mark_hitbox(Vector2 position);
            hitbox build_player_dog_across_hitbox(Vector2 position);
            hitbox build_player_dog_down_hitbox(Vector2 position);

            hitbox build_test_decoration_hitbox(Vector2 position);
            hitbox build_gargoyle_hitbox(Vector2 position);
            hitbox build_table_hitbox(Vector2 position);
            hitbox build_food_counter_hitbox(Vector2 position);
            hitbox build_food_hitbox(Vector2 position);
    }
    extern h_builder_;
} // namespace hitbox


#endif
