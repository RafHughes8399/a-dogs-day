/**
 *  header file for handling spritesheets for entities, holds the animation details too. 
 *  ! basic overview of how it works !
 *  author: raffa, october 25 
 */
#ifndef SPRITE_H
#define SPRITE_H

#include "animation.h"
#include <vector>

namespace sprite{
    class sprite{
        public:
            ~sprite() = default;
            sprite(Texture2D texture, float frame_width, float frame_height, int frames, int animations, Vector2 draw_position_offset = Vector2Zero())
                : sprite_animation_(animation::animation(frame_width, frame_height, frames, animations)),
                sprite_texture_(texture),
                draw_position_offset_(draw_position_offset){}
            sprite(const sprite& other) = default;
            sprite(sprite&& other) = default;
            
            sprite& operator=(const sprite& other) = default;
            sprite& operator=(sprite&& other) = delete;

            animation::animation& get_animation();
            const Texture2D& get_texture();
            void render(Vector2 position, int frame);
        private:
            // has the texture 
            // and the animation
            animation::animation sprite_animation_;
            const Texture2D sprite_texture_; 
            Vector2 draw_position_offset_;
    };
    class spriteset{
        public:
            ~spriteset() = default;
            spriteset(std::vector<sprite>& sprites, size_t index = 0)
            : sprites_(sprites), current_(index){}
            spriteset(const spriteset& other) = default;
            spriteset(spriteset&& other) = default;

            spriteset& operator=(const spriteset& other) = default;
            spriteset& operator=(spriteset&& other) = default;

            sprite& operator[](size_t index){
                return sprites_[index];
            }
            
            size_t index();
            sprite& get_sprite();
            std::vector<sprite>& get_sprites();
            
            void set_index(size_t index);
            void render(Vector2 position, int frame);
        private:
            size_t current_;
            std::vector<sprite> sprites_;
    };
    // a sprite should have multiple textures, up to 4 (up down left right)
    // potential use for displaying things like shop items
    // i.e all the hats exist on one sprite sheet, then the shop displays them, idk more thought needded
    class spritesheet{
        public:
        private:
        // has a texture
        // not an animation though, something else, ? maybe a grid or something ?
    };
    class tilesheet{
        public:
        private:
            // has a texture
            // and a grid thing as well, denoting how many tiles 
            // and can select a tile by referencing its position in the grid 

    };

    // perhaps a sprite builder could be of use
    // sprite_builder::build_background();
}
#endif
