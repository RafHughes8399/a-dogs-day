#include "hitbox.h"
#include "sprite.h"

#include <vector>
namespace body{
    class body{
        public:
            ~body() = default;
            body(size_t index = 0)
            : hitboxes_({}), sprites_({}), index_(index){}
            body(std::vector<hitbox::hitbox>& hitboxes, std::vector<sprite::sprite>& sprites, size_t index = 0)
            : hitboxes_(hitboxes), sprites_(sprites), index_(index){}
            
            body(const body& other) = default;
            body(body&& other) = default;

            body& operator=(const body& other) = default;
            body& operator=(body&& other) = default;
            
            size_t get_index();
            void set_index(size_t index);

            hitbox::hitbox& get_hitbox();
            sprite::sprite& get_sprite();

            std::vector<hitbox::hitbox> get_hitboxes();
            std::vector<sprite::sprite> get_sprites();

            void render(Vector2 position);
            void update_hitboxes(Vector2 new_position);

        private:
            // structured as parallel arrays
            std::vector<hitbox::hitbox> hitboxes_;
            std::vector<sprite::sprite> sprites_;
            size_t index_;
    };
}