#ifndef HITBOX_H
#define HITBOX_H

#include <algorithm>
#include <vector>
#include "config.h"

#include "raylib.h"
namespace hitbox{
    class hitbox{
        public:
            ~hitbox() = default;
            hitbox(Rectangle box)
            : box_(box), sub_boxes_({}){};
            hitbox(Rectangle box, std::vector<Rectangle> sub_boxes)
            : box_(box), sub_boxes_(sub_boxes){};

            hitbox(const hitbox& other) = default;
            hitbox(hitbox&& other) = default;

            hitbox& operator=(const hitbox& other) = default;
            hitbox& operator=(hitbox&& other) = default;

            bool check_collision(const hitbox& other);
            std::vector<Rectangle> get_sub_boxes();
            Rectangle& get_box();
            Rectangle get_sub_box(size_t index);
            void update(Vector2 delta);    
        private:
            Rectangle box_;
            std::vector<Rectangle> sub_boxes_;
    };

    class hitbox_builder{
        public:
            ~hitbox_builder() = default;
            hitbox_builder() {};
            hitbox_builder(const hitbox_builder& other) = default;
            hitbox_builder(hitbox_builder&& other) = default;

            hitbox_builder& operator=(const hitbox_builder& other) = default;
            hitbox_builder& operator=(hitbox_builder&& other) = default;

            hitbox build_cursor_hitbox(Vector2 position);
            hitbox build_paw_mark_hitbox(Vector2 position);
            hitbox build_player_dog_hitbox(Vector2 position);
    }
    extern h_builder_;
} // namespace hitbox


#endif