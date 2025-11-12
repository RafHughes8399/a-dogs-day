#ifndef HITBOX_H
#define HITBOX_H

#include <vector>
#include "raylib.h"
namespace hitbox{
    class hitbox{
        public:
            ~hitbox() = default;
            hitbox()
            : rectangles_({}){};
            hitbox(std::vector<Rectangle> rectangles)
            : rectangles_(rectangles){};

            hitbox(const hitbox& other) = default;
            hitbox(hitbox&& other) = default;

            hitbox& operator=(const hitbox& other) = default;
            hitbox& operator=(hitbox&& other) = default;

            bool check_collision(const hitbox& other);
            std::vector<Rectangle> get_hitbox();
            Rectangle get_box(size_t index);
            
        private:
            std::vector<Rectangle> rectangles_;
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