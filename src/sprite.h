/**
 *  header file for handling spritesheets for entities, holds the animation details too. 
 *  ! basic overview of how it works !
 *  author: raffa, october 25 
 */
#ifndef SPRITE_H
#define SPRITE_H

#include "animation.h"
namespace sprite{
    class sprite{
        public:
            ~sprite() = default;
            sprite(Texture2D texture, float frame_width, float frame_height, int frames, int animations)
                : sprite_texture_(texture), sprite_animation_(animation::animation(frame_height, frame_width, frames, animations)){};
            sprite(const sprite& other) = default;
            sprite(sprite&& other) = default;
            
            sprite& operator=(const sprite& other) = default;
            sprite& operator=(sprite&& other) = default;

            animation::animation& get_animation();
            Texture2D& get_texture();
            void render(Vector2 position);
        private:
            // has the texture 
            animation::animation sprite_animation_;
            Texture2D sprite_texture_; 
            // and the animation
    };
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